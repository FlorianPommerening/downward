#! /usr/bin/env python

import os

from downward.experiment import FastDownwardExperiment
from downward.reports.absolute import AbsoluteReport
from downward.suites import Problem
from lab.environments import LocalEnvironment, BaselSlurmEnvironment

from common_setup import is_test_run, get_repo_base, get_data_dir

import planning_domains_api as api

def api_to_lab_problem(problem):
    problem = api.localize(problem)
    lb = problem.get('lower_bound') or 0
    ub = problem.get('upper_bound') or float('inf')
    assert lb < ub
    return Problem(problem['domain'], problem['problem'],
        domain_file=problem['domain_path'],
        problem_file=problem['problem_path'],
        properties={
            'api_problem_id': problem['problem_id'],
            'api_lower_bound': lb,
            'api_upper_bound': ub,
        })

REVISION = 'release-19.06.0'
# exclude domains with unsupported features
DOMAINS = [
    153,  # flashfill
    141,  # petri-net-alignment
]

if is_test_run():
    ENVIRONMENT = LocalEnvironment(processes=1)
    SUITE = [api_to_lab_problem(api.get_problem(4992))]
else:
    ENVIRONMENT = BaselSlurmEnvironment(
        partition="infai_1",
        email="florian.pommerening@unibas.ch",
        export=["PATH"])
    SUITE = []
    for d in DOMAINS:
        SUITE += [api_to_lab_problem(problem)
            for problem in api.simple_query('/classical/problems/search?domain=%s&openbounds=1' % d)]
    print "Found %d problems with open bounds" % len(SUITE)

exp = FastDownwardExperiment(path=get_data_dir(), environment=ENVIRONMENT)
exp.add_suite('', SUITE)

exp.add_algorithm(
    'astar-hmax',
    get_repo_base(),
    REVISION,
    ["--search", "astar(hmax())"],
    driver_options=[
        "--overall-time-limit", "6h",
        "--overall-memory-limit", "3800M"
    ])

exp.add_step('build', exp.build)
exp.add_step('start', exp.start_runs)
exp.add_fetcher(name='fetch')

exp.add_parser(exp.EXITCODE_PARSER)
exp.add_parser(exp.TRANSLATOR_PARSER)
exp.add_parser(exp.PLANNER_PARSER)
exp.add_parser("custom-parser.py")

exp.add_report(AbsoluteReport(attributes=['coverage', 'cost', 'error', 'api_*', 'run_dir', 'last_f_value']))

exp.run_steps()
