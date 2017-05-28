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

missing_lb_problems = [p for p in all_problems if get_lb(p) == 0]
missing_ub_problems = [p for p in all_problems if get_ub(p) == float('inf')]
open_bounds_problems = [p for p in all_problems if get_lb(p) < get_ub(p)]

print "Number of problems:", len(all_problems)
print "Problems without lower bound:", len(missing_lb_problems)
print "Problems without upper bound:", len(missing_ub_problems)
print "Problems with open bounds:", len(open_bounds_problems)

print "Domains with missing lower bounds"
missing_lb_domains = Counter([p["domain"] for p in missing_lb_problems])
for domain, num_problems in missing_lb_domains.most_common():
    print "   ", num_problems, domain

print "Domains with missing upper bounds"
missing_ub_domains = Counter([p["domain"] for p in missing_ub_problems])
for domain, num_problems in missing_ub_domains.most_common():
    print "   ", num_problems, domain


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
