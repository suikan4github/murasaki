/*
 * debug.cpp
 *
 *  Created on: 2018/01/03
 *      Author: takemasa
 */

#include <debugger.hpp>
#include <string.h>
#include <stdarg.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <algorithm>

#include "murasaki_assert.hpp"

// Watch the logger and trigger the rewind.
static void AutoRePrintTaskBody(const void* ptr);


namespace murasaki {


Debugger::Debugger(LoggerStrategy * logger):
		helpers_{
			new murasaki::DebuggerFifo(PLATFORM_CONFIG_DEBUG_BUFFER_SIZE),
			logger
		},
	    tx_task_( new murasaki::DebuggerTxTask("DebugTask",                              // name of task
	                                            PLATFORM_CONFIG_DEBUG_TASK_STACK_SIZE,                        // stack depth
	                                            PLATFORM_CONFIG_DEBUG_TASK_PRIORITY,           // execusion priority of task
	                                            &helpers_                              // parameter to task
	                                            ))

{
    // initialize internal variable;
    MURASAKI_ASSERT(logger != nullptr)
    MURASAKI_ASSERT(helpers_.fifo != nullptr)
    MURASAKI_ASSERT(tx_task_ != nullptr);


    auto_reprint_enable_ = false;
    auto_reprint_task = nullptr;


    tx_task_->Start();

}

Debugger::~Debugger()
{
    // Delete task;
    if (tx_task_ != nullptr)
        delete tx_task_;

    // Delete task;
    if (auto_reprint_task != nullptr)
        delete auto_reprint_task;

    // Delete FIFO
    if (helpers_.fifo != nullptr)
        delete helpers_.fifo;
}

void Debugger::Printf(const char * fmt, ...)
{
        // obtain variable parameter list
        va_list argp;

        MURASAKI_ASSERT(nullptr != fmt);// nullptr check. Perhaps, overkill.

        portDISABLE_INTERRUPTS();// ARM dependent API. OK to use in task and ISR.
        {
#if 0
            // Test the remains of task stack in byte.
            // To use the uxTaskGetStackHighWaterMark(), Add following to the
            // FreeRTOSConfig.h
            // #define INCLUDE_uxTaskGetStackHighWaterMark 1

            ::snprintf((char *) line_,
                    sizeof(line_) - 1,
                    "stack remains : %8d bytes. ",
                    (int) uxTaskGetStackHighWaterMark(tx_task_));
            AppendToBuffer();
#endif
            ::va_start(argp, fmt);
            // convert the number with formt string to the line string.
            // The string length have to be N - 1. Where N is the length of the destination variable.
            ::vsnprintf((char *) line_, sizeof(line_) - 1, fmt, argp);

            ::va_end(argp);
            // Append the line to the buffer to be sent.
            helpers_.fifo->Put(reinterpret_cast<uint8_t*>(line_), ::strlen(line_));

        }
        portENABLE_INTERRUPTS();

        // Notify to the consumer task, the new data has come.
    helpers_.fifo->NotifyData();

}

char Debugger::GetchFromTask()
{
    MURASAKI_ASSERT(IsTaskContext());

    return helpers_.logger->getCharacter();
}



/*
 * This function is not protected from interrupt.
 * There is no easy way to solve this problem.
 */
void Debugger::RePrint()
{
    helpers_.fifo->ReWind();
}

void Debugger::AutoRePrint()
{
    MURASAKI_ASSERT(IsTaskContext());
    // protecting from double task creation
    if (auto_reprint_enable_)
        return;
    else
    {
        auto_reprint_task = new murasaki::Task("DebugTask",  // name of task
                PLATFORM_CONFIG_DEBUG_TASK_STACK_SIZE,       // stack depth
                PLATFORM_CONFIG_DEBUG_TASK_PRIORITY,         // execusion priority of task
                &helpers_,                                   // parameter to task
                &AutoRePrintTaskBody
                );
        MURASAKI_ASSERT(auto_reprint_task != nullptr);

        auto_reprint_task->Start();

        auto_reprint_enable_ = true;
    }
}

void Debugger::DoPostMortem() {
	// Call post mortem processing. Never return.
	helpers_.logger->DoPostMortem( helpers_.fifo);
}


} /* namespace platform */


/**
 * Keep watching the input from the loggin device. If some input comes, call the RwWind()
 * member function to flash out the message FIFO.
 */
static void AutoRePrintTaskBody(const void* ptr)
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
