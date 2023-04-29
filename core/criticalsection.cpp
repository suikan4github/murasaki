/*
 * CriticalSection.cpp
 *
 *  Created on: 2018/01/27
 *      Author: Seiichi "Suikan" Horie
 */

#include "criticalsection.hpp"
#include "murasaki_assert.hpp"

namespace murasaki {

CriticalSection::CriticalSection():
		mutex_(xSemaphoreCreateMutex())            // create semaphore as "empty" state

{
    MURASAKI_ASSERT(nullptr != mutex_)

}

CriticalSection::~CriticalSection()
{
    if ( nullptr != mutex_)
        vSemaphoreDelete(mutex_);
}

void CriticalSection::Enter()
{
    MURASAKI_ASSERT(IsTaskContext());
    bool result = xSemaphoreTake(mutex_, kwmsIndefinitely);
    MURASAKI_ASSERT(result);    // true if xSemaphoreTake() success

}

void CriticalSection::Leave()
{
    MURASAKI_ASSERT(IsTaskContext());
    bool result = xSemaphoreGive(mutex_);
    MURASAKI_ASSERT(result);    // true if xSemaphoreGive() success
}


} /* namespace murasaki */
