#! /bin/bash
set -e
for build in cmake make; do
    rm -rf builds/$build-maia
    mkdir -p builds/$build-maia
    scp -r maia:projects/downward/issues/issue67/builds/$build/bin builds/$build-maia/
done
./fast-downward.py --build cmake-maia benchmarks/sokoban-opt11-strips/p09.pddl --search "astar(blind())"
./fast-downward.py --build make-maia benchmarks/sokoban-opt11-strips/p09.pddl --search "astar(blind())"
