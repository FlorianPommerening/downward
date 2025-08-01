#include "pattern_collection_generator_manual.h"

#include "validation.h"

#include "../task_proxy.h"

#include "../plugins/plugin.h"
#include "../utils/logging.h"

#include <iostream>

using namespace std;

namespace pdbs {
PatternCollectionGeneratorManual::PatternCollectionGeneratorManual(
    const vector<Pattern> &patterns, utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity),
      patterns(make_shared<PatternCollection>(patterns)) {
}

string PatternCollectionGeneratorManual::name() const {
    return "manual pattern collection generator";
}

PatternCollectionInformation PatternCollectionGeneratorManual::compute_patterns(
    const shared_ptr<AbstractTask> &task) {
    if (log.is_at_least_normal()) {
        log << "Manual pattern collection: " << *patterns << endl;
    }
    TaskProxy task_proxy(*task);
    return PatternCollectionInformation(task_proxy, patterns, log);
}

class PatternCollectionGeneratorManualFeature
    : public plugins::TypedFeature<
          PatternCollectionGenerator, PatternCollectionGeneratorManual> {
public:
    PatternCollectionGeneratorManualFeature()
        : TypedFeature("manual_patterns") {
        add_list_option<Pattern>(
            "patterns",
            "list of patterns (which are lists of variable numbers of the planning "
            "task).");
        add_generator_options_to_feature(*this);
    }

    virtual shared_ptr<PatternCollectionGeneratorManual> create_component(
        const plugins::Options &opts) const override {
        return plugins::make_shared_from_arg_tuples<
            PatternCollectionGeneratorManual>(
            opts.get_list<Pattern>("patterns"),
            get_generator_arguments_from_options(opts));
    }
};

static plugins::FeaturePlugin<PatternCollectionGeneratorManualFeature> _plugin;
}
