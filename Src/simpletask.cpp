/*
 * task.cpp
 *
 *  Created on: 2019/02/03
 *      Author: Seiichi "Suikan" Horie
 */

#include <simpletask.hpp>

// Pass through the parameters to the person class. Also, set the task body funciton pointer.
murasaki::Task::Task(
                     const char* task_name,
                     unsigned short stack_depth,
                     UBaseType_t task_priority,
                     const void* task_parameter,
                     void (*task_body_func)(const void*))
        :
          murasaki::TaskStrategy(
                                 task_name,
                                 stack_depth,
                                 task_priority,
                                 task_parameter),
          task_body_func_(task_body_func)
                                       {
}

void murasaki::Task::TaskBody(const void* ptr)
                              {
    // Call task body function
    task_body_func_(ptr);
}
