/*
 * murasaki_defs.cpp
 *
 *  Created on: Nov 14, 2019
 *      Author: takemasa
 */

#include "murasaki_defs.hpp"

#if defined(__CORE_CM0_H_GENERIC) || defined(__CORE_CM0PLUS_H_GENERIC)
// The Cortex-M0 and Cortex-M0+ core don't have CYCCNT register.
// Thus, we have to provide alternative function.
// Application developer can override this function by defining murasaki::InitCycleCounter().
// This function is discarded at link phase, if developer has his/her own InitCycleCounter().
__attribute__((weak)) void murasaki::InitCycleCounter()
{
    // There is not free running 32bit counter without bothering application.
    // Then, we just return.
}

#else
void murasaki::InitCycleCounter() {
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
}
#endif

#if defined(__CORE_CM0_H_GENERIC) || defined (__CORE_CM0PLUS_H_GENERIC)
// The Cortex-M0 and Cortex-M0+ core don't have CYCCNT register.
// Thus, we have to provide alternative function.

// Application developer can override this function by defining murasaki::GetCycleCounter().
// This function is discarded at link phase, if developer has his/her own GetCycleCounter().
__attribute__((weak)) unsigned int murasaki::GetCycleCounter()
{
    // There is not free running 32bit counter without bothering application.
    // Then, we just return 0.
    return 0;
}

#else
unsigned int murasaki::GetCycleCounter()
{
    return DWT->CYCCNT;
}
#endif

