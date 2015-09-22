#! /bin/bash
set -e
set -x

rm -rf builds/cmake
mkdir -p builds/cmake/bin
mkdir -p builds/cmake/.obj
mkdir -p builds/cmake/.obj/{landmarks,pdbs,merge_and_shrink,open_lists,potentials}

cd builds/cmake

SOURCES="
abstract_task
additive_heuristic
axioms
blind_search_heuristic
causal_graph
cea_heuristic
cg_cache
cg_heuristic
combining_evaluator
cost_adapted_task
countdown_timer
delegating_task
domain_transition_graph
eager_search
enforced_hill_climbing_search
equivalence_relation
evaluation_context
evaluation_result
exact_timer
ff_heuristic
g_evaluator
global_operator
global_state
globals
goal_count_heuristic
heuristic
heuristic_cache
hm_heuristic
int_packer
ipc_max_heuristic
iterated_search
landmarks/exploration
landmarks/h_m_landmarks
landmarks/lama_ff_synergy
landmarks/landmark_cost_assignment
landmarks/landmark_count_heuristic
landmarks/landmark_factory
landmarks/landmark_factory_rpg_exhaust
landmarks/landmark_factory_rpg_sasp
landmarks/landmark_factory_zhu_givan
landmarks/landmark_graph
landmarks/landmark_graph_merged
landmarks/landmark_status_manager
landmarks/util
lazy_search
lm_cut_heuristic
lp_internals
lp_solver
max_evaluator
max_heuristic
merge_and_shrink/distances
merge_and_shrink/factored_transition_system
merge_and_shrink/fts_factory
merge_and_shrink/heuristic_representation
merge_and_shrink/label_equivalence_relation
merge_and_shrink/labels
merge_and_shrink/merge_and_shrink_heuristic
merge_and_shrink/merge_dfp
merge_and_shrink/merge_linear
merge_and_shrink/merge_strategy
merge_and_shrink/shrink_bisimulation
merge_and_shrink/shrink_bucket_based
merge_and_shrink/shrink_fh
merge_and_shrink/shrink_random
merge_and_shrink/shrink_strategy
merge_and_shrink/transition_system
open_lists/alternation_open_list
open_lists/bucket_open_list
open_lists/open_list
open_lists/pareto_open_list
open_lists/standard_scalar_open_list
open_lists/tiebreaking_open_list
operator_cost
option_parser
option_parser_util
pdbs/canonical_pdbs_heuristic
pdbs/dominance_pruner
pdbs/match_tree
pdbs/max_cliques
pdbs/pattern_database
pdbs/pattern_generation_edelkamp
pdbs/pattern_generation_haslum
pdbs/pdb_heuristic
pdbs/util
pdbs/zero_one_pdbs_heuristic
per_state_information
planner
plugin
potentials/diverse_potential_heuristics
potentials/potential_function
potentials/potential_heuristic
potentials/potential_max_heuristic
potentials/potential_optimizer
potentials/sample_based_potential_heuristics
potentials/single_potential_heuristics
potentials/util
pref_evaluator
priority_queue
relaxation_heuristic
rng
root_task
sampling
scalar_evaluator
search_engine
search_node_info
search_progress
search_space
search_statistics
segmented_vector
state_id
state_registry
successor_generator
sum_evaluator
task_proxy
task_tools
timer
tracer
utilities
utilities_hash
utilities_windows
variable_order_finder
weighted_evaluator
"

for src in $SOURCES; do
    g++ -m32 -std=c++11 -g -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -I../../src/search/ext -o .obj/$src.o -c ../../src/search/$src.cc
done

