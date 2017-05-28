#! /usr/bin/env python

import planning_domains_api as api
from collections import Counter

def get_lb(p):
    try:
        return int(p.get('lower_bound', 0))
    except:
        return 0

def get_ub(p):
    try:
        return float(p.get('upper_bound', 'inf'))
    except:
        return float('inf')


all_problems = api.simple_query('/classical/problems')


print "Number of problems:", len(all_problems)
print "Problems without lower bound:", len([p for p in all_problems if get_lb(p) == 0])
print "Problems without upper bound:", len([p for p in all_problems if get_ub(p) == float('inf')])
print "Problems with open bounds:", len([p for p in all_problems if get_lb(p) < get_ub(p)])


print "Lower bound contributors:"
lower_bound_contributor = Counter([
    p.get("lower_bound_description").split("/")[0]
    for p in all_problems if get_lb(p) > 0])
for contributor, num_bounds in lower_bound_contributor.most_common():
    print "   ", num_bounds, contributor

print "Upper bound contributors:"
upper_bound_contributor = Counter([
    p.get("upper_bound_description")
    for p in all_problems if get_ub(p) < float('inf')])
for contributor, num_bounds in upper_bound_contributor.most_common():
    print "   ", num_bounds, contributor
