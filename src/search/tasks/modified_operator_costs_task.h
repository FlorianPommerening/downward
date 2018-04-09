#ifndef TASKS_MODIFIED_OPERATOR_COSTS_TASK_H
#define TASKS_MODIFIED_OPERATOR_COSTS_TASK_H

#include "delegating_task.h"

#include <vector>

namespace extra_tasks {
class ModifiedOperatorCostsTask : public tasks::DelegatingTask {
    std::vector<int> operator_costs;

public:
    ModifiedOperatorCostsTask(
        const std::shared_ptr<AbstractTask> &parent,
        const std::vector<int> &costs);
    virtual ~ModifiedOperatorCostsTask() override = default;

    virtual int get_operator_cost(int index, bool is_axiom) const override;

    std::vector<int> &get_operator_costs() {return operator_costs; }
};
}

#endif
