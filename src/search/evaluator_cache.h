#ifndef EVALUATOR_CACHE_H
#define EVALUATOR_CACHE_H

#include "evaluation_result.h"
#include "task_proxy.h"

#include <unordered_map>

class Evaluator;

using EvaluationResults = std::unordered_map<Evaluator *, EvaluationResult>;

/*
  Store a state and evaluation results for this state.
*/
class EvaluatorCache {
    EvaluationResults eval_results;
    State state;

public:
    explicit EvaluatorCache(State &&state);
    ~EvaluatorCache() = default;

    EvaluatorCache(EvaluatorCache &&other) = default;
    EvaluatorCache(const EvaluatorCache &other) = default;
    EvaluatorCache &operator=(EvaluatorCache &&other) = default;

    EvaluationResult &operator[](Evaluator *eval);

    const State &get_state() const;

    template<class Callback>
    void for_each_evaluator_result(const Callback &callback) const {
        for (const auto &element : eval_results) {
            const Evaluator *eval = element.first;
            const EvaluationResult &result = element.second;
            callback(eval, result);
        }
    }
};

#endif
