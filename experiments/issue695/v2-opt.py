#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os

from lab.environments import LocalEnvironment, BaselSlurmEnvironment

import common_setup
from common_setup import IssueConfig, IssueExperiment
from relativescatter import RelativeScatterPlotReport

DIR = os.path.dirname(os.path.abspath(__file__))
BENCHMARKS_DIR = os.environ["DOWNWARD_BENCHMARKS"]
REVISIONS = ["issue695-base", "issue695-v2"]
SUITE = common_setup.DEFAULT_OPTIMAL_SUITE
ENVIRONMENT = BaselSlurmEnvironment(
    partition="infai_1",
    email="salome.eriksson@unibas.ch",
    export=["PATH", "DOWNWARD_BENCHMARKS"])

CONFIGS = [
    IssueConfig("astar_blind", [
        "--search",
            "astar(blind())"
        ]),
    IssueConfig("astar_blind-64", [
        "--search",
            "astar(blind())"
        ], build_options = ["release64"], driver_options = ["--build", "release64"]),

    IssueConfig("seq-opt-bjolp", [], driver_options=["--alias", "seq-opt-bjolp"]),
    IssueConfig("seq-opt-bjolp-64", [], build_options = ["release64"],
        driver_options=["--build", "release64", "--alias", "seq-opt-bjolp"]),

]


if common_setup.is_test_run():
    SUITE = IssueExperiment.DEFAULT_TEST_SUITE
    ENVIRONMENT = LocalEnvironment(processes=1)

exp = IssueExperiment(
    revisions=REVISIONS,
    configs=CONFIGS,
    environment=ENVIRONMENT,
)

exp.add_parser(exp.EXITCODE_PARSER)
exp.add_parser(exp.TRANSLATOR_PARSER)
exp.add_parser(exp.SINGLE_SEARCH_PARSER)
exp.add_parser(exp.PLANNER_PARSER)

exp.add_step('build', exp.build)
exp.add_step('start', exp.start_runs)
exp.add_fetcher(name='fetch')

#exp.add_parser('lab_driver_parser', exp.LAB_DRIVER_PARSER)
#exp.add_parser('exitcode_parser', exp.EXITCODE_PARSER)
#exp.add_parser('translator_parser', exp.TRANSLATOR_PARSER)
#exp.add_parser('single_search_parser', exp.SINGLE_SEARCH_PARSER)

exp.add_suite(BENCHMARKS_DIR, SUITE)
exp.add_absolute_report_step()
exp.add_comparison_table_step()

for attribute in ["total_time"]:
    for config in CONFIGS:
        exp.add_report(
            RelativeScatterPlotReport(
                attributes=[attribute],
                filter_algorithm=["{}-{}".format(rev, config.nick) for rev in REVISIONS],
                get_category=lambda run1, run2: run1.get("domain"),
            ),
            outfile="{}-{}-{}-{}-{}.png".format(exp.name, attribute, config.nick, *REVISIONS)
        )

exp.run_steps()
