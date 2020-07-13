#ifndef OPERATOR_COUNTING_STATE_EQUATION_CONSTRAINTS_H
#define OPERATOR_COUNTING_STATE_EQUATION_CONSTRAINTS_H

#include "constraint_generator.h"

#include <set>

class TaskProxy;

namespace lp {
class LPConstraint;
}

namespace operator_counting {
class StateEquationConstraints : public OperatorCountingConstraint {
    std::vector<std::vector<int>> constraint_indices;
    // Map goal variables to their goal value and other variables to max int.
    std::vector<int> goal_state;

public:
    StateEquationConstraints(const options::Options &options,
                             const std::shared_ptr<AbstractTask> &task);
    virtual void initialize_constraints(std::vector<lp::LPConstraint> &constraints,
                                        double infinity);
    virtual bool update_constraints(const State &state, lp::LPSolver &lp_solver);
};
}

#endif
