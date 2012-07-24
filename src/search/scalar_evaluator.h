#ifndef SCALAR_EVALUATOR_H
#define SCALAR_EVALUATOR_H

#include "evaluator.h"

class ScalarEvaluator : public Evaluator {
protected:
    virtual int get_last_evaluated_value() const = 0;
public:
    virtual ~ScalarEvaluator() {}

    int get_value(int evaluation_context) const {
        validate_evaluation_context(evaluation_context);
        return get_last_evaluated_value();
    }
};

#endif
