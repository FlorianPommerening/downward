#!/usr/bin/env python

from pathlib import Path
import json
import shutil
import sys

def handle_rundir(run_dir, out_dir):
    props_file = run_dir / "properties"
    props = json.load(props_file.open())
    staticprops_file = run_dir / "static-properties"
    staticprops = json.load(staticprops_file.open())

    domain = staticprops.get("domain", "")
    problem = staticprops.get("problem", "")
    time = float(props.get("total_time", 1800))

    if time > 60:
        shutil.copy(run_dir / "model.mps",
                    out_dir / f"{domain}-{problem}.mps")


def handle_experiment(exp_dir, out_dir):
    for run_dir in exp_dir.glob("runs-*/*"):
        handle_rundir(run_dir, out_dir)

if __name__ == "__main__":
    out_dir = Path("models")
    out_dir.mkdir(exist_ok=True)
    exp_dir = Path(sys.argv[0])
    handle_experiment(exp_dir, out_dir)
