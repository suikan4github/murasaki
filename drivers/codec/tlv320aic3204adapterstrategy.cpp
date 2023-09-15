/*
 * tlv320aic3204adapterstrtegy.cpp
 *
 *  Created on: 2023/09/15
 *      Author: Seiichi "Suikan" Horie
 */

#include "tlv320aic3204adapterstrategy.hpp"

#include <algorithm>

#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

// Macro for easy-to-read
#define CODEC_SYSLOG(fmt, ...) \
  MURASAKI_SYSLOG(this, kfaAudioCodec, kseDebug, fmt, ##__VA_ARGS__)

#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {

Tlv320aic3204AdaptorStrategy::Tlv320aic3204AdaptorStrategy(
    murasaki::I2cMasterStrategy *controller  // I2C master controller
) {}

void Tlv320aic3204AdaptorStrategy::SetPage(u_int8_t page_number) {}

void Tlv320aic3204AdaptorStrategy::SendCommand(const uint8_t command[],
                                               int size) {}

void Tlv320aic3204AdaptorStrategy::Reset() {}

void Tlv320aic3204AdaptorStrategy::WaitPllLock(void) {}

void ConfigurePll(uint32_t r,  // numarator
                  uint32_t j,  // integer part of multiply factor
                  uint32_t d,  // fractional part of the multiply factor
                  uint32_t p   // denominator
);

void Tlv320aic3204AdaptorStrategy::ConfigureRole(
    murasaki::Tlv320aic3204::I2sRole) {}

void Tlv320aic3204AdaptorStrategy::ConfigurePllSource(
    murasaki::Tlv320aic3204::PllSource) {}

void Tlv320aic3204AdaptorStrategy::ShutdownPll(void) {}

void Tlv320aic3204AdaptorStrategy::ConfigurePins(bool) {}

void Tlv320aic3204AdaptorStrategy::ConfigureCODEC(void) {}

void Tlv320aic3204AdaptorStrategy::ShutdownCODEC(void) {}

void Tlv320aic3204AdaptorStrategy::ShutdownAnalog(void) {}

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED
