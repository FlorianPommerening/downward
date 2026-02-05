#ifndef PYTHON_BINDING_GENERATED_H
#define PYTHON_BINDING_GENERATED_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <nanobind/nanobind.h>
#pragma GCC diagnostic pop

#include <string>
#include <unordered_set>

namespace nb = nanobind;

extern void bind_feature_classes(
    nb::module_ &m, const std::unordered_set<std::string> &skip);
extern void bind_list_classes(nb::module_ &m);
extern void bind_enums(nb::module_ &m);
extern void bind_features(nb::module_ &m);

#endif
