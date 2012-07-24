#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <set>
#include <iostream>

class Heuristic;
class Operator;
class State;

struct InvalidEvaluationContextError {
    InvalidEvaluationContextError() : msg("") {}
    InvalidEvaluationContextError(std::string m) : msg(m) {}

    std::string msg;

    friend std::ostream &operator<<(std::ostream &out, const InvalidEvaluationContextError &e) {
        out << "Invalid evaluation context: " << e.msg << std::endl;
        return out;
    }
};


class Evaluator {
protected:
    int current_evaluation_context;

    void start_evaluation_context(int evaluation_context) {
        current_evaluation_context = evaluation_context;
    }

    void validate_evaluation_context(int evaluation_context) const {
        if (evaluation_context != current_evaluation_context) {
            // TODO add information about the origin of the error
            throw InvalidEvaluationContextError();
        }
    }

    virtual void evaluate(int g, bool preferred) = 0;
    virtual bool is_last_evaluated_dead_end() const = 0;
public:
    virtual ~Evaluator() {}

    void evaluate(int evaluation_context, int g, bool preferred) {
        start_evaluation_context(evaluation_context);
        return evaluate(g, preferred);
    }

    bool is_dead_end(int evaluation_context) const {
        validate_evaluation_context(evaluation_context);
        return is_last_evaluated_dead_end();
    }
    virtual bool dead_end_is_reliable() const = 0;
    virtual void get_involved_heuristics(std::set<Heuristic *> &hset) = 0;
};

#endif
