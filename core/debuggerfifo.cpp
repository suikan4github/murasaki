/*
 * debuggerfifo.cpp
 *
 *  Created on: 2018/03/01
 *      Author: Seiichi "Suikan" Horie
 */

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <string.h>

#include "murasaki_defs.hpp"
#include <debuggerfifo.hpp>
#include "murasaki_assert.hpp"

namespace murasaki {

DebuggerFifo::DebuggerFifo(unsigned int buffer_size)
        : FifoStrategy(buffer_size),
          sync_(new Synchronizer())
{
    MURASAKI_ASSERT(sync_ != nullptr);

    // Clean up buffer.
    // This is essential to outptu clear data when ReWind() is called at very initial stage.
    ::memset(buffer_, ' ', size_of_buffer_);

    // Initial condition is considered healthy.
    post_mortem_ = false;
}

DebuggerFifo::~DebuggerFifo()
{
    if (sync_ != nullptr)
        delete sync_;
}

void DebuggerFifo::NotifyData()
{

    sync_->Release();

}

unsigned int DebuggerFifo::Get(uint8_t data[], unsigned int size)
                               {
    unsigned int ret_val;

    if (!post_mortem_) {  // if normal condition
        MURASAKI_ASSERT(! xPortIsInsideInterrupt())

        taskENTER_CRITICAL();
        {
            ret_val = inherited::Get(data, size);
        }
        taskEXIT_CRITICAL();

        // wait for the arriaval of the data.
        if (ret_val == 0)
            sync_->Wait(1000);
    }
    else
        // if undefined exception happend, no sync processing
        ret_val = inherited::Get(data, size);

    return ret_val;

}

void DebuggerFifo::ReWind()
{
    portDISABLE_INTERRUPTS();
    {
        inherited::ReWind();
    }
    portENABLE_INTERRUPTS();
}

void DebuggerFifo::SetPostMortem() {
    post_mortem_ = true;
}

} /* namespace murasaki */

