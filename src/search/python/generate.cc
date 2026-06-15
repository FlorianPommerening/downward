#include "generate.h"

#include "../plugins/plugin.h"
#include "../plugins/registry.h"
#include "../plugins/types.h"

#include "../parser/abstract_syntax_tree.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
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

static string to_camel_case(const string &input) {
    istringstream stream(input);
    string word;
    string result;
    while (stream >> word) {
        word[0] = toupper(word[0]);
        for (size_t i = 1; i < word.length(); i++) {
            word[i] = tolower(word[i]);
        }
        result += word;
    }
    return result;
}

static void print_header() {
    cout << "#include \"python/binding_generated.h\"" << endl
         << "#include \"python/binding_utils.h\"" << endl
         << "#include \"binding_generated_private.h\"" << endl
         << endl
         << "#include \"plugins/options.h\"" << endl
         << "#include \"plugins/plugin.h\"" << endl
         << "#include \"plugins/raw_registry.h\"" << endl
         << "#include \"plugins/registry.h\"" << endl
         << "#include \"python/exception.h\"" << endl
         << "#include \"tasks/root_task.h\"" << endl
         << endl
         << "#include <limits>" << endl
         << "#include <nanobind/stl/optional.h>" << endl
         << "#include <nanobind/stl/shared_ptr.h>" << endl
         << "#include <nanobind/stl/string.h>" << endl
         << "#include <nanobind/stl/bind_vector.h>" << endl
         << "#include <optional>" << endl
         << "#include <sstream>" << endl
         << endl
         << "using namespace std;" << endl
         << endl;
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

static void print_bind_list_classes(
    const vector<const plugins::Type *> &types) {
    cout << "void bind_list_classes(nb::module_ &m) {" << endl;
    for (const plugins::Type *type : types) {
        string type_name = to_camel_case(type->name());
        string fully_qualified_name = type->fully_qualified_name();
        cout << "    nb::bind_vector<" << fully_qualified_name << ">(m, \""
             << type_name << "\");" << endl;
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

static bool has_simple_default_value(const plugins::ArgumentInfo &info) {
    return info.has_default() && (info.type.is_basic_type() || info.type.is_enum_type());
}

static bool has_pointer_default_value(const plugins::ArgumentInfo &info) {
    return info.is_optional() && info.type.is_feature_type();
}

static bool has_complex_default_value(const plugins::ArgumentInfo &info) {
    return info.is_optional() && !has_simple_default_value(info) && !has_pointer_default_value(info);
}

static string get_nanobind_default_value(const plugins::ArgumentInfo &info) {
    if (has_complex_default_value(info)) {
        return "nb::none()";
    } else if (has_pointer_default_value(info)) {
        return "nullptr";
    } else if (info.type.is_basic_type()) {
        if (info.type.get_basic_type_index() == type_index(typeid(int))) {
            plugins::Any value_any = parser::parse_as(info.default_value, info.type);
            int value = any_cast<int>(value_any);
            if (value == numeric_limits<int>::max()) {
                return "numeric_limits<int>::max()";
            } else {
                return to_string(value);
            }
        } else if (info.type.get_basic_type_index() == type_index(typeid(double))) {
            plugins::Any value_any = parser::parse_as(info.default_value, info.type);
            double value = any_cast<double>(value_any);
            if (value == numeric_limits<double>::infinity()) {
                return "numeric_limits<double>::infinity()";
            } else {
                return to_string(value);
            }
        } else if (info.type.get_basic_type_index() == type_index(typeid(bool))
                   || info.type.get_basic_type_index() == type_index(typeid(string))) {
            return info.default_value;
        } else {
            ABORT("Unknown basic type");
        }
    } else if (info.type.is_enum_type()) {
        return info.type.fully_qualified_name() + "::" + to_upper(info.default_value);
    } else {
        ABORT("Unknown type for default value computation");
    }
}

static string get_pointer_type_name(const plugins::Type &type) {
    string name = type.fully_qualified_name();
    if (type.is_feature_type()) {
        name = "std::shared_ptr<" + name + ">";
    }
    return name;
}

static string get_feature_arg_cpp_type_name(const plugins::ArgumentInfo &info) {
    string name = get_pointer_type_name(info.type);
    if (has_complex_default_value(info)) {
        name = "std::optional<" + name + ">";
    }
    return name;
}

static void print_set_arg_to_options(const plugins::ArgumentInfo &info) {
    if (has_pointer_default_value(info)) {
        cout << "        if(" << info.key << ") {" << endl
             << "            opts.set(\"" << info.key << "\", " << info.key << ");"
             << endl
             << "        }" << endl;
    } else if (has_complex_default_value(info)) {
        cout << "        if(" << info.key << ") {" << endl
             << "            opts.set(\"" << info.key << "\", " << info.key
             << ".value());"
             << endl
             << "        }" << endl;
    } else {
        cout << "        opts.set(\"" << info.key << "\", " << info.key << ");"
             << endl;
    }
}

static void print_feature_arg_nanobind_def(const plugins::ArgumentInfo &info) {
    cout << "        nb::arg(\"" << info.key << "\")";
    if (info.is_optional()) {
        cout << " = " << get_nanobind_default_value(info);
    }
}

static void print_bind_stub_for_feature(const plugins::Feature &feature) {
    string key = feature.get_key();
    string pointer_type_name = get_pointer_type_name(feature.get_type());
    const std::vector<plugins::ArgumentInfo> &args = feature.get_arguments();

    cout << "static void bind_" << key
         << "(nb::module_ &m, const plugins::Registry &registry) {" << endl
         << "    shared_ptr<const plugins::Feature> feature = "
         << "registry.get_feature(\"" << key << "\");" << endl
         << "    auto construct = [feature](" << endl;
    string sep = "";
    for (const plugins::ArgumentInfo &info : args) {
        cout << sep << "        " << get_feature_arg_cpp_type_name(info)
             << " " << info.key;
        sep = ",\n";
    }
    cout << ") {"
         << endl
         << "        plugins::Options opts;" << endl;
    for (const plugins::ArgumentInfo &info : args) {
        print_set_arg_to_options(info);
    }
    cout << "        utils::Context context;" << endl
         << "        try {" << endl
         << "            add_default_values(opts, *feature, context);" << endl
         << "            check_bounds(opts, *feature, context);" << endl
         << "            return plugins::any_cast<" << pointer_type_name
         << ">(feature->construct(opts, context));" << endl
         << "        } catch (const utils::ContextError &e) {" << endl
         << "            throw ConstructionError(e.get_message());" << endl
         << "        }" << endl
         << "    };" << endl
         << "    string doc = get_doc(registry, feature->get_key());" << endl
         << "    m.def(\"" << key << "\", construct, doc.c_str()";
    for (const plugins::ArgumentInfo &info : args) {
        cout << ",\n        ";
        print_feature_arg_nanobind_def(info);
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
         << "    const plugins::Registry &registry = "
         << "*plugins::Registry::instance();" << endl;

    for (const string &key : keys) {
        cout << "    bind_" << key << "(m, registry);" << endl;
    }
    cout << "}" << endl << endl;
}

void create_python_binding_code() {
    print_header();
    const plugins::Registry &registry = *plugins::Registry::instance();
    plugins::FeatureTypes feature_types = registry.get_feature_types();
    sort(
        feature_types.begin(), feature_types.end(),
        [](const plugins::FeatureType *t1, const plugins::FeatureType *t2) {
            return t1->name() < t2->name();
        });
    print_bind_feature_classes(feature_types);

    plugins::TypeRegistry &type_registry = *plugins::TypeRegistry::instance();
    vector<const plugins::Type *> list_types = type_registry.get_list_types();
    print_bind_list_classes(list_types);

    vector<const plugins::Type *> enum_types = type_registry.get_enum_types();
    print_bind_enums(enum_types);

    vector<string> keys;
    for (const plugins::FeatureType *type : feature_types) {
        print_bind_stubs_for_category(registry, *type, keys);
    }
    print_bind_features(keys);
}
