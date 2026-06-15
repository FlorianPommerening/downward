#ifndef PYTHON_EXCEPTION_H
#define PYTHON_EXCEPTION_H

#include <exception>
#include <string>

class ConstructionError : public std::exception {
    std::string message;
public:
    explicit ConstructionError(const std::string &message);
    const char *what() const noexcept override;
};

#endif
