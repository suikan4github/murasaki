/*
 * debuggerautoreprinttask.cpp
 *
 *  Created on: 2018/03/02
 *      Author: takemasa
 */

#include <debuggerautoreprinttask.hpp>
#include "debuggerfifo.hpp"
#include "murasaki_assert.hpp"

namespace murasaki {

DebuggerAutoRePrintTask::DebuggerAutoRePrintTask(const char * task_name,
                                                 unsigned short stack_depth,
                                                 UBaseType_t task_priority,
                                                 const void * task_parameter)
        // Forward initialization parameter to the parent class
        : TaskStrategy(task_name, stack_depth, task_priority, task_parameter)
{
    // Do nothing
}

// Watch the logger and trigger the rewind.
void DebuggerAutoRePrintTask::TaskBody(const void* ptr)
{
    MURASAKI_ASSERT(ptr != nullptr);

    // ptr is regarded as pointer to the LoggingHelpers
    // This struct contains the logger and fifo.
    const murasaki::LoggingHelpers * const helpers = static_cast<const murasaki::LoggingHelpers * const >(ptr);

    while (true) {
        // Any character?
        helpers->logger->getCharacter();

        // Then rewind.
        helpers->fifo->ReWind();
    }

}


} /* namespace murasaki */
