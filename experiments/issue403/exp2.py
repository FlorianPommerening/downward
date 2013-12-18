#! /usr/bin/env python
# -*- coding: utf-8 -*-

from downward.configs import default_configs_optimal

import common_setup


REVS = ["issue403-v1"]
CONFIGS = {
        'astar_lmcut': [
            '--search',
            'astar(lmcut)'],
        'astar_hmax': [
            '--search',
            'astar(hmax)'],
        'eager_greedy_ff': [
            '--heuristic',
            'h=ff()',
            '--search',
            'eager_greedy(h, preferred=h)'],
}

TEST_RUN = True

if TEST_RUN:
    SUITE = "miconic-simpleadl:s1-0.pddl"
    PRIORITY = None  # "None" means local experiment
else:
    SUITE = ['miconic-simpleadl']
    PRIORITY = 0     # number means maia experiment


# TODO: I'd like to specify "search_revisions" (which uses the same
# translator and preprocessor for everything) instead of "revisions"
# here, but I can't seem to make this work with the REVS argument for
# CompareRevisionsReport.

exp = common_setup.MyExperiment(
    grid_priority=PRIORITY,
    revisions=REVS,
    configs=CONFIGS,
    suite=SUITE
    )


exp.add_comparison_table_step()
exp.add_scatter_plot_step()

exp()
