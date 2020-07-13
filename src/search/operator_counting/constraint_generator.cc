#include "constraint_generator.h"

#include "../plugin.h"

using namespace std;

namespace operator_counting {
void OperatorCountingConstraint::initialize_constraints(
    vector<lp::LPConstraint> &, double) {
}

static PluginTypePlugin<OperatorCountingConstraintGeneratorBase> _type_plugin(
    "ConstraintGenerator",
    // TODO: Replace empty string by synopsis for the wiki page.
    "");
}
