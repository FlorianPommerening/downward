import planning_domains_api as api

import os, re
from collections import defaultdict

BENCHMARKS_DIR = os.environ["DOWNWARD_BENCHMARKS"]


planning_domains_to_downward = {
    100: "airport",
    18: "barman-opt11-strips",
    25: "barman-opt14-strips",
#    128: "blocks",  ## domain 128 has some tasks in common but neither domain contains the other
    116: "childsnack-opt14-strips",
    67: "depot",
    83: "driverlog",
    58: "elevators-opt08-strips",
    93: "elevators-opt11-strips",
    20: "floortile-opt11-strips",
    10: "floortile-opt14-strips",
    30: "freecell",
    29: "freecell",
    22: "ged-opt14-strips",
    62: "grid",
    119: "gripper",
    89: "hiking-opt14-strips",
#    ??: "logistics00",
    38: "logistics98",
    12: "miconic",
    44: "movie",
    101: "mprime",
    68: "mystery",
    36: "nomystery-opt11-strips",
    31: "openstacks-opt08-strips",
    104: "openstacks-opt11-strips",
    66: "openstacks-opt14-strips",
    64: "openstacks-strips",
    85: "parcprinter-08-strips",
    42: "parcprinter-opt11-strips",
    81: "parking-opt11-strips",
    107: "parking-opt14-strips",
#    45: "pathways-noneg", ## the files in Fast Downward include a bug fix
    9: "pegsol-08-strips",
    88: "pegsol-opt11-strips",
    90: "pipesworld-notankage",
    14: "pipesworld-tankage",
    60: "psr-small",
    49: "rovers",
    32: "satellite",
    63: "scanalyzer-08-strips",
    82: "scanalyzer-opt11-strips",
    4: "sokoban-opt08-strips",
    86: "sokoban-opt11-strips",
    35: "storage",
    39: "tetris-opt14-strips",
    109: "tidybot-opt11-strips",
    105: "tidybot-opt14-strips",
    19: "tpp",
    57: "transport-opt08-strips",
    13: "transport-opt11-strips",
    11: "transport-opt14-strips",
    16: "trucks-strips",
    23: "visitall-opt11-strips",
    71: "visitall-opt14-strips",
    55: "woodworking-opt08-strips",
    111: "woodworking-opt11-strips",
    17: "zenotravel",
}

def translate_domain_filename(downward_name, df):
    ddf = os.path.join(BENCHMARKS_DIR, downward_name, os.path.basename(df))
    if not os.path.exists(ddf):
        ddf = os.path.join(BENCHMARKS_DIR, downward_name, "domain.pddl")
    return ddf

def translate_problem_filename(downward_name, pf):
    basename = os.path.basename(pf)
    if downward_name == "barman-opt14-strips":
        root, ext = os.path.splitext(basename)
        basename = root.replace(".", "-") + ext
    if downward_name in ["zenotravel", "freecell", "depot", "driverlog"]:
        m = re.match("pfile(\d+).pddl", basename)
        if m:
            basename = "p%02d.pddl" % int(m.group(1))

    return os.path.join(BENCHMARKS_DIR, downward_name, basename)

downward_to_planning_domains = defaultdict(dict)

for did, downward_name in planning_domains_to_downward.items():
    probs = map(api.localize, api.get_problems(did))
    for p in probs:
        downward_problem = os.path.basename(translate_problem_filename(downward_name, p['problem_path']))
        downward_to_planning_domains[downward_name][downward_problem] = p["problem_id"]
