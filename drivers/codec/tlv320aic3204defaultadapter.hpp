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

  virtual void ConfigureAnalog(void);
};  // class

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED

#endif /* _MURASAKI_TLV320AIC3204_DEFAULT_ADAPTER_HPP_ */
