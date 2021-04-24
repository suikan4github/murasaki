/*
 * i2cmasterstubsi5351.cpp
 *
 *  Created on: Feb 28, 2021
 *      Author: takemasa
 */

#include "Thirdparty/i2cmasterstubsi5351.hpp"
#include "murasaki_assert.hpp"

namespace murasaki {

I2cMasterStubSi5351::I2cMasterStubSi5351()
{
    // TODO Auto-generated constructor stub

}

I2cMasterStubSi5351::~I2cMasterStubSi5351()
{
    // TODO Auto-generated destructor stub
}

murasaki::I2cStatus I2cMasterStubSi5351::Receive(
                                                 unsigned int addrs,
                                                 uint8_t *rx_data,
                                                 unsigned int rx_size,
                                                 unsigned int *transfered_count,
                                                 unsigned int timeout_ms)
                            {
	MURASAKI_ASSERT(false);
   return( murasaki::ki2csUnknown);

}

murasaki::I2cStatus I2cMasterStubSi5351::TransmitThenReceive(
                                                             unsigned int addrs,
                                                             const uint8_t *tx_data,
                                                             unsigned int tx_size,
                                                             uint8_t *rx_data,
                                                             unsigned int rx_size,
                                                             unsigned int *tx_transfered_count,
                                                             unsigned int *rx_transfered_count,
                                                             unsigned int timeout_ms)
                                        {
	MURASAKI_ASSERT(false);
   return( murasaki::ki2csUnknown);

}



} /* namespace murasaki */
