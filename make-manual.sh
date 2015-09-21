#! /bin/bash
set -e
set -x

rm -rf builds/make

cd src/preprocess
rm -rf .obj preprocess
mkdir .obj

PREPROCESS_SOURCES="
planner
causal_graph
axiom
domain_transition_graph
helper_functions
max_dag
mutex_group
operator
scc
state
successor_generator
variable
"

for src in $PREPROCESS_SOURCES; do
    g++ -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c $src.cc -o .obj/$src.o
done

g++  -m32 -g  -static -static-libgcc .obj/planner.o .obj/axiom.o .obj/causal_graph.o .obj/domain_transition_graph.o .obj/helper_functions.o .obj/max_dag.o .obj/mutex_group.o .obj/operator.o .obj/scc.o .obj/state.o .obj/successor_generator.o .obj/variable.o   -Wl,-Bstatic -lrt -o preprocess
cd ../..

cd src/search
rm -rf .obj downward-release
mkdir .obj
mkdir .obj/{landmarks,pdbs,merge_and_shrink,open_lists,potentials}

SOURCES="
planner
abstract_task
axioms
causal_graph
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
g_evaluator
global_operator
global_state
globals
heuristic_cache
heuristic
int_packer
ipc_max_heuristic
iterated_search
lazy_search
max_evaluator
operator_cost
option_parser
option_parser_util
segmented_vector
per_state_information
pref_evaluator
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
variable_order_finder
weighted_evaluator
open_lists/alternation_open_list
open_lists/bucket_open_list
open_lists/open_list
open_lists/pareto_open_list
open_lists/standard_scalar_open_list
open_lists/tiebreaking_open_list
lp_internals
lp_solver
plugin
priority_queue
utilities_windows
additive_heuristic
blind_search_heuristic
cea_heuristic
cg_heuristic
cg_cache
ff_heuristic
goal_count_heuristic
hm_heuristic
lm_cut_heuristic
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
landmarks/exploration
landmarks/h_m_landmarks
landmarks/lama_ff_synergy
landmarks/landmark_cost_assignment
landmarks/landmark_count_heuristic
landmarks/landmark_status_manager
landmarks/landmark_graph_merged
landmarks/landmark_graph
landmarks/landmark_factory
landmarks/landmark_factory_rpg_exhaust
landmarks/landmark_factory_rpg_sasp
landmarks/landmark_factory_zhu_givan
landmarks/util
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
potentials/diverse_potential_heuristics
potentials/potential_function
potentials/potential_heuristic
potentials/potential_max_heuristic
potentials/potential_optimizer
potentials/sample_based_potential_heuristics
potentials/single_potential_heuristics
potentials/util
"

for src in $SOURCES; do
    g++ -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c $src.cc -o .obj/$src.release.o
done

