#!/usr/bin/env python

import json, os, sys, time

import planning_domains_api as api

NUM_NEW_BOUNDS = 0
NUM_CLOSED_BOUNDS = 0


def handle_runs(runs, exp_name, description):
    for i, (run_id, run) in enumerate(runs.items()):
        print "Handling run {} of {}".format(i, len(runs))
        handle_run(run_id, run, exp_name, description)


def submit_lower_bound(pid, lb, description):
    p = api.get_problem(pid)
    api_lb = p.get("lower_bound", 0)
    api_ub = p.get("upper_bound", float('inf'))
    assert api_ub is None or api_ub == "null" or lb <= float(api_ub)
    if api_lb is None or api_lb == "null" or int(api_lb) < lb:
        for _ in range(10):
            try:
                api.update_problem_stat(pid, "lower_bound", lb, description)
                break
            except:
                print "Failed to update bound %d for problem %d. Retrying in 5 seconds" % (lb, pid)
                time.sleep(5)


def submit_upper_bound(pid, exp_name, run):
    plan_path = os.path.join('data', exp_name, run['run_dir'], 'sas_plan')
    with open(plan_path) as f:
        plan = f.read()
    for _ in range(10):
        try:
            api.submit_plan(pid, plan)
            break
        except:
            print "Failed to submit plan from '%s' for problem %d. Retrying in 5 seconds" % (plan_path, pid)
            time.sleep(5)


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

    pid = run['api_problem_id']
    lb = int(run["api_lower_bound"] or 0)
    ub = float(run["api_upper_bound"] or "inf")

    assert last_f_value <= ub
    assert opt is None or lb <= opt <= ub
    if opt is not None:
        NUM_NEW_BOUNDS += 1
        NUM_CLOSED_BOUNDS += 1
        if lb < opt:
            submit_lower_bound(pid, int(opt), description)
        if opt < ub:
            submit_upper_bound(pid, exp_name, run)
    else:
        if lb < last_f_value:
            NUM_NEW_BOUNDS += 1
            if last_f_value == ub:
                NUM_CLOSED_BOUNDS += 1
            submit_lower_bound(pid, int(last_f_value), description + "/explored f-layer")


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
    version = sys.argv[1]
    description = sys.argv[2]
    main("planning-domain-bounds-%s" % version, description)
