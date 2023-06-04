/*
 * synchronizer.cpp
 *
 *  Created on: 2018/01/26
 *      Author: Seiichi "Suikan" Horie
 */

#include <synchronizer.hpp>
#include "murasaki_assert.hpp"

namespace murasaki {

Synchronizer::Synchronizer()
        :
          // Create a semaphore as "empty" state.
          // Because it is empty, task is blocked if a task take that semaphore.
          semaphore_(xSemaphoreCreateBinary())
{
    MURASAKI_ASSERT(semaphore_ != nullptr)
}

Synchronizer::~Synchronizer()
{
    if (semaphore_ != nullptr)
        vSemaphoreDelete(semaphore_);
}

bool Synchronizer::Wait(unsigned int timeout_ms)
                        {
    MURASAKI_ASSERT(! murasaki::IsInsideInterrupt());

    // If the timeout_ms is the kmsIndefinitely, pass portMAX_DELAY to wait indefinitely.
    // If not, pass the timeout_ms after converting to tick to wait desired duration.
    if (murasaki::kwmsIndefinitely == timeout_ms)
        return (pdTRUE == xSemaphoreTake(semaphore_, portMAX_DELAY));
    else
        return (pdTRUE == xSemaphoreTake(semaphore_, timeout_ms / portTICK_PERIOD_MS));

}

void Synchronizer::Release()
{
    // The FreeRTOS API is context dependent.
    // To work correctly, we have to refer the context informaiton.
    if (! murasaki::IsInsideInterrupt())
        ::xSemaphoreGive(semaphore_ );
    else {
        ::xSemaphoreGiveFromISR(semaphore_, nullptr);
        // This is essential part to trigger the context switch right after returning from ISR.
        // Without calling this macro, context switch happens only at the next context switch request in task.
        // This is very confusing design of FreeRTOS.
        portYIELD_FROM_ISR(true);
    }

}

} /* namespace murasaki */
