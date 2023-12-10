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
 * This class is a specific strategy for the CODEC implementation.
 * The internal implementation of the Tlv320aic3204 class is done
 * here instead of that class.
 *
 * Usually, the internal implementation is done in the private or
 * protected method. But such the implementation is difficult for the
 * unit test in C++.
 *
 * We use adapter pattern for the internal implementation. There are
 * several advantage compare to the private/protected method.
 * \li Easy to test : The internal implementations are implemented
 * as public member functions. Thus, they are easy to test from the
 * unit test tool.
 * \li Internal implementation is still hidden. While the implementations
 * are public in the adapters, the adapter object is a private variable
 * in the Tlv320aic3204 class. Thus, the implementation is hidden from
 * the Tlv320aic3204 class user.
 *
 * Some of the people will dislike this approach, because it is less
 * flexible to the refactoring, compared to the protect/private member
 * functions.
 *
 * We think this is a sense of value. We decided the unit test is much
 * more important than refactoring flexibility. So, the adapter is the better..
 *
 * In this class, we implement certain member classes which are common
 * to the all adapter.
 *
 * For the details of the programming, refer following documentation
 * from TI :
 * \li TLV320AIC3204 data sheet
 * \li TLV320AIC3204 Application Reference Guide (SLA557)
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
   * The I2C address is the one from the TLV320AIC3204 data sheet.
   *
   * Given two parameters are stored in the protected variables
   * for the consecutive member function call.
   */
  Tlv320aic3204AdapterStrategy(
      murasaki::I2cMasterStrategy *controller,  // I2C master controller
      u_int8_t device_addr);

  /**
   * \brief Set the CODEC register page to page_number
   * \param page_number 0-255. Selects the Register Page for next read or write.
   * \details
   * The TLV320AIC3204 register has page. Programmer have to set correct page
   * number fore accessing each register.  Refer the CODEC documentation from
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
   * \param role
   * \li kMaster : I2S pins are output
   * \li kSlave : I2S pins are input
   *
   * \param pll_source
   * \li kMCLK : PLL source is set to MCLK input.
   * \li kBCLK : PLL source is set to BCLK input.
   * @details
   * If the plls_source is kBCLK, the role must be kSlave. Otherwise,
   * assertion is triggered.
   *
   * In the case of the pll_source == kBCLK, the kMCLK must be tied to GND
   * because it is left as input.
   *
   * This function set the I2S word length to 32bits.
   *
   * We assume the internal PLL output frequency as :
   * \li 86.016 MHz for Fs==48kHz ( Fs*128*2*7)
   * \li 84.672 MHz for Fs==44.1kHz (Fs*128*3*5)
   */
  virtual void ConfigureClock(
      murasaki::Tlv320aic3204::I2sRole const role,  // Digital Pin direction
      murasaki::Tlv320aic3204::PllSource const pll_source  // Source of PLL
  );

  /**
   * @brief Initialize the PLL with given fs and master clock.
   * @param r : R in the PLL factor. 1,2,3,4
   * @param j : J in the PLL factor. 1,2,3,...63
   * @param d : D in the PLL factor. 0,1,3,...9999
   * @param p : P in the PLL factor. 1,2,3,4,...8
   * @details
   * At first, initialize the PLL based on the given fs and master clock.
   * The PLL multiplication factor f is defined as :
   * @code
   *       R * J.D
   * f = ----------
   *          P
   * @endcode
   *
   *
   * The PLL frequency must be one of :
   * \li 86.016 MHz for Fs==48kHz ( Fs*128*2*7)
   * \li 84.672 MHz for Fs==44.1kHz (Fs*128*3*5)
   */
  virtual void ConfigurePll(
      uint32_t const r,  // numerator
      uint32_t const j,  // integer part of multiply factor
      uint32_t const d,  // fractional part of the multiply factor
      uint32_t const p   // denominator
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
   *
   * This function set the CODEC based on the assumption of PLL frequency :
   * \li 86.016 MHz for Fs==48kHz ( Fs*128*2*7)
   * \li 84.672 MHz for Fs==44.1kHz (Fs*128*3*5)
   *
   * This function mut be called after ConfigurePLL()
   *
   */
  virtual void ConfigureCODEC(uint32_t const fs);

  /**
   * \brief Stop the codec and set to low power mode.
   */
  virtual void ShutdownCODEC(void);

  /**
   * \brief Power on the CODEC and run.
   * \details
   * This function must call after the all configuration of
   * clocks, PLLs, CODECs, and Analogs.
   */
  virtual void StartCODEC(void);

  /**
   * \brief Set up analog path of the codec.
   * \details
   * This is board dependent function.
   */
  virtual void ConfigureAnalog(void) = 0;

  /**
   * \brief Set analog path to low power mode.
   */
  virtual void ShutdownAnalog(void);

  /**
   * \brief Set the analog input PGA gain.
   * \param left_gain Gain by dB. [0 ... 47.5],  The gain value outside of the
   * acceptable range will be saturated. Gain is truncated to 0.5dB step.
   * \param right_gain Gain by dB. [0 ..
   * 47.5], The gain value outside of the acceptable range will be saturated.
   * Gain is truncated to 0.5dB step.
   * \details
   * Call this function before turning on the CODEC.
   */
  virtual void SetInputGain(float left_gain, float right_gain);

  /**
   * \brief Set the line output gain and enable the relevant mixer.
   * \param left_gain Gain by dB. [6 .. -12], The gain value outside of the
   * acceptable range will be saturated.
   *  \param right_gain Gain by dB. [6 ..
   * -12], The gain value outside of the acceptable range will be saturated.
   * \param mute set true to mute
   * \details
   * This function assumes to be called before the CODEC ON.
   *
   * If muted, the gain setting values are ignored.
   */
  virtual void SetLineOutputGain(float left_gain, float right_gain, bool mute);

  /**
   * \brief Set the headphone output gain and enable the relevant mixer.
   * \param left_gain Gain by dB. [-6 .. 29], The gain value outside of the
   * acceptable range will be saturated. Gain step is 1dB.
   * \param right_gain Gain by dB. [-6 ..
   * -12], The gain value outside of the acceptable range will be saturated.
   * . Gain step is 1dB.
   * \param mute set true to mute, false to un-mute.
   * \details
   * This function assumes to be called before the CODEC ON.
   *
   * If muted, the gain setting values are ignored.
   */
  virtual void SetHpOutputGain(float left_gain, float right_gain, bool mute);

 protected:
  murasaki::I2cMasterStrategy *const i2c_;
  const unsigned int device_addr_;
};

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED

#endif /* _MURASAKI_TLV320AIC3204_ADAPTER_STRATEGY_HPP_ */
