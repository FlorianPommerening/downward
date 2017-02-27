#!/usr/bin/env python

import os, sys
import planning_domains_api as api


def handle_bound(pid, plan_path):
    with open(plan_path) as f:
        api.submit_plan(pid, f.read())

def handle_bounds(bounds_filename):
    with open(bounds_filename) as f:
        for line in f:
            try:
                pid, plan_path, opt = line.split(",")
                handle_bound(int(pid), os.path.join(plan_path.strip(), "sas_plan"))
            except:
                print "Could not parse line '{}'".format(line)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "Usage: ./submit_bounds.py <bounds filename>"
        sys.exit(1)
    handle_bounds(sys.argv[1])
