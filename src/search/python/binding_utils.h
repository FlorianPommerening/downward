#ifndef PYTHON_BINDING_UTILS_H
#define PYTHON_BINDING_UTILS_H

#include <string>

namespace plugins {
class Any;
class Feature;
class Options;
class Registry;
class Type;
}

namespace utils {
class Context;
}

plugins::Any parse_as(const std::string &value, const plugins::Type &type, utils::Context context);
void add_default_values(plugins::Options &opts, const plugins::Feature &feature, utils::Context &context);
void check_bounds(const plugins::Options &opts, const plugins::Feature &feature, utils::Context &context);
std::string get_doc(const plugins::Registry &registry, const std::string &name);

#endif
