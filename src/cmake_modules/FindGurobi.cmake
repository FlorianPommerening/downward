# - Find the Gurobi LP solver.
# This code defines the following variables:
#
#  GUROBI_FOUND                 - TRUE if Gurobi was found.
#  GUROBI_INCLUDE_DIRS          - Full paths to all include dirs.
#  GUROBI_LIBRARIES             - Full paths to all libraries.
#  GUROBI_RUNTIME_LIBRARY       - Full path to the dll file on windows
#
# Usage:
#  find_package(Gurobi)
#
# The location of Gurobi can be specified using the environment variable
# or cmake parameter DOWNWARD_GUROBI_ROOT. If different installations
# for release/debug versions of Gurobi are available,they can be
# specified with
#   DOWNWARD_GUROBI_ROOT
#   DOWNWARD_GUROBI_ROOT_RELEASE
#   DOWNWARD_GUROBI_ROOT_DEBUG
# More specific paths are preferred over less specific ones when searching
# for libraries.
#
# Note that the standard FIND_PACKAGE features are supported
# (QUIET, REQUIRED, etc.).

foreach(BUILDMODE "RELEASE" "DEBUG")
    set(GUROBI_HINT_PATHS_${BUILDMODE}
        ${DOWNWARD_GUROBI_ROOT_${BUILDMODE}}
        $ENV{DOWNWARD_GUROBI_ROOT_${BUILDMODE}}
        ${DOWNWARD_GUROBI_ROOT}
        $ENV{DOWNWARD_GUROBI_ROOT}
    )
endforeach()

find_path(GUROBI_INCLUDE_DIRS
    NAMES
    gurobi_c++.h
    HINTS
    ${GUROBI_HINT_PATHS_RELEASE}
    ${GUROBI_HINT_PATHS_DEBUG}
    PATH_SUFFIXES
    include
)

find_library(GUROBI_LIBRARY_RELEASE
    NAMES
    gurobi81
    gurobi_c++
    HINTS
    ${GUROBI_HINT_PATHS_RELEASE}
    PATH_SUFFIXES
    "lib"
)

find_library(GUROBI_LIBRARY_DEBUG
    NAMES
    gurobi81
    gurobi_c++
    HINTS
    ${GUROBI_HINT_PATHS_DEBUG}
    PATH_SUFFIXES
    "lib"
)

if(GUROBI_LIBRARY_RELEASE OR GUROBI_LIBRARY_DEBUG)
    set(GUROBI_LIBRARIES
        optimized ${GUROBI_LIBRARY_RELEASE}
        debug ${GUROBI_LIBRARY_DEBUG}
    )
endif()

# Check if everything was found and set GUROBI_FOUND.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Gurobi
    REQUIRED_VARS GUROBI_INCLUDE_DIRS GUROBI_LIBRARIES
)

mark_as_advanced(
    GUROBI_INCLUDE_DIRS GUROBI_LIBRARIES
    GUROBI_LIBRARY_RELEASE GUROBI_LIBRARY_DEBUG
)
