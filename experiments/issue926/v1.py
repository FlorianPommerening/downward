#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os

from lab.environments import LocalEnvironment, BaselSlurmEnvironment

import common_setup
from common_setup import IssueConfig, IssueExperiment
from relativescatter import RelativeScatterPlotReport

DIR = os.path.dirname(os.path.abspath(__file__))
BENCHMARKS_DIR = os.environ["DOWNWARD_BENCHMARKS"]
REVISIONS = ["issue926-v1"]
CONFIGS = [
    IssueConfig("opcount-seq-lmcut-gurobi", ["--search", "astar(operatorcounting([state_equation_constraints(), lmcut_constraints()], lpsolver=gurobi))"]),
    IssueConfig("diverse-potentials-gurobi", ["--search", "astar(diverse_potentials(lpsolver=gurobi))"]),
    IssueConfig("optimal-lmcount-gurobi", ["--search", "astar(lmcount(lm_merged([lm_rhw(),lm_hm(m=1)]), admissible=true, optimal=true, lpsolver=gurobi))"]),
    IssueConfig("opcount-seq-lmcut-cplex", ["--search", "astar(operatorcounting([state_equation_constraints(), lmcut_constraints()], lpsolver=cplex))"]),
    IssueConfig("diverse-potentials-cplex", ["--search", "astar(diverse_potentials(lpsolver=cplex))"]),
    IssueConfig("optimal-lmcount-cplex", ["--search", "astar(lmcount(lm_merged([lm_rhw(),lm_hm(m=1)]), admissible=true, optimal=true, lpsolver=cplex))"]),
]
SUITE = common_setup.DEFAULT_OPTIMAL_SUITE
ENVIRONMENT = BaselSlurmEnvironment(partition="infai_2", email="florian.pommerening@unibas.ch")

if common_setup.is_test_run():
    SUITE = IssueExperiment.DEFAULT_TEST_SUITE
    ENVIRONMENT = LocalEnvironment(processes=4)

exp = IssueExperiment(
    revisions=REVISIONS,
    configs=CONFIGS,
    environment=ENVIRONMENT,
)
exp.add_suite(BENCHMARKS_DIR, SUITE)

exp.add_parser(exp.EXITCODE_PARSER)
exp.add_parser(exp.TRANSLATOR_PARSER)
exp.add_parser(exp.SINGLE_SEARCH_PARSER)
exp.add_parser(exp.PLANNER_PARSER)

exp.add_step('build', exp.build)
exp.add_step('start', exp.start_runs)
exp.add_fetcher(name='fetch')

exp.add_absolute_report_step()

for nick in ["opcount-seq-lmcut", "diverse-potentials", "optimal-lmcount"]:
    exp.add_report(RelativeScatterPlotReport(
        attributes=["total_time"],
        filter_algorithm=["issue926-v1-%s-%s" % (nick, solver) for solver in ["cplex", "gurobi"]],
        get_category=lambda r1, r2: r1["domain"]),
        outfile="issue926-v1-scatter-total-time-%s.png" % nick)

exp.run_steps()
