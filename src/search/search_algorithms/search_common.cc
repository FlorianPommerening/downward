#include "search_common.h"

#include "../open_list_factory.h"

#include "../evaluators/g_evaluator.h"
#include "../evaluators/sum_evaluator.h"
#include "../evaluators/weighted_evaluator.h"
#include "../plugins/options.h"
#include "../open_lists/alternation_open_list.h"
#include "../open_lists/best_first_open_list.h"
#include "../open_lists/tiebreaking_open_list.h"

#include "../tasks/root_task.h"
#include "../state_registry.h"

#include <memory>

using namespace std;

namespace search_common {
using GEval = g_evaluator::GEvaluator;
using SumEval = sum_evaluator::SumEvaluator;
using WeightedEval = weighted_evaluator::WeightedEvaluator;

shared_ptr<OpenListFactory> create_standard_scalar_open_list_factory(
    const shared_ptr<Evaluator> &eval, bool pref_only) {
    plugins::Options options;
    options.set("eval", eval);
    options.set("pref_only", pref_only);
    return make_shared<standard_scalar_open_list::BestFirstOpenListFactory>(options);
}

static shared_ptr<OpenListFactory> create_alternation_open_list_factory(
    const vector<shared_ptr<OpenListFactory>> &subfactories, int boost) {
    plugins::Options options;
    options.set("sublists", subfactories);
    options.set("boost", boost);
    return make_shared<alternation_open_list::AlternationOpenListFactory>(options);
}

/*
  Helper function for common code of create_greedy_open_list_factory
  and create_wastar_open_list_factory.
*/
static shared_ptr<OpenListFactory> create_alternation_open_list_factory_aux(
    const vector<shared_ptr<Evaluator>> &evals,
    const vector<shared_ptr<Evaluator>> &preferred_evaluators,
    int boost) {
    if (evals.size() == 1 && preferred_evaluators.empty()) {
        return create_standard_scalar_open_list_factory(evals[0], false);
    } else {
        vector<shared_ptr<OpenListFactory>> subfactories;
        for (const shared_ptr<Evaluator> &evaluator : evals) {
            subfactories.push_back(
                create_standard_scalar_open_list_factory(
                    evaluator, false));
            if (!preferred_evaluators.empty()) {
                subfactories.push_back(
                    create_standard_scalar_open_list_factory(
                        evaluator, true));
            }
        }
        return create_alternation_open_list_factory(subfactories, boost);
    }
}

shared_ptr<OpenListFactory> create_greedy_open_list_factory(
    const plugins::Options &options) {
    return create_alternation_open_list_factory_aux(
        options.get_list<shared_ptr<Evaluator>>("evals"),
        options.get_list<shared_ptr<Evaluator>>("preferred"),
        options.get<int>("boost"));
}

/*
  Helper function for creating a single g + w * h evaluator
  for weighted A*-style search.

  If w = 1, we do not introduce an unnecessary weighted evaluator:
  we use g + h instead of g + 1 * h.

  If w = 0, we omit the h-evaluator altogether:
  we use g instead of g + 0 * h.
*/
static shared_ptr<Evaluator> create_wastar_eval(const plugins::Options &options,
                                                const shared_ptr<GEval> &g_eval, int w,
                                                const shared_ptr<Evaluator> &h_eval) {
    if (w == 0) {
        return g_eval;
    }
    shared_ptr<Evaluator> w_h_eval = nullptr;
    if (w == 1) {
        w_h_eval = h_eval;
    } else {
        plugins::Options weighted_evaluator_options;
        weighted_evaluator_options.set<utils::Verbosity>(
            "verbosity", options.get<utils::Verbosity>("verbosity"));
        weighted_evaluator_options.set<shared_ptr<Evaluator>>("eval", h_eval);
        weighted_evaluator_options.set<int>("weight", w);
        w_h_eval = make_shared<WeightedEval>(weighted_evaluator_options);
    }
    plugins::Options sum_evaluator_options;
    sum_evaluator_options.set<utils::Verbosity>(
        "verbosity", options.get<utils::Verbosity>("verbosity"));
    sum_evaluator_options.set<vector<shared_ptr<Evaluator>>>(
        "evals", vector<shared_ptr<Evaluator>>({g_eval, w_h_eval}));
    return make_shared<SumEval>(sum_evaluator_options);
}

shared_ptr<OpenListFactory> create_wastar_open_list_factory(
    const plugins::Options &options) {
    vector<shared_ptr<Evaluator>> base_evals =
        options.get_list<shared_ptr<Evaluator>>("evals");
    int w = options.get<int>("w");

    plugins::Options g_evaluator_options;
    g_evaluator_options.set<utils::Verbosity>(
        "verbosity", options.get<utils::Verbosity>("verbosity"));
    shared_ptr<GEval> g_eval = make_shared<GEval>(g_evaluator_options);
    vector<shared_ptr<Evaluator>> f_evals;
    f_evals.reserve(base_evals.size());
    for (const shared_ptr<Evaluator> &eval : base_evals)
        f_evals.push_back(create_wastar_eval(options, g_eval, w, eval));

    return create_alternation_open_list_factory_aux(
        f_evals,
        options.get_list<shared_ptr<Evaluator>>("preferred"),
        options.get<int>("boost"));
}

pair<shared_ptr<OpenListFactory>, const shared_ptr<Evaluator>>
create_astar_open_list_factory_and_f_eval(const plugins::Options &opts) {
    plugins::Options g_evaluator_options;
    g_evaluator_options.set<utils::Verbosity>(
        "verbosity", opts.get<utils::Verbosity>("verbosity"));
    shared_ptr<GEval> g = make_shared<GEval>(g_evaluator_options);
    shared_ptr<Evaluator> h = opts.get<shared_ptr<Evaluator>>("eval");
    plugins::Options f_evaluator_options;
    f_evaluator_options.set<utils::Verbosity>(
        "verbosity", opts.get<utils::Verbosity>("verbosity"));
    f_evaluator_options.set<vector<shared_ptr<Evaluator>>>(
        "evals", vector<shared_ptr<Evaluator>>({g, h}));
    shared_ptr<Evaluator> f = make_shared<SumEval>(f_evaluator_options);
    vector<shared_ptr<Evaluator>> evals = {f, h};

    plugins::Options options;
    options.set("evals", evals);
    options.set("pref_only", false);
    options.set("unsafe_pruning", false);
    shared_ptr<OpenListFactory> open =
        make_shared<tiebreaking_open_list::TieBreakingOpenListFactory>(options);
    return make_pair(open, f);
}

static double rescale_weight_for_speedstar(
    double s, const shared_ptr<Evaluator> &h, const shared_ptr<Evaluator> &d,
    const TaskProxy &task_proxy) {
    StateRegistry temporary_registry(task_proxy);
    EvaluationContext evaluation_context(temporary_registry.get_initial_state());
    EvaluationResult init_h = h->compute_result(evaluation_context);
    if (init_h.is_infinite()) {
        return 0; // no need to consider d, h will detect unsolvability as soon as the search starts.
    }
    EvaluationResult init_d = d->compute_result(evaluation_context);
    if (init_d.is_infinite()) {
        return 1; // d will detect unsolvability as soon as the search starts.
    }
    if (init_d.get_evaluator_value() == 0) {
        return 0; // no need to consider d. TODO is this true?
    }
    return (s - 1) * init_h.get_evaluator_value() / init_d.get_evaluator_value();
}

pair<shared_ptr<OpenListFactory>, const shared_ptr<Evaluator>>
create_speedstar_open_list_factory_and_f_eval(const plugins::Options &opts) {
    plugins::Options g_evaluator_options;
    g_evaluator_options.set<utils::Verbosity>(
        "verbosity", opts.get<utils::Verbosity>("verbosity"));
    shared_ptr<GEval> g = make_shared<GEval>(g_evaluator_options);
    shared_ptr<Evaluator> h = opts.get<shared_ptr<Evaluator>>("eval");
    shared_ptr<Evaluator> d = opts.get<shared_ptr<Evaluator>>("d_eval");
    double s = opts.get<double>("s");
    TaskProxy task_proxy(*tasks::g_root_task); // TODO: avoid using the global variable here.
    double s_prime = rescale_weight_for_speedstar(s, h, d, task_proxy);

    plugins::Options weighted_d_evaluator_options;
    weighted_d_evaluator_options.set<utils::Verbosity>(
        "verbosity", opts.get<utils::Verbosity>("verbosity"));
    weighted_d_evaluator_options.set<shared_ptr<Evaluator>>(
        "eval", shared_ptr<Evaluator>(d));
    weighted_d_evaluator_options.set<int>( // TODO: only int weights so far, have to adapt
        "weight", static_cast<int>(s_prime));
    shared_ptr<Evaluator> weighted_d = make_shared<WeightedEval>(weighted_d_evaluator_options);

    plugins::Options f_evaluator_options;
    f_evaluator_options.set<utils::Verbosity>(
        "verbosity", opts.get<utils::Verbosity>("verbosity"));
    f_evaluator_options.set<vector<shared_ptr<Evaluator>>>(
        "evals", vector<shared_ptr<Evaluator>>({g, h, weighted_d}));
    shared_ptr<Evaluator> f = make_shared<SumEval>(f_evaluator_options);
    vector<shared_ptr<Evaluator>> evals = {f, d, h}; // TODO: rethink this

    plugins::Options options;
    options.set("evals", evals);
    options.set("pref_only", false);
    options.set("unsafe_pruning", false);
    shared_ptr<OpenListFactory> open =
        make_shared<tiebreaking_open_list::TieBreakingOpenListFactory>(options);
    return make_pair(open, f);
}
}
