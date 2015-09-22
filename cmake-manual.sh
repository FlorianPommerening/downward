#! /bin/bash
set -e
set -x

rm -rf builds/cmake
mkdir -p builds/cmake/bin
mkdir -p builds/cmake/preprocess/CMakeFiles/preprocess.dir
mkdir -p builds/cmake/search/CMakeFiles/downward.dir
mkdir -p builds/cmake/search/CMakeFiles/downward.dir/{landmarks,pdbs,merge_and_shrink,open_lists,potentials}

CXX=g++

cd builds/cmake
cp -r ../../src/translate bin/

cd preprocess

PREPROCESS_SOURCES="
axiom
causal_graph
domain_transition_graph
helper_functions
max_dag
mutex_group
operator
planner
scc
state
successor_generator
variable
"

for src in $PREPROCESS_SOURCES; do
    $CXX -m32 -std=c++11 -g -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer   -o CMakeFiles/preprocess.dir/$src.cc.o -c ../../../src/preprocess/$src.cc
done

$CXX -m32 -std=c++11 -g -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer   -m32 -g CMakeFiles/preprocess.dir/planner.cc.o CMakeFiles/preprocess.dir/axiom.cc.o CMakeFiles/preprocess.dir/causal_graph.cc.o CMakeFiles/preprocess.dir/domain_transition_graph.cc.o CMakeFiles/preprocess.dir/helper_functions.cc.o CMakeFiles/preprocess.dir/max_dag.cc.o CMakeFiles/preprocess.dir/mutex_group.cc.o CMakeFiles/preprocess.dir/operator.cc.o CMakeFiles/preprocess.dir/scc.cc.o CMakeFiles/preprocess.dir/state.cc.o CMakeFiles/preprocess.dir/successor_generator.cc.o CMakeFiles/preprocess.dir/variable.cc.o  -o ../bin/preprocess -rdynamic

cd ..

cd search

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
    $CXX -m32 -std=c++11 -g -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -I../../../src/search/ext -o CMakeFiles/downward.dir/$src.cc.o -c ../../../src/search/$src.cc
done

