#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys

from lab.environments import LocalEnvironment, MaiaEnvironment

from common_setup import IssueConfig, IssueExperiment, is_test_run

from missing_suite_v6 import open_bounds


BENCHMARKS_DIR = os.environ["DOWNWARD_BENCHMARKS"]
REVISIONS = ["08bff46b616b"]
CONFIGS = [
    IssueConfig("seq_cegar", [
        "--search",
        "astar(cegar())"],
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
exp.add_fetcher(name='parse-again', parsers=['custom-parser.py'])

exp()
