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
  MURASAKI_ASSERT(4 <= j && j <= 63)
  MURASAKI_ASSERT(d <= 9999)
  MURASAKI_ASSERT(1 <= p && p <= 8)

  u_int8_t reg5 = 0;  // Clock Setting Register 2, PLL P and R Values
  u_int8_t reg6 = 0;  // Clock Setting Register 3, PLL J Values
  u_int8_t reg7 = 0;  // Clock Setting Register 4, PLL D Values (MSB)
  u_int8_t reg8 = 0;  // Clock Setting Register 5, PLL D Values (LSB)

  // Reg5 Clock Setting Register 2, PLL P and R Values
  reg5 |= 1 << 7;           // D7 : PLL Power. 1=> Up.
  reg5 |= (p & 0x07) << 4;  // D6:4 : PLL P value. Caution : 8 must be set as 0
  reg5 |= r << 0;           // D3:0 : PLL R value;

  // Reg 6 Clock Setting Register 3, PLL J Values
  reg6 |= j << 0;  // D5:0 : PLL J value;

  // Reg 7 Clock Setting Register 4, PLL D Values (MSB)
  reg7 |= d >> 8;  // PLL divider D value (MSB)

  // Reg 8 Clock Setting Register 5, PLL D Values(LSB)
  reg8 |= d & 0xFF;  // PLL divider D value (LSB)

  // Command table for PLL configuration.
  uint8_t pll_table[] = {5,  // First address ( reg 5)
                         reg5, reg6, reg7, reg8};

  // Sending command to configure PLL
  SetPage(0);  // Page 0 for PLL registers.
  SendCommand(pll_table, sizeof(pll_table));

  CODEC_SYSLOG("Leave.")
}

void Tlv320aic3204AdapterStrategy::ShutdownPll(void) {
  CODEC_SYSLOG("Enter.")
  uint8_t command[] = {
      4,  // Register Address => Clock Setting Register
      0   // PLL Power Down
  };

  SetPage(0);                             // Page 0 for software reset register.
  SendCommand(command, sizeof(command));  // Write to PLL power down.

  CODEC_SYSLOG("Leave.")
}

/**
 * We assume the internal PLL clock as :
 * \li 86.016 MHz for Fs==48kHz ( Fs*128*2*7)
 * \li 84.672 MHz for Fs==44.1kHz (Fs*128*3*5)
 *
 * In this function, we set the NDAC,MDAC, to 1, always.
 * The NADC and MADC is left untouchged, and ADC_MOD_CLK is from DAC.
 * And then set the AOSR and DOSR to :
 * \li 128 for Fs=48kHz and 44.1kHz.
 * \li 64 for Fs=96kHz and 88.2kHz.
 * \li 23 for Fs=192kHz and 176.4kHz.
 */
