#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os
from downward.experiment import DownwardExperiment
from downward.checkouts import Translator, Preprocessor, Planner
from lab.environments import MaiaEnvironment
from downward.suites import suite_optimal_with_ipc11, suite_minitest
from downward.reports.absolute import AbsoluteReport


HOME = os.path.expanduser('~')
EXPNAME = 'issue469-airport-buffered-pressure'
DATADIR = os.path.join(os.path.dirname(__file__), 'data')
EXPPATH = os.path.join(DATADIR, EXPNAME)
REPO = os.path.join(HOME, 'projects/downward/issues/issue469')
PRE_REV = '3372a2d5249d'
REV = 'd004fed906f0'

exp = DownwardExperiment(EXPPATH, REPO,
                         combinations=[(Translator(REPO, rev=PRE_REV),
                                        Preprocessor(REPO, rev=PRE_REV),
                                        Planner(REPO, rev=REV))],
                         limits={'search_memory': 3072},
                         environment=MaiaEnvironment(email='florian.pommerening@unibas.ch'))

exp.add_suite('airport')

LIMITS = {
    '2GiB': 2 * 1024 * 1024 * 1024,
}

RESERVES = {
    '2MiB':        2 * 1024 * 1024,
    '1MiB':        1 * 1024 * 1024,
    '512KiB':           512 * 1024,
    '256KiB':           256 * 1024,
    '128KiB':           128 * 1024,
    '64KiB':             64 * 1024,
    '32KiB':             32 * 1024,
    '16KiB':             16 * 1024,
    '8KiB':               8 * 1024,
    '4KiB':               4 * 1024,
}

for limit_name, limit in LIMITS.items():
    for reserve_name, reserve in RESERVES.items():
        pressure = min(32768, reserve / 2)
        exp.add_config('astar_blind_L%s_R%s_P%s' % (limit_name, reserve_name, pressure),
            [
                '--search', 'astar(blind)',
                '--memory-limit', str(limit),
                '--memory-reserve', str(reserve),
                '--additional-memory-pressure', str(pressure)
            ])

# ====== Reports =====================

exp.add_report(AbsoluteReport(
                    resolution='combined',
                    attributes=['coverage', 'cost', 'initial_h_value', 'expansions_until_last_jump', 'memory', 'total_time', 'search_time','evaluations', 'error*', 'run_dir', 'score_*', 'seqbased_*'],
               ),
               outfile='%s.html' % EXPNAME)

exp()

