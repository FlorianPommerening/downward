#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "evaluator.h"
#include "operator_id.h"
#include "per_state_information.h"
#include "task_proxy.h"

#include "algorithms/ordered_set.h"

#include <memory>
#include <vector>

class TaskProxy;

namespace plugins {
class Feature;
class Options;
}

class Heuristic : public Evaluator {
    struct HEntry {
        /* dirty is conceptually a bool, but Visual C++ does not support
           packing ints and bools together in a bitfield. */
        int h : 31;
        unsigned int dirty : 1;

        HEntry(int h, bool dirty) : h(h), dirty(dirty) {
        }
    };
    static_assert(sizeof(HEntry) == 4, "HEntry has unexpected size.");

    /*
      TODO: We might want to get rid of the preferred_operators
      attribute. It is currently only used by compute_result() and the
      methods it calls (compute_heuristic() directly, further methods
      indirectly), and we could e.g. change this by having
      compute_heuristic return an EvaluationResult object.

      If we do this, we should be mindful of the cost incurred by not
      being able to reuse the data structure from one iteration to the
      next, but this seems to be the only potential downside.
    */
    ordered_set::OrderedSet<OperatorID> preferred_operators;

protected:
    /*
      Cache for saving h values
      Before accessing this cache always make sure that the
      cache_evaluator_values flag is set to true - as soon as the cache is
      accessed it will create entries for all existing states
    */
    PerStateInformation<HEntry> heuristic_cache;
    bool cache_evaluator_values;

    // Hold a reference to the task implementation and pass it to objects that
    // need it.
    const std::shared_ptr<AbstractTask> task;
    // Use task_proxy to access task information.
    TaskProxy task_proxy;

    enum {
        DEAD_END = -1,
        NO_VALUE = -2
    };

    virtual int compute_heuristic(const State &ancestor_state) = 0;

    /*
      Usage note: Marking the same operator as preferred multiple times
      is OK -- it will only appear once in the list of preferred
      operators for this heuristic.
    */
    void set_preferred(const OperatorProxy &op);

    State convert_ancestor_state(const State &ancestor_state) const;

public:
    /*
      issue559
      It looks wrong that heuristic gets two task parameters but is intended:

      `task` is the task passed in when binding the component (not yet done)
            and is stored in the base class TaskSpecificComponent. Eventually
            this will be the only input. For now it is unused.

      `transform` it the argument parsed from the commandline. Eventually, we
            want to remove it but we first have to have a replacement for
            configurations that currently use it. The goal is to create a
            heuristic `CostAdaptedHeuristic` (or similar) that takes another
            heuristic and a cost type as a parameter, transforms the task and
            passes it on to the other evaluator. Once we have that, we have to
            either remove `transform` from the arguments of heuristics, or
            keep it as a deprecated feature but re-write it in the option
            parser (i.e., rewrite expressions like
               "h(transform=cost_adapted_task(type), **kwargs)"
            to
               "cost_adapted_heuristic(h(**kwargs), type)",
            ideally on the level of the AST, not on a string level.
    */
    Heuristic(
        const std::shared_ptr<AbstractTask> &task,
        const std::shared_ptr<AbstractTask> &transform, bool cache_estimates,
        const std::string &description, utils::Verbosity verbosity);

    virtual void get_path_dependent_evaluators(
        std::set<Evaluator *> & /*evals*/) override {
    }

    virtual EvaluationResult compute_result(
        EvaluationContext &eval_context) override;

    virtual bool does_cache_estimates() const override;
    virtual bool is_estimate_cached(const State &state) const override;
    virtual int get_cached_estimate(const State &state) const override;
};

extern void add_heuristic_options_to_feature(
    plugins::Feature &feature, const std::string &description);
extern std::tuple<
    std::shared_ptr<AbstractTask>, bool, std::string, utils::Verbosity>
get_heuristic_arguments_from_options(const plugins::Options &opts);
#endif
