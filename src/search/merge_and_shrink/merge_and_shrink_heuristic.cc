#include "merge_and_shrink_heuristic.h"

#include "distances.h"
#include "factored_transition_system.h"
#include "merge_and_shrink_algorithm.h"
#include "merge_and_shrink_representation.h"
#include "transition_system.h"
#include "types.h"

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"
#include "../utils/markup.h"
#include "../utils/system.h"

#include <cassert>
#include <iostream>
#include <utility>

using namespace std;
using utils::ExitCode;

namespace merge_and_shrink {
MergeAndShrinkHeuristic::MergeAndShrinkHeuristic(
    const shared_ptr<MergeStrategyFactory> &merge_strategy,
    const shared_ptr<ShrinkStrategy> &shrink_strategy,
    const shared_ptr<LabelReduction> &label_reduction,
    bool prune_unreachable_states, bool prune_irrelevant_states, int max_states,
    int max_states_before_merge, int threshold_before_merge,
    double main_loop_max_time, const shared_ptr<AbstractTask> &transform,
    bool cache_estimates, const string &description, utils::Verbosity verbosity)
    : Heuristic(transform, cache_estimates, description, verbosity) {
    log << "Initializing merge-and-shrink heuristic..." << endl;
    MergeAndShrinkAlgorithm algorithm(
        merge_strategy, shrink_strategy, label_reduction,
        prune_unreachable_states, prune_irrelevant_states, max_states,
        max_states_before_merge, threshold_before_merge, main_loop_max_time,
        verbosity);
    FactoredTransitionSystem fts =
        algorithm.build_factored_transition_system(task_proxy);
    extract_factors(fts);
    log << "Done initializing merge-and-shrink heuristic." << endl << endl;
}

void MergeAndShrinkHeuristic::extract_factor(
    FactoredTransitionSystem &fts, int index) {
    /*
      Extract the factor at the given index from the given factored transition
      system, compute goal distances if necessary and store the M&S
      representation, which serves as the heuristic.
    */
    auto final_entry = fts.extract_factor(index);
    unique_ptr<MergeAndShrinkRepresentation> mas_representation =
        move(final_entry.first);
    unique_ptr<Distances> distances = move(final_entry.second);
    if (!distances->are_goal_distances_computed()) {
        const bool compute_init = false;
        const bool compute_goal = true;
        distances->compute_distances(compute_init, compute_goal, log);
    }
    assert(distances->are_goal_distances_computed());
    mas_representation->set_distances(*distances);
    mas_representations.push_back(move(mas_representation));
}

bool MergeAndShrinkHeuristic::extract_unsolvable_factor(
    FactoredTransitionSystem &fts) {
    /* Check if there is an unsolvable factor. If so, extract and store it and
       return true. Otherwise, return false. */
    for (int index : fts) {
        if (!fts.is_factor_solvable(index)) {
            mas_representations.reserve(1);
            extract_factor(fts, index);
            if (log.is_at_least_normal()) {
                log << fts.get_transition_system(index).tag()
                    << "use this unsolvable factor as heuristic." << endl;
            }
            return true;
        }
    }
    return false;
}

void MergeAndShrinkHeuristic::extract_nontrivial_factors(
    FactoredTransitionSystem &fts) {
    // Iterate over remaining factors and extract and store the nontrivial ones.
    for (int index : fts) {
        if (fts.is_factor_trivial(index)) {
            if (log.is_at_least_verbose()) {
                log << fts.get_transition_system(index).tag() << "is trivial."
                    << endl;
            }
        } else {
            extract_factor(fts, index);
        }
    }
}

void MergeAndShrinkHeuristic::extract_factors(FactoredTransitionSystem &fts) {
    /*
      TODO: This method has quite a bit of fiddling with aspects of
      transition systems and the merge-and-shrink representation (checking
      whether distances have been computed; computing them) that we would
      like to have at a lower level. See also the TODO in
      factored_transition_system.h on improving the interface of that class
      (and also related classes like TransitionSystem etc).
    */
    assert(mas_representations.empty());

    int num_active_factors = fts.get_num_active_entries();
    if (log.is_at_least_normal()) {
        log << "Number of remaining factors: " << num_active_factors << endl;
    }

    bool unsolvalbe = extract_unsolvable_factor(fts);
    if (!unsolvalbe) {
        extract_nontrivial_factors(fts);
    }

    int num_factors_kept = mas_representations.size();
    if (log.is_at_least_normal()) {
        log << "Number of factors kept: " << num_factors_kept << endl;
    }
}

int MergeAndShrinkHeuristic::compute_heuristic(const State &ancestor_state) {
    State state = convert_ancestor_state(ancestor_state);
    int heuristic = 0;
    for (const unique_ptr<MergeAndShrinkRepresentation> &mas_representation :
         mas_representations) {
        int cost = mas_representation->get_value(state);
        if (cost == PRUNED_STATE || cost == INF) {
            // If state is unreachable or irrelevant, we encountered a dead end.
            return DEAD_END;
        }
        heuristic = max(heuristic, cost);
    }
    return heuristic;
}

class MergeAndShrinkHeuristicFeature
    : public plugins::TypedFeature<Evaluator, MergeAndShrinkHeuristic> {
public:
    MergeAndShrinkHeuristicFeature() : TypedFeature("merge_and_shrink") {
        document_title("Merge-and-shrink heuristic");
        document_synopsis(
            "This heuristic implements the algorithm described in the following "
            "paper:" +
            utils::format_conference_reference(
                {"Silvan Sievers", "Martin Wehrle", "Malte Helmert"},
                "Generalized Label Reduction for Merge-and-Shrink Heuristics",
                "https://ai.dmi.unibas.ch/papers/sievers-et-al-aaai2014.pdf",
                "Proceedings of the 28th AAAI Conference on Artificial"
                " Intelligence (AAAI 2014)",
                "2358-2366", "AAAI Press", "2014") +
            "\n" +
            "For a more exhaustive description of merge-and-shrink, see the journal "
            "paper" +
            utils::format_journal_reference(
                {"Silvan Sievers", "Malte Helmert"},
                "Merge-and-Shrink: A Compositional Theory of Transformations "
                "of Factored Transition Systems",
                "https://ai.dmi.unibas.ch/papers/sievers-helmert-jair2021.pdf",
                "Journal of Artificial Intelligence Research", "71", "781-883",
                "2021") +
            "\n" +
            "The following paper describes how to improve the DFP merge strategy "
            "with tie-breaking, and presents two new merge strategies (dyn-MIASM "
            "and SCC-DFP):" +
            utils::format_conference_reference(
                {"Silvan Sievers", "Martin Wehrle", "Malte Helmert"},
                "An Analysis of Merge Strategies for Merge-and-Shrink Heuristics",
                "https://ai.dmi.unibas.ch/papers/sievers-et-al-icaps2016.pdf",
                "Proceedings of the 26th International Conference on Automated "
                "Planning and Scheduling (ICAPS 2016)",
                "294-298", "AAAI Press", "2016") +
            "\n" +
            "Details of the algorithms and the implementation are described in the "
            "paper" +
            utils::format_conference_reference(
                {"Silvan Sievers"},
                "Merge-and-Shrink Heuristics for Classical Planning: Efficient "
                "Implementation and Partial Abstractions",
                "https://ai.dmi.unibas.ch/papers/sievers-socs2018.pdf",
                "Proceedings of the 11th Annual Symposium on Combinatorial Search "
                "(SoCS 2018)",
                "90-98", "AAAI Press", "2018"));

        add_merge_and_shrink_algorithm_options_to_feature(*this);
        add_heuristic_options_to_feature(*this, "merge_and_shrink");

        document_note(
            "Note",
            "Conditional effects are supported directly. Note, however, that "
            "for tasks that are not factored (in the sense of the JACM 2014 "
            "merge-and-shrink paper), the atomic transition systems on which "
            "merge-and-shrink heuristics are based are nondeterministic, "
            "which can lead to poor heuristics even when only perfect shrinking "
            "is performed.");
        document_note(
            "Note",
            "When pruning unreachable states, admissibility and consistency is "
            "only guaranteed for reachable states and transitions between "
            "reachable states. While this does not impact regular A* search which "
            "will never encounter any unreachable state, it impacts techniques "
            "like symmetry-based pruning: a reachable state which is mapped to an "
            "unreachable symmetric state (which hence is pruned) would falsely be "
            "considered a dead-end and also be pruned, thus violating optimality "
            "of the search.");
        document_note(
            "Note",
            "When using a time limit on the main loop of the merge-and-shrink "
            "algorithm, the heuristic will compute the maximum over all heuristics "
            "induced by the remaining factors if terminating the merge-and-shrink "
            "algorithm early. Exception: if there is an unsolvable factor, it will "
            "be used as the exclusive heuristic since the problem is unsolvable.");
        document_note(
            "Note",
            "A currently recommended good configuration uses bisimulation "
            "based shrinking, the merge strategy SCC-DFP, and the appropriate "
            "label reduction setting (max_states has been altered to be between "
            "10k and 200k in the literature). As merge-and-shrink heuristics "
            "can be expensive to compute, we also recommend limiting time by "
            "setting {{{main_loop_max_time}}} to a finite value. A sensible "
            "value would be half of the time allocated for the planner.\n"
            "{{{\nmerge_and_shrink(shrink_strategy=shrink_bisimulation(greedy=false),"
            "merge_strategy=merge_sccs(order_of_sccs=topological,merge_selector="
            "score_based_filtering(scoring_functions=[goal_relevance(),dfp(),"
            "total_order()])),label_reduction=exact(before_shrinking=true,"
            "before_merging=false),max_states=50k,threshold_before_merge=1)\n}}}\n");

        document_language_support("action costs", "supported");
        document_language_support(
            "conditional effects", "supported (but see note)");
        document_language_support("axioms", "not supported");

        document_property("admissible", "yes (but see note)");
        document_property("consistent", "yes (but see note)");
        document_property("safe", "yes");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<MergeAndShrinkHeuristic> create_component(
        const plugins::Options &opts) const override {
        return plugins::make_shared_from_arg_tuples<MergeAndShrinkHeuristic>(
            get_merge_and_shrink_algorithm_arguments_from_options(opts),
            get_heuristic_arguments_from_options(opts));
    }
};

static plugins::FeaturePlugin<MergeAndShrinkHeuristicFeature> _plugin;
}
