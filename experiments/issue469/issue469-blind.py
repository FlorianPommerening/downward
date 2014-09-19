#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os
from downward.experiment import DownwardExperiment
from downward.checkouts import Translator, Preprocessor, Planner
from lab.environments import MaiaEnvironment
from downward.suites import suite_optimal_with_ipc11, suite_minitest
from downward.reports.absolute import AbsoluteReport


HOME = os.path.expanduser('~')
EXPNAME = 'issue469-blind'
DATADIR = os.path.join(os.path.dirname(__file__), 'data')
EXPPATH = os.path.join(DATADIR, EXPNAME)
REPO = os.path.join(HOME, 'projects/downward/issues/issue469')
PRE_REV = '3372a2d5249d'
REV = PRE_REV

exp = DownwardExperiment(EXPPATH, REPO,
                         combinations=[(Translator(REPO, rev=PRE_REV),
                                        Preprocessor(REPO, rev=PRE_REV),
                                        Planner(REPO, rev=REV))],
                         environment=MaiaEnvironment(email='florian.pommerening@unibas.ch'))

exp.add_suite(suite_optimal_with_ipc11())

exp.add_config('astar_blind', ['--search', 'astar(blind)'])

# ====== Reports =====================

exp.add_report(AbsoluteReport(
                    resolution='combined',
                    attributes=['coverage', 'cost', 'initial_h_value', 'expansions_until_last_jump', 'memory', 'total_time', 'search_time','evaluations', 'error*', 'run_dir', 'score_*', 'seqbased_*'],
               ),
               outfile='%s.html' % EXPNAME)

exp()

