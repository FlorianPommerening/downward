#include "binding_generated.h"
#include "binding_static.h"

namespace nb = nanobind;

using namespace std;

NB_MODULE(pydownward, m) {
    unordered_set<string> manually_specified = bind_basic_classes(m);
    bind_feature_classes(m, manually_specified);
    bind_list_classes(m);
    bind_enums(m);
    bind_features(m);
}
