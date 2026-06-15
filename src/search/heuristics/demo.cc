/*
  TODO: This file is just meant to experiment with different default values.
        We should remove it before merging the issue.
*/

#include "demo.h"

#include "../plugins/plugin.h"
#include "../task_utils/task_properties.h"
#include "../utils/logging.h"

using namespace std;

namespace std {
ostream &operator<<(ostream &stream, const utils::Verbosity &verbosity);
ostream &operator<<(ostream &stream, const utils::Verbosity &verbosity) {
    if (verbosity == utils::Verbosity::DEBUG) {
        stream << "DEBUG";
    } else if (verbosity == utils::Verbosity::NORMAL) {
        stream << "NORMAL";
    } else if (verbosity == utils::Verbosity::SILENT) {
        stream << "SILENT";
    } else if (verbosity == utils::Verbosity::VERBOSE) {
        stream << "VERBOSE";
    } else {
        stream << "unknown value";
    }
    return stream;
}
ostream &operator<<(ostream &stream, const shared_ptr<Evaluator> &eval);
ostream &operator<<(ostream &stream, const shared_ptr<Evaluator> &eval) {
    if (eval) {
        stream << eval->get_description();
    } else {
        stream << "nullptr";
    }
    return stream;
}

template<typename T>
ostream &operator<<(ostream &stream, const optional<T> &opt) {
    if (opt.has_value()) {
        stream << opt.value();
    } else {
        stream << "nullopt";
    }
    return stream;
}
}


