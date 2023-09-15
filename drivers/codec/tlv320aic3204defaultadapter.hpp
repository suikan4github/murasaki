/**
 * @file tlv320aic3204defaultadapter.hpp
 *
 * @date 2023/08/11
 * @author: Seiichi "Suikan" Horie
 */

#ifndef _MURASAKI_TLV320AIC3204_DEFAULT_ADAPTER_HPP_
#define _MURASAKI_TLV320AIC3204_DEFAULT_ADAPTER_HPP_

#include "tlv320aic3204adapterstrategy.hpp"

#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {

class Tlv320aic3204DefaultAdaptor {
 public:
  // Disabling default constructor.
  Tlv320aic3204DefaultAdaptor() = delete;

  /**
   * @brief Constructor
   * @param fs Sampling Frequency [Hz]
   * @param controller I2C master controller
   */
  Tlv320aic3204DefaultAdaptor(
      murasaki::I2cMasterStrategy *controller  // I2C master controller
  );

  virtual void ConfigureAnalog(void);

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
}

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED

#endif /* _MURASAKI_TLV320AIC3204_DEFAULT_ADAPTER_HPP_ */
