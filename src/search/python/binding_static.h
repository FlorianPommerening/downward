#ifndef PYTHON_BINDING_STATIC_H
#define PYTHON_BINDING_STATIC_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <nanobind/nanobind.h>
#pragma GCC diagnostic pop

#include <string>
#include <unordered_set>

namespace nb = nanobind;

extern std::unordered_set<std::string> bind_basic_classes(nb::module_ &m);

#endif
