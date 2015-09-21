#! /bin/bash
set -e
echo $PATH
rm -rf builds/make
rm -rf builds/cmake

## cmake via Python script:
# ./build.py -j8

## cmake called directly:
# mkdir -p builds/cmake/
# cd builds/cmake
# cmake -DCMAKE_BUILD_TYPE=Release -DUSE_LP=NO ../../src
# make VERBOSE=1 -j8 | tee ../../cmake.log
# cd ../..

## cmake compiler calls run manually:
./cmake-manual.sh

cd src
./build_all distclean
cd ..

## using build_all and make:
# cd src
# ./build_all -j8 | tee ../make.log
# cd ..

## make compiler calls run manually:
./make-manual.sh

mkdir -p builds/make/bin
cp -r src/translate builds/make/bin/
cp src/preprocess/preprocess builds/make/bin/
cp src/search/downward-release builds/make/bin/downward
mv builds/release32 builds/cmake
if [[ "$TESTME" != "" ]]; then
    ./fast-downward.py --build cmake benchmarks/sokoban-opt11-strips/p09.pddl --search "astar(blind())"
    ./fast-downward.py --build make benchmarks/sokoban-opt11-strips/p09.pddl --search "astar(blind())"
fi
