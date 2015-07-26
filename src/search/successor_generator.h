#ifndef SUCCESSOR_GENERATOR_H
#define SUCCESSOR_GENERATOR_H

#include "task_proxy.h"
#include "utilities_hash.h"

#include <fstream>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class AbstractTask;
class GeneratorBase;
class GlobalOperator;
class GlobalState;
class OperatorProxy;
class State;

enum class SuccessorGeneratorVariableOrder {
    INPUT,
    RANDOM,
    GREEDY,
    GREEDY_DYNAMIC,
    DOMAIN_SIZE
};

class SuccessorGenerator {
    const std::shared_ptr<AbstractTask> task;
    TaskProxy task_proxy;

    std::unique_ptr<GeneratorBase> root;

    typedef std::vector<FactProxy> Condition;
    GeneratorBase *construct_recursive(
        int switch_var_id, std::list<OperatorProxy> &operator_queue);
    GeneratorBase *construct_dynamic_recursive(
        std::list<OperatorProxy> &operator_queue,
        std::unordered_set<int> variable_queue);

    std::vector<Condition> conditions;
    std::vector<std::unordered_map<int, int>> condition_sets;
    std::vector<int> num_unchecked_conditions;
    std::vector<int> variable_order;
    std::vector<int> variable_order_index;
    std::vector<Condition::const_iterator> next_condition_by_op;
    int num_empty_nodes;
    int num_leaf_nodes;
    int num_switch_nodes;
    int num_nodes;

    SuccessorGenerator(const SuccessorGenerator &) = delete;
public:
    SuccessorGenerator(const std::shared_ptr<AbstractTask> task,
                       SuccessorGeneratorVariableOrder order =
                       SuccessorGeneratorVariableOrder::INPUT);
    ~SuccessorGenerator();

    void generate_applicable_ops(
        const State &state, std::vector<OperatorProxy> &applicable_ops) const;
    // Transitional method, used until the search is switched to the new task interface.
    void generate_applicable_ops(
        const GlobalState &state, std::vector<const GlobalOperator *> &applicable_ops) const;
    void dump_dot();
    void print_statistics();
};

namespace std {

template<>
struct hash<FactProxy > {
    size_t operator()(const FactProxy &fact) const {
        size_t hash = 0;
        hash_combine(hash, fact.get_variable().get_id());
        hash_combine(hash, fact.get_value());
        return hash;
    }
};
}

#endif
