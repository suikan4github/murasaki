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

/**
 * @ingroup MURASAKI_THIRDPARTY_TESTER
 */
class I2cMasterStubSi5351 : public I2cMasterStubStragegy {
 public:
    struct DataBuffer
    {
        int buffer_num_;    ///< Number of the buffers in this struct.
        int buffer_size;    ///< Size of the each buffer.
        uint8_t **buffers_;  ///< A pointer to an array of the pointers to the buffers.
        int *data_sizes;    ///< Stores actual data size in the each buffer
        int data_num;       ///< Number of data buffere actually used. if 3, buffer 0..2 are used.
    };

	/**
     * @brief Constructor with internal buffer size.
     * @param tx_buf_count The count of the TX buffer which stores TX data
     * @param tx_buf_size The size of the each TX buffer.
     * @details
     * Store the tx_buf_count and create internal tx buffer as specified by tx_buf_count.
	 */
    I2cMasterStubSi5351(int tx_buf_count, int tx_buf_size);
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

 private:
    // Internal buffer to store the TX data. Stored for each time TX function is called.
    DataBuffer tx_data_buffer_;
    // Internal buffer to store the RX data. Used for each time RX function is called.
    DataBuffer *rx_Data_buffer_;
    // Number of the buffer used in the RX data buffer.
};

} /* namespace murasaki */

#endif /* I2CMASTERSTUBSI5351_HPP_ */
