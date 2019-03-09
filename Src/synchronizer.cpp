/*
 * synchronizer.cpp
 *
 *  Created on: 2018/01/26
 *      Author: Seiichi "Suikan" Horie
 */

#include <synchronizer.hpp>
#include "murasaki_assert.hpp"

namespace murasaki {

Synchronizer::Synchronizer():
		semaphore_ (xSemaphoreCreateBinary())      // create semaphore as "empty" state
{

    MURASAKI_ASSERT(semaphore_ != nullptr)
}

Synchronizer::~Synchronizer()
{
    if (semaphore_ != nullptr)
        vSemaphoreDelete(semaphore_);
}

bool Synchronizer::Wait(WaitMilliSeconds timeout_ms)
{
    MURASAKI_ASSERT(IsTaskContext());
    return (pdTRUE == xSemaphoreTake(semaphore_, timeout_ms / portTICK_PERIOD_MS));

}

void Synchronizer::Release()
{
    if (IsTaskContext())
        ::xSemaphoreGive(semaphore_ );
    else
        ::xSemaphoreGiveFromISR(semaphore_, nullptr);

}

} /* namespace murasaki */
