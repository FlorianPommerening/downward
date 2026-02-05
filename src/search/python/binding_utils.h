#ifndef PYTHON_BINDING_UTILS_H
#define PYTHON_BINDING_UTILS_H

#include <string>

namespace plugins {
class Feature;
class Options;
class Registry;
}

namespace utils {
class Context;
}

void add_default_values(plugins::Options &opts, const plugins::Feature &feature, utils::Context &context);
void check_bounds(const plugins::Options &opts, const plugins::Feature &feature, utils::Context &context);
std::string get_doc(const plugins::Registry &registry, const std::string &name);

#endif
