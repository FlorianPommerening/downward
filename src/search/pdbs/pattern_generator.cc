#include "pattern_generator.h"

#include "utils.h"

#include "../plugins/plugin.h"

using namespace std;

namespace pdbs {
PatternCollectionGenerator::PatternCollectionGenerator(
    utils::Verbosity verbosity)
    : log(utils::get_log_for_verbosity(verbosity)) {
}

PatternCollectionInformation PatternCollectionGenerator::generate(
    const shared_ptr<AbstractTask> &task) {
    if (log.is_at_least_normal()) {
        log << "Generating patterns using: " << name() << endl;
    }
    utils::Timer timer;
    PatternCollectionInformation pci = compute_patterns(task);
    dump_pattern_collection_generation_statistics(name(), timer(), pci, log);
    return pci;
}

PatternGenerator::PatternGenerator(utils::Verbosity verbosity)
    : log(utils::get_log_for_verbosity(verbosity)) {
}

PatternInformation PatternGenerator::generate(
    const shared_ptr<AbstractTask> &task) {
    if (log.is_at_least_normal()) {
        log << "Generating pattern using: " << name() << endl;
    }
    utils::Timer timer;
    PatternInformation pattern_info = compute_pattern(task);
    dump_pattern_generation_statistics(name(), timer.stop(), pattern_info, log);
    return pattern_info;
}

void add_generator_options_to_feature(plugins::Feature &feature) {
    utils::add_log_options_to_feature(feature);
}

tuple<utils::Verbosity> get_generator_arguments_from_options(
    const plugins::Options &opts) {
    return utils::get_log_arguments_from_options(opts);
}

static class PatternCollectionGeneratorCategoryPlugin
    : public plugins::TypedCategoryPlugin<PatternCollectionGenerator> {
public:
    PatternCollectionGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PatternCollectionGenerator") {
        document_synopsis("Factory for pattern collections");
    }
} _category_plugin_collection;

static class PatternGeneratorCategoryPlugin
    : public plugins::TypedCategoryPlugin<PatternGenerator> {
public:
    PatternGeneratorCategoryPlugin() : TypedCategoryPlugin("PatternGenerator") {
        document_synopsis("Factory for single patterns");
    }
} _category_plugin_single;
}