void Tlv320aic3204AdapterStrategy::ConfigureCODEC(uint32_t const fs) {
  CODEC_SYSLOG("Enter fs : %d.", fs)
  // Parameter validation.
  MURASAKI_ASSERT(fs == 44100 || fs == 88200 || fs == 176400 || fs == 48000 ||
                  fs == 96000 || fs == 192000)

  u_int8_t reg11 = 0;  // Clock Setting Register 6, NDAC Values
  u_int8_t reg12 = 0;  // Clock Setting Register 7, MDAC Values
  u_int8_t reg13 = 0;  // Clock Setting Register 4, DOSR (MSB)
  u_int8_t reg14 = 0;  // Clock Setting Register 5, DOSR (LSB)
  u_int8_t reg20 = 0;  // Clock Setting Register 4, AOSR (MSB)

  // Set the MDAC and NDAC divider
  reg11 = 1 << 7 |  // NDAC Divider power up.
          1;        // DNAC = 1;
  reg12 = 1 << 7 |  // MDAC Divider power up.
          1;        // MNAC = 1;

  // We don't set the MADC nor NADC.
  // By leaving MADC power down, ADC_MOD_CLK will use MDAC output.

  switch (fs) {
    case 44100:
      /* Assume PLL clock is 84.672 MHz */
      reg13 = 0;                    // DOSR MSB
      reg14 = 128;                  // DOSR LSB
      SetPage(0);                   // PLL clocks are on the page 0.
      {                             // Command table for PLL configuration.
        uint8_t pll_table[] = {11,  // First register number
                               reg11, reg12, reg13, reg14};
        SendCommand(pll_table, sizeof(pll_table));  // Write to PLL power down.
      }
      reg20 = 0x80;  // AOSR = 128 ( see section 5.2.20 of SLAA577)
      {              // Command table for PLL configuration.
        uint8_t pll_table[] = {20,  // First register number
                               reg20};
        SendCommand(pll_table, sizeof(pll_table));  // Write to PLL power down.
      }
      break;

    case 88200:
      /* Assume PLL clock is 84.672 MHz */
      reg13 = 0;                    // DOSR MSB
      reg14 = 64;                   // DOSR LSB
      SetPage(0);                   // PLL clocks are on the page 0.
      {                             // Command table for PLL configuration.
        uint8_t pll_table[] = {11,  // First register number
                               reg11, reg12, reg13, reg14};
        SendCommand(pll_table, sizeof(pll_table));  // Write to PLL power down.
      }
      reg20 = 0x40;  // AOSR = 64 ( see section 5.2.20 of SLAA577)
      {              // Command table for PLL configuration.
        uint8_t pll_table[] = {20,  // First register number
                               reg20};
        SendCommand(pll_table, sizeof(pll_table));  // Write to PLL power down.
      }
      break;
    case 176400:
      /* Assume PLL clock is 84.672 MHz */
      MURASAKI_ASSERT(false)  // Not implemented yet.
      break;
    case 48000:
      /* Assume PLL clock is 86.016 MHz */
      reg13 = 0;                    // DOSR MSB
      reg14 = 128;                  // DOSR LSB
      SetPage(0);                   // PLL clocks are on the page 0.
      {                             // Command table for PLL configuration.
        uint8_t pll_table[] = {11,  // First register number
                               reg11, reg12, reg13, reg14};
        SendCommand(pll_table, sizeof(pll_table));  // Write to PLL power down.
      }
      reg20 = 0x80;  // AOSR = 128 ( see section 5.2.20 of SLAA577)
      {              // Command table for PLL configuration.
        uint8_t pll_table[] = {20,  // First register number
                               reg20};
        SendCommand(pll_table, sizeof(pll_table));  // Write to PLL power down.
      }
      break;
    case 96000:
      /* Assume PLL clock is 86.016 MHz */
      reg13 = 0;                    // DOSR MSB
      reg14 = 64;                   // DOSR LSB
      SetPage(0);                   // PLL clocks are on the page 0.
      {                             // Command table for PLL configuration.
        uint8_t pll_table[] = {11,  // First register number
                               reg11, reg12, reg13, reg14};
        SendCommand(pll_table, sizeof(pll_table));  // Write to PLL power down.
      }
      reg20 = 0x40;  // AOSR = 64 ( see section 5.2.20 of SLAA577)
      {              // Command table for PLL configuration.
        uint8_t pll_table[] = {20,  // First register number
                               reg20};
        SendCommand(pll_table, sizeof(pll_table));  // Write to PLL power down.
      }
      break;
    case 192000:
      /* Assume PLL clock is 84.016 MHz */
      MURASAKI_ASSERT(false)  // Not implemented yet.
      break;

    default:
      break;
  }

  CODEC_SYSLOG("Leave.")
}

void Tlv320aic3204AdapterStrategy::ShutdownCODEC(void) {
  MURASAKI_ASSERT(false)  // Not implemented yet.
}

void Tlv320aic3204AdapterStrategy::ShutdownAnalog(void) {
  MURASAKI_ASSERT(false)  // Not implemented yet.
}

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED
