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

Tlv320aic3204DefaultAdaptor::Tlv320aic3204DefaultAdaptor(
    murasaki::I2cMasterStrategy *controller  // I2C master controller
) {}

void Tlv320aic3204DefaultAdaptor::ConfigureAnalog(void) {}

void Tlv320aic3204DefaultAdaptor::SetLineInputGain(float left_gain,
                                                   float right_gain,
                                                   bool mute = false) {}

void Tlv320aic3204DefaultAdaptor::SetAuxInputGain(float left_gain,
                                                  float right_gain,
                                                  bool mute = false) {}

void Tlv320aic3204DefaultAdaptor::SetLineOutputGain(float left_gain,
                                                    float right_gain,
                                                    bool mute = false) {}

void Tlv320aic3204DefaultAdaptor::SetHpOutputGain(float left_gain,
                                                  float right_gain,
                                                  bool mute = false) {}

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED
