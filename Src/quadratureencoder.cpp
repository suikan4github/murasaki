/*
 * quadratureencoder.cpp
 *
 *  Created on: 2020/02/19
 *      Author: takemasa
 */

#include "quadratureencoder.hpp"
#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

// Macro for easy-to-read
#define ENC_SYSLOG(fmt, ...)    MURASAKI_SYSLOG( this, kfaEncoder, kseDebug, fmt, ##__VA_ARGS__)

namespace murasaki {

#ifdef HAL_TIM_MODULE_ENABLED

QuadratureEncoder::QuadratureEncoder(TIM_HandleTypeDef *htim)
        :
        QuadratureEncoderStrategy(),
        peripheral_(htim)
{
    ENC_SYSLOG("Enter with %p", htim);

    HAL_TIM_Encoder_Start(peripheral_,
                          0);           // We don:t use the comparator. Thus, this can be zero.

    ENC_SYSLOG("Exit");
}

unsigned int QuadratureEncoder::Get()
{
    ENC_SYSLOG("Enter");

    unsigned int ret_val = __HAL_TIM_GET_COUNTER(peripheral_);

    ENC_SYSLOG("Exit with %d", ret_val);
    return ret_val;
}

void QuadratureEncoder::Set(unsigned int value)
                            {
    ENC_SYSLOG("Enter with %d", value);

    __HAL_TIM_SET_COUNTER(peripheral_, value);

    ENC_SYSLOG("Exit");

}

void* QuadratureEncoder::GetPeripheralHandle()
{
    ENC_SYSLOG("Enter");

    void *ret_val = reinterpret_cast<void*>(peripheral_);

    ENC_SYSLOG("Exit with %p", ret_val);

    return ret_val;
}

#endif /* HAL_TIM_MODULE_ENABLED */

} /* namespace murasaki */
