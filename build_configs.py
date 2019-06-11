release = ["-DCMAKE_BUILD_TYPE=Release"]
debug = ["-DCMAKE_BUILD_TYPE=Debug"]
releasenolp = ["-DCMAKE_BUILD_TYPE=Release", "-DUSE_LP=NO"]
debugnolp = ["-DCMAKE_BUILD_TYPE=Debug", "-DUSE_LP=NO"]
minimal = ["-DCMAKE_BUILD_TYPE=Release", "-DDISABLE_PLUGINS_BY_DEFAULT=YES"]
states = ["-DCMAKE_BUILD_TYPE=Release", "-DDISABLE_PLUGINS_BY_DEFAULT=YES", "-DPLUGIN_PLUGIN_ASTAR_ENABLED=YES", "-DPLUGIN_BLIND_SEARCH_HEURISTIC_ENABLED=YES"]

DEFAULT = "release"
DEBUG = "debug"
