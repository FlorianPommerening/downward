#!/usr/bin/env python

import os, sys
import planning_domains_api as api


def handle_upper_bound(pid, plan_path):
    with open(plan_path) as f:
        api.submit_plan(pid, f.read())

def handle_lower_bound(pid, opt, description):
    p = api.get_problem(pid)
    lb = p.get("lower_bound", 0)
    if lb == "null" or int(lb) < opt:
        api.update_problem_stat(pid, "lower_bound", opt, description)

def handle_bounds(bounds_filename, description):
    with open(bounds_filename) as f:
        for line in f:
            try:
                pid, plan_path, opt = line.split(",")
                handle_upper_bound(int(pid), os.path.join(plan_path.strip(), "sas_plan"))
                handle_lower_bound(int(pid), opt, description)
            except object as e:
                print "Could not parse line '{}'".format(line)
                print e

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print "Usage: ./submit_bounds.py <bounds filename> <description>"
        sys.exit(1)
    handle_bounds(sys.argv[1], sys.argv[2])
