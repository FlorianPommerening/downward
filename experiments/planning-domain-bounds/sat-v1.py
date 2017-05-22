#! /usr/bin/env python

import os

from downward.experiment import FastDownwardExperiment
from lab.environments import LocalEnvironment, MaiaEnvironment

from common_setup import is_test_run, get_repo_base, get_data_dir

import planning_domains_api as api


class ApiProblem(object):
    def __init__(self, problem):
        problem = api.localize(problem)
        self.benchmarks_dir = ''
        self.domain = problem['domain']
        if not self.domain.endswith('.pddl'):
            self.domain += '.pddl'
        self.problem = problem['problem']
        if not self.problem.endswith('.pddl'):
            self.problem += '.pddl'
        self.problem_file = problem['problem_path']
        if not self.problem_file.endswith('.pddl'):
            self.problem_file += '.pddl'
        self.domain_file = problem['domain_path']
        if not self.domain_file.endswith('.pddl'):
            self.domain_file += '.pddl'
        self.api_problem_id = problem['problem_id']

    def __str__(self):
        return '%s:%s' % (self.domain, self.problem)

    def __repr__(self):
        return '<Problem %s:%s>' % (self.domain, self.problem)

    def __hash__(self):
        return hash((self.domain_file, self.problem_file))

    def __cmp__(self, other):
        return cmp((self.domain_file, self.problem_file), (other.domain_file, other.problem_file))


class ApiExperiment(FastDownwardExperiment):
    def _get_tasks(self):
        return [ApiProblem(p) for p in self.api_suite]

    def set_api_suite(self, api_suite):
        self.api_suite = api_suite

    def _add_runs(self):
        FastDownwardExperiment._add_runs(self)
        for run in self.runs:
            run.set_property('api_problem_id', run.task.api_problem_id)

REVISION = '08bff46b616b'

if is_test_run():
    ENVIRONMENT = LocalEnvironment(processes=1)
    SUITE = [api.get_problem(210)]
else:
    ENVIRONMENT = MaiaEnvironment(priority=-100, email="florian.pommerening@unibas.ch")
    SUITE = []
    for domain in api.get_domains(10):
        for problem in api.get_problems(domain['domain_id']):
            lb = problem.get('lower_bound') or 0
            ub = problem.get('upper_bound') or float('inf')
            if lb < ub:
                SUITE.append(problem)

exp = ApiExperiment(path=get_data_dir(), environment=ENVIRONMENT)

exp.set_api_suite(SUITE)

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
