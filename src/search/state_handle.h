#ifndef STATE_HANDLE_H
#define STATE_HANDLE_H

#include "state_id.h"

class StateRegistry;

class StateHandle {
    const StateRegistry *registry;
    const StateID id;
public:
    StateHandle(const StateRegistry *registry, const StateID id)
        : registry(registry), id(id) {
    }

    const StateRegistry *get_registry() const {
        return registry;
    }

    const StateID get_id() const {
        return id;
    }

    static const StateHandle unregistered_state;
};


#endif
