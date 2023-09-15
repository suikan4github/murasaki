/**
 * @file tlv320aic3204.hpp
 *
 * @date 2023/08/11
 * @author: Seiichi "Suikan" Horie
 */

#ifndef MURASAKI_TP_TLV320AIC3204_HPP_
#define MURASAKI_TP_TLV320AIC3204_HPP_

#include <audiocodecstrategy.hpp>

#include "i2cmasterstrategy.hpp"

#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {

class Tlv320aic3204AdaptorStrategy;

/**
 * @ingroup MURASAKI_THIRDPARTY_GROUP
 * \brief Audio Codec LSI class
 * @details
 * Initialize the Tlv320aic3204 codec based on the given parameter.
 */

class Tlv320aic3204 : public AudioCodecStrategy {
 public:
  /**
   * \brief  Sync signal direction of the I2S interface.
   */
  enum I2sRole {
    kMaster, /**< Sync signals are output*/
    kSlave   /**< Sync singals are input */
  };

  /**
   * @breif specify the reference signal source of the PLL.
   */
  enum PllSource {
    kMclk,  // MCLK pin.
    kBclk   // BCLK pin.
  };

  // Surpressing default constructor.
  Tlv320aic3204() = delete;

  /**
   * \brief constructor.
   * \param fs Sampling frequency[Hz]
   * @param master_clock Input master clock frequency to the MCLK pin[Hz]
   * \param controller Pass the I2C controller object.
   * \param role Signal direction of the I2s Interface. kMaster : output,
   * kSlave : input.
   * \param i2c_device_addr I2C device address. value range is
   * from 0 to 127
   * \details initialize the internal variables. This constructor
   * assumes the codec receive a master clock from outside. And output the
   * I2C        clocks as clock master.
   *
   *   The fs parameter is the sampling frequency of the CODEC in Hz. This
   * parameter is limited as one of the following :
   *   @li 48000
   *   @li 96000
   *   @li 192000
   *   @li 44100
   *   @li 88200
   *   @li 176400
   *
   *   The master_clock parameter is the PLL reference input in Hz.
   * This parameter must be one of followings :
   *   @li  2822400 // PLL source must be BCLK
   *   @li  3072000 // PLL source must be BCLK
   *   @li  5644800 // PLL source must be BCLK
   *   @li  6144000 // PLL source must be BCLK
   *   @li 11289600
   *   @li 12000000
   *   @li 12288000
   *   @li 16000000
   *   @li 19200000
   *   @li 24576000
   *
   *
   *   The analog signals are routed as following :
   *   @li Line In  : LINN/RINN single ended.
   *   @li Aux In : LAUX/RAUX input
   *   @li LINE out : LOUTP/ROUTP single ended
   *   @li HP out   : LHP/RHP
   */
  Tlv320aic3204(
      unsigned int fs,               // Sampling frequency[Hz]
      unsigned int reference_clock,  // Reference signal to the PLL [Hz]
      PllSource pll_source,          // Where PLL reference comes from.
      I2sRole role,                  // I2S signal direction
      murasaki::I2cMasterStrategy *controller,  // I2C master controller
      unsigned int i2c_device_addr              // 7bit I2C device address
  );

  /**
   * @brief Destructor
   * @details
   * Shutdown the CODEC.
   */
  virtual ~Tlv320aic3204();
  /**
   * \brief Set up the Tlv320aic3204 codec,  and then, start the codec.
   * \details
   *   This method starts the Tlv320aic3204 AD/DA conversion and I2S
   * communication.
   *
   *   The line in is configured to use the Single-End negative input. This is
   * funny but Tlv320aic3204 datasheet specifies to do it. The positive in and
   * diff in are killed. All biases are set as "normal".
   *
   *   The CODEC is configured as master mode. That mean, bclk and WS are given
   * from Tlv320aic3204 to the micro processor.
   *
   *   At initial state, Tlv320aic3204 is set as :
   *   @li All input and output channels are set as 0.0dB and muted.
   */
  virtual void Start(void);

  /**
   * @brief Set channel gain
   * @param channel CODEC input output channels like line-in, line-out,
   * aux-in, headphone-out
   * @param left_gain Gain by dB. [6 .. -12],  The gain value outside of the
   * acceptable range will be saturated.
   * @param right_gain Gain by dB. [6 .. -12],  The gain value outside of the
   * acceptable range will be saturated.
   */
  virtual void SetGain(murasaki::CodecChannel channel, float left_gain,
                       float right_gain);

  /**
   * @brief Mute the specific channel.
   * @param channel Channel to mute on / off
   * @param mute On if true, off if false.
   */
  virtual void Mute(murasaki::CodecChannel channel, bool mute = true);

 private:
  const unsigned int reference_clock_;
  murasaki::I2cMasterStrategy *const i2c_;
  const unsigned int device_addr_;
  const I2sRole role_;
  const PllSource pll_source_;

  float line_input_left_gain_;
  float line_input_right_gain_;
  float aux_input_left_gain_;
  float aux_input_right_gain_;
  float line_output_left_gain_;
  float line_output_right_gain_;
  float hp_output_left_gain_;  // headphone
  float hp_output_right_gain_;

  bool line_input_mute_;
  bool aux_input_mute_;
  bool line_output_mute_;
  bool hp_output_mute_;  // headphone
};

#endif  //  HAL_I2C_MODULE_ENABLED

#endif /* MURASAKI_TP_TLV320AIC3204_HPP_ */
