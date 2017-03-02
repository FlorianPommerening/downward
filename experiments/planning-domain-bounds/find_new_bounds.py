#!/usr/bin/env python

import os, json
from collections import defaultdict

import planning_domains_api as api
from id_translation import downward_to_planning_domains


problem_cache = {}
def get_problem(pid):
    if pid not in problem_cache:
        problem_cache[pid] = api.get_problem(pid)
    return problem_cache[pid]

def handle_runs(runs, bounds_file, missing_file, exp_name):
    missing_bounds = {}
    for i, (run_id, run) in enumerate(runs.items()):
        print "Handling run {} of {}".format(i, len(runs))
        handle_run(run_id, run, bounds_file, missing_bounds, exp_name)

    missing_file.write("open_bounds = [\n")
    for (domain, problem), missing in sorted(missing_bounds.items()):
        if missing:
            missing_file.write('    "{}:{}",\n'.format(domain, problem))
    missing_file.write("]\n")

def handle_run(run_id, run, bounds_file, missing_bounds, exp_name):
    domain = run["domain"]
    problem = run["problem"]
    pid = downward_to_planning_domains.get(domain, {}).get(problem)
    if pid is None:
        return
    p = get_problem(pid)

    lb = p["lower_bound"] or 0
    ub = p["upper_bound"] or float("inf")

    opt = run.get("cost")
    if opt is None:
        key = (domain, problem)
        if key not in missing_bounds:
            missing_bounds[key] = True
        return

    assert lb <= opt <= ub
    if lb < opt or opt < ub:
        handle_new_bound(run, p, bounds_file, exp_name)
        missing_bounds[(domain, problem)] = False

def handle_new_bound(run, p, bounds_file, exp_name):
    pid = p["problem_id"]
    plan_path = os.path.join("data", exp_name, run["run_dir"])
    cost = run["cost"]
    bounds_file.write("{}, {}, {}\n".format(pid, plan_path, int(cost)))


def main(bounds_filename, missing_filename, exp_name):
    with open("data/{}-eval/properties".format(exp_name)) as runs_file:
        runs = json.load(runs_file)
    with open(bounds_filename, "w") as bounds_file:
        with open(missing_filename, "w") as missing_file:
            handle_runs(runs, bounds_file, missing_file, exp_name)


if __name__ == "__main__":
    version = 4
    main(
        "new_bounds_v%d" % version,
        "missing_suite_v%d.py" % version,
        "planning-domain-bounds-v%d" % version)
