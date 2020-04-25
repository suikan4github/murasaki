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
#include "exticallbackrepositorysingleton.hpp"

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
#if (EXTI_LINE_0 != 1 )
        case 1:  // fall through to next line
#endif
        case EXTI_LINE_0:
            line_ = 1;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_0);
            break;
#if (EXTI_LINE_1 != 2 )
        case 2:  // fall through to next line
#endif
        case EXTI_LINE_1:
            line_ = 2;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_1);
            break;
#if (EXTI_LINE_2 != 4 )
        case 4:  // fall through to next line
#endif
        case EXTI_LINE_2:
            line_ = 4;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_2);
            break;
#if (EXTI_LINE_3 != 8 )
        case 8:  // fall through to next line
#endif
        case EXTI_LINE_3:
            line_ = 8;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_3);
            break;
#if (EXTI_LINE_4 != 16 )
        case 16:  // fall through to next line
#endif
        case EXTI_LINE_4:
            line_ = 16;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_4);
            break;
#if (EXTI_LINE_5 != 32 )
        case 32:  // fall through to next line
#endif
        case EXTI_LINE_5:
            line_ = 32;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_5);
            break;
#if (EXTI_LINE_6 != 64 )
        case 64:  // fall through to next line
#endif
        case EXTI_LINE_6:
            line_ = 64;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_6);
            break;
#if (EXTI_LINE_7 != 128 )
        case 128:  // fall through to next line
#endif
        case EXTI_LINE_7:
            line_ = 128;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_7);
            break;
#if (EXTI_LINE_8 != 256 )
        case 256:  // fall through to next line
#endif
        case EXTI_LINE_8:
            line_ = 256;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_8);
            break;
#if (EXTI_LINE_9 != 512 )
        case 512:  // fall through to next line
#endif
        case EXTI_LINE_9:
            line_ = 512;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_9);
            break;
#if (EXTI_LINE_10 != 1024 )
        case 1024:  // fall through to next line
#endif
        case EXTI_LINE_10:
            line_ = 1024;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_10);
            break;
#if (EXTI_LINE_11 != 2048 )
        case 2048:  // fall through to next line
#endif
        case EXTI_LINE_11:
            line_ = 2048;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_11);
            break;
#if (EXTI_LINE_12 != 4096 )
        case 4096:  // fall through to next line
#endif
        case EXTI_LINE_12:
            line_ = 4096;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_12);
            break;
#if (EXTI_LINE_13 != 8192 )
        case 8192:  // fall through to next line
#endif
        case EXTI_LINE_13:
            line_ = 8192;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_13);
            break;
#if (EXTI_LINE_14 != 16384 )
        case 16384:  // fall through to next line
#endif
        case EXTI_LINE_14:
            line_ = 16384;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_14);
            break;
#if (EXTI_LINE_15 != 32768 )
        case 32768:  // fall through to next line
#endif
        case EXTI_LINE_15:
            line_ = 32768;
            stat = HAL_EXTI_GetHandle(&hexti_, EXTI_LINE_15);
            break;
        default:
            MURASAKI_SYSLOG(this, kfaExti, kseError, "Unexpected line. Only 0 to 15 are supported")
            MURASAKI_ASSERT(false)
            break;
    }
    MURASAKI_ASSERT(HAL_OK == stat)

    // Retrieve the interrupt configuration from peripheral.
    stat = HAL_EXTI_GetConfigLine(&hexti_, &hconfig_);
    MURASAKI_ASSERT(HAL_OK == stat)

    // Register this object to the list of the interrupt handler class.
    ExtiCallbackRepositorySingleton::GetInstance()->AddExtiObject(this);

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

    // The first wait() calling turn the interrupt ready.
    ready_ = true;
    // wait for interrupt.
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

    if (Match(line)) {
        EXTI_SYSLOG("Matched and release")

        sync_->Release();

        EXTI_SYSLOG("Exit with true.")
        return true;
    }
    else {
        EXTI_SYSLOG("Not Matched")
        EXTI_SYSLOG("Exit with false.")
        return false;
    }

}

bool Exti::Match(unsigned int line)
                 {
    EXTI_SYSLOG("Enter")

    if (line == line_) {
        EXTI_SYSLOG("Matched. Exit with true")
        return true;
    }
    else {
        EXTI_SYSLOG("Not Matched. Exit with false")
        return false;
    }

}

bool Exti::isReady()
{
    MURASAKI_SYSLOG(nullptr,
                    kfaExti,
                    kseDebug,
                    "Enter")

    MURASAKI_SYSLOG(nullptr,
                    kfaExti,
                    kseDebug,
                    "Exit with %s",
                    ready_ ?
                             "true" :
                             "false");

    return ready_;
}

bool Exti::ready_ = false;

}

/* namespace murasaki */

#endif // HAL_EXTI_MODULE_ENABLED
