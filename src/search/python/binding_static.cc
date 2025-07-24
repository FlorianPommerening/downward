#include "binding_static.h"

#include "../abstract_task.h"
#include "../operator_id.h"
#include "../search_algorithm.h"

#include "../plugins/options.h"
#include "../plugins/plugin.h"
#include "../plugins/raw_registry.h"
#include "../plugins/registry.h"
#include "../tasks/root_task.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/trampoline.h>
#pragma GCC diagnostic pop

#include <fstream>

namespace nb = nanobind;

using namespace std;

class PyTask : public AbstractTask {
    NB_TRAMPOLINE(AbstractTask, 1);
public:
    int get_num_variables() const override {
        NB_OVERRIDE_PURE(get_num_variables);
    }
    virtual string get_variable_name(int var) const override {
        NB_OVERRIDE_PURE(get_variable_name, var);
    }
    virtual int get_variable_domain_size(int var) const override {
        NB_OVERRIDE_PURE(get_variable_domain_size, var);
    }
    virtual int get_variable_axiom_layer(int var) const override {
        NB_OVERRIDE_PURE(get_variable_axiom_layer, var);
    }
    virtual int get_variable_default_axiom_value(int var) const override {
        NB_OVERRIDE_PURE(get_variable_default_axiom_value, var);
    }
    virtual string get_fact_name(const FactPair &fact) const override {
        NB_OVERRIDE_PURE(get_fact_name, fact);
    }
    virtual bool are_facts_mutex(
        const FactPair &fact1, const FactPair &fact2) const override {
        NB_OVERRIDE_PURE(are_facts_mutex, fact1, fact2);
    }
    virtual int get_operator_cost(int index, bool is_axiom) const override {
        NB_OVERRIDE_PURE(get_operator_cost, index, is_axiom);
    }
    virtual string get_operator_name(int index, bool is_axiom) const override {
        NB_OVERRIDE_PURE(get_operator_name, index, is_axiom);
    }
    virtual int get_num_operators() const override {
        NB_OVERRIDE_PURE(get_num_operators);
    }
    virtual int get_num_operator_preconditions(
        int index, bool is_axiom) const override {
        NB_OVERRIDE_PURE(get_num_operator_preconditions, index, is_axiom);
    }
    virtual FactPair get_operator_precondition(
        int op_index, int fact_index, bool is_axiom) const override {
        NB_OVERRIDE_PURE(
            get_operator_precondition, op_index, fact_index, is_axiom);
    }
    virtual int get_num_operator_effects(
        int op_index, bool is_axiom) const override {
        NB_OVERRIDE_PURE(get_num_operator_effects, op_index, is_axiom);
    }
    virtual int get_num_operator_effect_conditions(
        int op_index, int eff_index, bool is_axiom) const override {
        NB_OVERRIDE_PURE(
            get_num_operator_effect_conditions, op_index, eff_index, is_axiom);
    }
    virtual FactPair get_operator_effect_condition(
        int op_index, int eff_index, int cond_index,
        bool is_axiom) const override {
        NB_OVERRIDE_PURE(
            get_operator_effect_condition, op_index, eff_index, cond_index,
            is_axiom);
    }
    virtual FactPair get_operator_effect(
        int op_index, int eff_index, bool is_axiom) const override {
        NB_OVERRIDE_PURE(get_operator_effect, op_index, eff_index, is_axiom);
    }
    virtual int convert_operator_index(
        int index, const AbstractTask *ancestor_task) const override {
        NB_OVERRIDE_PURE(convert_operator_index, index, ancestor_task);
    }
    virtual int get_num_axioms() const override {
        NB_OVERRIDE_PURE(get_num_axioms);
    }
    virtual int get_num_goals() const override {
        NB_OVERRIDE_PURE(get_num_goals);
    }
    virtual FactPair get_goal_fact(int index) const override {
        NB_OVERRIDE_PURE(get_goal_fact, index);
    }
    virtual vector<int> get_initial_state_values() const override {
        NB_OVERRIDE_PURE(get_initial_state_values);
    }
    virtual void convert_ancestor_state_values(
        vector<int> &values, const AbstractTask *ancestor_task) const override {
        NB_OVERRIDE_PURE(convert_ancestor_state_values, values, ancestor_task);
    }
};

static void read_task(const string &sas_file) {
    ifstream task_file(sas_file);
    tasks::read_root_task(task_file);
}

static void hacks(nb::module_ &m) {
    m.def("read_task", &read_task);
}

unordered_set<string> bind_basic_classes(nb::module_ &m) {
    unordered_set<string> manually_specified;
    hacks(m);
    nb::class_<FactPair>(m, "FactPair")
        .def_rw("var", &FactPair::var)
        .def_rw("value", &FactPair::value);
    nb::class_<OperatorID>(m, "OperatorID")
        .def("get_index", &OperatorID::get_index);
    nb::class_<AbstractTask, PyTask>(m, "AbstractTask")
        .def("get_num_variables", &AbstractTask::get_num_variables)
        .def("get_variable_name", &AbstractTask::get_variable_name)
        .def(
            "get_variable_domain_size", &AbstractTask::get_variable_domain_size)
        .def(
            "get_variable_axiom_layer", &AbstractTask::get_variable_axiom_layer)
        .def(
            "get_variable_default_axiom_value",
            &AbstractTask::get_variable_default_axiom_value)
        .def("get_fact_name", &AbstractTask::get_fact_name)
        .def("are_facts_mutex", &AbstractTask::are_facts_mutex)
        .def("get_operator_cost", &AbstractTask::get_operator_cost)
        .def("get_operator_name", &AbstractTask::get_operator_name)
        .def("get_num_operators", &AbstractTask::get_num_operators)
        .def(
            "get_num_operator_preconditions",
            &AbstractTask::get_num_operator_preconditions)
        .def(
            "get_operator_precondition",
            &AbstractTask::get_operator_precondition)
        .def(
            "get_num_operator_effects", &AbstractTask::get_num_operator_effects)
        .def(
            "get_num_operator_effect_conditions",
            &AbstractTask::get_num_operator_effect_conditions)
        .def(
            "get_operator_effect_condition",
            &AbstractTask::get_operator_effect_condition)
        .def("get_operator_effect", &AbstractTask::get_operator_effect)
        .def("convert_operator_index", &AbstractTask::convert_operator_index)
        .def("get_num_axioms", &AbstractTask::get_num_axioms)
        .def("get_num_goals", &AbstractTask::get_num_goals)
        .def("get_goal_fact", &AbstractTask::get_goal_fact)
        .def(
            "get_initial_state_values", &AbstractTask::get_initial_state_values)
        .def(
            "convert_ancestor_state_values",
            &AbstractTask::convert_ancestor_state_values);
    manually_specified.insert("AbstractTask");
    nb::class_<SearchAlgorithm>(m, "SearchAlgorithm")
        .def("search", &SearchAlgorithm::search)
        .def("get_plan", &SearchAlgorithm::get_plan);
    manually_specified.insert("SearchAlgorithm");
    return manually_specified;
}
