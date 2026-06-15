#include "binding_utils.h"

#include "exception.h"

#include "../plugins/doc_printer.h"
#include "../plugins/options.h"
#include "../plugins/plugin.h"
#include "../plugins/raw_registry.h"
#include "../plugins/registry.h"

#include "../parser/abstract_syntax_tree.h"
#include "../parser/lexical_analyzer.h"
#include "../parser/syntax_analyzer.h"

#include <sstream>

using namespace std;


void add_default_values(plugins::Options &opts, const plugins::Feature &feature, utils::Context &context) {
    utils::TraceBlock block(context, "Adding default values of feature '"
                                         + feature.get_key() + "'");
    for (const plugins::ArgumentInfo &info : feature.get_arguments()) {
        if (!opts.contains(info.key)) {
            if (info.is_optional()) {
                if (info.has_default()) {
                    plugins::Any default_value = parser::parse_as(info.default_value, info.type);
                    opts.set(info.key, default_value);
                }
            } else {
                context.error(
                    "Required argument '" + info.key + "' of feature '"
                    + feature.get_key() + "' was not specified and has no default value.");
            }
        }
    }
}

void check_bounds(const plugins::Options &opts, const plugins::Feature &feature, utils::Context &context) {
    utils::TraceBlock block(context, "Checking bounds of feature '"
                                         + feature.get_key() + "'");
    for (const plugins::ArgumentInfo &info : feature.get_arguments()) {
        utils::TraceBlock block(context, "Argument '" + info.key + "'");
        if (!opts.contains(info.key) && info.is_optional()) {
            continue;
        }
        if (info.bounds.has_bound()) {
            plugins::Any min_value;
            {
                utils::TraceBlock block(context, "Constructing min value");
                min_value = parser::parse_as(info.bounds.min, info.type);
            }
            plugins::Any max_value;
            {
                utils::TraceBlock block(context, "Constructing max value");
                max_value  = parser::parse_as(info.bounds.max, info.type);
            }

            bool bound_satisfied;
            if (info.type.get_basic_type_index() == typeid(int)) {
                int value = opts.get<int>(info.key);
                int min = plugins::any_cast<int>(min_value);
                int max = plugins::any_cast<int>(max_value);
                bound_satisfied = (min <= value) && (value <= max);
            } else if (info.type.get_basic_type_index() == typeid(double)) {
                double value = opts.get<double>(info.key);
                double min = plugins::any_cast<double>(min_value);
                double max = plugins::any_cast<double>(max_value);
                bound_satisfied = (min <= value) && (value <= max);
            } else {
                ABORT("Bounds are only supported for int and double arguments.");
            }
            if (!bound_satisfied) {
                throw utils::ContextError(
                    "Bounds for argument '" + info.key + "' of feature '"
                    + feature.get_key() + "' are not satisfied.");
            }
        }
    }
}

string get_doc(const plugins::Registry &registry, const string &name) {
    std::ostringstream oss;
    unique_ptr<plugins::PlainPrinter> doc_printer =
        make_unique<plugins::PlainPrinter>(oss, registry);
    doc_printer->print_feature(name);
    return oss.str();
}
