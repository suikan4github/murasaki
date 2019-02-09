/*
 * debuggertxtask.cpp
 *
 *  Created on: 2018/03/02
 *      Author: takemasa
 */

#include <debuggertxtask.hpp>
#include "debuggerfifo.hpp"
#include "murasaki_assert.hpp"

namespace murasaki {

DebuggerTxTask::DebuggerTxTask(const char * task_name,
                               unsigned short stack_depth,
                               UBaseType_t task_priority,
                               const void * task_parameter)
        // Forward initialization parameter to the parent class
        : TaskStrategy(task_name, stack_depth, task_priority, task_parameter)
{
        // Do nothing
}

// Copy data from FIFO to Logger.
void DebuggerTxTask::TaskBody(const void* ptr)
{
    MURASAKI_ASSERT(ptr != nullptr);

    // ptr is regarded as pointer to the LoggingHelpers
    // This struct contains the logger and fifo.
    const murasaki::LoggingHelpers * const helpers = static_cast<const murasaki::LoggingHelpers * const >(ptr);


    while (true) {
        // Copy data from FIFO
        unsigned int copy_size = helpers->fifo->Get(block, sizeof(block));

        // Then, put it to logger, if data exsit.
        if (copy_size != 0)
            helpers->logger->putMessage(reinterpret_cast<char *>(block), copy_size);
    }

}

} /* namespace murasaki */
