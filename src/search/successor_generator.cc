#include "successor_generator.h"

#include "global_state.h"
#include "task_tools.h"
#include "utilities.h"

#include <algorithm>
#include <cassert>
using namespace std;

/* NOTE on possible optimizations:

   * Sharing "GeneratorEmpty" instances might help quite a bit with
     reducing memory usage and possibly even speed, because there are
     bound to be many instance of those. However, it complicates
     deleting the successor generator, and memory doesn't seem to be
     an issue. Speed appears to be fine now, too. So this is probably
     an unnecessary complication.

   * Using slist instead of list led to a further 10% speedup on the
     largest Logistics instance, logistics-98/prob28.pddl. It would of
     course also reduce memory usage. However, it would make the code
     g++-specific, so it's probably not worth it.

*/

class GeneratorBase {
public:
    GeneratorBase() {
        static int id_counter = 0;
        id = id_counter++;
    }
    virtual ~GeneratorBase() = default;
    virtual void generate_applicable_ops(
        const State &state, vector<OperatorProxy> &applicable_ops) const = 0;
    // Transitional method, used until the search is switched to the new task interface.
    virtual void generate_applicable_ops(
        const GlobalState &state, vector<const GlobalOperator *> &applicable_ops) const = 0;
    int id;
    virtual bool dump_dot(ofstream &f) const = 0;
};

class GeneratorSwitch : public GeneratorBase {
    VariableProxy switch_var;
    list<OperatorProxy> immediate_operators;
    vector<GeneratorBase *> generator_for_value;
    GeneratorBase *default_generator;
public:
    virtual ~GeneratorSwitch() override;
    GeneratorSwitch(const VariableProxy &switch_var,
                    list<OperatorProxy> && immediate_operators,
                    const vector<GeneratorBase *> && generator_for_value,
                    GeneratorBase *default_generator);
    virtual void generate_applicable_ops(
        const State &state, vector<OperatorProxy> &applicable_ops) const;
    // Transitional method, used until the search is switched to the new task interface.
    virtual void generate_applicable_ops(
        const GlobalState &state, vector<const GlobalOperator *> &applicable_ops) const;
    bool dump_dot(ofstream &f) const;
};

class GeneratorLeaf : public GeneratorBase {
    list<OperatorProxy> applicable_operators;
public:
    GeneratorLeaf(list<OperatorProxy> && applicable_operators);
    virtual void generate_applicable_ops(
        const State &state, vector<OperatorProxy> &applicable_ops) const;
    // Transitional method, used until the search is switched to the new task interface.
    virtual void generate_applicable_ops(
        const GlobalState &state, vector<const GlobalOperator *> &applicable_ops) const;
    bool dump_dot(ofstream &f) const;
};

class GeneratorEmpty : public GeneratorBase {
public:
    virtual void generate_applicable_ops(
        const State &state, vector<OperatorProxy> &applicable_ops) const;
    // Transitional method, used until the search is switched to the new task interface.
    virtual void generate_applicable_ops(
        const GlobalState &state, vector<const GlobalOperator *> &applicable_ops) const;
    bool dump_dot(ofstream &f) const;
};

GeneratorSwitch::GeneratorSwitch(
    const VariableProxy &switch_var, list<OperatorProxy> && immediate_operators,
    const vector<GeneratorBase *> && generator_for_value,
    GeneratorBase *default_generator)
    : switch_var(switch_var),
      immediate_operators(move(immediate_operators)),
      generator_for_value(move(generator_for_value)),
      default_generator(default_generator) {
}

GeneratorSwitch::~GeneratorSwitch() {
    for (GeneratorBase *generator : generator_for_value)
        delete generator;
    delete default_generator;
}

void GeneratorSwitch::generate_applicable_ops(
    const State &state, vector<OperatorProxy> &applicable_ops) const {
    applicable_ops.insert(applicable_ops.end(),
                          immediate_operators.begin(),
                          immediate_operators.end());
    int val = state[switch_var].get_value();
    generator_for_value[val]->generate_applicable_ops(state, applicable_ops);
    default_generator->generate_applicable_ops(state, applicable_ops);
}

void GeneratorSwitch::generate_applicable_ops(
    const GlobalState &state, vector<const GlobalOperator *> &applicable_ops) const {
    for (OperatorProxy op : immediate_operators) {
        applicable_ops.push_back(op.get_global_operator());
    }
    int val = state[switch_var.get_id()];
    generator_for_value[val]->generate_applicable_ops(state, applicable_ops);
    default_generator->generate_applicable_ops(state, applicable_ops);
}

