/**
 * @file taskstrategy.hpp
 *
 * @date 2018/02/20
 * @author: Seiichi "Suikan" Horie
 * @brief Mother of All Tasks.
 */


#ifndef TASKSTRATEGY_HPP_
#define TASKSTRATEGY_HPP_

#include <FreeRTOS.h>
#include <task.h>

namespace murasaki {

/**
 * @brief A mother of all tasks.
 * @details
 * Encapsulate a FreeRTOS task.
 *
 * The constructor just stores given parameter internally.
 * And then, these parameter is passed to a task when Start() member function is called.
 * Actual task creation is done inside Start().
 *
 * The destructor deletes the task.
 * Releasing thask from all the resources ( ex: semaphore ) before deleting, is the responsibility of the programmer.
 *
 * Base on the description at http://idken.net/posts/2017-02-01-freertos_task_cpp/
 * @ingroup MURASAKI_ABSTRACT_GROUP
 */
class TaskStrategy
{
 public:
    TaskStrategy() = delete;
    /**
     * @brief Constractor. Task entity is not created here.
     * @param task_name Name of task. Will be passed to task when started.
     * @param stack_depth [Byte]
     * @param task_priority Priority of the task. from 1 to up to configMAX_PRIORITIES -1. The high number is the high priority.
     * @param task_parameter Optional parameter to the task.
     */
    TaskStrategy(const char * task_name, unsigned short stack_depth, UBaseType_t task_priority, const void * task_parameter);
    /**
     * @brief Destructor
     */
    virtual ~TaskStrategy();
    /**
     * @brief Create a task and run it.
     * @details
     * A task is created with given parameter to the constructors and then run.
     */
    void Start();
    /**
     * @brief Resume a task from suspended state.
     */
    void Resume();
    /**
     * @brief Put the task into suspend state.
     */
    void Suspend();
    /**
     * @brief Get a name of task.
     * @return A name of task.
     */
    const char * GetName();

    /**
     * @brief Obtain the size of the stack.
     * @return Total depth of the task stack [byte]
     */
    unsigned int getStackDepth();

    /**
     * @brief Obtain the headroom of the stack.
     * @return The remained headroom in stack [byte]. 0 mean stack is overflown. -1 mean Stack overflow check is not provided.
     * @details
     * Return value is the avairable stack size in byte.
     *
     * Internally, this function uses <a href= "https://www.freertos.org/Stacks-and-stack-overflow-checking.html">
     * Stack Usage and Stack Overflow Checking</a>.
     *
     * Thus,
     * - INCLUDE_uxTaskGetStackHighWaterMark have to be non zero
     * - configCHECK_FOR_STACK_OVERFLOW have to be non zero
     *
     * If above conditions are not met, this function returns -1.
     */
    int getStackMinHeadroom();
 protected:
    TaskHandle_t task_;                 // Task handle of FreeRTOS
    const char * const name_;           // Name of task in FreeRTOS
    const unsigned short stack_depth_;  // Stack depth specification.
    const void * const parameter_;            // Optional parameter to pass the @ref TaskBody().
    const UBaseType_t priority_;        //

    /**
     * @brief Internal use only. Create a task from TaskBody()
     * @param ptr passing "this" pointer.
     */
    static void Launch(void * ptr);
    /**
     * @brief Actual task entity. Must be overridden by programmer.
     * @param ptr Optional parameter to the task body. This ptr is copied from the task_parameter of the Constructor.
     * @details
     * The task body is called only once as task entity.
     * Programmer have to override this member function with his/her own TaskBody().
     *
     * From this member function, class members are able to access.
     */
    virtual void TaskBody(const void * ptr) = 0;
};

} /* namespace murasaki */

#endif /* TASKSTRATEGY_HPP_ */
