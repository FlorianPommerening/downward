#include "task_proxy.h"

#include "task_utils/causal_graph.h"

using namespace std;

State TaskProxy::convert_ancestor_state(const State &ancestor_state) const {
    if (task->transforms_states()) {
        TaskProxy ancestor_task_proxy = ancestor_state.get_task();
        // Create a copy of the state values for the new state.
        std::vector<int> state_values = ancestor_state.unpack();
        task->convert_state_values(state_values, ancestor_task_proxy.task);
        return {
                   *task, std::move(state_values)
        };
    } else {
        return {
                   *task, copy_buffer(ancestor_state),
                   ancestor_state.get_state_packer(), StateHandle::unregistered_state
        };
    }
}

const causal_graph::CausalGraph &TaskProxy::get_causal_graph() const {
    return causal_graph::get_causal_graph(task);
}
