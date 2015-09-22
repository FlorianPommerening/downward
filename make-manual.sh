#! /bin/bash
set -e
set -x

BUILD_DIR=make

rm -rf builds/$BUILD_DIR
mkdir -p builds/$BUILD_DIR/bin
mkdir -p builds/$BUILD_DIR/.obj
mkdir -p builds/$BUILD_DIR/.obj/{landmarks,pdbs,merge_and_shrink,open_lists,potentials}

cd builds/$BUILD_DIR/

# SOURCES="
# planner
# abstract_task
# axioms
# causal_graph
# combining_evaluator
# cost_adapted_task
# countdown_timer
# delegating_task
# domain_transition_graph
# eager_search
# enforced_hill_climbing_search
# equivalence_relation
# evaluation_context
# evaluation_result
# exact_timer
# g_evaluator
# global_operator
# global_state
# globals
# heuristic_cache
# heuristic
# int_packer
# ipc_max_heuristic
# iterated_search
# lazy_search
# max_evaluator
# operator_cost
# option_parser
# option_parser_util
# segmented_vector
# per_state_information
# pref_evaluator
# relaxation_heuristic
# rng
# root_task
# sampling
# scalar_evaluator
# search_engine
# search_node_info
# search_progress
# search_space
# search_statistics
# state_id
# state_registry
# successor_generator
# sum_evaluator
# task_proxy
# task_tools
# timer
# tracer
# utilities
# utilities_hash
# variable_order_finder
# weighted_evaluator
# open_lists/alternation_open_list
# open_lists/bucket_open_list
# open_lists/open_list
# open_lists/pareto_open_list
# open_lists/standard_scalar_open_list
# open_lists/tiebreaking_open_list
# lp_internals
# lp_solver
# plugin
# priority_queue
# utilities_windows
# additive_heuristic
# blind_search_heuristic
# cea_heuristic
# cg_heuristic
# cg_cache
# ff_heuristic
# goal_count_heuristic
# hm_heuristic
# lm_cut_heuristic
# max_heuristic
# merge_and_shrink/distances
# merge_and_shrink/factored_transition_system
# merge_and_shrink/fts_factory
# merge_and_shrink/heuristic_representation
# merge_and_shrink/label_equivalence_relation
# merge_and_shrink/labels
# merge_and_shrink/merge_and_shrink_heuristic
# merge_and_shrink/merge_dfp
# merge_and_shrink/merge_linear
# merge_and_shrink/merge_strategy
# merge_and_shrink/shrink_bisimulation
# merge_and_shrink/shrink_bucket_based
# merge_and_shrink/shrink_fh
# merge_and_shrink/shrink_random
# merge_and_shrink/shrink_strategy
# merge_and_shrink/transition_system
# landmarks/exploration
# landmarks/h_m_landmarks
# landmarks/lama_ff_synergy
# landmarks/landmark_cost_assignment
# landmarks/landmark_count_heuristic
# landmarks/landmark_status_manager
# landmarks/landmark_graph_merged
# landmarks/landmark_graph
# landmarks/landmark_factory
# landmarks/landmark_factory_rpg_exhaust
# landmarks/landmark_factory_rpg_sasp
# landmarks/landmark_factory_zhu_givan
# landmarks/util
# pdbs/canonical_pdbs_heuristic
# pdbs/dominance_pruner
# pdbs/match_tree
# pdbs/max_cliques
# pdbs/pattern_database
# pdbs/pattern_generation_edelkamp
# pdbs/pattern_generation_haslum
# pdbs/pdb_heuristic
# pdbs/util
# pdbs/zero_one_pdbs_heuristic
# potentials/diverse_potential_heuristics
# potentials/potential_function
# potentials/potential_heuristic
# potentials/potential_max_heuristic
# potentials/potential_optimizer
# potentials/sample_based_potential_heuristics
# potentials/single_potential_heuristics
# potentials/util
# "
#
# for src in $SOURCES; do
#     g++ -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -I../../src/search/ext -O3 -DNDEBUG -fomit-frame-pointer -c ../../src/search/$src.cc -o .obj/$src.o
# done
cp -r ../common/.obj .

LINK_ORDER="
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

OBJECTS=()
for f in $LINK_ORDER; do
    OBJECTS+=(.obj/$f.o)
done

g++ -m32 -g -rdynamic ${OBJECTS[@]} -lrt -o bin/downward

cd ../..
