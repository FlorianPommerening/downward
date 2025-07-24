#include "generate.h"

#include "../plugins/plugin.h"
#include "../plugins/raw_registry.h"
#include "../plugins/registry.h"
#include "../plugins/types.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

static string strip_namespace(const string &qualified_name) {
    string_view view = qualified_name;
    auto pos = view.rfind("::");
    if (pos == string_view::npos) {
        return qualified_name;
    } else {
        return string(view.substr(pos + 2));
    }
}

static string to_upper(const string &value) {
    string value_upper;
    value_upper.reserve(value.size());
    ranges::transform(value, back_inserter(value_upper), ::toupper);
    return value_upper;
}

static void print_header() {
    cout
        << "#include \"python/binding_generated.h\"" << endl
        << "#include \"python/exception.h\"" << endl
        << endl
        << "#include \"plugins/doc_printer.h\"" << endl
        << endl
        << "#include <sstream>" << endl
        << endl
        << "using namespace std;" << endl
        << endl
        << "static string get_doc(const plugins::Registry &registry, const string &name) {"
        << endl
        << "    std::ostringstream oss;" << endl
        << "    unique_ptr<plugins::PlainPrinter> doc_printer = " << endl
        << "make_unique<plugins::PlainPrinter>(oss, registry);" << endl
        << "    doc_printer->print_feature(name);" << endl
        << "    return oss.str();" << endl
        << "}" << endl;
}

static void print_bind_feature_classes(const plugins::FeatureTypes &types) {
    cout << "void bind_feature_classes(nb::module_ &m, "
         << "const unordered_set<string> &skip) {" << endl;
    for (const plugins::FeatureType *type : types) {
        string type_name = type->name();
        string fully_qualified_name = type->fully_qualified_name();
        cout << "    if (!skip.count(\"" << type_name << "\")) {" << endl
             << "        nb::class_<" << fully_qualified_name << ">(m, \""
             << type_name << "\");" << endl
             << "    }" << endl;
    }
    cout << "}" << endl << endl;
}

static void print_bind_enums(const vector<const plugins::Type *> &enum_types) {
    cout << "void bind_enums(nb::module_ &m) {" << endl;
    for (const plugins::Type *type : enum_types) {
        string fully_qualified_name = type->fully_qualified_name();
        cout << "    nb::enum_<" << fully_qualified_name << ">(m, \""
             << strip_namespace(fully_qualified_name) << "\")";
        const plugins::EnumInfo &enum_info = type->get_documented_enum_values();
        for (const auto &entry : enum_info) {
            const string &value = entry.first;
            string value_upper = to_upper(value);
            const string &doc = entry.second;
            cout << endl
                 << "        .value(\"" << value << "\", "
                 << fully_qualified_name << "::" << value_upper << ", \"" << doc
                 << "\")";
        }
        cout << ";" << endl;
    }
    cout << "}" << endl << endl;
}

static void print_feature_arg(const plugins::ArgumentInfo &info) {
    cout << "        nb::arg(\"" << info.key << "\")";
    if (info.has_default()) {
        if (info.type.is_enum_type()) {
            cout << " = " << info.type.fully_qualified_name()
                 << "::" << to_upper(info.default_value);
        } else if (info.type.is_list_type() && info.default_value == "[]") {
            /*
              TODO: this doesn't work. Maybe exposing the vector to nanobind
              would help, but maybe we have to handle defaults in a different
              way altogether.
            */
            //            cout << " = " << info.type.fully_qualified_name() <<
            //            "()";
        } else if (info.type.is_basic_type()) {
            if (info.type.get_basic_type_index() == type_index(typeid(int))) {
                if (info.default_value == "infinity") {
                    cout << " = " << to_string(numeric_limits<int>::max());
                } else {
                    string value;
                    bool valid = true;
                    for (char c : info.default_value) {
                        switch (tolower(c)) {
                        case 'k':
                            value += "000";
                            break;
                        case 'm':
                            value += "000000";
                            break;
                        case 'g':
                            value += "000000000";
                            break;
                        default:
                            if (isdigit(c)) {
                                value += c;
                            } else {
                                valid = false;
                            }
                            break;
                        }
                    }
                    if (valid) {
                        cout << " = " << value;
                    }
                }
            } else if (
                info.type.get_basic_type_index() ==
                type_index(typeid(double))) {
                if (info.default_value == "infinity") {
                    cout << " = " << to_string(numeric_limits<double>::max());
                }
            } else {
                // HACK: this will not work in all cases
                cout << " = " << info.default_value;
            }
        }
    }
}

