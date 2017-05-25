#!/usr/bin/env python

import json, sys, time

import planning_domains_api as api
from id_translation import downward_to_planning_domains

NUM_NEW_BOUNDS = 0
NUM_CLOSED_BOUNDS = 0

problem_cache = {}
def get_problem(pid):
    if pid not in problem_cache:
        for _ in range(10):
            try:
                problem_cache[pid] = api.get_problem(pid)
                break
            except:
                print "Failed to get problem %d. Retrying in 5 seconds" % pid
                time.sleep(5)
    return problem_cache[pid]


def handle_runs(runs, exp_name, description):
    for i, (run_id, run) in enumerate(runs.items()):
        print "Handling run {} of {}".format(i, len(runs))
        handle_run(run_id, run, exp_name, description)


def handle_run(run_id, run, exp_name, description):
    global NUM_NEW_BOUNDS
    global NUM_CLOSED_BOUNDS
    domain = run["domain"]
    problem = run["problem"]

    last_f_value = run.get("last_f_value")
    opt = run.get("cost")
    if last_f_value is None:
        assert opt is None
        return
    assert opt is None or last_f_value == opt, (domain, problem, opt, last_f_value)

    pid = downward_to_planning_domains.get(domain, {}).get(problem)
    if pid is None:
        return
    p = get_problem(pid)

    lb = p["lower_bound"] or 0
    ub = p["upper_bound"] or float("inf")

    assert last_f_value <= float(ub)
    if lb is None or lb == "null" or int(lb) < last_f_value:
        NUM_NEW_BOUNDS += 1
        if last_f_value == float(ub):
            NUM_CLOSED_BOUNDS += 1
            if last_f_value == opt:
                print "New bound based on plan"
            else:
                print "New bound based on f-layer"
        for _ in range(10):
            try:
#                api.update_problem_stat(pid, "lower_bound", last_f_value, description)
                break
            except:
                print "Failed to update bound %d for problem %d. Retrying in 5 seconds" % (last_f_value, pid)
                time.sleep(5)


def main(exp_name, description):
    with open("data/{}-eval/properties".format(exp_name)) as runs_file:
        runs = json.load(runs_file)
    handle_runs(runs, exp_name, description)
    print "New bounds:", NUM_NEW_BOUNDS
    print "Closed bounds:", NUM_CLOSED_BOUNDS


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print "Usage: ./submit_bounds.py <version> <description>"
        sys.exit(1)
    version = int(sys.argv[1])
    description = sys.argv[2]
    main("planning-domain-bounds-v%d" % version, description)
