/*
 * i2cmasterstub.hpp
 *
 *  Created on: Feb 27, 2021
 *      Author: takemasa
 */

#ifndef THIRDPARTY_I2CMASTERSTUB_HPP_
#define THIRDPARTY_I2CMASTERSTUB_HPP_

#include <i2cmasterstrategy.hpp>

namespace murasaki {

/**
 * @defgroup MURASAKI_THIRDPARTY_TESTER Test modules for the thirdparty devices.
 * @details
 * This is a collection  of the class and functions to test the thirdparty devices.
 *
 * The third party devices uses the Murasaki classes. For example, The @ref Si5351 class uses
 * the @ref I2cMasterStrategy class. To test such the classes, a set of the driver and stub are provied.
 *
 * Following is the rule of the name
 *
 * @li TestClassname() : Test driver function.
 * @li peripheralnameClassname_stub : Peripheral stub class
 *
 * For example, TestSi5351() is a test driver function. This function test all functionality of the Si5351 class.
 * This function uses the I2cMasterStubSi5351 class as test stub of the Si5351.
 */


/**
 * @ingroup MURASAKI_THIRDPARTY_GROUP
 * @brief Debug Stub for the I2C master client.
 * @details
 * This stub class printout the given output data on the transmit member function.
 *
 * For the actual test, this class have to be customized by the sub-class, to
 * provide the specific test feature. For each class-under-test, customized sub-class
 * might be created.
 */
class I2cMasterStub : public I2cMasterStrategy {
 public:
    /**
     * @brief List of data buffers.
     * @details
     * A set of buffers and its management variables.
     *
     * These buffers are used to store the TX and RX data in I2C stub.
     * For example, Transimit() / TransmitAndThenReceive() will copy the TX data to one of empty buffer from the head.
     * For each time TX data is written, data_num is incremented.
     * In the other words, data_num_ filed is the index of the next buffer to be written.
     *
     * Also, Receive() / TransmitAndThenReceive() will copy the data from buffer as RX data.
     * For each time TX data is read, the read_index is incremented.
     *
     * Overflow or Underflow will trigger an Assertion failure.
     *
     */
    struct DataBuffer
    {
        int buffer_num_;    ///< Number of the buffers in this struct.
        int buffer_size_;    ///< Size of the each buffer [BYTE].
        uint8_t **buffers_;  ///< A pointer to an array of the pointers to the buffers.
        int *data_sizes_;    ///< Stores actual data size in the each buffer
        int write_index_;       ///< Number of data buffer actually used. if 3, buffer 0..2 are used.
        int read_index_;     ///< Next index of the buffer to read.
    };

    /**
     * @brief Constructor with internal buffer size.
     * @param buf_count The count of the TX buffer which stores TX data
     * @param buf_size The size of the each TX buffer.
     * @details
     * Create the tx_buffer_ and rx_data_buffer_ based on the given parameters.
     * The size and count of buffer are common between TX and RX.
     * Both buffers are Initialized as empty.
     */
    I2cMasterStub(int buf_count, int buf_size);
    virtual ~I2cMasterStub();

    /**
     * @brief Get how many buffers are in use in the TX buffers.
     *
     * @return Number of the buffer in use. 0 means no buffer is used.
     * @details
     * The TX buffer is used for each @ref Transmit() or @ref TransmitThenReceive() is called.
     * For each time of use, the TX data is stored to the new buffers.
     * In the other word, each buffers contain the data from independent TX.
     */
    int getNumTxBuffer();

    /**
     * @brief Make all buffers empty.
     *
     */
    void clearTxBuffer();
    /**
     * @brief Make all buffers empty.
     *
     */
    void clearRxBuffer();

    /**
     * @brief Copy a set of data into a buffer.
     *
     * @param data Pointer to the data.
     * @param length Data length to write [BYTE].
     * @details
     * Find a new RX buffer and copy the byte string from data. If buffers are empty, assertion fails.
     */
    void writeRxBuffer(uint8_t *data, int length);

    /**
     * @brief Copy a set of data from a buffer.
     *
     * @param data Pointer to the data area.
     * @param max_size of the data buffer [BYTE]
     * @param length Data length to read [BYTE].
     * @details
     * Find a oldest TX buffer which is still not read, and copy the byte string from buffer to data.
     * If there is not buffers still not be read, assertion failed.
     */

    void readTxBuffer(uint8_t *data, int max_length, int *length);

