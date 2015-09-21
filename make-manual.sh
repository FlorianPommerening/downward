#! /bin/bash
set -e
set -x

rm -rf builds/make

cd src/preprocess
rm -rf .obj preprocess
mkdir .obj
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c planner.cc -o .obj/planner.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c causal_graph.cc -o .obj/causal_graph.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c axiom.cc -o .obj/axiom.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c domain_transition_graph.cc -o .obj/domain_transition_graph.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c helper_functions.cc -o .obj/helper_functions.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c max_dag.cc -o .obj/max_dag.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c mutex_group.cc -o .obj/mutex_group.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c operator.cc -o .obj/operator.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c scc.cc -o .obj/scc.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c state.cc -o .obj/state.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c successor_generator.cc -o .obj/successor_generator.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -O3 -DNDEBUG -fomit-frame-pointer -c variable.cc -o .obj/variable.o
g++  -m32 -g  -static -static-libgcc .obj/planner.o .obj/axiom.o .obj/causal_graph.o .obj/domain_transition_graph.o .obj/helper_functions.o .obj/max_dag.o .obj/mutex_group.o .obj/operator.o .obj/scc.o .obj/state.o .obj/successor_generator.o .obj/variable.o   -Wl,-Bstatic -lrt -o preprocess
cd ../..

