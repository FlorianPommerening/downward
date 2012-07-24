#ifndef WEIGHTED_EVALUATOR_H
#define WEIGHTED_EVALUATOR_H

#include "scalar_evaluator.h"

#include <string>
#include <vector>

class Options;

class WeightedEvaluator : public ScalarEvaluator {
private:
    ScalarEvaluator *evaluator;
    int w;
    int value;
protected:
    void evaluate(int g, bool preferred);
    bool is_last_evaluated_dead_end() const;
    int get_last_evaluated_value() const;
public:
    WeightedEvaluator(const Options &opts);
    WeightedEvaluator(ScalarEvaluator *eval, int weight);
    ~WeightedEvaluator();

    bool dead_end_is_reliable() const;
    void get_involved_heuristics(std::set<Heuristic *> &hset);
};

#endif
