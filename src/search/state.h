#ifndef STATE_H
#define STATE_H

#include "abstract_task.h"
#include "state_handle.h"

#include "algorithms/int_packer.h"
#include "utils/hash.h"

#include <cassert>
#include <vector>

class FactProxy;
class OperatorProxy;
class TaskProxy;
class VariableProxy;

using StatePacker = int_packer::IntPacker;
using PackedStateBin = StatePacker::Bin;

class State {
    const AbstractTask *task;
    const PackedStateBin *buffer;
    const StatePacker &state_packer;
    StateHandle handle;
    bool owns_buffer;

    void sanity_check() const;
public:
    using ItemType = FactProxy;
    State(
        const AbstractTask &task,
        const PackedStateBin *buffer,
        const StatePacker &state_packer,
        StateHandle handle);
    State(const AbstractTask &task, std::vector<int> &&values);
    ~State();

    State(const State &other);
    State &operator=(State &&other);
    State(State &&other);

    bool operator==(const State &other) const {
        assert(task == other.task);
        const PackedStateBin *bin = buffer;
        const PackedStateBin *other_bin = other.buffer;
        return std::equal(bin, bin + state_packer.get_num_bins(), other_bin);
    }

    bool operator!=(const State &other) const {
        return !(*this == other);
    }


    std::size_t size() const {
        return task->get_num_variables();
    }

    FactProxy operator[](std::size_t var_id) const;
    FactProxy operator[](VariableProxy var) const;

    inline TaskProxy get_task() const;

    StateID get_id() const {
        return handle.get_id();
    }

    const StateRegistry *get_registry() const {
        return handle.get_registry();
    }

    StateHandle get_handle() const {
        return handle;
    }

    const PackedStateBin *get_buffer() const {
        return buffer;
    }

    std::vector<int> unpack() const;

    const StatePacker &get_state_packer() const {
        return state_packer;
    }

    State get_successor(const OperatorProxy &op) const;
};


namespace utils {
inline void feed(HashState &hash_state, const State &state) {
    int num_bins = state.get_state_packer().get_num_bins();
    feed(hash_state, static_cast<uint64_t>(num_bins));
    const PackedStateBin *tmp = state.get_buffer();
    for (int i = 0; i < num_bins; ++i) {
        feed(hash_state, *tmp);
        ++tmp;
    }
}
}

#endif