g++  -m32 -g -rdynamic  .obj/planner.release.o .obj/abstract_task.release.o .obj/axioms.release.o .obj/causal_graph.release.o .obj/combining_evaluator.release.o .obj/cost_adapted_task.release.o .obj/countdown_timer.release.o .obj/delegating_task.release.o .obj/domain_transition_graph.release.o .obj/eager_search.release.o .obj/enforced_hill_climbing_search.release.o .obj/equivalence_relation.release.o .obj/evaluation_context.release.o .obj/evaluation_result.release.o .obj/exact_timer.release.o .obj/g_evaluator.release.o .obj/global_operator.release.o .obj/global_state.release.o .obj/globals.release.o .obj/heuristic_cache.release.o .obj/heuristic.release.o .obj/int_packer.release.o .obj/ipc_max_heuristic.release.o .obj/iterated_search.release.o .obj/lazy_search.release.o .obj/max_evaluator.release.o .obj/operator_cost.release.o .obj/option_parser.release.o .obj/option_parser_util.release.o .obj/segmented_vector.release.o .obj/per_state_information.release.o .obj/pref_evaluator.release.o .obj/relaxation_heuristic.release.o .obj/rng.release.o .obj/root_task.release.o .obj/sampling.release.o .obj/scalar_evaluator.release.o .obj/search_engine.release.o .obj/search_node_info.release.o .obj/search_progress.release.o .obj/search_space.release.o .obj/search_statistics.release.o .obj/state_id.release.o .obj/state_registry.release.o .obj/successor_generator.release.o .obj/sum_evaluator.release.o .obj/task_proxy.release.o .obj/task_tools.release.o .obj/timer.release.o .obj/tracer.release.o .obj/utilities.release.o .obj/utilities_hash.release.o .obj/variable_order_finder.release.o .obj/weighted_evaluator.release.o .obj/open_lists/alternation_open_list.release.o .obj/open_lists/bucket_open_list.release.o .obj/open_lists/open_list.release.o .obj/open_lists/pareto_open_list.release.o .obj/open_lists/standard_scalar_open_list.release.o .obj/open_lists/tiebreaking_open_list.release.o .obj/lp_internals.release.o .obj/lp_solver.release.o .obj/plugin.release.o .obj/priority_queue.release.o .obj/utilities_windows.release.o .obj/additive_heuristic.release.o .obj/blind_search_heuristic.release.o .obj/cea_heuristic.release.o .obj/cg_heuristic.release.o .obj/cg_cache.release.o .obj/ff_heuristic.release.o .obj/goal_count_heuristic.release.o .obj/hm_heuristic.release.o .obj/lm_cut_heuristic.release.o .obj/max_heuristic.release.o .obj/merge_and_shrink/distances.release.o .obj/merge_and_shrink/factored_transition_system.release.o .obj/merge_and_shrink/fts_factory.release.o .obj/merge_and_shrink/heuristic_representation.release.o .obj/merge_and_shrink/label_equivalence_relation.release.o .obj/merge_and_shrink/labels.release.o .obj/merge_and_shrink/merge_and_shrink_heuristic.release.o .obj/merge_and_shrink/merge_dfp.release.o .obj/merge_and_shrink/merge_linear.release.o .obj/merge_and_shrink/merge_strategy.release.o .obj/merge_and_shrink/shrink_bisimulation.release.o .obj/merge_and_shrink/shrink_bucket_based.release.o .obj/merge_and_shrink/shrink_fh.release.o .obj/merge_and_shrink/shrink_random.release.o .obj/merge_and_shrink/shrink_strategy.release.o .obj/merge_and_shrink/transition_system.release.o .obj/landmarks/exploration.release.o .obj/landmarks/h_m_landmarks.release.o .obj/landmarks/lama_ff_synergy.release.o .obj/landmarks/landmark_cost_assignment.release.o .obj/landmarks/landmark_count_heuristic.release.o .obj/landmarks/landmark_status_manager.release.o .obj/landmarks/landmark_graph_merged.release.o .obj/landmarks/landmark_graph.release.o .obj/landmarks/landmark_factory.release.o .obj/landmarks/landmark_factory_rpg_exhaust.release.o .obj/landmarks/landmark_factory_rpg_sasp.release.o .obj/landmarks/landmark_factory_zhu_givan.release.o .obj/landmarks/util.release.o .obj/pdbs/canonical_pdbs_heuristic.release.o .obj/pdbs/dominance_pruner.release.o .obj/pdbs/match_tree.release.o .obj/pdbs/max_cliques.release.o .obj/pdbs/pattern_database.release.o .obj/pdbs/pattern_generation_edelkamp.release.o .obj/pdbs/pattern_generation_haslum.release.o .obj/pdbs/pdb_heuristic.release.o .obj/pdbs/util.release.o .obj/pdbs/zero_one_pdbs_heuristic.release.o .obj/potentials/diverse_potential_heuristics.release.o .obj/potentials/potential_function.release.o .obj/potentials/potential_heuristic.release.o .obj/potentials/potential_max_heuristic.release.o .obj/potentials/potential_optimizer.release.o .obj/potentials/sample_based_potential_heuristics.release.o .obj/potentials/single_potential_heuristics.release.o .obj/potentials/util.release.o   -lrt -o downward-release
cd ../..

mkdir -p builds/make/bin
cp -r src/translate builds/make/bin/
cp src/preprocess/preprocess builds/make/bin/
cp src/search/downward-release builds/make/bin/downward