cd src/search
rm -rf .obj downward-release
mkdir .obj
mkdir .obj/{landmarks,pdbs,merge_and_shrink,open_lists,potentials}
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c planner.cc -o .obj/planner.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c abstract_task.cc -o .obj/abstract_task.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c axioms.cc -o .obj/axioms.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c causal_graph.cc -o .obj/causal_graph.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c combining_evaluator.cc -o .obj/combining_evaluator.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c cost_adapted_task.cc -o .obj/cost_adapted_task.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c countdown_timer.cc -o .obj/countdown_timer.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c delegating_task.cc -o .obj/delegating_task.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c domain_transition_graph.cc -o .obj/domain_transition_graph.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c eager_search.cc -o .obj/eager_search.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c enforced_hill_climbing_search.cc -o .obj/enforced_hill_climbing_search.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c equivalence_relation.cc -o .obj/equivalence_relation.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c evaluation_context.cc -o .obj/evaluation_context.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c evaluation_result.cc -o .obj/evaluation_result.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c exact_timer.cc -o .obj/exact_timer.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c g_evaluator.cc -o .obj/g_evaluator.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c global_operator.cc -o .obj/global_operator.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c global_state.cc -o .obj/global_state.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c globals.cc -o .obj/globals.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c heuristic_cache.cc -o .obj/heuristic_cache.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c heuristic.cc -o .obj/heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c int_packer.cc -o .obj/int_packer.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c ipc_max_heuristic.cc -o .obj/ipc_max_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c iterated_search.cc -o .obj/iterated_search.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c lazy_search.cc -o .obj/lazy_search.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c max_evaluator.cc -o .obj/max_evaluator.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c operator_cost.cc -o .obj/operator_cost.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c option_parser.cc -o .obj/option_parser.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c option_parser_util.cc -o .obj/option_parser_util.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c segmented_vector.cc -o .obj/segmented_vector.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c per_state_information.cc -o .obj/per_state_information.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c pref_evaluator.cc -o .obj/pref_evaluator.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c relaxation_heuristic.cc -o .obj/relaxation_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c rng.cc -o .obj/rng.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c root_task.cc -o .obj/root_task.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c sampling.cc -o .obj/sampling.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c scalar_evaluator.cc -o .obj/scalar_evaluator.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c search_engine.cc -o .obj/search_engine.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c search_node_info.cc -o .obj/search_node_info.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c search_progress.cc -o .obj/search_progress.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c search_space.cc -o .obj/search_space.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c search_statistics.cc -o .obj/search_statistics.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c state_id.cc -o .obj/state_id.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c state_registry.cc -o .obj/state_registry.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c successor_generator.cc -o .obj/successor_generator.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c sum_evaluator.cc -o .obj/sum_evaluator.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c task_proxy.cc -o .obj/task_proxy.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c task_tools.cc -o .obj/task_tools.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c timer.cc -o .obj/timer.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c tracer.cc -o .obj/tracer.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c utilities.cc -o .obj/utilities.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c utilities_hash.cc -o .obj/utilities_hash.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c variable_order_finder.cc -o .obj/variable_order_finder.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c weighted_evaluator.cc -o .obj/weighted_evaluator.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c open_lists/alternation_open_list.cc -o .obj/open_lists/alternation_open_list.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c open_lists/bucket_open_list.cc -o .obj/open_lists/bucket_open_list.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c open_lists/open_list.cc -o .obj/open_lists/open_list.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c open_lists/pareto_open_list.cc -o .obj/open_lists/pareto_open_list.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c open_lists/standard_scalar_open_list.cc -o .obj/open_lists/standard_scalar_open_list.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c open_lists/tiebreaking_open_list.cc -o .obj/open_lists/tiebreaking_open_list.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c lp_internals.cc -o .obj/lp_internals.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c lp_solver.cc -o .obj/lp_solver.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c plugin.cc -o .obj/plugin.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c priority_queue.cc -o .obj/priority_queue.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c utilities_windows.cc -o .obj/utilities_windows.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c additive_heuristic.cc -o .obj/additive_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c blind_search_heuristic.cc -o .obj/blind_search_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c cea_heuristic.cc -o .obj/cea_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c cg_heuristic.cc -o .obj/cg_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c cg_cache.cc -o .obj/cg_cache.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c ff_heuristic.cc -o .obj/ff_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c goal_count_heuristic.cc -o .obj/goal_count_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c hm_heuristic.cc -o .obj/hm_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c lm_cut_heuristic.cc -o .obj/lm_cut_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c max_heuristic.cc -o .obj/max_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/distances.cc -o .obj/merge_and_shrink/distances.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/factored_transition_system.cc -o .obj/merge_and_shrink/factored_transition_system.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/fts_factory.cc -o .obj/merge_and_shrink/fts_factory.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/heuristic_representation.cc -o .obj/merge_and_shrink/heuristic_representation.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/label_equivalence_relation.cc -o .obj/merge_and_shrink/label_equivalence_relation.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/labels.cc -o .obj/merge_and_shrink/labels.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/merge_and_shrink_heuristic.cc -o .obj/merge_and_shrink/merge_and_shrink_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/merge_dfp.cc -o .obj/merge_and_shrink/merge_dfp.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/merge_linear.cc -o .obj/merge_and_shrink/merge_linear.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/merge_strategy.cc -o .obj/merge_and_shrink/merge_strategy.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/shrink_bisimulation.cc -o .obj/merge_and_shrink/shrink_bisimulation.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/shrink_bucket_based.cc -o .obj/merge_and_shrink/shrink_bucket_based.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/shrink_fh.cc -o .obj/merge_and_shrink/shrink_fh.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/shrink_random.cc -o .obj/merge_and_shrink/shrink_random.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/shrink_strategy.cc -o .obj/merge_and_shrink/shrink_strategy.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c merge_and_shrink/transition_system.cc -o .obj/merge_and_shrink/transition_system.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/exploration.cc -o .obj/landmarks/exploration.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/h_m_landmarks.cc -o .obj/landmarks/h_m_landmarks.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/lama_ff_synergy.cc -o .obj/landmarks/lama_ff_synergy.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/landmark_cost_assignment.cc -o .obj/landmarks/landmark_cost_assignment.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/landmark_count_heuristic.cc -o .obj/landmarks/landmark_count_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/landmark_status_manager.cc -o .obj/landmarks/landmark_status_manager.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/landmark_graph_merged.cc -o .obj/landmarks/landmark_graph_merged.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/landmark_graph.cc -o .obj/landmarks/landmark_graph.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/landmark_factory.cc -o .obj/landmarks/landmark_factory.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/landmark_factory_rpg_exhaust.cc -o .obj/landmarks/landmark_factory_rpg_exhaust.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/landmark_factory_rpg_sasp.cc -o .obj/landmarks/landmark_factory_rpg_sasp.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/landmark_factory_zhu_givan.cc -o .obj/landmarks/landmark_factory_zhu_givan.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c landmarks/util.cc -o .obj/landmarks/util.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c pdbs/canonical_pdbs_heuristic.cc -o .obj/pdbs/canonical_pdbs_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c pdbs/dominance_pruner.cc -o .obj/pdbs/dominance_pruner.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c pdbs/match_tree.cc -o .obj/pdbs/match_tree.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c pdbs/max_cliques.cc -o .obj/pdbs/max_cliques.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c pdbs/pattern_database.cc -o .obj/pdbs/pattern_database.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c pdbs/pattern_generation_edelkamp.cc -o .obj/pdbs/pattern_generation_edelkamp.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c pdbs/pattern_generation_haslum.cc -o .obj/pdbs/pattern_generation_haslum.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c pdbs/pdb_heuristic.cc -o .obj/pdbs/pdb_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c pdbs/util.cc -o .obj/pdbs/util.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c pdbs/zero_one_pdbs_heuristic.cc -o .obj/pdbs/zero_one_pdbs_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c potentials/diverse_potential_heuristics.cc -o .obj/potentials/diverse_potential_heuristics.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c potentials/potential_function.cc -o .obj/potentials/potential_function.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c potentials/potential_heuristic.cc -o .obj/potentials/potential_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c potentials/potential_max_heuristic.cc -o .obj/potentials/potential_max_heuristic.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c potentials/potential_optimizer.cc -o .obj/potentials/potential_optimizer.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c potentials/sample_based_potential_heuristics.cc -o .obj/potentials/sample_based_potential_heuristics.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c potentials/single_potential_heuristics.cc -o .obj/potentials/single_potential_heuristics.release.o
g++  -g -m32 -std=c++11 -Wall -Wextra -pedantic -Werror -Iext -O3 -DNDEBUG -fomit-frame-pointer -c potentials/util.cc -o .obj/potentials/util.release.o
g++  -m32 -g -rdynamic  .obj/planner.release.o .obj/abstract_task.release.o .obj/axioms.release.o .obj/causal_graph.release.o .obj/combining_evaluator.release.o .obj/cost_adapted_task.release.o .obj/countdown_timer.release.o .obj/delegating_task.release.o .obj/domain_transition_graph.release.o .obj/eager_search.release.o .obj/enforced_hill_climbing_search.release.o .obj/equivalence_relation.release.o .obj/evaluation_context.release.o .obj/evaluation_result.release.o .obj/exact_timer.release.o .obj/g_evaluator.release.o .obj/global_operator.release.o .obj/global_state.release.o .obj/globals.release.o .obj/heuristic_cache.release.o .obj/heuristic.release.o .obj/int_packer.release.o .obj/ipc_max_heuristic.release.o .obj/iterated_search.release.o .obj/lazy_search.release.o .obj/max_evaluator.release.o .obj/operator_cost.release.o .obj/option_parser.release.o .obj/option_parser_util.release.o .obj/segmented_vector.release.o .obj/per_state_information.release.o .obj/pref_evaluator.release.o .obj/relaxation_heuristic.release.o .obj/rng.release.o .obj/root_task.release.o .obj/sampling.release.o .obj/scalar_evaluator.release.o .obj/search_engine.release.o .obj/search_node_info.release.o .obj/search_progress.release.o .obj/search_space.release.o .obj/search_statistics.release.o .obj/state_id.release.o .obj/state_registry.release.o .obj/successor_generator.release.o .obj/sum_evaluator.release.o .obj/task_proxy.release.o .obj/task_tools.release.o .obj/timer.release.o .obj/tracer.release.o .obj/utilities.release.o .obj/utilities_hash.release.o .obj/variable_order_finder.release.o .obj/weighted_evaluator.release.o .obj/open_lists/alternation_open_list.release.o .obj/open_lists/bucket_open_list.release.o .obj/open_lists/open_list.release.o .obj/open_lists/pareto_open_list.release.o .obj/open_lists/standard_scalar_open_list.release.o .obj/open_lists/tiebreaking_open_list.release.o .obj/lp_internals.release.o .obj/lp_solver.release.o .obj/plugin.release.o .obj/priority_queue.release.o .obj/utilities_windows.release.o .obj/additive_heuristic.release.o .obj/blind_search_heuristic.release.o .obj/cea_heuristic.release.o .obj/cg_heuristic.release.o .obj/cg_cache.release.o .obj/ff_heuristic.release.o .obj/goal_count_heuristic.release.o .obj/hm_heuristic.release.o .obj/lm_cut_heuristic.release.o .obj/max_heuristic.release.o .obj/merge_and_shrink/distances.release.o .obj/merge_and_shrink/factored_transition_system.release.o .obj/merge_and_shrink/fts_factory.release.o .obj/merge_and_shrink/heuristic_representation.release.o .obj/merge_and_shrink/label_equivalence_relation.release.o .obj/merge_and_shrink/labels.release.o .obj/merge_and_shrink/merge_and_shrink_heuristic.release.o .obj/merge_and_shrink/merge_dfp.release.o .obj/merge_and_shrink/merge_linear.release.o .obj/merge_and_shrink/merge_strategy.release.o .obj/merge_and_shrink/shrink_bisimulation.release.o .obj/merge_and_shrink/shrink_bucket_based.release.o .obj/merge_and_shrink/shrink_fh.release.o .obj/merge_and_shrink/shrink_random.release.o .obj/merge_and_shrink/shrink_strategy.release.o .obj/merge_and_shrink/transition_system.release.o .obj/landmarks/exploration.release.o .obj/landmarks/h_m_landmarks.release.o .obj/landmarks/lama_ff_synergy.release.o .obj/landmarks/landmark_cost_assignment.release.o .obj/landmarks/landmark_count_heuristic.release.o .obj/landmarks/landmark_status_manager.release.o .obj/landmarks/landmark_graph_merged.release.o .obj/landmarks/landmark_graph.release.o .obj/landmarks/landmark_factory.release.o .obj/landmarks/landmark_factory_rpg_exhaust.release.o .obj/landmarks/landmark_factory_rpg_sasp.release.o .obj/landmarks/landmark_factory_zhu_givan.release.o .obj/landmarks/util.release.o .obj/pdbs/canonical_pdbs_heuristic.release.o .obj/pdbs/dominance_pruner.release.o .obj/pdbs/match_tree.release.o .obj/pdbs/max_cliques.release.o .obj/pdbs/pattern_database.release.o .obj/pdbs/pattern_generation_edelkamp.release.o .obj/pdbs/pattern_generation_haslum.release.o .obj/pdbs/pdb_heuristic.release.o .obj/pdbs/util.release.o .obj/pdbs/zero_one_pdbs_heuristic.release.o .obj/potentials/diverse_potential_heuristics.release.o .obj/potentials/potential_function.release.o .obj/potentials/potential_heuristic.release.o .obj/potentials/potential_max_heuristic.release.o .obj/potentials/potential_optimizer.release.o .obj/potentials/sample_based_potential_heuristics.release.o .obj/potentials/single_potential_heuristics.release.o .obj/potentials/util.release.o   -lrt -o downward-release
cd ../..

mkdir -p builds/make/bin
cp -r src/translate builds/make/bin/
cp src/preprocess/preprocess builds/make/bin/
cp src/search/downward-release builds/make/bin/downward
