# -*- coding: utf-8 -*-
#
# downward uses the lab package to conduct experiments with the
# Fast Downward planning system.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from collections import defaultdict
import os

from lab import tools

from downward.reports.scatter import ScatterPlotReport
from downward.reports.plot import PlotReport

class RelativeScatterPlotReport(ScatterPlotReport):
    """
    Generate a scatter plot that shows how a specific attribute in two
    configurations. The attribute value in config 1 is shown on the
    x-axis and the relation to the value in config 2 on the y-axis.
    If the value in config 1 is c1 and the value in config 2 is c2, the
    plot contains contains the point (c1, c1/c2) if c1 > c2 and
    (c1, -c2/c1) otherwise.
    """
    def _fill_categories(self, runs):
        # We discard the *runs* parameter.
        # Map category names to value tuples
        categories = defaultdict(list)
        self.ylim_bottom = 0
        self.ylim_top = 0
        for (domain, problem), runs in self.problem_runs.items():
            if len(runs) != 2:
                continue
            run1, run2 = runs
            assert (run1['config'] == self.configs[0] and
                    run2['config'] == self.configs[1])
            val1 = run1.get(self.attribute)
            val2 = run2.get(self.attribute)
            if val1 is None or val2 is None:
                continue
            category = self.get_category(run1, run2)
            assert val1 > 0, (domain, problem, self.configs[0], val1)
            assert val2 > 0, (domain, problem, self.configs[1], val2)
            x = val1
            if val1 > val2:
                y = val1 / float(val2) - 1
            elif val2 > val1:
                y = -val2 / float(val1) + 1
            categories[category].append((x, y))
            self.ylim_top = max(self.ylim_top, y)
            self.ylim_bottom = min(self.ylim_bottom, y)
        return categories

    def _set_scales(self, xscale, yscale):
        # ScatterPlots use log-scaling on the x-axis by default.
        default_xscale = 'log'
        if self.attribute and self.attribute in self.LINEAR:
            default_xscale = 'linear'
        PlotReport._set_scales(self, xscale or default_xscale, 'linear')
