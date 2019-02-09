/**
 * @file debuggerautoreprinttask.hpp
 *
 * @date 2018/03/02
 * @author takemasa
 */

#ifndef DEBUGGERAUTOREPRINTTASK_HPP_
#define DEBUGGERAUTOREPRINTTASK_HPP_

#include <taskstrategy.hpp>

namespace murasaki {
/**
 * @brief Watching task for auto re-print
 * @details
 * Dedicated task for the murasaki::Debugger.
 *
 * Watches logger input. If any character is received, call the AbstractFifo::ReWind().
 * So, from the view point of user, typing any key to re-print the data on screen.
 *
 * @ingroup MURASAKI_HELPER_GROUP
 */

class DebuggerAutoRePrintTask : public TaskStrategy
{
 public:
    /**
     * @brief Constractor. Task entity is not created here.
     * @param task_name Name of task. Will be passed to task when started.
     * @param stack_depth [Byte]
     * @param task_priority Priority of the task. from 1 to up to configMAX_PRIORITIES -1. The high number is the high priority.
     * @param task_parameter The pointer to the murasaki::LoggingHelpers type variable.
     * @details
     * The task implementation of the auto reprint task.
     * This task is dedicated to the murasaki::Debugger.
     *
     * Basically, this task should have higher prority than other task in the system.
     * Otherwise, the debug message will be kept waiting until higher priority task yields.
     * This is up side down.
     */
    DebuggerAutoRePrintTask(const char * task_name,
                            unsigned short stack_depth,
                            UBaseType_t task_priority,
                            const void * task_parameter);
 private:
    /**
     * @brief Watch the character input then ReWind FIFO
     * @param ptr The pointer to the murasaki::LoggingHelpers type variable.
     *
     */
    virtual void TaskBody(const void * ptr);
};

} /* namespace murasaki */

#endif /* DEBUGGERAUTOREPRINTTASK_HPP_ */