$CXX -m32 -std=c++11 -g -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer   -m32 -g CMakeFiles/downward.dir/planner.cc.o CMakeFiles/downward.dir/abstract_task.cc.o CMakeFiles/downward.dir/axioms.cc.o CMakeFiles/downward.dir/causal_graph.cc.o CMakeFiles/downward.dir/cost_adapted_task.cc.o CMakeFiles/downward.dir/countdown_timer.cc.o CMakeFiles/downward.dir/delegating_task.cc.o CMakeFiles/downward.dir/domain_transition_graph.cc.o CMakeFiles/downward.dir/equivalence_relation.cc.o CMakeFiles/downward.dir/evaluation_context.cc.o CMakeFiles/downward.dir/evaluation_result.cc.o CMakeFiles/downward.dir/exact_timer.cc.o CMakeFiles/downward.dir/global_operator.cc.o CMakeFiles/downward.dir/globals.cc.o CMakeFiles/downward.dir/global_state.cc.o CMakeFiles/downward.dir/heuristic_cache.cc.o CMakeFiles/downward.dir/heuristic.cc.o CMakeFiles/downward.dir/int_packer.cc.o CMakeFiles/downward.dir/operator_cost.cc.o CMakeFiles/downward.dir/option_parser.cc.o CMakeFiles/downward.dir/option_parser_util.cc.o CMakeFiles/downward.dir/per_state_information.cc.o CMakeFiles/downward.dir/plugin.cc.o CMakeFiles/downward.dir/priority_queue.cc.o CMakeFiles/downward.dir/rng.cc.o CMakeFiles/downward.dir/root_task.cc.o CMakeFiles/downward.dir/sampling.cc.o CMakeFiles/downward.dir/scalar_evaluator.cc.o CMakeFiles/downward.dir/search_engine.cc.o CMakeFiles/downward.dir/search_node_info.cc.o CMakeFiles/downward.dir/search_progress.cc.o CMakeFiles/downward.dir/search_space.cc.o CMakeFiles/downward.dir/search_statistics.cc.o CMakeFiles/downward.dir/segmented_vector.cc.o CMakeFiles/downward.dir/state_id.cc.o CMakeFiles/downward.dir/state_registry.cc.o CMakeFiles/downward.dir/successor_generator.cc.o CMakeFiles/downward.dir/task_proxy.cc.o CMakeFiles/downward.dir/task_tools.cc.o CMakeFiles/downward.dir/timer.cc.o CMakeFiles/downward.dir/tracer.cc.o CMakeFiles/downward.dir/utilities.cc.o CMakeFiles/downward.dir/utilities_hash.cc.o CMakeFiles/downward.dir/utilities_windows.cc.o CMakeFiles/downward.dir/variable_order_finder.cc.o CMakeFiles/downward.dir/open_lists/alternation_open_list.cc.o CMakeFiles/downward.dir/open_lists/bucket_open_list.cc.o CMakeFiles/downward.dir/open_lists/open_list.cc.o CMakeFiles/downward.dir/open_lists/pareto_open_list.cc.o CMakeFiles/downward.dir/open_lists/standard_scalar_open_list.cc.o CMakeFiles/downward.dir/open_lists/tiebreaking_open_list.cc.o CMakeFiles/downward.dir/g_evaluator.cc.o CMakeFiles/downward.dir/combining_evaluator.cc.o CMakeFiles/downward.dir/max_evaluator.cc.o CMakeFiles/downward.dir/pref_evaluator.cc.o CMakeFiles/downward.dir/weighted_evaluator.cc.o CMakeFiles/downward.dir/sum_evaluator.cc.o CMakeFiles/downward.dir/eager_search.cc.o CMakeFiles/downward.dir/lazy_search.cc.o CMakeFiles/downward.dir/enforced_hill_climbing_search.cc.o CMakeFiles/downward.dir/iterated_search.cc.o CMakeFiles/downward.dir/lp_internals.cc.o CMakeFiles/downward.dir/lp_solver.cc.o CMakeFiles/downward.dir/relaxation_heuristic.cc.o CMakeFiles/downward.dir/ipc_max_heuristic.cc.o CMakeFiles/downward.dir/additive_heuristic.cc.o CMakeFiles/downward.dir/blind_search_heuristic.cc.o CMakeFiles/downward.dir/cea_heuristic.cc.o CMakeFiles/downward.dir/cg_heuristic.cc.o CMakeFiles/downward.dir/cg_cache.cc.o CMakeFiles/downward.dir/ff_heuristic.cc.o CMakeFiles/downward.dir/goal_count_heuristic.cc.o CMakeFiles/downward.dir/hm_heuristic.cc.o CMakeFiles/downward.dir/lm_cut_heuristic.cc.o CMakeFiles/downward.dir/max_heuristic.cc.o CMakeFiles/downward.dir/merge_and_shrink/distances.cc.o CMakeFiles/downward.dir/merge_and_shrink/factored_transition_system.cc.o CMakeFiles/downward.dir/merge_and_shrink/fts_factory.cc.o CMakeFiles/downward.dir/merge_and_shrink/heuristic_representation.cc.o CMakeFiles/downward.dir/merge_and_shrink/label_equivalence_relation.cc.o CMakeFiles/downward.dir/merge_and_shrink/labels.cc.o CMakeFiles/downward.dir/merge_and_shrink/merge_and_shrink_heuristic.cc.o CMakeFiles/downward.dir/merge_and_shrink/merge_dfp.cc.o CMakeFiles/downward.dir/merge_and_shrink/merge_linear.cc.o CMakeFiles/downward.dir/merge_and_shrink/merge_strategy.cc.o CMakeFiles/downward.dir/merge_and_shrink/shrink_bisimulation.cc.o CMakeFiles/downward.dir/merge_and_shrink/shrink_bucket_based.cc.o CMakeFiles/downward.dir/merge_and_shrink/shrink_fh.cc.o CMakeFiles/downward.dir/merge_and_shrink/shrink_random.cc.o CMakeFiles/downward.dir/merge_and_shrink/shrink_strategy.cc.o CMakeFiles/downward.dir/merge_and_shrink/transition_system.cc.o CMakeFiles/downward.dir/landmarks/exploration.cc.o CMakeFiles/downward.dir/landmarks/h_m_landmarks.cc.o CMakeFiles/downward.dir/landmarks/lama_ff_synergy.cc.o CMakeFiles/downward.dir/landmarks/landmark_cost_assignment.cc.o CMakeFiles/downward.dir/landmarks/landmark_count_heuristic.cc.o CMakeFiles/downward.dir/landmarks/landmark_factory.cc.o CMakeFiles/downward.dir/landmarks/landmark_factory_rpg_exhaust.cc.o CMakeFiles/downward.dir/landmarks/landmark_factory_rpg_sasp.cc.o CMakeFiles/downward.dir/landmarks/landmark_factory_zhu_givan.cc.o CMakeFiles/downward.dir/landmarks/landmark_graph.cc.o CMakeFiles/downward.dir/landmarks/landmark_graph_merged.cc.o CMakeFiles/downward.dir/landmarks/landmark_status_manager.cc.o CMakeFiles/downward.dir/landmarks/util.cc.o CMakeFiles/downward.dir/pdbs/canonical_pdbs_heuristic.cc.o CMakeFiles/downward.dir/pdbs/dominance_pruner.cc.o CMakeFiles/downward.dir/pdbs/match_tree.cc.o CMakeFiles/downward.dir/pdbs/max_cliques.cc.o CMakeFiles/downward.dir/pdbs/pattern_database.cc.o CMakeFiles/downward.dir/pdbs/pattern_generation_edelkamp.cc.o CMakeFiles/downward.dir/pdbs/pattern_generation_haslum.cc.o CMakeFiles/downward.dir/pdbs/pdb_heuristic.cc.o CMakeFiles/downward.dir/pdbs/util.cc.o CMakeFiles/downward.dir/pdbs/zero_one_pdbs_heuristic.cc.o CMakeFiles/downward.dir/potentials/diverse_potential_heuristics.cc.o CMakeFiles/downward.dir/potentials/potential_function.cc.o CMakeFiles/downward.dir/potentials/potential_heuristic.cc.o CMakeFiles/downward.dir/potentials/potential_max_heuristic.cc.o CMakeFiles/downward.dir/potentials/potential_optimizer.cc.o CMakeFiles/downward.dir/potentials/sample_based_potential_heuristics.cc.o CMakeFiles/downward.dir/potentials/single_potential_heuristics.cc.o CMakeFiles/downward.dir/potentials/util.cc.o  -o ../bin/downward -rdynamic -lrt

cd ..

cd ../..
