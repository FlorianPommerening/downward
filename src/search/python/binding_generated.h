#include "../abstract_task.h"
#include "../evaluator.h"
#include "../open_list_factory.h"
#include "../operator_cost.h"
#include "../pruning_method.h"
#include "../search_algorithm.h"

#include "../cartesian_abstractions/split_selector.h"
#include "../cartesian_abstractions/subtask_generators.h"
#include "../landmarks/landmark_cost_partitioning_heuristic.h"
#include "../landmarks/landmark_factory.h"
#include "../lp/lp_solver.h"
#include "../merge_and_shrink/label_reduction.h"
#include "../merge_and_shrink/merge_scoring_function.h"
#include "../merge_and_shrink/merge_scoring_function_total_order.h"
#include "../merge_and_shrink/merge_selector.h"
#include "../merge_and_shrink/merge_strategy_factory.h"
#include "../merge_and_shrink/merge_strategy_factory_sccs.h"
#include "../merge_and_shrink/merge_tree.h"
#include "../merge_and_shrink/merge_tree_factory.h"
#include "../merge_and_shrink/shrink_bisimulation.h"
#include "../merge_and_shrink/shrink_fh.h"
#include "../merge_and_shrink/utils.h"
#include "../operator_counting/delete_relaxation_rr_constraints.h"
#include "../pdbs/pattern_generator.h"
#include "../plugins/options.h"
#include "../plugins/plugin.h"
#include "../plugins/raw_registry.h"
#include "../plugins/registry.h"
#include "../pruning/stubborn_sets_atom_centric.h"
#include "../search_algorithms/enforced_hill_climbing_search.h"
#include "../task_utils/variable_order_finder.h"
#include "../tasks/default_value_axioms_task.h"
#include "../tasks/root_task.h"
#include "../utils/logging.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <nanobind/nanobind.h>
#pragma GCC diagnostic pop

#include <string>
#include <unordered_set>

namespace nb = nanobind;

extern void bind_feature_classes(
    nb::module_ &m, const std::unordered_set<std::string> &skip);
extern void bind_list_classes(nb::module_ &m);
extern void bind_enums(nb::module_ &m);
extern void bind_features(nb::module_ &m);
