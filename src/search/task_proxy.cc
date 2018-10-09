#include "task_proxy.h"

#include "axioms.h"

#include "task_utils/causal_graph.h"

#include <iostream>

using namespace std;

State State::get_successor(OperatorProxy op) const {
    assert(!op.is_axiom());
    //assert(is_applicable(op, state));
    std::vector<int> new_values = values;
    for (EffectProxy effect : op.get_effects()) {
        if (does_fire(effect, *this)) {
            FactProxy effect_fact = effect.get_fact();
            new_values[effect_fact.get_variable().get_id()] = effect_fact.get_value();
        }
    }
    if (task->get_num_axioms() > 0) {
        AxiomEvaluator &axiom_evaluator = g_axiom_evaluators[TaskProxy(*task)];
        axiom_evaluator.evaluate(new_values);
    }
    return State(*task, std::move(new_values));
}

const causal_graph::CausalGraph &TaskProxy::get_causal_graph() const {
    return causal_graph::get_causal_graph(task);
}
