/*
 * exti.cpp
 *
 *  Created on: Feb 29, 2020
 *      Author: takemasa
 */

#include "exti.hpp"

#ifdef HAL_EXTI_MODULE_ENABLED

#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

// Macro for easy-to-read
#define EXTI_SYSLOG(fmt, ...)    MURASAKI_SYSLOG(this, kfaExti, kseDebug, fmt, ##__VA_ARGS__)

namespace murasaki {

Exti::Exti(unsigned int line)
        :
        InterruptStrategy(),
        sync_(new Synchronizer())
{
    EXTI_SYSLOG("Enter with %08X.", line)

    MURASAKI_ASSERT(sync_)

    // Set the given line to hexti_

    HAL_StatusTypeDef stat;

    switch (line) {
        case 1:  // fall through to next line
        case EXTI_LINE_0:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_0);
            line_ = 1;
            break;
        case 2:  // fall through to next line
        case EXTI_LINE_1:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_1);
            line_ = 2;
            break;
        case 4:  // fall through to next line
        case EXTI_LINE_2:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_2);
            line_ = 4;
            break;
        case 8:  // fall through to next line
        case EXTI_LINE_3:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_3);
            line_ = 8;
            break;
        case 16:  // fall through to next line
        case EXTI_LINE_4:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_4);
            line_ = 16;
            break;
        case 32:  // fall through to next line
        case EXTI_LINE_5:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_5);
            line_ = 32;
            break;
        case 64:  // fall through to next line
        case EXTI_LINE_6:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_6);
            line_ = 64;
            break;
        case 128:  // fall through to next line
        case EXTI_LINE_7:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_7);
            line_ = 128;
            break;
        case 256:  // fall through to next line
        case EXTI_LINE_8:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_8);
            line_ = 256;
            break;
        case 512:  // fall through to next line
        case EXTI_LINE_9:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_9);
            line_ = 512;
            break;
        case 1024:  // fall through to next line
        case EXTI_LINE_10:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_10);
            line_ = 1024;
            break;
        case 2048:  // fall through to next line
        case EXTI_LINE_11:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_11);
            line_ = 2048;
            break;
        case 4096:  // fall through to next line
        case EXTI_LINE_12:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_12);
            line_ = 4096;
            break;
        case 8192:  // fall through to next line
        case EXTI_LINE_13:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_13);
            line_ = 8192;
            break;
        case 16384:  // fall through to next line
        case EXTI_LINE_14:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_14);
            line_ = 16384;
            break;
        case 32768:  // fall through to next line
        case EXTI_LINE_15:
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_15);
            line_ = 32768;
            break;
        default:
            MURASAKI_SYSLOG(this, kfaExti, kseError, "Unexpected line. Only 0 to 15 are supported")
            MURASAKI_ASSERT(false)
            break;
    }
    MURASAKI_ASSERT(HAL_OK == stat)

    // Retrieve the interrupt configuraiton from peripheral.
    stat = HAL_EXTI_GetConfigLine(&hexti_, &hconfig_);
    MURASAKI_ASSERT(HAL_OK == stat)

    EXTI_SYSLOG("Exit.")
}

Exti::~Exti()
{
    EXTI_SYSLOG("Enter")

    delete sync_;

    EXTI_SYSLOG("Exit.")
}

void Exti::Enable()
{
    EXTI_SYSLOG("Enter")

    // Set the EXTI with recorded interrupt state. Now, this line is enab.ed
    HAL_StatusTypeDef stat =
            HAL_EXTI_SetConfigLine(&hexti_, &hconfig_);
    MURASAKI_ASSERT(HAL_OK == stat)

    EXTI_SYSLOG("Exit.")
}

void Exti::Disable()
{
    EXTI_SYSLOG("Enter")

    // Clear the EXTI. Now, this line is disabled.
    HAL_StatusTypeDef stat =
            HAL_EXTI_ClearConfigLine(&hexti_);
    MURASAKI_ASSERT(HAL_OK == stat)

    EXTI_SYSLOG("Exit.")
}

murasaki::InterruptStatus Exti::Wait(unsigned int timeout)
                                     {
    EXTI_SYSLOG("Enter")

    bool stat = sync_->Wait(timeout);

    murasaki::InterruptStatus ret_val =
            stat ?
                   murasaki::kisOK :
                   murasaki::kisTimeOut;

    EXTI_SYSLOG("Exit with %d ", ret_val)

    return ret_val;
}

bool Exti::Release(unsigned int line)
                   {
    EXTI_SYSLOG("Enter")

    if (line == line_) {
        EXTI_SYSLOG("Matched")

        sync_->Release();

        EXTI_SYSLOG("Exit.")
        return true;
    }
    else {
        EXTI_SYSLOG("Not Matched")
        EXTI_SYSLOG("Exit.")
        return false;
    }

}

}
/* namespace murasaki */

#endif // HAL_EXTI_MODULE_ENABLED
