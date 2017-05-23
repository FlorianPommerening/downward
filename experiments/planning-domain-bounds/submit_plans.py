#! /usr/bin/env python

from lab.tools import Properties

import planning_domains_api as api

import os, sys


def submit_plan(pid, plan_path):
    with open(plan_path) as f:
        api.submit_plan(pid, f.read())

def submit_plans(experiment_name):
    data_dir = os.path.join('data', experiment_name)
    runs = Properties(os.path.join(data_dir + '-eval', 'properties'))
    for run in runs.values():
        pid = run['api_problem_id']
        run_dir = run['run_dir']
        last_plan = len(run.get('cost_all', []))
        if last_plan:
            last_plan_path = os.path.join(data_dir, run_dir, 'sas_plan.%d' % last_plan)
            print pid, last_plan_path
            submit_plan(int(pid), last_plan_path)


if __name__ == "__main__":
    submit_plans(sys.argv[1])
