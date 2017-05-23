#! /usr/bin/env python

import os

from downward.experiment import FastDownwardExperiment
from downward.suites import Problem
from lab.environments import LocalEnvironment, MaiaEnvironment

from common_setup import is_test_run, get_repo_base, get_data_dir

import planning_domains_api as api

def api_to_lab_problem(problem):
    problem = api.localize(problem)
    return Problem(problem['domain'], problem['problem'],
        domain_file=problem['domain_path'],
        problem_file=problem['problem_path'],
        properties={'api_problem_id': problem['problem_id']})

REVISION = '08bff46b616b'

if is_test_run():
    ENVIRONMENT = LocalEnvironment(processes=1)
    SUITE = [api_to_lab_problem(api.get_problem(210))]
else:
    ENVIRONMENT = MaiaEnvironment(priority=-100, email="florian.pommerening@unibas.ch")
    SUITE = []
    for domain in api.get_domains(10):
        for problem in api.get_problems(domain['domain_id']):
            lb = problem.get('lower_bound') or 0
            ub = problem.get('upper_bound') or float('inf')
            if lb < ub:
                SUITE.append(api_to_lab_problem(problem))
    print "Found %d problems with open bounds" % len(SUITE)

exp = FastDownwardExperiment(path=get_data_dir(), environment=ENVIRONMENT)

exp.add_suite('', SUITE)

exp.add_algorithm(
    'lama-24h',
    get_repo_base(),
    REVISION,
    [],
    driver_options=[
        "--alias", "seq-sat-lama-2011",
        "--search-time-limit", "24h",
        "--search-memory-limit", "3800M"
    ])

exp()
