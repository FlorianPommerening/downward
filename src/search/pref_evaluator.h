#ifndef PREF_EVALUATOR_H
#define PREF_EVALUATOR_H

#include "scalar_evaluator.h"

#include <string>
#include <vector>


class PrefEvaluator : public ScalarEvaluator {
private:
    bool value_preferred;
protected:
    void evaluate(int g, bool preferred);
    bool is_last_evaluated_dead_end() const;
    int get_last_evaluated_value() const;
public:
    PrefEvaluator();
    ~PrefEvaluator();

    bool dead_end_is_reliable() const;
    void get_involved_heuristics(std::set<Heuristic *> &) {return; }
};

#endif
