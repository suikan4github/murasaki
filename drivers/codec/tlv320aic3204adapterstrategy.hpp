/**
 * @file tlv320aic3204adapterstrategy.hpp
 *
 * @date 2023/08/11
 * @author: Seiichi "Suikan" Horie
 */

#ifndef _MURASAKI_TLV320AIC3204_ADAPTER_STRATEGY_HPP_
#define _MURASAKI_TLV320AIC3204_ADAPTER_STRATEGY_HPP_

#include "tlv320aic3204.hpp"

#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {

/**
 * \brief AdapterStrategy pattern for the CODEC implementation test.
 * \details
 * This class is a specific strategy for the CODEC implemenation.
 * The internal implementation of the Tlv320aic3204 class is done
 * here instead of that class.
 *
 * Usually, the internal implementaiton is done in the private or
 * protected method. But such the implementation is difficul for the
 * unit test in C++.
 *
 * We use adapter pattern for the internal implementation. There are
 * several advantage compare to the private/protected method.
 * \li Easy to test : The internal implemenations are implemented
 * as publice member functions. Thus, they are easy to test from the
 * unit test tool.
 * \li Internal implementaion is still hidden. While the implementations
 * are public in the adapters, the adapter object is a private variable
 * in the Tlv320aic3204 class. Thus, the implementation is hidden from
 * the Tlv320aic3204 class user.
 *
 * Some of the people will dislike this approach, becuase it is less
 * flexible to the refactoring, compared to the protect/private member
 * functions.
 *
 * We think this is a sense of value. We decided the unit test is much
 * more important than refactoring flexibility. So, the adapter is
 * benefitable.
 *
 * In this class, we implement certain member classes which are commont
 * to the all adapter.
 *
 * For the dtails of the programming, refer following documentation
 * from TI :
 * \li TLV320AIC3204 datasheet
 * \li TLV320AIC3204 Application Refernce Guide (SLA557)
 */
class Tlv320aic3204AdapterStrategy {
 public:
  // Disabling default constructor.
  Tlv320aic3204AdapterStrategy() = delete;

  /**
   * @brief Constructor
   * @param controller I2C master controller object.
   * @param device_addr 7bit I2C address of the TLV320AIC3204.
   * @details
   * Declare the I2C controller class and I2C device address.
   * The controller is the C++ class in Murasaki library.
   * The I2C address is the one from the TLV320AIC3204 datasheet.
   *
   * Given two parameters are stored in the protected variables
   * for the consequtive member function call.
   */
  Tlv320aic3204AdapterStrategy(
      murasaki::I2cMasterStrategy *controller,  // I2C master controller
      u_int8_t device_addr);

  /**
   * \brief Set the CODEC register page to page_number
   * \param page_number 0-255. Selects the Register Page for next read or write.
   * \details
   * The TLV320AIC3204 register has page. Programmer have to set correct page
   * number fore accessing each register.  Refere the CODEC documenation from
   * TI.
   */
  virtual void SetPage(u_int8_t page_number);

  /**
   *
   * \brief send one command to Tlv320aic3204.
   * \param command command data array. It have to have register address of
   * Tlv320aic3204 in first two bytes.
   * \param size number of bytes in the command, including the register address.
   * \details Send one complete command to TLV320AIC3204 by I2C.
   * In the typical case, the command length is 2.
   * \li command[0] : register address.
   * \li command[1] : Value to Wright the register.
   */
  virtual void SendCommand(const uint8_t command[], int size);

  /**
   * \brief Reset the entire TLV320AIC3204 CODEC by software
   */
  virtual void Reset();

  /**
   * \brief wait until PLL locks.
   * \details
   *  Just waiting 10mS. See the TLV320AIC3204 application reference guide.
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

  /**
   * \brief Set up the digital interface pins.
   * \param role
   * \li kMaster : I2S pins are output
   * \li kSlave : I2S pins are input
   *
   * \param pll_source
   * \li kMCLK : PLL source is set to MCLK input.
   * \li kBCLK : PLL source is set to BCLK input.
   *
   * \details
   * If the plls_source is kBCLK, the role must be kSlave. Otherwise,
   * assertion is triggered.
   *
   * In the case of the pll_source == kBCLK, the kMCLK must be tied to GND
   * because it is left as input.
   */
  virtual void ConfigureRoleAndSource(
      murasaki::Tlv320aic3204::I2sRole role,         // Digital Pin direction
      murasaki::Tlv320aic3204::PllSource pll_source  // Source of PLL
  );

  /**
   * \brief Stop the PLL and set it to low per mode.
   */
  virtual void ShutdownPll(void);

  /**
   * \brief Configure CODEC to the given Fs.
   * \param fs Sampling frequency[Hz]. Only following values are allowed :
   * \li 44100
   * \li 48000
   * \li 88200
   * \li 96000
   * \li 176400
   * \li 192000
   *
   */
  virtual void ConfigureCODEC(uint32_t fs);

  /**
   * \brief Stop the codec and set to low poower mode.
   */
  virtual void ShutdownCODEC(void);

  /**
   * \brief Set up analog path of the codec.
   * \details
   * This is board dependent function.
   */
  virtual void ConfigureAnalog(void) = 0;

  virtual void ShutdownAnalog(void);

  /**
   * \brief Set the line input gain and enable the relevant mixer.
   * \param left_gain Gain by dB. [6 .. -12],  The gain value outside of the
   * acceptable range will be saturated.
   * \param right_gain Gain by dB. [6 ..
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
   * acceptable range will be saturated.
   * \param right_gain Gain by dB. [6 ..
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
   * acceptable range will be saturated.
   *  \param right_gain Gain by dB. [6 ..
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
   * acceptable range will be saturated.
   * \param right_gain Gain by dB. [6 ..
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

#endif /* _MURASAKI_TLV320AIC3204_ADAPTER_STRATEGY_HPP_ */
