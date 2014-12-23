#! /usr/bin/env python
# -*- coding: utf-8 -*-

from downward import suites
from downward.reports.scatter import ScatterPlotReport
from relativescatter import RelativeScatterPlotReport

import common_setup

CONFIGS = {
        'astar_lmcount': [
            '--search',
            'astar(lmcount(lm_merged([lm_rhw(),lm_hm(m=1)]),admissible=true,optimal=true),mpd=true)'],
}

exp = common_setup.IssueExperiment(
    search_revisions=["issue443-base", "issue443-experimental-v4", "issue443-experimental-v5"],
    configs=CONFIGS,
    suite=suites.suite_optimal_with_ipc11(),
    )

exp.add_comparison_table_step()

exp.add_report(RelativeScatterPlotReport(
                   attributes=["total_time"],
                   get_category=lambda r1,r2: r1["domain"],
                   filter_config=[
                        "issue443-base-astar_lmcount",
                        "issue443-experimental-v4-astar_lmcount",
                   ]
              ),
              outfile="%s-relative-scatter-time-v4.png" % exp.name,
)

exp.add_report(RelativeScatterPlotReport(
                   attributes=["total_time"],
                   get_category=lambda r1,r2: r1["domain"],
                   filter_config=[
                        "issue443-base-astar_lmcount",
                        "issue443-experimental-v5-astar_lmcount",
                   ]
              ),
              outfile="%s-relative-scatter-time-v5.png" % exp.name,
)

exp.add_report(RelativeScatterPlotReport(
                   attributes=["total_time"],
                   get_category=lambda r1,r2: r1["domain"],
                   filter_config=[
                        "issue443-experimental-v4-astar_lmcount",
                        "issue443-experimental-v5-astar_lmcount",
                   ]
              ),
              outfile="%s-relative-scatter-time-v4-v5.png" % exp.name,
)

exp()
