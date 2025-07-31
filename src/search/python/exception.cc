#include "exception.h"

using namespace std;

ConstructionError::ConstructionError(const std::string &message)
    : message(message) {
}

const char *ConstructionError::what() const noexcept {
    return message.c_str();
}