namespace demo_heuristic {
// construction and destruction
DemoHeuristic::DemoHeuristic(
    int int_def1_no_bound,
    int int_definfty_no_bound,
//    int int_defneginfty_no_bound,
    optional<int> int_nodefault,
    int int_def2_bound12,
    int int_def3_bound12,
    double double_def1_no_bound,
    double double_definfty_no_bound,
//    double double_defneginfty_no_bound,
    optional<double> double_nodefault,
    double double_def2_bound12,
    double double_def3_bound12,
    bool bool_deftrue,
    bool bool_deffalse,
    optional<bool> bool_nodefault,
    string string_defempty,
    string string_defhello,
    optional<string> string_nodefault,
    utils::Verbosity enum_defnormal,
    utils::Verbosity enum_defsilent,
    optional<utils::Verbosity> enum_nodefault,
    vector<int> listint_defempty,
    vector<int> listint_def123,
    optional<vector<int>> listint_nodefault,
    vector<vector<int>> listlistint_defempty,
    vector<vector<int>> listlistint_deflistempty,
    vector<vector<int>> listlistint_def123456,
    optional<vector<vector<int>>> listlistint_nodefault,
    vector<shared_ptr<Evaluator>> listeval_defempty,
    vector<shared_ptr<Evaluator>> listeval_defff,
    optional<vector<shared_ptr<Evaluator>>> listeval_nodefault,
    shared_ptr<Evaluator> eval_defff,
    shared_ptr<Evaluator> eval_nodefault,
    const shared_ptr<AbstractTask> &transform,
    bool cache_estimates, const string &description, utils::Verbosity verbosity)
    : Heuristic(transform, cache_estimates, description, verbosity) {
    log << "Initializing Demo heuristic..." << endl;
    cout << boolalpha;
    cout << "int_def1_no_bound: " << int_def1_no_bound << endl;
    cout << "int_definfty_no_bound: " << int_definfty_no_bound << endl;
    cout << "int_nodefault: " << int_nodefault << endl;
    cout << "int_def2_bound12: " << int_def2_bound12 << endl;
    cout << "int_def3_bound12: " << int_def3_bound12 << endl;
    cout << "double_def1_no_bound: " << double_def1_no_bound << endl;
    cout << "double_definfty_no_bound: " << double_definfty_no_bound << endl;
    cout << "double_nodefault: " << double_nodefault << endl;
    cout << "double_def2_bound12: " << double_def2_bound12 << endl;
    cout << "double_def3_bound12: " << double_def3_bound12 << endl;
    cout << "bool_deftrue: " << bool_deftrue << endl;
    cout << "bool_deffalse: " << bool_deffalse << endl;
    cout << "bool_nodefault: " << bool_nodefault << endl;
    cout << "string_defempty: " << string_defempty << endl;
    cout << "string_defhello: " << string_defhello << endl;
    cout << "string_nodefault: " << string_nodefault << endl;
    cout << "enum_defnormal: " << enum_defnormal << endl;
    cout << "enum_defsilent: " << enum_defsilent << endl;
    cout << "enum_nodefault: " << enum_nodefault << endl;
    cout << "listint_defempty: " << listint_defempty << endl;
    cout << "listint_def123: " << listint_def123 << endl;
    cout << "listint_nodefault: " << listint_nodefault << endl;
    cout << "listlistint_defempty: " << listlistint_defempty << endl;
    cout << "listlistint_deflistempty: " << listlistint_deflistempty << endl;
    cout << "listlistint_def123456: " << listlistint_def123456 << endl;
    cout << "listlistint_nodefault: " << listlistint_nodefault << endl;
    cout << "listeval_defempty: " << listeval_defempty << endl;
    cout << "listeval_defff: " << listeval_defff << endl;
    cout << "listeval_nodefault: " << listeval_nodefault << endl;
    cout << "eval_defff: " << eval_defff << endl;
    cout << "eval_nodefault: " << eval_nodefault << endl;
}

int DemoHeuristic::compute_heuristic(const State &) {
    return 0;
}

class DemoHeuristicFeature
    : public plugins::TypedFeature<Evaluator, DemoHeuristic> {
public:
    DemoHeuristicFeature() : TypedFeature("demo") {
        document_title("Demo heuristic");

        add_option<int>("int_def1_no_bound", "", "1");
        add_option<int>("int_definfty_no_bound", "", "infinity");
        add_option<int>("int_nodefault", "", plugins::ArgumentInfo::NO_DEFAULT);
        add_option<int>("int_def2_bound12", "", "2", plugins::Bounds("1", "2"));
        add_option<int>("int_def3_bound12", "", "3", plugins::Bounds("1", "2"));

        add_option<double>("double_def1_no_bound", "", "1");
        add_option<double>("double_definfty_no_bound", "", "infinity");
        add_option<double>("double_nodefault", "", plugins::ArgumentInfo::NO_DEFAULT);
        add_option<double>("double_def2_bound12", "", "2", plugins::Bounds("1", "2"));
        add_option<double>("double_def3_bound12", "", "3", plugins::Bounds("1", "2"));

        add_option<bool>("bool_deftrue", "", "true");
        add_option<bool>("bool_deffalse", "", "false");
        add_option<bool>("bool_nodefault", "", plugins::ArgumentInfo::NO_DEFAULT);

        add_option<string>("string_defempty", "", "\"\"");
        add_option<string>("string_defhello", "", "\"hello\"");
        add_option<string>("string_nodefault", "", plugins::ArgumentInfo::NO_DEFAULT);

        add_option<utils::Verbosity>("enum_defnormal", "", "normal");
        add_option<utils::Verbosity>("enum_defsilent", "", "silent");
        add_option<utils::Verbosity>("enum_nodefault", "", plugins::ArgumentInfo::NO_DEFAULT);

        add_option<vector<int>>("listint_defempty", "", "[]");
        add_option<vector<int>>("listint_def123", "", "[1,2,3]");
        add_option<vector<int>>("listint_nodefault", "", plugins::ArgumentInfo::NO_DEFAULT);

        add_option<vector<vector<int>>>("listlistint_defempty", "", "[]");
        add_option<vector<vector<int>>>("listlistint_deflistempty", "", "[[]]");
        add_option<vector<vector<int>>>("listlistint_def123456", "", "[[1,2,3],[4,5,6]]");
        add_option<vector<vector<int>>>("listlistint_nodefault", "", plugins::ArgumentInfo::NO_DEFAULT);

        add_option<vector<shared_ptr<Evaluator>>>("listeval_defempty", "", "[]");
        add_option<vector<shared_ptr<Evaluator>>>("listeval_defff", "", "[ff()]");
        add_option<vector<shared_ptr<Evaluator>>>("listeval_nodefault", "", plugins::ArgumentInfo::NO_DEFAULT);

        add_option<shared_ptr<Evaluator>>("eval_defff", "", "ff()");
        add_option<shared_ptr<Evaluator>>("eval_nodefault", "", plugins::ArgumentInfo::NO_DEFAULT);

        add_heuristic_options_to_feature(*this, "demo");
    }

    virtual shared_ptr<DemoHeuristic> create_component(
        const plugins::Options &opts) const override {
        return plugins::make_shared_from_arg_tuples<DemoHeuristic>(
            opts.get<int>("int_def1_no_bound"),
            opts.get<int>("int_definfty_no_bound"),
            opts.contains("int_nodefault") ? optional<int>(opts.get<int>("int_nodefault")) : nullopt,
            opts.get<int>("int_def2_bound12"),
            opts.get<int>("int_def3_bound12"),
            opts.get<double>("double_def1_no_bound"),
            opts.get<double>("double_definfty_no_bound"),
            opts.contains("double_nodefault") ? optional<double>(opts.get<double>("double_nodefault")) : nullopt,
            opts.get<double>("double_def2_bound12"),
            opts.get<double>("double_def3_bound12"),
            opts.get<bool>("bool_deftrue"),
            opts.get<bool>("bool_deffalse"),
            opts.contains("bool_nodefault") ? optional<bool>(opts.get<bool>("bool_nodefault")) : nullopt,
            opts.get<string>("string_defempty"),
            opts.get<string>("string_defhello"),
            opts.contains("string_nodefault") ? optional<string>(opts.get<string>("string_nodefault")) : nullopt,
            opts.get<utils::Verbosity>("enum_defnormal"),
            opts.get<utils::Verbosity>("enum_defsilent"),
            opts.contains("enum_nodefault") ? optional<utils::Verbosity>(opts.get<utils::Verbosity>("enum_nodefault")) : nullopt,
            opts.get<vector<int>>("listint_defempty"),
            opts.get<vector<int>>("listint_def123"),
            opts.contains("listint_nodefault") ? optional<vector<int>>(opts.get<vector<int>>("listint_nodefault")) : nullopt,
            opts.get<vector<vector<int>>>("listlistint_defempty"),
            opts.get<vector<vector<int>>>("listlistint_deflistempty"),
            opts.get<vector<vector<int>>>("listlistint_def123456"),
            opts.contains("listlistint_nodefault") ? optional<vector<vector<int>>>(opts.get<vector<vector<int>>>("listlistint_nodefault")) : nullopt,
            opts.get<vector<shared_ptr<Evaluator>>>("listeval_defempty"),
            opts.get<vector<shared_ptr<Evaluator>>>("listeval_defff"),
            opts.contains("listeval_nodefault") ? optional<vector<shared_ptr<Evaluator>>>(opts.get<vector<shared_ptr<Evaluator>>>("listeval_nodefault")) : nullopt,
            opts.get<shared_ptr<Evaluator>>("eval_defff"),
            opts.get<shared_ptr<Evaluator>>("eval_nodefault", nullptr),
            get_heuristic_arguments_from_options(opts));
    }
};

static plugins::FeaturePlugin<DemoHeuristicFeature> _plugin;
}
