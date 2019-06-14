#include "task_proxy.h"

#include "axioms.h"
#include "state_registry.h"

#include "task_utils/causal_graph.h"

#include <iostream>

using namespace std;

State State::get_successor(const OperatorProxy &op) const {
    assert(!op.is_axiom());
    //assert(is_applicable(op, state));
    vector<int> new_values = *values;
    for (EffectProxy effect : op.get_effects()) {
        if (does_fire(effect, *this)) {
            FactPair effect_fact = effect.get_fact().get_pair();
            new_values[effect_fact.var] = effect_fact.value;
        }
    }
    if (task->get_num_axioms() > 0) {
        AxiomEvaluator &axiom_evaluator = g_axiom_evaluators[TaskProxy(*task)];
        axiom_evaluator.evaluate(new_values);
    }
    return State(*task, move(new_values), StateHandle::unregistered_state);
}

FactProxy State::operator[](size_t var_id) const {
    assert(var_id < size());
    if (values) {
        return FactProxy(*task, var_id, (*values)[var_id]);
    }
    const StateRegistry *registry = handle.get_registry();
    assert(registry);
    const int_packer::IntPacker &state_packer = registry->get_state_packer();
    assert(buffer);
    return FactProxy(*task, var_id,  state_packer.get(buffer, var_id));
}

void State::unpack() const {
    if (!values) {
        int num_variables = size();
        const StateRegistry *registry = handle.get_registry();
        assert(registry);
        const int_packer::IntPacker &state_packer = registry->get_state_packer();
        assert(buffer);
        values = make_shared<vector<int>>(num_variables);
        for (int var = 0; var < num_variables; ++var) {
            (*values)[var] = state_packer.get(buffer, var);
        }
    }
}


const causal_graph::CausalGraph &TaskProxy::get_causal_graph() const {
    return causal_graph::get_causal_graph(task);
}
