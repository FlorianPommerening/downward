#ifndef POTENTIALS_POTENTIAL_MAX_HEURISTIC_H
#define POTENTIALS_POTENTIAL_MAX_HEURISTIC_H

#include "../heuristic.h"

#include <memory>
#include <vector>

namespace potentials {
class PotentialFunction;

/*
  Maximize over multiple potential functions.
*/
class PotentialMaxHeuristic : public Heuristic {
    std::vector<std::unique_ptr<PotentialFunction>> functions;

protected:
    virtual int compute_heuristic(const State &ancestor_state) override;

public:
    PotentialMaxHeuristic(
        std::vector<std::unique_ptr<PotentialFunction>> &&functions,
        const std::shared_ptr<AbstractTask> &transform, bool cache_estimates,
        const std::string &description, utils::Verbosity verbosity);
};
}

#endif
