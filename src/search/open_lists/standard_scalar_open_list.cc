// HACK! Ignore this if used as a top-level compile target.
#ifdef OPEN_LISTS_STANDARD_SCALAR_OPEN_LIST_H

#include "../scalar_evaluator.h"
#include "../option_parser.h"

#include <cassert>

using namespace std;


template<class Entry>
OpenList<Entry> *StandardScalarOpenList<Entry>::_parse(OptionParser &parser) {
    parser.add_list_option<ScalarEvaluator *>("evaluators");
    parser.add_option<bool>("pref_only", false,
                            "insert only preferred operators");
    Options opts = parser.parse();

    opts.verify_list_non_empty<ScalarEvaluator *>("evaluators");
    /* NOTE: should size be exactly one? Similar in BucketOpenList.
       And in that case, why was there a parser call to parse a whole
       list in the old version? */

    if (parser.dry_run())
        return 0;
    else
        return new StandardScalarOpenList<Entry>(opts);
}

/*
  Open list indexed by a single int, using FIFO tie-breaking.
  Implemented as a map from int to deques.
*/

template<class Entry>
StandardScalarOpenList<Entry>::StandardScalarOpenList(const Options &opts)
    : OpenList<Entry>(opts.get<bool>("pref_only")),
      size(0),
      evaluator(opts.get_list<ScalarEvaluator *>("evaluators")[0]) {
}

template<class Entry>
StandardScalarOpenList<Entry>::StandardScalarOpenList(
    ScalarEvaluator *eval, bool preferred_only)
    : OpenList<Entry>(preferred_only), size(0), evaluator(eval) {
}


template<class Entry>
StandardScalarOpenList<Entry>::~StandardScalarOpenList() {
}

template<class Entry>
int StandardScalarOpenList<Entry>::insert(const Entry &entry) {
    if (OpenList<Entry>::only_preferred && !last_preferred)
        return 0;
    if (dead_end)
        return 0;
    int key = last_evaluated_value;
    buckets[key].push_back(entry);
    size++;
    return 1;
}

template<class Entry>
Entry StandardScalarOpenList<Entry>::remove_min(vector<int> *key) {
    assert(size > 0);
    typename std::map<int, Bucket>::iterator it;
    it = buckets.begin();
    assert(it != buckets.end());
    if (key) {
        assert(key->empty());
        key->push_back(it->first);
    }
    assert(!it->second.empty());
    Entry result = it->second.front();
    it->second.pop_front();
    if (it->second.empty())
        buckets.erase(it);
    --size;
    return result;
}


template<class Entry>
bool StandardScalarOpenList<Entry>::empty() const {
    return size == 0;
}

template<class Entry>
void StandardScalarOpenList<Entry>::clear() {
    buckets.clear();
    size = 0;
}

template<class Entry>
void StandardScalarOpenList<Entry>::evaluate(int g, bool preferred) {
    get_evaluator()->evaluate(Evaluator::current_evaluation_context, g, preferred);
    last_evaluated_value = get_evaluator()->get_value(Evaluator::current_evaluation_context);
    last_preferred = preferred;
    dead_end = get_evaluator()->is_dead_end(Evaluator::current_evaluation_context);
    dead_end_reliable = get_evaluator()->dead_end_is_reliable();
}

template<class Entry>
bool StandardScalarOpenList<Entry>::is_last_evaluated_dead_end() const {
    return dead_end;
}

template<class Entry>
bool StandardScalarOpenList<Entry>::dead_end_is_reliable() const {
    return dead_end_reliable;
}

template<class Entry>
void StandardScalarOpenList<Entry>::get_involved_heuristics(
    std::set<Heuristic *> &hset) {
    evaluator->get_involved_heuristics(hset);
}
#endif