    /**
     * @brief Thread safe, synchronous transmission over I2C.
     * @param addrs 7bit address of the I2C device.
     * @param tx_data Data array to transmit.
     * @param tx_size Data counts[bytes] to transmit.
     * @param transfered_count the count of the bytes transfered during the API execution.
     * @param timeout_ms Time ou [mS]. By default, there is not timeout.
     * @return Result of the processing
     * @details
     * This member function is programmed to run in the task context of RTOS.
     * This function should be internally exclusive between multiple task access.
     * In other words, it should be thread-safe.
     */
    virtual murasaki::I2cStatus Transmit(
                                         unsigned int addrs,
                                         const uint8_t *tx_data,
                                         unsigned int tx_size,
                                         unsigned int *transfered_count = nullptr,
                                         unsigned int timeout_ms = murasaki::kwmsIndefinitely);

    /**
     * @brief Thread safe, synchronous receiving over I2C.
     * @param addrs 7bit address of the I2C device.
     * @param rx_data Data array to transmit.
     * @param rx_size Data counts[bytes] to transmit.
     * @param transfered_count the count of the bytes transfered during the API execution.
     * @param timeout_ms Time ou [mS]. By default, there is not timeout.
     * @return Result of the processing
     * @details
     * This function just triggers an assert. To avoid it, override this funciton.
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
     * This function just triggers an assert. To avoid it, override this function.
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
    /**
     * \brief Call back to be called notify the transfer is complete.
     * \param ptr Pointer for generic use. Usually, points a struct of a peripheral control
     * \return true: ptr matches with peripheral and handle the call back. false : doesn't match.
     * \details
     * A call back to notify the end of entire block or byte transfer. The definition of calling timing is
     * depend on the implementation. This is called from an DMA ISR.
     *
     * For the test purpose, this member function may not be needed. Anyway, if needed, override it.
     */
    virtual bool TransmitCompleteCallback(void *ptr) {
        return true;
    }
    ;
    /**
     * \brief Call back to be called for entire block transfer is complete.
     * \param ptr Pointer for generic use. Usually, points a struct of a peripheral control
     * \return true: ptr matches with peripheral and handle the call back. false : doesn't match.
     * \details
     * This member function is a callback to notify the end of the entire block or byte transfer.
     * The definition of calling timing depends on the implementation.
     * You must call this function from an ISR.

     * For the test purpose, this member function may not be needed. Anyway, if needed, override it.
     */
    virtual bool ReceiveCompleteCallback(void *ptr) {
        return true;
    }
    ;
    /**
     * @brief Handling error report of device.
     * @param ptr Pointer for generic use. Usually, points a struct of a device control
     * @return true if ptr matches with device and handle the error. false if ptr doesn't match
     * This member function is a callback to detect error.
     *
     * For the test purpose, this member function may not be needed. Anyway, if needed, override it.
     */
    virtual bool HandleError(void *ptr) {
        return true;
    }
    ;

 private:

    // Internal buffer to store the TX data. Stored for each time TX function is called.
    DataBuffer *tx_data_buffer_;
    // Internal buffer to store the RX data. Used for each time RX function is called.
    DataBuffer *rx_data_buffer_;
    // Number of the buffer used in the RX data buffer.

    /**
     * @brief Create a set of buffers by given parameter.
     *
     * @param buf_count Number of the buffers to create.
     * @param buf_size Size of the each buffer [BYTE]
     * @return Created Buffers
     * @details
     * Create a @ref DataBuffer variable from the heap, and then, allocate buffers based on the given parameters.
     * The DataBuffer::data_sizes_ are also allocated.
     *
     * The @ref DataBuffer::data_num_ are initialized to zero.
     */
    DataBuffer* newBuffers(int buf_count, int buf_size);

    /**
     * @brief Delete the given buffer.
     *
     * @param buffers Buffers to delete.
     * @deletes
     * Delete the all internal variables of the buffers and then, delete the buffer itself.
     */
    void deleteBuffers(DataBuffer *buffers);

 protected:
    void writeTxBuffer(uint8_t *data, int length);
    void readRxBuffer(uint8_t *data, int max_length, int *length);

    /**
     * @brief pass the raw peripheral handler
     * @return pointer to the raw peripheral handler hidden in a class.
     */
    virtual void* GetPeripheralHandle() {
        return nullptr;
    }
    ;
};

} /* namespace murasaki */

#endif /* THIRDPARTY_I2CMASTERSTUB_HPP_ */
