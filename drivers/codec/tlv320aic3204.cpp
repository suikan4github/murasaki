/*
 * tlv320aic3204.cpp
 *
 *  Created on: 2023/08/11
 *      Author: Seiichi "Suikan" Horie
 */

#include "tlv320aic3204.hpp"

#include <algorithm>

#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

// Macro for easy-to-read
#define CODEC_SYSLOG(fmt, ...) \
  MURASAKI_SYSLOG(this, kfaAudioCodec, kseDebug, fmt, ##__VA_ARGS__)

#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {

/*
 * Constructor. Initialize the internal variables.
 */
Tlv320aic3204::Tlv320aic3204(
    unsigned int fs,               // Sampling frequency[Hz]
    unsigned int reference_clock,  // Reference signal to the PLL [Hz]
    PllSource pll_source,          // Where PLL reference comes from.
    I2sRole role,                  // I2S signal direction
    murasaki::I2cMasterStrategy *controller,  // I2C master controller
    unsigned int i2c_device_addr              // 7bit I2C device address
    )
    : AudioCodecStrategy(fs),
      reference_clock_(reference_clock),
      i2c_(controller),
      device_addr_(i2c_device_addr),
      role_(role),
      pll_source_(pll_source) {}

/*
 *  Send single command
 *  table : command table :
 *  size : size of table.
 */
void Tlv320aic3204::SendCommand(const uint8_t table[], int size) {
  CODEC_SYSLOG("Enter %p, %d", table, size)

  /*
   * Send the given table to the I2C slave device at device_addr
   */
  i2c_->Transmit(device_addr_, table, size);

  CODEC_SYSLOG("Leave.")
}

/*
 * Send entire command table
 */

void Tlv320aic3204::SendCommandTable(const uint8_t table[][3], int rows) {
  CODEC_SYSLOG("Enter %p, %d", table, rows)

  /*
   * Send all rows of command table.
   */
  for (int i = 0; i < rows; i++) SendCommand(table[i], 3);

  CODEC_SYSLOG("Leave.")
}

void Tlv320aic3204::SetPage(u_int8_t page_number) {
  CODEC_SYSLOG("Enter. page_number : &d .", page_number)

  uint8_t command[] = {
      0,           // Regsiter Address => Page number
      page_number  // value to write
  };

  SendCommand(command, sizeof(command));

  CODEC_SYSLOG("Leave.")
}

void Tlv320aic3204::Reset() {
  CODEC_SYSLOG("Enter.")

  uint8_t command[] = {
      1,  // Regsiter Address => Software reset
      1   // Self clearning software reset
  };

  SetPage(0);                             // Page 0 for softare reset register.
  SendCommand(command, sizeof(command));  // Write to software reset.

  CODEC_SYSLOG("Leave.")
}

// loop while the PLL is not locked.
void Tlv320aic3204::WaitPllLock(void) {
  uint8_t status[6];
  CODEC_SYSLOG("Enter.")
  // Per request of the the TLV320AIC3204 Application Reference Guide ( SLAA557
  // ), we wait 10msec after PLL set. There is not register to display the lock
  // status.
  murasaki::Sleep(10);
  CODEC_SYSLOG("Leave.")
}

// Configure PLL and start. Then, initiate the core and set the CODEC Fs.
void Tlv320aic3204::ConfigurePll(
    uint32_t r,  // numarator
    uint32_t j,  // integer part of multiply factor
    uint32_t d,  // fractional part of the multiply factor
    uint32_t p   // denominator
) {
  CODEC_SYSLOG("Enter. r,j,d,p : %d,%d, %d, %d", r, j, d, p)
  MURASAKI_ASSERT(1 <= r && r <= 4)
  MURASAKI_ASSERT(1 <= j && j <= 63)
  MURASAKI_ASSERT(0 <= d && d <= 9999)
  MURASAKI_ASSERT(1 <= p && p <= 8)

  CODEC_SYSLOG("Leave.")
}

void Tlv320aic3204::ShutdownPll(void) {}

void Tlv320aic3204::ConfigurePins(bool master) {}

void Tlv320aic3204::ConfigureCODEC(void) {}

void Tlv320aic3204::ShutdownCODEC(void) {}

void Tlv320aic3204::ConfigureAnalog(void) {}

void Tlv320aic3204::ShutdownAnalog(void) {}

void Tlv320aic3204::Start(void) {
  CODEC_SYSLOG("Enter.")
#if 0
    // Check if target device exist.
    // send no payload. Just enquire address.
    murasaki::I2cStatus status;
    status = i2c_->Transmit(
                            device_addr_,
                            lock_status_address,
                            0);
    if (status != murasaki::ki2csOK)
            {
        // If it I2C returns NAK, there is no CODEC device on the specific I2C address.
        // Report it
        MURASAKI_SYSLOG(this,
                        kfaAudioCodec,
                        kseCritical,
                        "Given audio codec at I2C address %02x doesn't response or something is wrong",
                        device_addr_);
        MURASAKI_ASSERT(false);
    }

    // then, start of configuration as register address order
    SendCommand(init_core, sizeof(init_core));
    // And then, stop PLL explicitly.
    SendCommand(disable_pll, sizeof(disable_pll));
    // Now, start PLL again as desired frequency.
    ConfigurePll();

    // Set all registers.
    SendCommandTable(
                     config_Tlv320aic3204,
                     sizeof(config_Tlv320aic3204) / 3);  // init Tlv320aic3204 as default state.

    // Set certain signal pass. If it doen't fit to your system, override it by SendCommand()
    SendCommandTable(
                     config_UMB_Tlv320aic3204A,
                     sizeof(config_UMB_Tlv320aic3204A) / 3);  // init UMB-Tlv320aic3204 as default state.

    // Before calling mute() function, set the gain as appropriate value.
    // The value is not problem if it is in the appropriate range.
    // This value is not be able to set by SetGain() member function, because at this moment,
    // Mute is not clealy specified.
    line_input_left_gain_ = 0.0;
    line_input_right_gain_ = 0.0;
    aux_input_left_gain_ = 0.0;
    aux_input_right_gain_ = 0.0;
    line_output_left_gain_ = 0.0;
    line_output_right_gain_ = 0.0;
    hp_output_left_gain_ = 0.0;     // headphone
    hp_output_right_gain_ = 0.0;

    // Mute all channels.
    Mute(murasaki::kccLineInput);
    Mute(murasaki::kccAuxInput);
    Mute(murasaki::kccLineOutput);
    Mute(murasaki::kccHeadphoneOutput);
#endif
  CODEC_SYSLOG("Leave.")
}

/*
 * Set gain of the specific input channel.
 * The mute status is not affected.
 */
void Tlv320aic3204::SetGain(murasaki::CodecChannel channel, float left_gain,
                            float right_gain) {
  CODEC_SYSLOG("Enter. %d, %f, %f ", channel, left_gain, right_gain)
#if 0
    switch (channel)
    {
        case murasaki::kccLineInput:
            line_input_left_gain_ = left_gain;
            line_input_right_gain_ = right_gain;
            SetLineInputGain(line_input_left_gain_, line_input_right_gain_, line_input_mute_);
            break;
        case murasaki::kccAuxInput:
            aux_input_left_gain_ = left_gain;
            aux_input_right_gain_ = right_gain;
            SetAuxInputGain(aux_input_left_gain_, aux_input_right_gain_, aux_input_mute_);
            break;
        case murasaki::kccLineOutput:
            line_output_left_gain_ = left_gain;
            line_output_right_gain_ = right_gain;
            SetLineOutputGain(line_output_left_gain_, line_output_right_gain_, line_output_mute_);
            break;

        case murasaki::kccHeadphoneOutput:
            hp_output_left_gain_ = left_gain;
            hp_output_right_gain_ = right_gain;
            SetHpOutputGain(hp_output_left_gain_, hp_output_right_gain_, hp_output_mute_);
            break;
        default:
            MURASAKI_SYSLOG(this, kfaAudioCodec, kseCritical, "Unknown value in parameter channel : %d ", channel)
            MURASAKI_ASSERT(false)
            break;
    }
#endif
  CODEC_SYSLOG("Leave.")
}

/*
 * Set mute status of specific channels.
 * Channel gains are not affected.
 */
void Tlv320aic3204::Mute(murasaki::CodecChannel channel, bool mute) {
  CODEC_SYSLOG("Enter. %d, %s ", channel, mute ? "true" : "false")
#if 0
    switch (channel)
    {
        case murasaki::kccLineInput:
            line_input_mute_ = mute;
            SetLineInputGain(line_input_left_gain_, line_input_right_gain_, line_input_mute_);
            break;
        case murasaki::kccAuxInput:
            aux_input_mute_ = mute;
            SetAuxInputGain(aux_input_left_gain_, aux_input_right_gain_, aux_input_mute_);
            break;
        case murasaki::kccLineOutput:
            line_output_mute_ = mute;
            SetLineOutputGain(line_output_left_gain_, line_output_right_gain_, line_output_mute_);
            break;

        case murasaki::kccHeadphoneOutput:
            hp_output_mute_ = mute;
            SetHpOutputGain(hp_output_left_gain_, hp_output_right_gain_, hp_output_mute_);
            break;
        default:
            MURASAKI_SYSLOG(this, kfaAudioCodec, kseCritical, "Unknown value in parameter channel : %d ", channel)
            MURASAKI_ASSERT(false)
            break;
    }
#endif
  CODEC_SYSLOG("Leave.")
}

#define DATA 2 /* data payload of register */
#define ADDL 1 /* lower address of register */
#define ADDH 0 /* upper address of register */

#define SET_INPUT_GAIN(x, mute) ((mute) ? 0x00 : (x << 1))
/*
 * This function assumes the single-end input. The gain control is LINNG.
 * See Figure 31 "Record Signal Path" in the Tlv320aic3204 data sheet
 *
 */
void Tlv320aic3204::SetLineInputGain(float left_gain, float right_gain,
                                     bool mute) {
#if 0
    uint8_t txbuf[3];
    uint8_t rxbuf[1];
    int left, right;

    CODEC_SYSLOG("Enter. %d, %d, %d", (int )left_gain, (int )right_gain, mute)
    // set left gain
    left = std::max(left, -12);
    left = std::min(left, 6);
    left = (left_gain + 15) / 3;  // See table 31 LINNG

    // set right gain
    right = std::max(right, -12);
    right = std::min(right, 6);
    right = (right_gain + 15) / 3;  // See table 31 LINNG

    /*
     *  *************** Read Modify light the Left Channel Gain *********************
     */
    // R4 : Record Mixer Left
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x0a;

    // Obtain the Register
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte  */
                              rxbuf, /* Retrieve data buffer */
                              1); /* retrieve a register */
    CODEC_SYSLOG("R4 : 0x%02x", rxbuf[0]);
    // Create a register value
    txbuf[DATA] = (rxbuf[0] & 0xF1) | SET_INPUT_GAIN(left, mute);
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);

    // Set the R4.
    i2c_->Transmit(device_addr_, txbuf, 3);  // R4: Record Mixer Left

    /*
     *  *************** Read Modify light the Right Channel Gain *********************
     */
    // R6 : Record Mixer 2
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x0c;

    // Obtain the Register
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte  */
                              rxbuf, /* Retrieve data buffer */
                              1); /* retrieve a register */
    CODEC_SYSLOG("R6 : 0x%02x", rxbuf[0]);
    // Create a register value
    txbuf[DATA] = (rxbuf[0] & 0xF1) | SET_INPUT_GAIN(right, mute);
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);

    // Set the R4.
    i2c_->Transmit(device_addr_, txbuf, 3);  // R4:  Record Mixer Right
