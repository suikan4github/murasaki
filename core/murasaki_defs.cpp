/*
 * murasaki_defs.cpp
 *
 *  Created on: Nov 14, 2019
 *      Author: takemasa
 */

#include "murasaki_defs.hpp"

__attribute__((weak)) void murasaki::InitCycleCounter()
{
#if defined(__CORE_CM0_H_GENERIC) || defined(__CORE_CM0PLUS_H_GENERIC)
// The Cortex-M0 and Cortex-M0+ core don't have CYCCNT register.
// Thus, we have to provide alternative function.
// Application developer can override this function by defining murasaki::InitCycleCounter().
// This function is discarded at link phase, if developer has his/her own InitCycleCounter().
#else
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;     // Enable trace and debug block.
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;                // Enable cycle count.
    DWT->CYCCNT = 0;                                    // Initialize counter.
#endif
}

__attribute__((weak)) unsigned int murasaki::GetCycleCounter()
{
#if defined(__CORE_CM0_H_GENERIC) || defined (__CORE_CM0PLUS_H_GENERIC)
// The Cortex-M0 and Cortex-M0+ core don't have CYCCNT register.
// Thus, we have to provide alternative function.

// Application developer can override this function by defining murasaki::GetCycleCounter().
// This function is discarded at link phase, if developer has his/her own GetCycleCounter().

// By default, we return 0.
    return 0;

#else
    return DWT->CYCCNT;
#endif

}