static void print_bind_stub_for_feature(const plugins::Feature &feature) {
    string key = feature.get_key();
    string pointer_type_name =
        "shared_ptr<" + feature.get_type().fully_qualified_name() + ">";

    cout << "static void bind_" << key
         << "(nb::module_ &m, const plugins::Registry &registry) {" << endl
         << "    shared_ptr<const plugins::Feature> feature = "
         << "registry.get_feature(\"" << key << "\");" << endl
         << "    auto construct = [feature](" << endl;
    string sep = "";
    for (const plugins::ArgumentInfo &info : feature.get_arguments()) {
        cout << sep << "        ";
        if (info.type.is_feature_type()) {
            cout << "std::shared_ptr<" << info.type.fully_qualified_name()
                 << ">";
        } else {
            cout << info.type.fully_qualified_name();
        }
        cout << " " << info.key;
        sep = ",\n";
    }
    cout << ") {" << endl << "        plugins::Options opts;" << endl;
    for (const plugins::ArgumentInfo &info : feature.get_arguments()) {
        cout << "        opts.set(\"" << info.key << "\", " << info.key << ");"
             << endl;
    }
    cout << "        utils::Context context;" << endl
         << "        try {" << endl
         << "            return plugins::any_cast<" << pointer_type_name
         << ">(feature->construct(opts, context));" << endl
         << "        } catch (const utils::ContextError &e) {" << endl
         << "            throw ConstructionError(e.get_message());" << endl
         << "        }" << endl
         << "    };" << endl
         << "    string doc = get_doc(registry, feature->get_key());" << endl
         << "    m.def(\"" << key << "\", construct, doc.c_str()";
    for (const plugins::ArgumentInfo &info : feature.get_arguments()) {
        cout << ",\n        ";
        print_feature_arg(info);
    }
    cout << ");" << endl << "}" << endl << endl;
}

static void print_bind_stubs_for_category(
    const plugins::Registry &registry, const plugins::FeatureType &type,
    vector<string> &keys) {
    vector<const plugins::Feature *> features;
    for (const shared_ptr<const plugins::Feature> &feature :
         registry.get_features()) {
        if (feature->get_type() == type) {
            keys.push_back(feature->get_key());
            features.push_back(feature.get());
        }
    }
    sort(
        features.begin(), features.end(),
        [](const plugins::Feature *p1, const plugins::Feature *p2) {
            return p1->get_key() < p2->get_key();
        });

    for (const plugins::Feature *feature : features) {
        print_bind_stub_for_feature(*feature);
    }
}

static void print_bind_features(const vector<string> &keys) {
    cout << "void bind_features(nb::module_ &m) {" << endl
         << "    plugins::Registry registry = "
         << "plugins::RawRegistry::instance()->construct_registry();" << endl;

    for (const string &key : keys) {
        cout << "    bind_" << key << "(m, registry);" << endl;
    }
    cout << "}" << endl << endl;
}

void create_python_binding_code() {
    print_header();
    plugins::Registry registry =
        plugins::RawRegistry::instance()->construct_registry();
    plugins::FeatureTypes feature_types = registry.get_feature_types();
    sort(
        feature_types.begin(), feature_types.end(),
        [](const plugins::FeatureType *t1, const plugins::FeatureType *t2) {
            return t1->name() < t2->name();
        });
    print_bind_feature_classes(feature_types);

    plugins::TypeRegistry &type_registry = *plugins::TypeRegistry::instance();
    vector<const plugins::Type *> enum_types = type_registry.get_enum_types();
    print_bind_enums(enum_types);

    vector<string> keys;
    for (const plugins::FeatureType *type : feature_types) {
        print_bind_stubs_for_category(registry, *type, keys);
    }
    print_bind_features(keys);
}
