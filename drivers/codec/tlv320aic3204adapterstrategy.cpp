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
      0,           // Register Address => Page number
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
      1,  // Register Address => Software reset
      1   // Self cleaning software reset
  };

  SetPage(0);                             // Page 0 for software reset register.
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

void Tlv320aic3204AdapterStrategy::ConfigureClock(
    murasaki::Tlv320aic3204::I2sRole const role,
    murasaki::Tlv320aic3204::PllSource const pll_source) {
  CODEC_SYSLOG("Enter r:")
  // parameter validation
  // pll_source == BCLK and role == master is not allowed.
  MURASAKI_ASSERT(
      !(pll_source == Tlv320aic3204::kBclk && role == Tlv320aic3204::kMaster))

  // CODEC registers.
  u_int8_t r4 = 0;   // Clock setting register 1
  u_int8_t r27 = 0;  // Register 27: Audio Interface Setting Register 1

  // PLL input
  if (pll_source == Tlv320aic3204::kBclk) r4 |= 1 << 2;  // D3:2

  // CODEC_CLLKIN is connected to PLL output
  r4 |= 3;  // D1:0 CODECK CLock In. 3 => PLL Clock.

  // By reset, DIN and DOUT are connected to ADC and DAC, respectively.

  // BCLK AND WCLK pin
  if (role == Tlv320aic3204::kMaster) {
    r27 |= 1 << 2;  // D2 : WCLK
    r27 |= 1 << 3;  // D3 : BCLK
  }

  // Audio Data Word Length
  r27 |= 3 << 4;  // D5:4 : Audio Data Word Length. 03 => 32bit.

  // Write to registers.
  u_int8_t table[2];

  // Set PLL source, CODEC source.
  table[0] = 4;  // Clock setting register 1
  table[1] = r4;

  SetPage(0);
  SendCommand(table, sizeof(table));

  // Set BCLK, WCLK,  and data word length.
  table[0] = 27;  // Clock setting register 1
  table[1] = r27;
  SendCommand(table, sizeof(table));

  CODEC_SYSLOG("Leave.")
}

void Tlv320aic3204AdapterStrategy::ConfigurePll(
    uint32_t const r,  // numerator
    uint32_t const j,  // integer part of multiply factor
    uint32_t const d,  // fractional part of the multiply factor
    uint32_t const p)  // denominator
{
  CODEC_SYSLOG("Enter r:%d, j:%d, d:%d, p:%d.", r, j, d, p)
  // parameter validation
  MURASAKI_ASSERT(1 <= r && r <= 4)
  MURASAKI_ASSERT(1 <= j && j <= 63)
  MURASAKI_ASSERT(d <= 9999)
  MURASAKI_ASSERT(1 <= p && p <= 8)

  CODEC_SYSLOG("Leave.")
}

void Tlv320aic3204AdapterStrategy::ShutdownPll(void) {}

void Tlv320aic3204AdapterStrategy::ConfigureCODEC(uint32_t const fs) {
  CODEC_SYSLOG("Enter fs : %d.", fs)
  // Parameter validation.
  MURASAKI_ASSERT(fs == 44100 || fs == 88200 || fs == 176400 || fs == 48000 ||
                  fs == 96000 || fs == 192000

  )
  CODEC_SYSLOG("Leave.")
}

void Tlv320aic3204AdapterStrategy::ShutdownCODEC(void) {}

void Tlv320aic3204AdapterStrategy::ShutdownAnalog(void) {}

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED
