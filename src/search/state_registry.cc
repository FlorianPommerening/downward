#include "state_registry.h"

#include "per_state_information.h"
#include "task_proxy.h"

#include "task_utils/task_properties.h"

using namespace std;

StateRegistry::StateRegistry(const TaskProxy &task_proxy)
    : task_proxy(task_proxy),
      state_packer(task_properties::g_state_packers[task_proxy]),
      axiom_evaluator(g_axiom_evaluators[task_proxy]),
      num_variables(task_proxy.get_variables().size()),
      state_data_pool(get_bins_per_state()),
      registered_states(
          StateIDSemanticHash(state_data_pool, get_bins_per_state()),
          StateIDSemanticEqual(state_data_pool, get_bins_per_state())) {
}

StateID StateRegistry::insert_id_or_pop_state() {
    /*
      Attempt to insert a StateID for the last state of state_data_pool
      if none is present yet. If this fails (another entry for this state
      is present), we have to remove the duplicate entry from the
      state data pool.
    */
    StateID id(state_data_pool.size() - 1);
    pair<int, bool> result = registered_states.insert(id.value);
    bool is_new_entry = result.second;
    if (!is_new_entry) {
        state_data_pool.pop_back();
    }
    assert(registered_states.size() == static_cast<int>(state_data_pool.size()));
    return StateID(result.first);
}

State StateRegistry::lookup_state(StateID id) const {
    const PackedStateBin *buffer = state_data_pool[id.value];
    vector<int> values(num_variables);
    for (int var = 0; var < num_variables; ++var) {
        values[var] = get_state_value(buffer, var);
    }
    return task_proxy.create_state(move(values), StateHandle(this, id));
}

State StateRegistry::register_state(State &&state) {
    if (state.get_registry()) {
        cerr << "Tried to register an already registered state." << endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
    if (state.get_task().get_id() != task_proxy.get_id()) {
        cerr << "Tried to register a state from an incompatible task." << endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

    PackedStateBin *buffer = state_data_pool.push_back_empty_element();
    // Avoid garbage values in half-full bins.
    fill_n(buffer, get_bins_per_state(), 0);
    for (size_t i = 0; i < state.size(); ++i) {
        state_packer.set(buffer, i, state[i].get_value());
    }

    StateID id = insert_id_or_pop_state();
    return State(move(state), StateHandle(this, id));
}

State StateRegistry::get_successor_state(
    const State &predecessor, const OperatorProxy &op) {
    assert(!op.is_axiom());
    state_data_pool.push_back(state_data_pool[predecessor.get_id().value]);
    PackedStateBin *buffer = state_data_pool[state_data_pool.size() - 1];
    vector<int> new_values = predecessor.get_values();
    for (EffectProxy effect : op.get_effects()) {
        if (does_fire(effect, predecessor)) {
            FactPair effect_pair = effect.get_fact().get_pair();
            state_packer.set(buffer, effect_pair.var, effect_pair.value);
            new_values[effect_pair.var] = effect_pair.value;
        }
    }
    // TODO: axiom_evaluator.evaluate(buffer, state_packer);
    StateID id = insert_id_or_pop_state();
    return task_proxy.create_state(move(new_values), StateHandle(this, id));
}

int StateRegistry::get_bins_per_state() const {
    return state_packer.get_num_bins();
}

int StateRegistry::get_state_size_in_bytes() const {
    return get_bins_per_state() * sizeof(PackedStateBin);
}

void StateRegistry::print_statistics() const {
    cout << "Number of registered states: " << size() << endl;
    registered_states.print_statistics();
}
