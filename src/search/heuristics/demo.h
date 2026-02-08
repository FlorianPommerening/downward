#ifndef HEURISTICS_DEMO_H
#define HEURISTICS_DEMO_H

/*
  TODO: This file is just meant to experiment with different default values.
        We should remove it before merging the issue.
*/

#include "../heuristic.h"

#include <optional>
#include <memory>
#include <string>

namespace demo_heuristic {
class DemoHeuristic : public Heuristic {
protected:
    virtual int compute_heuristic(const State &ancestor_state) override;
public:
    DemoHeuristic(
        int int_def1_no_bound,
        int int_definfty_no_bound,
        std::optional<int> int_nodefault,
        int int_def2_bound12,
        int int_def3_bound12,
        double double_def1_no_bound,
        double double_definfty_no_bound,
        std::optional<double> double_nodefault,
        double double_def2_bound12,
        double double_def3_bound12,
        bool bool_deftrue,
        bool bool_deffalse,
        std::optional<bool> bool_nodefault,
        std::string string_defempty,
        std::string string_defhello,
        std::optional<std::string> string_nodefault,
        utils::Verbosity enum_defnormal,
        utils::Verbosity enum_defsilent,
        std::optional<utils::Verbosity> enum_nodefault,
        std::vector<int> listint_defempty,
        std::vector<int> listint_def123,
        std::optional<std::vector<int>> listint_nodefault,
        std::vector<std::vector<int>> listlistint_defempty,
        std::vector<std::vector<int>> listlistint_deflistempty,
        std::vector<std::vector<int>> listlistint_def123456,
        std::optional<std::vector<std::vector<int>>> listlistint_nodefault,
        std::vector<std::shared_ptr<Evaluator>> listeval_defempty,
        std::vector<std::shared_ptr<Evaluator>> listeval_defff,
        std::optional<std::vector<std::shared_ptr<Evaluator>>> listeval_nodefault,
        std::shared_ptr<Evaluator> eval_defff,
        std::shared_ptr<Evaluator> eval_nodefault,
        const std::shared_ptr<AbstractTask> &transform, bool cache_estimates,
        const std::string &description, utils::Verbosity verbosity);
};
}

#endif