#endif
  CODEC_SYSLOG("Leave.")
}

#define SET_AUX_GAIN(x, mute) ((mute) ? 0x00 : x)

/*
 * This function assumes the input is the single end. Then,
 */
void Tlv320aic3204::SetAuxInputGain(float left_gain, float right_gain,
                                    bool mute) {
#if 0
    uint8_t txbuf[3];
    uint8_t rxbuf[1];
    int left, right;

    CODEC_SYSLOG("Enter. %d, %d, %d", (int )left_gain, (int )right_gain, mute)
    // set left gain LDBOOST is muted.
    left = std::max(left, -12);
    left = std::min(left, 6);
    left = (left_gain + 15) / 3;  // See table 32 MX1AUGXG
    // set right gain. LDBOOST is muted.
    right = std::max(right, -12);
    right = std::min(right, 6);
    right = (right_gain + 15) / 3;  // See table 34 MX1AUGXG

    /*
     *  *************** Read Modify light the Left Channel Gain *********************
     */
    // R5 : Record Mixer Left Control 1
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x0b;

    // Obtain the Register
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte  */
                              rxbuf, /* Retrieve data buffer */
                              1); /* retrieve a register */
    CODEC_SYSLOG("Obtain R5 : 0x%02x", rxbuf[0]);
    // Create a register value
    txbuf[DATA] = (rxbuf[0] & 0xF8) | SET_AUX_GAIN(left, mute);
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);
    // Set the R5.
    i2c_->Transmit(device_addr_, txbuf, 3);  // R5:

    /*
     *  *************** Read Modify light the Right Channel Gain *********************
     */
    // R7 : Record Mixer Right Control 1
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x0d;

    // Obtain the Register
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte  */
                              rxbuf, /* Retrieve data buffer */
                              1); /* retrieve a register */
    CODEC_SYSLOG("Obtain R7 : 0x%02x", rxbuf[0]);
    // Create a register value
    txbuf[DATA] = (rxbuf[0] & 0xF8) | SET_AUX_GAIN(left, mute);
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);
    // Set the R7.
    i2c_->Transmit(device_addr_, txbuf, 3);  // R7:
#endif
  CODEC_SYSLOG("Leave.")
}

#define SET_LO_GAIN(x, unmute, headphone) \
  ((x << 2) | (unmute << 1) | (headphone))

// Read modify right the R31 and R32
void Tlv320aic3204::SetLineOutputGain(float left_gain, float right_gain,
                                      bool mute) {
#if 0
    uint8_t txbuf[3];
    uint8_t rxbuf[1];
    int left, right;

    CODEC_SYSLOG("Enter. %d, %d, %d", (int )left_gain, (int )right_gain, mute)
    // set 0 if mute, set 1 if unmute;
    int unmute_flag = mute ? 0 : 1;

    // Calc left gain setting.
    // LOUTVOL[5:0] = 0  ; -57dB
    // LOUTVOL[5:0] = 63 ; 6dB
    left = left_gain + 57;
    left = std::max(left, 0);
    left = std::min(left, 63);

    // Calc left gain setting.
    // ROUTVOL[5:0] = 0  ; -57dB
    // ROUTVOL[5:0] = 63 ; 6dB
    right = right_gain + 57;
    right = std::max(right, 0);
    right = std::min(right, 63);

    /*
     *  *************** Read Modify light the Left Channel Gain *********************
     */
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x25;  // R31: LOUTVOL

    // Obtain the R31
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte : R31 */
                              rxbuf, /* Retrieve R31 data */
                              1); /* retrieve a register */
    CODEC_SYSLOG("R31 : 0x%02x", rxbuf[0]);

    // Set line out of Left
    txbuf[DATA] = SET_LO_GAIN(
                              left, /* GAIN */
                              unmute_flag, /* UNMUTE */
                              rxbuf[0] & 1); /* LOMODE of R31*/
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);
    // Set LOUTVOL : R31
    i2c_->Transmit(device_addr_, txbuf, 3);

    /*
     *  *************** Read Modify light the Right Channel Gain *********************
     */
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x26;  // R32: ROUTVOL

    // Obtain the R32
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte : R31 */
                              rxbuf, /* Retrieve R32 data */
                              1); /* retrieve a register */
    CODEC_SYSLOG("R32 : 0x%02x", rxbuf[0]);

    txbuf[DATA] = SET_LO_GAIN(
                              right, /* GAIN */
                              unmute_flag, /* UNMUTE */
                              rxbuf[0] & 1); /* ROMODE of R32 */
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);
    // Set ROUTVOL : R32
    i2c_->Transmit(device_addr_, txbuf, 3);
