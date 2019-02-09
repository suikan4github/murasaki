/**
 * @file debuggertxtask.hpp
 *
 * @date 2018/03/02
 * @author takemasa
 */

#ifndef DEBUGGERTXTASK_HPP_
#define DEBUGGERTXTASK_HPP_

#include <taskstrategy.hpp>

namespace murasaki {
/**
 * @brief Copy task from FIFO to logger
 * @details
 * Dedicated task for the murasaki::Debugger.
 *
 * Receives a murasaki::AbstractLogger type instance and murasaki::AbstractFifo instance
 * through the task_param parameter of the constructor.
 * Then, it is passed to the TaskBody().
 *
 * TaskBody takes data form FIFO and put it into the logger.
 *
 * @ingroup MURASAKI_HELPER_GROUP
 */

class DebuggerTxTask : public TaskStrategy
{
 public:
    /**
     * @brief Constractor. Task entity is not created here.
     * @param task_name Name of task. Will be passed to task when started.
     * @param stack_depth [Byte]
     * @param task_priority Priority of the task. from 1 to up to configMAX_PRIORITIES -1. The high number is the high priority.
     * @param task_parameter The pointer to the murasaki::LoggingHelpers type variable.
     * @details
     * The task implementation of the data copy task.
     * This task is dedicated to the murasaki::Debugger.
     *
     * Basically, this task should have higher prority than other task in the system.
     * Otherwise, the debug message will be kept waiting until higher priority task yields.
     * This is up side down.
     */

    DebuggerTxTask(const char * task_name,
                   unsigned short stack_depth,
                   UBaseType_t task_priority,
                   const void * task_parameter);
 private:
    /**
     * @brief  data backet from FIFO to logger
     */
    uint8_t block[100];

    /**
     * @brief Take data from FIFO and then put to logger.
     * @param ptr The pointer to the murasaki::LoggingHelpers type variable.
     * @details
     * The task body of the data copy task. This task is dedicated to the murasaki::Debugger.
     *
     */
    virtual void TaskBody(const void * ptr);


};

} /* namespace murasaki */

#endif /* DEBUGGERTXTASK_HPP_ */