bool GeneratorSwitch::dump_dot(ofstream &f) const {
    f << "n" << id
      << " [label=\"V " << switch_var.get_id() << "\"];" << endl;

    if (!immediate_operators.empty()) {
        f << "imm_n" << id << " [label=\"";
        for (OperatorProxy op : immediate_operators) {
            f << op.get_id() << ", ";
        }
        f << "\", shape=box];" << endl;
        f << "n" << id << " -> imm_n" << id << ";" << endl;
    }

    for (int val = 0; val < switch_var.get_domain_size(); ++val) {
        if (generator_for_value[val]->dump_dot(f)) {
            f << "n" << id << " -> n" << generator_for_value[val]->id
              << " [label=\"" << val
              << "\"];" << endl;
        }
    }
    if (default_generator->dump_dot(f)) {
        f << "n" << id << " -> n" << default_generator->id
          << " [label=\"*\"];" << endl;
    }
    return true;
}

GeneratorLeaf::GeneratorLeaf(list<OperatorProxy> && applicable_operators)
    : applicable_operators(move(applicable_operators)) {
}

void GeneratorLeaf::generate_applicable_ops(
    const State &, vector<OperatorProxy> &applicable_ops) const {
    applicable_ops.insert(applicable_ops.end(),
                          applicable_operators.begin(),
                          applicable_operators.end());
}

void GeneratorLeaf::generate_applicable_ops(
    const GlobalState &, vector<const GlobalOperator *> &applicable_ops) const {
    for (OperatorProxy op : applicable_operators) {
        applicable_ops.push_back(op.get_global_operator());
    }
}

bool GeneratorLeaf::dump_dot(ofstream &f) const {
    if (!applicable_operators.empty()) {
        f << "n" << id << " [label=\"";
        for (OperatorProxy op : applicable_operators) {
            f << op.get_id() << ", ";
        }
        f << "\", shape=box];" << endl;
        return true;
    }
    return false;
}

void GeneratorEmpty::generate_applicable_ops(
    const State &, vector<OperatorProxy> &) const {
}

void GeneratorEmpty::generate_applicable_ops(
    const GlobalState &, vector<const GlobalOperator *> &) const {
}

bool GeneratorEmpty::dump_dot(ofstream &) const {
    return false;
}

SuccessorGenerator::SuccessorGenerator(const shared_ptr<AbstractTask> task,
                                       SuccessorGeneratorVariableOrder order)
    : task(task),
      task_proxy(*task) {
    num_switch_nodes = 0;
    num_leaf_nodes = 0;
    num_empty_nodes = 0;
    num_nodes = 0;
    OperatorsProxy operators = task_proxy.get_operators();
    VariablesProxy vars = task_proxy.get_variables();
    if (order == SuccessorGeneratorVariableOrder::INPUT
            || order == SuccessorGeneratorVariableOrder::RANDOM
            || order == SuccessorGeneratorVariableOrder::GREEDY
            || order == SuccessorGeneratorVariableOrder::DOMAIN_SIZE) {
        for (VariableProxy var : vars) {
            variable_order.push_back(var.get_id());
        }
        if (order == SuccessorGeneratorVariableOrder::RANDOM) {
            random_shuffle(variable_order.begin(), variable_order.end() );
        } else if (order == SuccessorGeneratorVariableOrder::GREEDY) {
            vector<int> occurrences_in_preconditions(vars.size(), 0);
            for (OperatorProxy op : operators) {
                for (FactProxy pre : op.get_preconditions()) {
                    int pre_var_id = pre.get_variable().get_id();
                    ++occurrences_in_preconditions[pre_var_id];
                }
            }
            sort(variable_order.begin(), variable_order.end(),
                [&](int i, int j) {
                    return occurrences_in_preconditions[i] > occurrences_in_preconditions[j]
                           || (occurrences_in_preconditions[i] == occurrences_in_preconditions[j]
                               && vars[i].get_domain_size() > vars[j].get_domain_size());
                });
        } else if (order == SuccessorGeneratorVariableOrder::DOMAIN_SIZE) {
            vector<int> domain_size(vars.size(), 0);
            for (VariableProxy var : task_proxy.get_variables()) {
                domain_size[var.get_id()] = var.get_domain_size();
            }
            sort(variable_order.begin(), variable_order.end(),
                [&](int i, int j) {
                    return domain_size[i] > domain_size[j];
                });
        }
        variable_order_index.resize(vars.size());
        for (int i = 0; i < (int) variable_order.size(); ++i) {
            variable_order_index[variable_order[i]] = i;
        }

        // We need the iterators to conditions to be stable:
        conditions.reserve(operators.size());
        list<OperatorProxy> all_operators;
        for (OperatorProxy op : operators) {
            Condition cond;
            cond.reserve(op.get_preconditions().size());
            for (FactProxy pre : op.get_preconditions()) {
                cond.push_back(pre);
            }
            // Conditions must be ordered by variable id.
            sort(cond.begin(), cond.end(),
                 [&](const FactProxy &p1, const FactProxy &p2) {
                     return variable_order_index[p1.get_variable().get_id()]
                            < variable_order_index[p2.get_variable().get_id()];
                 });
            all_operators.push_back(op);
            conditions.push_back(cond);
            next_condition_by_op.push_back(conditions.back().begin());
        }

        root = unique_ptr<GeneratorBase>(construct_recursive(0, all_operators));
        release_vector_memory(conditions);
        release_vector_memory(next_condition_by_op);
        release_vector_memory(variable_order);
        release_vector_memory(variable_order_index);
    } else if (order == SuccessorGeneratorVariableOrder::GREEDY_DYNAMIC) {
        // We need the iterators to conditions to be stable:
        condition_sets.reserve(operators.size());
        num_unchecked_conditions.reserve(operators.size());
        list<OperatorProxy> all_operators;
        for (OperatorProxy op : operators) {
            unordered_map<int, int> cond;
            cond.reserve(op.get_preconditions().size());
            for (FactProxy pre : op.get_preconditions()) {
                cond[pre.get_variable().get_id()] = pre.get_value();
            }
            all_operators.push_back(op);
            condition_sets.push_back(cond);
            num_unchecked_conditions.push_back(cond.size());
        }
        unordered_set<int> variable_queue;
        for (VariableProxy var : vars) {
            variable_queue.insert(var.get_id());
        }
        root = unique_ptr<GeneratorBase>(
            construct_dynamic_recursive(all_operators,
            variable_queue)
        );
        release_vector_memory(condition_sets);
    } else {
        ABORT("Unknown variable order");
    }
}

