/*
 * i2cmasterstubstrategy.hpp
 *
 *  Created on: Feb 27, 2021
 *      Author: takemasa
 */

#ifndef THIRDPARTY_I2CMASTERSTUB_STRATEGY_HPP_
#define THIRDPARTY_I2CMASTERSTUB_STRATEGY_HPP_

#include <i2cmasterstrategy.hpp>

namespace murasaki {

/**
 * @brief Debug Stub for the I2C master client.
 * @details
 * This stub class printout the given output data on the transmit member function.
 */
class I2cMasterStubStragegy : public I2cMasterStrategy {
 public:
    I2cMasterStubStragegy();
    virtual ~I2cMasterStubStragegy();

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
     * \brief Call back to be called notify the transfer is complete.
     * \param ptr Pointer for generic use. Usually, points a struct of a peripheral control
     * \return true: ptr matches with peripheral and handle the call back. false : doesn't match.
     * \details
     * A call back to notify the end of entire block or byte transfer. The definition of calling timing is
     * depend on the implementation. This is called from an DMA ISR.
     *
     *
     * Typically, an implementation may check whether the given ptr parameter matches its own device, and if matched, handle it
     * and return true. If it doesn't match, just return false.
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

     * Typically, an implementation may check whether the given ptr parameter matches its device,
     * and if matched, handle it and return true. If it doesn't match, return false.
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
     * The error handling is depend on the implementation.
     */
    virtual bool HandleError(void *ptr) {
        return true;
    }
    ;

 protected:
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

#endif /* THIRDPARTY_I2CMASTERSTUB_STRATEGY_HPP_ */