#endif
  CODEC_SYSLOG("Leave.")
}

#define SET_HP_GAIN(x, unmute, headphone) \
  ((x << 2) | (unmute << 1) | (headphone))

// Read modify right the R29 and R30

void Tlv320aic3204::SetHpOutputGain(float left_gain, float right_gain,
                                    bool mute) {
#if 0
    uint8_t txbuf[3];
    uint8_t rxbuf[1];
    int left, right;

    CODEC_SYSLOG("Enter. %d, %d, %d", (int )left_gain, (int )right_gain, mute)
    // set 0 if mute, set 1 if unmute;
    int unmute_flag = mute ? 0 : 1;

    // Calc left gain setting.
    // LHPVOL[5:0] = 0  ; -57dB
    // LHPVOL[5:0] = 63 ; 6dB
    left = left_gain + 57;
    left = std::max(left, 0);
    left = std::min(left, 63);

    // Calc left gain setting.
    // RHPVOL[5:0] = 0  ; -57dB
    // RHPVOL[5:0] = 63 ; 6dB
    right = right_gain + 57;
    right = std::max(right, 0);
    right = std::min(right, 63);

    /*
     *  *************** Read Modify light the Left Channel Gain *********************
     */
    // R29: LHPVOL
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x23;

    // Obtain R29
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte : R29 */
                              rxbuf, /* Retrieve R29 data */
                              1); /* retrieve a register1 */
    CODEC_SYSLOG("R29 : 0x%02x", rxbuf[0]);

    // HP left out control data
    txbuf[DATA] = SET_HP_GAIN(
                              left, /* GAIN */
                              unmute_flag, /* UNMUTE */
                              rxbuf[0] & 1); /* HPEN of R29*/
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);
    // SET LHPVOL : R29
    i2c_->Transmit(device_addr_, txbuf, 3);

    /*
     *  *************** Read Modify light the Left Channel Gain *********************
     */
    // R30 : RHPVOL address
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x24;

    // Obtain R30
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte : R30 */
                              rxbuf, /* Retrieve R30 data */
                              1); /* retrieve a register1 */
    CODEC_SYSLOG("R30 : 0x%02x", rxbuf[0]);

    // HP right out control data
    txbuf[DATA] = SET_HP_GAIN(
                              right, /* GAIN */
                              unmute_flag, /* UNMUTE */
                              rxbuf[0] & 1); /* HPMODE of R30*/
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);
    // SET RHPVOL : R30
    i2c_->Transmit(device_addr_, txbuf, 3);
#endif
  CODEC_SYSLOG("Leave.")
}

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED
