/*
 * i2cmasterstubsi5351.hpp
 *
 *  Created on: Feb 28, 2021
 *      Author: takemasa
 */

#ifndef I2CMASTERSTUBSI5351_HPP_
#define I2CMASTERSTUBSI5351_HPP_

#include <Thirdparty/i2cmasterstubstrategy.hpp>

namespace murasaki {

class I2cMasterStubSi5351 : public I2cMasterStubStragegy {
 public:
    I2cMasterStubSi5351();
    virtual ~I2cMasterStubSi5351();

    /**
     * @brief Thread safe, synchronous receiving over I2C.
     * @param addrs 7bit address of the I2C device.
     * @param rx_data Data array to transmit.
     * @param rx_size Data counts[bytes] to transmit.
     * @param transfered_count the count of the bytes transfered during the API execution.
     * @param timeout_ms Time ou [mS]. By default, there is not timeout.
     * @return Result of the processing
     * @details
     * This member function is programmed to run in the task context of RTOS.
     * This function should be internally exclusive between multiple task access.
     * In other words, it should be thread-safe.
     */
    virtual murasaki::I2cStatus Receive(
                                        unsigned int addrs,
                                        uint8_t *rx_data,
                                        unsigned int rx_size,
                                        unsigned int *transfered_count = nullptr,
                                        unsigned int timeout_ms = murasaki::kwmsIndefinitely);
    /**
     * @brief Thread safe, synchronous transmission and then receiving over I2C.
     * @param addrs 7bit address of the I2C device.
     * @param tx_data Data array to transmit.
     * @param tx_size Data counts[bytes] to transmit.
     * @param rx_data Data array to transmit.
     * @param rx_size Data counts[bytes] to transmit.
     * @param tx_transfered_count the count of the bytes transmitted during the API execution.
     * @param rx_transfered_count the count of the bytes received during the API execution.
     * @param timeout_ms Time ou [mS]. By default, there is not timeout.
     * @return Result of the processing
     * @details
     * First, this member function to transmit the data, and then, by repeated start function,
     * it receives data. The transmission device address and receiving device address is the same.
     *
     * This member function is programmed to run in the task context of RTOS.
     * And also exclusive between the racing tasks. In other words, this function is thread-safe.
     *
     */
    virtual murasaki::I2cStatus TransmitThenReceive(
                                                    unsigned int addrs,
                                                    const uint8_t *tx_data,
                                                    unsigned int tx_size,
                                                    uint8_t *rx_data,
                                                    unsigned int rx_size,
                                                    unsigned int *tx_transfered_count = nullptr,
                                                    unsigned int *rx_transfered_count = nullptr,
                                                    unsigned int timeout_ms = murasaki::kwmsIndefinitely);

};

} /* namespace murasaki */

#endif /* I2CMASTERSTUBSI5351_HPP_ */
