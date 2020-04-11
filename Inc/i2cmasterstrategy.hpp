/**
 * \file i2cmasterstrategy.hpp
 *
 *  \date  2018/02/11
 *  \author: Seiichi "Suikan" Horie
 *  \brief Root class definition of the I2C Master.
 */

#ifndef I2CMASTERSTRATEGY_HPP_
#define I2CMASTERSTRATEGY_HPP_

#include "i2cstrategy.hpp"

namespace murasaki {
/**
 * \ingroup MURASAKI_ABSTRACT_GROUP
 * \brief Definition of the root class of I2C master.
 * \details
 * This member function is a prototype of the I2C master peripheral.

 * This prototype assumes the derived class will transmit/receive data in the task context on RTOS.
 * And these member functions should be synchronous.
 * That means, both member functions don't return until the transmit/receive terminates.

 * These two callback member functions synchronize with the interrupt,
 * which tells the end of Transmit/Receive.
 */

class I2CMasterStrategy : public murasaki::I2cStrategy
{
 public:

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
                                         unsigned int timeout_ms = murasaki::kwmsIndefinitely) = 0;
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
                                        unsigned int timeout_ms = murasaki::kwmsIndefinitely) = 0;
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
                                                    unsigned int timeout_ms = murasaki::kwmsIndefinitely) =0;
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
    virtual bool TransmitCompleteCallback(void *ptr)=0;
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
    virtual bool ReceiveCompleteCallback(void *ptr) = 0;
    /**
     * @brief Handling error report of device.
     * @param ptr Pointer for generic use. Usually, points a struct of a device control
     * @return true if ptr matches with device and handle the error. false if ptr doesn't match
     * This member function is a callback to detect error.
     *
     * The error handling is depend on the implementation.
     */
    virtual bool HandleError(void *ptr)= 0;

};

} /* namespace murasaki */

#endif /* I2CMASTERSTRATEGY_HPP_ */
