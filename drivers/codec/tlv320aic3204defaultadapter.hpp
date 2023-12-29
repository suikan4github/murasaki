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

/**
 * Configure the analog input and output circuit as default.
 *
 * Input configuration is as followings.
 * @li IN1_L : Line Input 1 Left.
 * @li IN1_R : Line Input 1 Right.
 * @li IN2_L : Line Input 2 Left Positive.
 * @li IN2_R : Line Input 2 Left Negative.
 * @li IN3_L : Line Input 2 Right Positive
 * @li IN3_R : Line Input 2 Right Negative.
 *
 * Output configuration is as followings.
 * @li HPL : Headphone Output Left.
 * @li HPR : Headphone Output Right.
 * @li LOL : Line Output L
 * @li LOR : Line Output R
 */
class Tlv320aic3204DefaultAdapter : public Tlv320aic3204AdapterStrategy {
 public:
  // Disabling default constructor.
  Tlv320aic3204DefaultAdapter() = delete;

  /**
   * @brief Constructor
   * @param controller I2C master controller
   * @param device_address 7bit I2C address of CODEC.
   */
  Tlv320aic3204DefaultAdapter(
      murasaki::I2cMasterStrategy *controller,  // I2C master controller
      u_int8_t device_address                   // 7bit I2C address.
  );

  /**
   * @brief Configure the analog input and output.
   * @details
   * HP Out is configured as stereo single ended.
   *
   * Set HP Out ( SLAA577 section 2.2.3.1 ):
   * @li Rpop to 25kOhm ( Assume Rload is 32ohm ).
   * @li Slow charging time N = 5.0 (Assume CC is 47uF ).
   * @li Soft stepping time is 200mS.
   *
   */
  virtual void ConfigureAnalog(void);
};  // class

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED

#endif /* _MURASAKI_TLV320AIC3204_DEFAULT_ADAPTER_HPP_ */