g++ -m32 -std=c++11 -g -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer   -m32 -g .obj/planner.o .obj/abstract_task.o .obj/axioms.o .obj/causal_graph.o .obj/cost_adapted_task.o .obj/countdown_timer.o .obj/delegating_task.o .obj/domain_transition_graph.o .obj/equivalence_relation.o .obj/evaluation_context.o .obj/evaluation_result.o .obj/exact_timer.o .obj/global_operator.o .obj/globals.o .obj/global_state.o .obj/heuristic_cache.o .obj/heuristic.o .obj/int_packer.o .obj/operator_cost.o .obj/option_parser.o .obj/option_parser_util.o .obj/per_state_information.o .obj/plugin.o .obj/priority_queue.o .obj/rng.o .obj/root_task.o .obj/sampling.o .obj/scalar_evaluator.o .obj/search_engine.o .obj/search_node_info.o .obj/search_progress.o .obj/search_space.o .obj/search_statistics.o .obj/segmented_vector.o .obj/state_id.o .obj/state_registry.o .obj/successor_generator.o .obj/task_proxy.o .obj/task_tools.o .obj/timer.o .obj/tracer.o .obj/utilities.o .obj/utilities_hash.o .obj/utilities_windows.o .obj/variable_order_finder.o .obj/open_lists/alternation_open_list.o .obj/open_lists/bucket_open_list.o .obj/open_lists/open_list.o .obj/open_lists/pareto_open_list.o .obj/open_lists/standard_scalar_open_list.o .obj/open_lists/tiebreaking_open_list.o .obj/g_evaluator.o .obj/combining_evaluator.o .obj/max_evaluator.o .obj/pref_evaluator.o .obj/weighted_evaluator.o .obj/sum_evaluator.o .obj/eager_search.o .obj/lazy_search.o .obj/enforced_hill_climbing_search.o .obj/iterated_search.o .obj/lp_internals.o .obj/lp_solver.o .obj/relaxation_heuristic.o .obj/ipc_max_heuristic.o .obj/additive_heuristic.o .obj/blind_search_heuristic.o .obj/cea_heuristic.o .obj/cg_heuristic.o .obj/cg_cache.o .obj/ff_heuristic.o .obj/goal_count_heuristic.o .obj/hm_heuristic.o .obj/lm_cut_heuristic.o .obj/max_heuristic.o .obj/merge_and_shrink/distances.o .obj/merge_and_shrink/factored_transition_system.o .obj/merge_and_shrink/fts_factory.o .obj/merge_and_shrink/heuristic_representation.o .obj/merge_and_shrink/label_equivalence_relation.o .obj/merge_and_shrink/labels.o .obj/merge_and_shrink/merge_and_shrink_heuristic.o .obj/merge_and_shrink/merge_dfp.o .obj/merge_and_shrink/merge_linear.o .obj/merge_and_shrink/merge_strategy.o .obj/merge_and_shrink/shrink_bisimulation.o .obj/merge_and_shrink/shrink_bucket_based.o .obj/merge_and_shrink/shrink_fh.o .obj/merge_and_shrink/shrink_random.o .obj/merge_and_shrink/shrink_strategy.o .obj/merge_and_shrink/transition_system.o .obj/landmarks/exploration.o .obj/landmarks/h_m_landmarks.o .obj/landmarks/lama_ff_synergy.o .obj/landmarks/landmark_cost_assignment.o .obj/landmarks/landmark_count_heuristic.o .obj/landmarks/landmark_factory.o .obj/landmarks/landmark_factory_rpg_exhaust.o .obj/landmarks/landmark_factory_rpg_sasp.o .obj/landmarks/landmark_factory_zhu_givan.o .obj/landmarks/landmark_graph.o .obj/landmarks/landmark_graph_merged.o .obj/landmarks/landmark_status_manager.o .obj/landmarks/util.o .obj/pdbs/canonical_pdbs_heuristic.o .obj/pdbs/dominance_pruner.o .obj/pdbs/match_tree.o .obj/pdbs/max_cliques.o .obj/pdbs/pattern_database.o .obj/pdbs/pattern_generation_edelkamp.o .obj/pdbs/pattern_generation_haslum.o .obj/pdbs/pdb_heuristic.o .obj/pdbs/util.o .obj/pdbs/zero_one_pdbs_heuristic.o .obj/potentials/diverse_potential_heuristics.o .obj/potentials/potential_function.o .obj/potentials/potential_heuristic.o .obj/potentials/potential_max_heuristic.o .obj/potentials/potential_optimizer.o .obj/potentials/sample_based_potential_heuristics.o .obj/potentials/single_potential_heuristics.o .obj/potentials/util.o  -o bin/downward -rdynamic -lrt

cd ../..
