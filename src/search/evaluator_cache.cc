#include "evaluator_cache.h"

using namespace std;


EvaluatorCache::EvaluatorCache(State &&state)
    : state(move(state)) {
}

EvaluationResult &EvaluatorCache::operator[](Evaluator *eval) {
    return eval_results[eval];
}

const State &EvaluatorCache::get_state() const {
    return state;
}
