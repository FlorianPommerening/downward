#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>

class AbstractTask;

/*
  Base class for all classes that represent components bound to a specific
  task, like Evaluator, SearchAlgorithm, and OpenList.
*/
class TaskSpecificComponent {
protected:
    /*
      Hold a reference to the task implementation and pass it to objects that
      need it.
    */
    const std::shared_ptr<AbstractTask> unused_task;
public:
    TaskSpecificComponent(const std::shared_ptr<AbstractTask> &task)
        : unused_task(task) {
    }

    virtual ~TaskSpecificComponent() = default;
};

#endif
