#include "state_handle.h"

#include "state_registry.h"

using namespace std;

const StateHandle StateHandle::unregistered_state = StateHandle(nullptr, StateID::no_state);
