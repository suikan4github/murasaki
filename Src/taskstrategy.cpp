/*
 * abstracttask.cpp
 *
 *  Created on: 2018/02/20
 *      Author: Seiichi "Suikan" Horie
 */

#include <taskstrategy.hpp>
#include "murasaki_assert.hpp"

namespace murasaki {

TaskStrategy::TaskStrategy(const char * task_name, unsigned short stack_depth, UBaseType_t priority, const void * parameter)
        : name_(task_name),
          stack_depth_(stack_depth),
          parameter_(parameter),
          priority_(priority)
{
    MURASAKI_ASSERT(nullptr!= task_name);
    MURASAKI_ASSERT(0 != stack_depth);                  // reject only very explict fault.
    MURASAKI_ASSERT(configMAX_PRIORITIES > priority);   // priority is allowed till ( configMAX_PRIORITIES - 1 )
    MURASAKI_ASSERT(0 < priority);                      // priority 0 is idle task
    task_ = 0;
}

TaskStrategy::~TaskStrategy()
{
    vTaskDelete(task_);
}

void TaskStrategy::Start()
{
    // Create a task.
    //
    // Because the real task body of RTOS is not be able to a member function of class,
    // a statick class member function is passed to FreeRTOS.
    // So FreeRTOS API can start the as task.
    // The passed static function calls TaskBody() member funciton, which is
    // defined by user.
    BaseType_t task_result = ::xTaskCreate(TaskStrategy::Launch,         // task entity;
                                           name_,                        // name of task
                                           stack_depth_,                 // stack depth [byte]
                                           this,                         // See AbstractTask::Launch for the details.
            priority_,                    // Execution priority of task
                                           &task_                        // receive the task handle if success.
                                           );
    MURASAKI_ASSERT(task_result != errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY);

}

void TaskStrategy::Resume()
{
    vTaskResume(task_);
}

void TaskStrategy::Suspend()
{
    vTaskSuspend(task_);
}

const char* TaskStrategy::GetName()
{
    return name_;
}

unsigned int TaskStrategy::getStackDepth() {
    return stack_depth_;
}

int TaskStrategy::getStackMinHeadroom()
{
#if ( INCLUDE_uxTaskGetStackHighWaterMark > 0 ) && ( configCHECK_FOR_STACK_OVERFLOW > 0 )
    return uxTaskGetStackHighWaterMark(task_);
#else
    return -1;
#endif
}


// This is a static member function
void TaskStrategy::Launch(void * ptr)
{
    MURASAKI_ASSERT(nullptr != ptr);

    // Enforce the ptr to be AbstractTask * type. This is safe because this class member funciton is called from
    // only @ref AbsutractTask::Start(). And that member function always passes its "this" pointer.
    TaskStrategy * this_ptr = static_cast<TaskStrategy *>(ptr);

    // Call its task body. This is virtual function. Then, while the ptr is enfoced to be AbstractTask type,
    // the TaksBody is always the member function of the callee class ( Descendants of AbstractTask ).
    this_ptr->TaskBody(this_ptr->parameter_);
}


} /* namespace murasaki */