SuccessorGenerator::~SuccessorGenerator() {
}

GeneratorBase *SuccessorGenerator::construct_recursive(
    int switch_var_id, list<OperatorProxy> &operator_queue) {
    if (operator_queue.empty()) {
        ++num_nodes;
        ++num_empty_nodes;
        return new GeneratorEmpty;
    }

    VariablesProxy variables = task_proxy.get_variables();
    int num_variables = variables.size();

    while (true) {
        // Test if no further switch is necessary (or possible).
        if (switch_var_id == num_variables) {
            ++num_nodes;
            ++num_leaf_nodes;
            return new GeneratorLeaf(move(operator_queue));
        }

        VariableProxy switch_var = variables[variable_order[switch_var_id]];
        int number_of_children = switch_var.get_domain_size();

        vector<list<OperatorProxy> > operators_for_val(number_of_children);
        list<OperatorProxy> default_operators;
        list<OperatorProxy> applicable_operators;

        bool all_ops_are_immediate = true;
        bool var_is_interesting = false;

        while (!operator_queue.empty()) {
            OperatorProxy op = operator_queue.front();
            operator_queue.pop_front();
            int op_id = op.get_id();
            assert(op_id >= 0 && op_id < (int)next_condition_by_op.size());
            Condition::const_iterator &cond_iter = next_condition_by_op[op_id];
            assert(cond_iter - conditions[op_id].begin() >= 0);
            assert(cond_iter - conditions[op_id].begin()
                   <= (int)conditions[op_id].size());
            if (cond_iter == conditions[op_id].end()) {
                var_is_interesting = true;
                applicable_operators.push_back(op);
            } else {
                all_ops_are_immediate = false;
                FactProxy fact = *cond_iter;
                if (fact.get_variable() == switch_var) {
                    var_is_interesting = true;
                    while (cond_iter != conditions[op_id].end() &&
                           cond_iter->get_variable() == switch_var) {
                        ++cond_iter;
                    }
                    operators_for_val[fact.get_value()].push_back(op);
                } else {
                    default_operators.push_back(op);
                }
            }
        }

        if (all_ops_are_immediate) {
            ++num_nodes;
            ++num_leaf_nodes;
            return new GeneratorLeaf(move(applicable_operators));
        } else if (var_is_interesting) {
            vector<GeneratorBase *> generator_for_val;
            for (list<OperatorProxy> ops : operators_for_val) {
                generator_for_val.push_back(
                    construct_recursive(switch_var_id + 1, ops));
            }
            GeneratorBase *default_generator = construct_recursive(
                switch_var_id + 1, default_operators);
            ++num_nodes;
            ++num_switch_nodes;
            return new GeneratorSwitch(switch_var,
                                       move(applicable_operators),
                                       move(generator_for_val),
                                       default_generator);
        } else {
            // this switch var can be left out because no operator depends on it
            ++switch_var_id;
            default_operators.swap(operator_queue);
        }
    }
}

