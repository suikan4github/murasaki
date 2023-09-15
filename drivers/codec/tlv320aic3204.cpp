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
      role_(role),
      pll_source_(pll_source) {}

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

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED
