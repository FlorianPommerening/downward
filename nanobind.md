# Installing Nanobind

To use the code, install nanobind in a venv:
```bash
python -m venv venv
source venv/bin/activate
pip install nanobind
```

I think you also need the dev package of Python (if someone doesn't already have
this installed, maybe try without it first, to see if it is needed).
```bash
sudo apt install python3-dev
```

# Trying Out the Python Interface

Compile the code as usual (see #Issues below if you get a build error)
```bash
./build.py
```
This will first build the planner, and then the Python interface. The second
build is enabled by a CMake option that is true by default in this prototype but
probably should be false by default once we merge.

The created file
(`builds/release/search/pydownward.cpython-312-x86_64-linux-gnu.so`) is inside
the build directory, in the long run, we'll have to install it in some way that
Python can find it. For now, we can just start the Python session in the right
place. Before that, use the translator to get a `*.sas` file.
```bash
./fast-downward.py --translate $DOWNWARD_BENCHMARKS/miconic/s1-0.pddl
mv output.sas builds/release/search/
cd builds/release/search/
python
```

Inside the Python session, you can now import the library, ask for help, and use
it. Note that you have to read in the task before creating other features. This
is a hack that will disappear once we finish implementing the task-independent
classes. Many arguments have default values, but not all of them, so you have to
explicitly specify arguments like `transform`.
```Python
import pydownward
help(pydownward) # press Q to exit help view

from pydownward import *
help(lmcut) # press Q to exit help view

read_task("output.sas")
hff = ff()
s = eager_greedy([hff])
s.search()
plan = s.get_plan()
task = no_transform()
print([task.get_operator_name(opid.get_index(), False) for opid in plan])
```

# Issues

Compiling with SoPlex can fail depending on how SoPlex was built. If you get a
warning involving `-fPIC`, you'll have to recompile SoPlex with this flag:

```bash
sudo apt install libgmp3-dev
export soplex_DIR=/opt/soplex # or wherever you want to install it.
git clone --depth 1 --branch release-710 https://github.com/scipopt/soplex.git
cmake -DCMAKE_INSTALL_PREFIX="$soplex_DIR" -DCMAKE_CXXFLAGS="-fPIC" -S soplex -B build
cmake --build build
sudo cmake --install build
rm -rf soplex build
```

# Description of the Implementation

## Changes in the Existing Planner

* The planner got new files `python/generate.{h,cc}` and has minor changes in
  `planner.cc` and a new command line option `--create-python-binding-code` to
  generate the Python binding code.
* The parser exposes new functionality to parse a value and convert it into a
  given type.
* The registry is now stored in the RawRegistry to avoid constructing it twice
  (we will split this off as its own issue).
* The remaining changes in the planner are mostly concerned with getting names
  from registered types. Those changes should be considered again before merging
  (see TODO in the diff) but are non-intrusive. There are also new methods in
  the type registry to access registered list and enum types. Finally, the
  method `get_type_name` changed because it returned typenames relative to its
  own namespace (e.g. `Verbosity` instead of `utils::Verbosity` because
  `get_type_name` was also in `utils`).

The planner remains independent of nanobind, so if the option to build the
Python interface is disabled, the planner can be build as usual, even without
nanobind installed.

## Changes in the Build System

In `src/search/CMakeLists.txt`, there is a new option `COMPILE_PYTHON_INTERFACE`
to control if the python interface is built. This has to be done in two passes,
because the downward binary is needed to generate the binding code. We also
introduce a new interface library called `downward_lib`. Instead of adding all
files, includes, etc. to the target `downward` directly, we now add them to
`downward_lib` and add `downward_lib` as a dependency to `downward`. This
indirection means we can easily add the same dependencies to the Python package.

The bulk of the new code is at the end of the file (it has to come after all
calls to `create_fast_downward_library` to find the right header files). It
works in four steps:
1. Figures out where nanobind is (it assumes it is installed through pip).
2. Generate `binding_generated.cc` by calling the planner with the new option
   `--create-python-binding-code`.
3. Generate `binding_generated_private.h` by looping through the files added to
   `downward_lib` and creating an include for every header file. In the long
   run, we don't want every header here, but filter for a specific naming
   pattern, then make sure by convention that the headers we have to include for
   the binding code fit that pattern.
4. Create a CMake target for the Python binding code from this generated code
   and some static code in `src/search/python`.

## Description of the Binding Code

The binding code is in `src/search/python` and in some dynamically generated
files that will be stored in the builds directory (see above).

* `src/search/python/generated.cc` is the entry point to the code, it creates
  the Python module and calls functions from `binding_static.h` and
  `binding_generated.h` to register classes and functions.
* `src/search/python/binding_static.{h,cc}` register classes that are not part
  of the registry (like `OperatorID`, `FactPair`, etc.) here, we define what
  functionality we want to expose in the interface. So far, this is minimal but
  it would be easy to extend. We also have classes here that are part of the
  registry but where we want to expose functionality, for example, for
  `SearchAlgorithm`, we want to expose methods `search()` and `get_plan()`. We
  add them to a list to later avoid registering them a second time. The class
  `AbstractTask` shows how we can use a "trampoline class" to support virtual
  functions and inheritance across the Python/C++ boundary. You can inherit from
  `AbstractTask` in Python, override its methods and create a task in Python
  this way. In theory, this task could then be used in a search, but our search
  classes still use the global task.
* `src/search/python/exception.{h,cc}` contains an exception class that is also
  exported in the static code above. With it, the C++ binding code can throw
  exceptions that are then raised as Python exceptions in the Python session.
* `src/search/python/binding_utils.{h,cc}` contains functions to access default
  values, bounds, and documentation of features. These are used in the generated
  code.
* `src/search/python/binding_generated.h` contains the interface to the code
  generated on the fly. This generated code has four functions:
  1. `bind_feature_classes()` will expose base classes of our features to Python
    (`Evaluator`, `SearchAlgorithm`, `LandmarkFactory`, and so on). The classes
    do not offer any functionality in Python. Any class that was already
    manually specified with a richer interface is skipped here.
  2. `bind_list_classes()` will expose all list classes used somewhere in
    arguments. For example, eager search takes a list of evaluators and this is
    a `vector<shared_prt<Evaluator>>` on the C++ side. On the Python side, it
    will look like a class `ListOfEvaluator` but this class behaves like a list
    and wraps a C++ vector in an efficient way. Conversions from normal Python
    lists are transparent and this setup avoids copies when crossing the
    language boundary.
  3. `bind_enums()` exposes all enums we use to Python. For example, verbosity
    can be specified as `Verbosity.debug`. Documentation for the individual
    values is also exported.
  4. `bind_features()` loops over all features (`lmcut`, `eager_greedy`,
    `astar`, ...) and creates one Python function for each one. In each case, we
    create a C++ lambda function that takes the right parameters, creates and
    returns an object using the `construct` function of the feature. This
    function is then exposed to Python with the documentation of the feature and
    some default values. This part (default values) is still under construction.
