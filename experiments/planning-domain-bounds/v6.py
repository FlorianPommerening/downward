#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys

from lab.environments import LocalEnvironment, MaiaEnvironment

from common_setup import IssueConfig, IssueExperiment, is_test_run

from missing_suite_v5 import open_bounds


BENCHMARKS_DIR = os.environ["DOWNWARD_BENCHMARKS"]
REVISIONS = ["08bff46b616b"]
CONFIGS = [
    IssueConfig("seq_lmcut_prune", [
        "--search",
        "astar(merge_and_shrink("
          "shrink_strategy=shrink_bisimulation(greedy=false),"
          "merge_strategy=merge_stateless("
            "merge_selector=score_based_filtering("
              "scoring_functions=[goal_relevance,dfp,total_order])),"
          "label_reduction=exact(before_shrinking=true,before_merging=false),"
          "max_states=100000,"
          "threshold_before_merge=1))"],
        driver_options=["--search-time-limit", "24h",
                        "--search-memory-limit", "3800M"])]
SUITE = open_bounds
ENVIRONMENT = MaiaEnvironment(
    priority=-100, email="florian.pommerening@unibas.ch")

if is_test_run():
    SUITE = IssueExperiment.DEFAULT_TEST_SUITE
    ENVIRONMENT = LocalEnvironment(processes=1)

exp = IssueExperiment(
    revisions=REVISIONS,
    configs=CONFIGS,
    environment=ENVIRONMENT,
)
exp.add_suite(BENCHMARKS_DIR, SUITE)

exp()
