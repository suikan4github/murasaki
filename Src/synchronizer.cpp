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
          semaphore_(xSemaphoreCreateBinary())  // create semaphore as "empty" state
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
    MURASAKI_ASSERT(IsTaskContext());

    // If the timeout_ms is the kmsIndefinitely, pass portMAX_DELAY which means indefinitely.
    // If not, pass the timeout_ms after converting to mS.
    if (murasaki::kwmsIndefinitely == timeout_ms)
        return (pdTRUE == xSemaphoreTake(semaphore_, portMAX_DELAY));
    else
        return (pdTRUE == xSemaphoreTake(semaphore_, timeout_ms / portTICK_PERIOD_MS));

}

void Synchronizer::Release()
{
    if (IsTaskContext())
        ::xSemaphoreGive(semaphore_ );
    else {
        ::xSemaphoreGiveFromISR(semaphore_, nullptr);
        portYIELD_FROM_ISR(true)
    }

}

} /* namespace murasaki */
