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
   *   The master_clock parameter is the MCLK input to the Tlv320aic3204 in
   * Hz.         This parameter must be one of followings :
   *   @li  2822400
   *   @li  3072000
   *   @li  5644800
   *   @li  6144000
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
   * \brief Set the CODEC register page to page_number
   * \param page_number 0-255. Selects the Register Page for next read or write.
   */
  virtual void SetPage(u_int8_t page_number);

  /**
   * \brief Reset the entire CODEC by software
   */
  virtual void Reset();
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

  /**
   *  Service function for the Tlv320aic3204 board implementer.
   *
   * \brief send one command to Tlv320aic3204.
   * \param command command data array. It have to have register address of
   * Tlv320aic3204 in first two bytes. \param size number of bytes in the
   * command, including the register address. \details Send one complete command
   * to ADAU3161 by I2C. In the typical case, the command length is 3. \li
   * command[0] : USB of the register address. 0x40. \li command[1] : LSB of the
   * register address. \li command[2] : Value to right the register.
   */
  virtual void SendCommand(const uint8_t command[], int size);

 protected:
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

  /**
   * \brief wait until PLL locks.
   * \details
   *   Service function for the Tlv320aic3204 board implementer.
   *
   *   Read the PLL status and repeat it until the PLL locks.
   */
  virtual void WaitPllLock(void);
  /**
   * @brief Initialize the PLL with given fs and master clock.
   * @param r : R in the PLL factor. 1,2,3,4
   * @param j : J in the PLL factor. 1,2,3,...63
   * @param d : D in the PLL factor. 0,1,3,...9999
   * @param p : P in the PLL factor. 1,2,3,4,...8
   * @details
   * At first, initialize the PLL based on the given fs and master clock.
   * Then, setup the Converter sampling rate.
   * The PLL mutiplification factor f is defined as :
   * @code
   *       R * J.D
   * f = ----------
   *          P
   * @endcode
   *
   *
   */
  virtual void ConfigurePll(
      uint32_t r,  // numarator
      uint32_t j,  // integer part of multiply factor
      uint32_t d,  // fractional part of the multiply factor
      uint32_t p   // denominator
  );

  virtual void ShutdownPll(void);

  virtual void ConfigurePins(bool master = true);

  virtual void ConfigureCODEC(void);

  virtual void ShutdownCODEC(void);

  virtual void ConfigureAnalog(void);

  virtual void ShutdownAnalog(void);

  /**
   * \brief Set the line input gain and enable the relevant mixer.
   * \param left_gain Gain by dB. [6 .. -12],  The gain value outside of the
   * acceptable range will be saturated. \param right_gain Gain by dB. [6 ..
   * -12], The gain value outside of the acceptable range will be saturated.
   * \param mute set true to mute
   * \details
   *   As same as start(), this gain control function uses the single-end
   * negative input only. Other input signal of the line in like positive signal
   * or diff signal are killed.
   *
   *   Other input line like aux are not killed. To kill it, user have to mute
   * them explicitly.
   */
  virtual void SetLineInputGain(float left_gain, float right_gain,
                                bool mute = false);

  /**
   * \brief Set the aux input gain and enable the relevant mixer.
   * \param left_gain Gain by dB. [6 .. -12], The gain value outside of the
   * acceptable range will be saturated. \param right_gain Gain by dB. [6 ..
   * -12], The gain value outside of the acceptable range will be saturated.
   * \param mute set true to mute
   * \details
   *   Other input lines are not killed. To kill it, user have to mute them
   * explicitly.
   */
  virtual void SetAuxInputGain(float left_gain, float right_gain,
                               bool mute = false);

  /**
   * \brief Set the line output gain and enable the relevant mixer.
   * \param left_gain Gain by dB. [6 .. -12], The gain value outside of the
   * acceptable range will be saturated. \param right_gain Gain by dB. [6 ..
   * -12], The gain value outside of the acceptable range will be saturated.
   * \param mute set true to mute
   * \details
   *   Other output lines are not killed. To kill it, user have to mute them
   * explicitly.
   *
   */
  virtual void SetLineOutputGain(float left_gain, float right_gain,
                                 bool mute = false);

  /**
   * \brief Set the headphone output gain and enable the relevant mixer.
   * \param left_gain Gain by dB. [6 .. -12], The gain value outside of the
   * acceptable range will be saturated. \param right_gain Gain by dB. [6 ..
   * -12], The gain value outside of the acceptable range will be saturated.
   * \param mute set true to mute
   * \details
   *   Other out line like line in are not killed. To kill it, user have to mute
   * them explicitly.
   */
  virtual void SetHpOutputGain(float left_gain, float right_gain,
                               bool mute = false);
  /**
   * \brief send one command to Tlv320aic3204.
   * \param table command table. All commands are stored in one row. Each row
   * has only 1 byte data after reg address. \param rows number of the rows in
   * the table. \details Service function for the Tlv320aic3204 board
   * implementer.
   *
   *   Send a list of command to Tlv320aic3204. All commands has 3 bytes length.
   * That mean, after two byte register address, only 1 byte data pay load is
   * allowed. Commadns are sent by I2C
   */
  virtual void SendCommandTable(const uint8_t table[][3], int rows);
};

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED

#endif /* MURASAKI_TP_TLV320AIC3204_HPP_ */
