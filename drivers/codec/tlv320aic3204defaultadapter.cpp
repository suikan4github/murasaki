/*
 * tlv320aic3204defaultadapter.cpp
 *
 *  Created on: 2023/09/15
 *      Author: Seiichi "Suikan" Horie
 */

#include "tlv320aic3204defaultadapter.hpp"

#include <algorithm>

#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

// Macro for easy-to-read
#define CODEC_SYSLOG(fmt, ...) \
  MURASAKI_SYSLOG(this, kfaAudioCodec, kseDebug, fmt, ##__VA_ARGS__)

#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {

Tlv320aic3204DefaultAdapter::Tlv320aic3204DefaultAdapter(
    murasaki::I2cMasterStrategy *controller,  // I2C master controller
    u_int8_t device_address                   // 7bit I2C address.
    )
    : Tlv320aic3204AdapterStrategy(controller, device_address) {}

void Tlv320aic3204DefaultAdapter::ConfigureAnalog(void) {}

void Tlv320aic3204DefaultAdapter::SetLineInputGain(float left_gain,
                                                   float right_gain,
                                                   bool mute) {}

void Tlv320aic3204DefaultAdapter::SetAuxInputGain(float left_gain,
                                                  float right_gain, bool mute) {
}

void Tlv320aic3204DefaultAdapter::SetLineOutputGain(float left_gain,
                                                    float right_gain,
                                                    bool mute) {}

void Tlv320aic3204DefaultAdapter::SetHpOutputGain(float left_gain,
                                                  float right_gain, bool mute) {
}

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED
