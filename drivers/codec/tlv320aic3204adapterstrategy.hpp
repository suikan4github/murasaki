/**
 * @file tlv320aic3204.hpp
 *
 * @date 2023/08/11
 * @author: Seiichi "Suikan" Horie
 */

#ifndef MURASAKI_TLV320AIC3204_ADAPTER_STRATEGY_HPP_
#define MURASAKI_TLV320AIC3204_ADAPTER_STRATEGY_HPP_

#include "tlv320aic3204.hpp"

#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {

class Tlv320aic3204AdapterStrategy {
 public:
  // Disabling default constructor.
  Tlv320aic3204AdapterStrategy() = delete;

  /**
   * @brief Constructor
   * @param fs Sampling Frequency [Hz]
   * @param controller I2C master controller
   */
  Tlv320aic3204AdapterStrategy(
      murasaki::I2cMasterStrategy *controller,  // I2C master controller
      u_int8_t device_addr);

  /**
   * \brief Set the CODEC register page to page_number
   * \param page_number 0-255. Selects the Register Page for next read or write.
   */
  virtual void SetPage(u_int8_t page_number);

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

  /**
   * \brief Reset the entire CODEC by software
   */
  virtual void Reset();

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
   * The PLL multiplication factor f is defined as :
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

  virtual void ConfigureRole(murasaki::Tlv320aic3204::I2sRole);

  virtual void ConfigurePllSource(murasaki::Tlv320aic3204::PllSource);

  virtual void ShutdownPll(void);

  virtual void ConfigurePins(bool master = true);

  virtual void ConfigureCODEC(void);

  virtual void ShutdownCODEC(void);

  virtual void ConfigureAnalog(void) = 0;

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
                                bool mute = false) = 0;

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
                               bool mute = false) = 0;

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
                                 bool mute = false) = 0;

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
                               bool mute = false) = 0;

 protected:
  murasaki::I2cMasterStrategy *const i2c_;
  const unsigned int device_addr_;
};

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED

#endif /* MURASAKI_TLV320AIC3204_ADAPTER_STRATEGY_HPP_ */
