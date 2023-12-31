/*
 * tlv320aic3204defaultadapter.cpp
 *
 *  Created on: 2023/09/15
 *      Author: Seiichi "Suikan" Horie
 */

#include "tlv320aic3204defaultadapter.hpp"

#include <algorithm>

#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

// Macro for easy-to-read
#define CODEC_SYSLOG(fmt, ...) \
  MURASAKI_SYSLOG(this, kfaAudioCodec, kseDebug, fmt, ##__VA_ARGS__)

#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {

Tlv320aic3204DefaultAdapter::Tlv320aic3204DefaultAdapter(
    murasaki::I2cMasterStrategy *controller,  // I2C master controller
    u_int8_t device_address                   // 7bit I2C address.
    )
    : Tlv320aic3204AdapterStrategy(controller, device_address) {}

void Tlv320aic3204DefaultAdapter::ConfigureAnalog(void) {
  CODEC_SYSLOG("Enter. %s", "")
  /*
   * HP output startup sequence ( SLAA577 section 2.2.3.1)
   * 1. Setup Rpop and N and soft stepping.
   * 2. Configure the output driver ( common-mode, power routing)
   * 3. Choose signal to route to HP.
   * 4. Start up the driver block. Keep muting these blocks.
   * 5. Unmute HPL and HPR
   * 6. TUrn the HPL HPR driver power on.
   * 7. After the completion of the soft-stepping, Unmute the signal source.
   */

  CODEC_SYSLOG("Leave.%s", "")
}

} /* namespace murasaki */

#endif  //  HAL_I2C_MODULE_ENABLED
