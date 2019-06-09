#include "state.h"

#include "axioms.h"
#include "state_registry.h"
#include "task_proxy.h"

#include "task_utils/task_properties.h"

#include <cstring>

using namespace std;

static PackedStateBin *pack_values(
    vector<int> &&values, const StatePacker &state_packer) {
    int num_vars = values.size();
    PackedStateBin *buffer = new PackedStateBin[state_packer.get_num_bins()];
    // Avoid garbage values in half-full bins.
    fill_n(buffer, state_packer.get_num_bins(), 0);
    for (int var = 0; var < num_vars; ++var) {
        state_packer.set(buffer, var, values[var]);
    }
    return buffer;
}

void State::sanity_check() const {
    assert((owns_buffer && handle == StateHandle::unregistered_state) ||
           (!owns_buffer && handle != StateHandle::unregistered_state));
}

State::State(
    const AbstractTask &task,
    const PackedStateBin *buffer,
    const StatePacker &state_packer,
    StateHandle handle)
    : task(&task), buffer(buffer), state_packer(state_packer), handle(handle),
      owns_buffer(handle == handle.unregistered_state) {
    sanity_check();
}

State::State(const AbstractTask &task, vector<int> &&values)
    : task(&task),
      buffer(pack_values(move(values), task_properties::g_state_packers[TaskProxy(task)])),
      state_packer(task_properties::g_state_packers[TaskProxy(task)]),
      handle(StateHandle::unregistered_state),
      owns_buffer(true) {
    sanity_check();
}

State::~State() {
    if (owns_buffer)
        delete[] buffer;
}

State::State(const State &other)
    : task(other.task),
      state_packer(other.state_packer),
      handle(other.handle),
      owns_buffer(other.owns_buffer) {
    if (handle == StateHandle::unregistered_state) {
        buffer = new PackedStateBin[state_packer.get_num_bins()];
        memcpy(&buffer, other.buffer, sizeof(PackedStateBin) * state_packer.get_num_bins());
    } else {
        buffer = other.buffer;
    }
    sanity_check();
}

State &State::operator=(State &&other) {
    if (this != &other) {
        assert(task == other.task);
        other.task = nullptr;
        buffer = other.buffer;
        other.buffer = nullptr;
        // No need to set the state packer since the two tasks are the same.
        handle = other.handle;
        other.handle = StateHandle::unregistered_state;
        owns_buffer = other.owns_buffer;
        other.owns_buffer = false;
    }
    sanity_check();
    return *this;
}

State::State(State &&other)
    : task(other.task),
      buffer(other.buffer),
      state_packer(other.state_packer),
      handle(other.handle),
      owns_buffer(other.owns_buffer) {
    assert(task == other.task);
    other.task = nullptr;
    other.buffer = nullptr;
    other.handle = StateHandle::unregistered_state;
    other.owns_buffer = false;
    sanity_check();
}

FactProxy State::operator[](std::size_t var_id) const {
    assert(var_id < size());
    return FactProxy(*task, var_id, state_packer.get(buffer, var_id));
}

FactProxy State::operator[](VariableProxy var) const {
    return (*this)[var.get_id()];
}

std::vector<int> State::unpack() const {
    int num_variables = size();
    vector<int> values(num_variables);
    for (int var = 0; var < num_variables; ++var)
        values[var] = (*this)[var].get_value();
    return values;
}

State State::get_successor(const OperatorProxy &op) const {
    assert(!op.is_axiom());
    assert(task_properties::is_applicable(op, *this));
    PackedStateBin *new_buffer = new PackedStateBin[state_packer.get_num_bins()];
    memcpy(new_buffer, buffer, sizeof(PackedStateBin) * state_packer.get_num_bins());
    for (EffectProxy effect : op.get_effects()) {
        if (does_fire(effect, *this)) {
            FactPair effect_fact = effect.get_fact().get_pair();
            state_packer.set(new_buffer, effect_fact.var, effect_fact.value);
        }
    }
    if (task->get_num_axioms() > 0) {
        AxiomEvaluator &axiom_evaluator = g_axiom_evaluators[TaskProxy(*task)];
        axiom_evaluator.evaluate(new_buffer, state_packer);
    }
    return State(*task, new_buffer, state_packer, StateHandle::unregistered_state);
}