GeneratorBase *SuccessorGenerator::construct_dynamic_recursive(
    list<OperatorProxy> &operator_queue,
    unordered_set<int> variable_queue) {
    if (operator_queue.empty()) {
        ++num_nodes;
        ++num_empty_nodes;
        return new GeneratorEmpty;
    }

    VariablesProxy variables = task_proxy.get_variables();
    int num_variables = variables.size();

    while (true) {
        vector<int> occurrences_in_preconditions(num_variables, 0);
        for (OperatorProxy op : operator_queue) {
            const unordered_map<int, int> &cond = condition_sets[op.get_id()];
            for (const pair<int, int> &pre : cond) {
                ++occurrences_in_preconditions[pre.first];
            }
        }
        int switch_var_id = -1;
        int switch_var_value = 0;
        for (int i : variable_queue) {
            if (occurrences_in_preconditions[i] > switch_var_value
                || (occurrences_in_preconditions[i] == switch_var_value
                    && variables[i].get_domain_size() > variables[switch_var_id].get_domain_size())) {
                switch_var_id = i;
                switch_var_value = occurrences_in_preconditions[i];
            }
        }

        // Test if no further switch is necessary (or possible).
        if (switch_var_value == 0) {
            ++num_nodes;
            ++num_leaf_nodes;
            return new GeneratorLeaf(move(operator_queue));
        }
        variable_queue.erase(switch_var_id);

        VariableProxy switch_var = variables[switch_var_id];
        int number_of_children = switch_var.get_domain_size();

        vector<list<OperatorProxy> > operators_for_val(number_of_children);
        list<OperatorProxy> default_operators;
        list<OperatorProxy> applicable_operators;

        bool all_ops_are_immediate = true;
        bool var_is_interesting = false;

        while (!operator_queue.empty()) {
            OperatorProxy op = operator_queue.front();
            operator_queue.pop_front();
            int op_id = op.get_id();
            const auto &it = condition_sets[op_id].find(switch_var_id);
            if (it == condition_sets[op_id].end()) {
                if (num_unchecked_conditions[op_id] == 0) {
                    var_is_interesting = true;
                    applicable_operators.push_back(op);
                } else {
                    default_operators.push_back(op);
                }
            } else {
                all_ops_are_immediate = false;
                var_is_interesting = true;
                operators_for_val[it->second].push_back(op);
                --num_unchecked_conditions[op_id];
            }
        }

        if (all_ops_are_immediate) {
            ++num_nodes;
            ++num_leaf_nodes;
            return new GeneratorLeaf(move(applicable_operators));
        } else if (var_is_interesting) {
            vector<GeneratorBase *> generator_for_val;
            for (list<OperatorProxy> ops : operators_for_val) {
                generator_for_val.push_back(
                    construct_dynamic_recursive(ops, variable_queue));
            }
            GeneratorBase *default_generator =
                construct_dynamic_recursive(default_operators, variable_queue);
            ++num_nodes;
            ++num_switch_nodes;
            return new GeneratorSwitch(switch_var,
                                       move(applicable_operators),
                                       move(generator_for_val),
                                       default_generator);
        } else {
            // this switch var can be left out because no operator depends on it
            ++switch_var_id;
            default_operators.swap(operator_queue);
        }
    }
}

void SuccessorGenerator::dump_dot() {
    ofstream f;
    f.open ("successorgenerator.dot");
    f << "digraph generator {" << endl;
    root->dump_dot(f);
    f << "}" << endl;
    f.close();
}

void SuccessorGenerator::print_statistics() {
    cout << "Successor generator has " << num_nodes << " nodes ["
         << num_switch_nodes << " inner nodes, "
         << num_leaf_nodes << " leaf nodes, "
         << num_empty_nodes << " empty nodes]."
         << endl;
}

void SuccessorGenerator::generate_applicable_ops(
    const State &state, vector<OperatorProxy> &applicable_ops) const {
    root->generate_applicable_ops(state, applicable_ops);
    // HACK for comparability in experiments
    sort(applicable_ops.begin(), applicable_ops.end(),
        [](const OperatorProxy &op1, const OperatorProxy &op2) {
            return (op1.get_id() < op2.get_id());
        });
}


void SuccessorGenerator::generate_applicable_ops(
    const GlobalState &state, std::vector<const GlobalOperator *> &applicable_ops) const {
    root->generate_applicable_ops(state, applicable_ops);
}
