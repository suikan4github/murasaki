/**
 * \file task.hpp
 *
 * @date 2019/02/03
 * @author Seiichi "Suikan" Horie
 * @brief Simplified Task class.
 */

#ifndef TASK_HPP_
#define TASK_HPP_

#include <taskstrategy.hpp>

namespace murasaki {

/**
 * \ingroup MURASAKI_GROUP
 * @brief An easy to use task class.
 * @details
 * This is handy class to encapsulate the task creation without inheriting.
 * A task can be created easy like :
 * @code
 *     // For demonstration of FreeRTOS task.
 *     murasaki::platform.task1 = new murasaki::Task(
 *                                                   "Master",
 *                                                   256,
 *                                                   (( configMAX_PRIORITIES > 1) ? 1 : 0),
 *                                                   nullptr,
 *                                                   &TaskBodyFunction
 *                                                   );
 * @endcode
 *
 * Then, task you can call Start() member function to run.
 *
 * @code
 *    murasaki::platform.task1->Start();
 * @endcode
 *
 */
class Task : public murasaki::TaskStrategy {
 public:
    /**
     * @brief Ease to use task class.
     * @param task_name A name of task. This is relevant to the FreeRTOS's API manner.
     * @param stack_depth Task stack size by byte.
     * @param task_priority The task priority. Max priority is defined by configMAX_PRIOIRTIES in FreeRTOSConfig.h
     * @param task_parameter A pointer to the parameter passed to task.
     * @param task_body_func A pointer to the task body function.
     * @details
     * Create an task object. Given parameters are stored internally. And then passed to the
     * FreeRTOS API when task is started by Start() member function.
     *
     * A task parameter can be passed to task through the task_parameter. This pointer is simply passed
     * to the task body function without modification.
     *
     */
    Task(
         const char * task_name,
         unsigned short stack_depth,
         UBaseType_t task_priority,
         const void * task_parameter,
         void (*task_body_func)(const void *));

 protected:
    /**
     * @brief Task member function.
     * @param ptr The task_parameter parameter of the constructor is passed to this parameter.
     * @details
     * This member function runs as task. In this function, the function passed thorough task_body_func parameter
     * is invoked as actual task body.
     */
    virtual void TaskBody(const void * ptr);

 private:
    /**
     *
     * @brief A storage for the pointer to the task body function.
     */
    void (*task_body_func_)(const void *);
    };

};
#endif /* TASK_HPP_ */
