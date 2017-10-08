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


SEARCH_REVS = ["issue556-v2"]
SUITE = suites.suite_satisficing_with_ipc11()

CONFIGS_TEMPLATES = {
    'astar_blind_%s': [
        '--search',
        'astar(blind())'],
    'lazy_greedy_cg_%s': [
        '--heuristic',
        'h=cg()',
        '--search',
        'lazy_greedy(h, preferred=h)'],
    'eager_greedy_ff_%s': [
        '--heuristic',
        'h=ff()',
        '--search',
        'eager_greedy(h, preferred=h)'],
    'lama_2011_first_%s': [
        "--if-unit-cost",
        "--heuristic",
        "hlm,hff=lm_ff_syn(lm_rhw(reasonable_orders=true))",
        "--search",
        "lazy_greedy([hff,hlm],preferred=[hff,hlm])",
        "--if-non-unit-cost",
        "--heuristic",
        "hlm1,hff1=lm_ff_syn(lm_rhw(reasonable_orders=true,"
        "                           lm_cost_type=one,cost_type=one))",
        "--heuristic",
        "hlm2,hff2=lm_ff_syn(lm_rhw(reasonable_orders=true,"
        "                           lm_cost_type=plusone,cost_type=plusone))",
        "--search",
        "lazy_greedy([hff1,hlm1],preferred=[hff1,hlm1], cost_type=one,reopen_closed=false)",
    ],
}


CONFIGS = {}
for nick_template, config_template in CONFIGS_TEMPLATES.items():
    for sg_nick, sg_config in [
            ("input", ""),
            ("random", "--sg-random"),
            ("greedy", "--sg-greedy"),
            ("dynamic", "--sg-greedy-dynamic"),
            ("domainsize", "--sg-domain-size"),
        ]:
        if sg_config:
            config_template = [sg_config] + config_template
        CONFIGS[nick_template % sg_nick] = (config_template)

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
