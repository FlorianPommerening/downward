#! /usr/bin/env python
# -*- coding: utf-8 -*-

from downward import suites
from downward.reports.scatter import ScatterPlotReport
# Cactus plots are experimental in lab, and require some changes to
# classes in lab, so we cannot add them es external files here.
try:
    from downward.reports.cactus import CactusPlotReport
    has_cactus_plot = True
except:
    has_cactus_plot = False

import common_setup
from relativescatter import RelativeScatterPlotReport


SEARCH_REVS = ["issue556-v1"]
SUITE = suites.suite_satisficing_with_ipc11()

CONFIGS = {
    'astar_blind_input': [
        '--search',
        'astar(blind())'],
    'astar_blind_random': [
        '--sg-random',
        '--search',
        'astar(blind())'],
    'astar_blind_greedy': [
        '--sg-greedy',
        '--search',
        'astar(blind())'],
    'astar_blind_greedy_dynamic': [
        '--sg-greedy-dynamic',
        '--search',
        'astar(blind())'],
}

exp = common_setup.IssueExperiment(
    revisions=SEARCH_REVS,
    configs=CONFIGS,
    suite=SUITE,
    )
exp.add_search_parser("custom-parser.py")

attributes = exp.DEFAULT_TABLE_ATTRIBUTES + ["successor_generator_*", "reopened_until_last_jump"]
exp.add_absolute_report_step(attributes=attributes)

for conf in CONFIGS:
    for attr in ("search_time", ):
        exp.add_report(
            RelativeScatterPlotReport(
                attributes=[attr],
                get_category=lambda run1, run2: run1.get("domain"),
                filter_config=["issue556-v1-astar_blind_input", "issue556-v1-%s" % conf]
            ),
            outfile='issue556_v1_input_%s_%s.png' % (conf, attr)
        )

if has_cactus_plot:
    exp.add_report(CactusPlotReport(attributes=['successor_generator_time'],
                 ylabel='successor_generator_time',
                 get_category=lambda run: run['config_nick'],
                 category_styles={
                    'astar_blind_input': {'linestyle': '-', 'c':'red'},
                    'astar_blind_random': {'linestyle': '-', 'c':'yellow'},
                    'astar_blind_greedy': {'linestyle': '-', 'c':'green'},
                    'astar_blind_greedy_dynamic': {'linestyle': '-', 'c':'blue'},
                }
            ))


exp()
