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

Tlv320aic3204AdapterStrategy::Tlv320aic3204AdapterStrategy(
    murasaki::I2cMasterStrategy *controller,  // I2C master controller
    uint8_t device_addr)
    : i2c_(controller), device_addr_(device_addr) {}

void Tlv320aic3204AdapterStrategy::SetPage(u_int8_t page_number) {
  CODEC_SYSLOG("Enter. page_number : &d .", page_number)

  uint8_t command[] = {
      0,           // Regsiter Address => Page number
      page_number  // value to write
  };

  SendCommand(command, sizeof(command));

  CODEC_SYSLOG("Leave.")
}

void Tlv320aic3204AdapterStrategy::SendCommand(const uint8_t command[],
                                               int size) {
  CODEC_SYSLOG("Enter %p, %d", command, size)

  /*
   * Send the given table to the I2C slave device at device_addr
   */
  i2c_->Transmit(device_addr_, command, size);

  CODEC_SYSLOG("Leave.")
}

void Tlv320aic3204AdapterStrategy::Reset() {
  CODEC_SYSLOG("Enter.")

  uint8_t command[] = {
      1,  // Regsiter Address => Software reset
      1   // Self clearning software reset
  };

  SetPage(0);                             // Page 0 for softare reset register.
  SendCommand(command, sizeof(command));  // Write to software reset.

  CODEC_SYSLOG("Leave.")
}

void Tlv320aic3204AdapterStrategy::WaitPllLock(void) {
  uint8_t status[6];
  CODEC_SYSLOG("Enter.")
  // Per request of the the TLV320AIC3204 Application Reference Guide ( SLAA557
  // ), we wait 10msec after PLL set. There is not register to display the lock
  // status.
  murasaki::Sleep(10);
  CODEC_SYSLOG("Leave.")
}

void Tlv320aic3204AdapterStrategy::ConfigurePll(
    uint32_t r,  // numerator
    uint32_t j,  // integer part of multiply factor
    uint32_t d,  // fractional part of the multiply factor
    uint32_t p   // denominator
) {}
void Tlv320aic3204AdapterStrategy::ConfigureRole(
    murasaki::Tlv320aic3204::I2sRole) {}

void Tlv320aic3204AdapterStrategy::ConfigurePllSource(
    murasaki::Tlv320aic3204::PllSource) {}

void Tlv320aic3204AdapterStrategy::ShutdownPll(void) {}

void Tlv320aic3204AdapterStrategy::ConfigurePins(bool) {}

void Tlv320aic3204AdapterStrategy::ConfigureCODEC(void) {}

void Tlv320aic3204AdapterStrategy::ShutdownCODEC(void) {}

void Tlv320aic3204AdapterStrategy::ShutdownAnalog(void) {}

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED
