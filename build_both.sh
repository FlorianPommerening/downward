#! /bin/bash
set -e
rm -rf builds
./build.py -j8
cd src
./build_all distclean
./build_all -j8
cd ..
mkdir -p builds/make/bin
cp -r src/translate builds/make/bin/
cp src/preprocess/preprocess builds/make/bin/
cp src/search/downward-release builds/make/bin/downward
mv builds/release32 builds/cmake
if [[ "$TESTME" != "" ]]; then
    ./fast-downward.py --build cmake benchmarks/sokoban-opt11-strips/p09.pddl --search "astar(blind())"
    ./fast-downward.py --build make benchmarks/sokoban-opt11-strips/p09.pddl --search "astar(blind())"
fi
