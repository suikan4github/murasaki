/*
 * i2cmasterstub.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: takemasa
 */

#include <Thirdparty/i2cmasterstubstrategy.hpp>
#include "murasaki.hpp"

namespace murasaki {

I2cMasterStubStragegy::I2cMasterStubStragegy()
{
    // TODO Auto-generated constructor stub

}

I2cMasterStubStragegy::~I2cMasterStubStragegy()
{
    // TODO Auto-generated destructor stub
}

murasaki::I2cStatus I2cMasterStubStragegy::Transmit(
                                            unsigned int addrs,
                                            const uint8_t *tx_data,
                                            unsigned int tx_size,
                                            unsigned int *transfered_count,
                                            unsigned int timeout_ms)
                                                    {
    // print address and write sign
    murasaki::debugger->Printf("%02x W ", addrs);
    // print data
    for (unsigned int i = 0; i < tx_size; i++)
        murasaki::debugger->Printf("0x02 ", tx_data[i]);
    // then, line feed
    murasaki::debugger->Printf("\n");

    return( murasaki::ki2csOK);

}

murasaki::I2cStatus I2cMasterStubStragegy::Receive(
                                    unsigned int addrs,
                                    uint8_t *rx_data,
                                    unsigned int rx_size,
                                    unsigned int *transfered_count,
                                    unsigned int timeout_ms )
{
	MURASAKI_ASSERT(false);
   return( murasaki::ki2csUnknown);

}

murasaki::I2cStatus I2cMasterStubStragegy::TransmitThenReceive(
                                                unsigned int addrs,
                                                const uint8_t *tx_data,
                                                unsigned int tx_size,
                                                uint8_t *rx_data,
                                                unsigned int rx_size,
                                                unsigned int *tx_transfered_count,
                                                unsigned int *rx_transfered_count,
                                                unsigned int timeout_ms )
{
	MURASAKI_ASSERT(false);
   return( murasaki::ki2csUnknown);

}


} /* namespace murasaki */
