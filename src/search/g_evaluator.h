#ifndef G_EVALUATOR_H
#define G_EVALUATOR_H

#include "scalar_evaluator.h"
#include <string>
#include <vector>

class Heuristic;

class GEvaluator : public ScalarEvaluator {
private:
    int value;
protected:
    void evaluate(int g, bool preferred);
    bool is_last_evaluated_dead_end() const;
    int get_last_evaluated_value() const;
public:
    GEvaluator();
    ~GEvaluator();

    bool dead_end_is_reliable() const;
    void get_involved_heuristics(std::set<Heuristic *> &) {}
};

#endif
