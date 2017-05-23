#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys

from lab.environments import LocalEnvironment, MaiaEnvironment

from common_setup import IssueConfig, IssueExperiment, is_test_run

from missing_suite_v3 import open_bounds


BENCHMARKS_DIR = os.environ["DOWNWARD_BENCHMARKS"]
REVISIONS = ["08bff46b616b"]
CONFIGS = [
    IssueConfig(heuristic, [
        "--search",
        "astar({})".format(heuristic)],
        driver_options=["--search-time-limit", "24h",
                        "--search-memory-limit", "3800M"])
    for heuristic in [
        "ipdb(max_time=3600, cache_estimates=false)"]
]
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
exp.add_fetcher(name='parse-again', parsers=['custom-parser.py'])

exp()
