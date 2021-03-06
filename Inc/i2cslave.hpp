/**
 * @file i2cslave.hpp
 *
 * @date 2018/10/07
 * @author Seiichi "Suikan" Horie
 * @brief I2C slave. Thread safe, synchronous IO
 *
 */

#ifndef I2CSLAVE_HPP_
#define I2CSLAVE_HPP_

#include <i2cslavestrategy.hpp>
#include <synchronizer.hpp>
#include "criticalsection.hpp"

// Check whether I2C module is enabled by CubeIDE.
#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {
/**
 * \ingroup MURASAKI_GROUP
 * @brief Thread safe, synchronous and blocking IO. Encapsulating I2C slave. Based on STM32Cube HAL driver and FreeRTOS
 * \details
 * The I2cSlave class is the wrapper of the I2C controller.
 *
 * ### Configuration
 * The configuration is the same as the master. See @ref murasaki::I2cMaster class.
 *
 * ### Creating a peripheral object
 *
 * To use the I2cSlave class,
 * create an instance with I2C_HandleTypeDef * type pointer. For example, to create
 * an instance for the I2C3 peripheral :
 * \code
 *     my_i2c3 = new murasaki::I2cSlave(&hi2c3);
 * \endcode
 * Where hi2c3 is the handle generated by CubeIDE for I2C3 peripheral.
 *
 *
 * ### Handling an interrupt
 *
 * Interrupt is handled automatically. Programmer doesn't need to care.
 *
 * ### Transmission and Receiving
 * Once the instance and callbacks are correctly prepared, we can use the Tx/Rx member function.
 *
 * The @ref I2cSlave::Transmit() member function is a synchronous function. A programmer can specify the timeout by timeout_ms parameter. By default, this parameter is set by kwmsIndefinitely which specifies eternal wait.
 *
 * The @ref  I2cSlave::Receive() member function is a synchronous function.  A programmer can specify the timeout by timeout_ms parameter. By default, this parameter is set by kwmsIndefinitely which specifies eternal wait.
 * Both methods can be called from only the task context. If these are called in the ISR context, the result is unknown.
 *
 * Note : In case an time out occurs during transmit / receive, this implementation
 * calls HAL_I2C_DeInit()/HAL_I2C_Init(). But it is unknown whether this is the right thing to do.
 * The HAL reference of the STM32F7 is not clear for this case. For example, it doesn't tell what programmer do to stop the transfer at the middle.
 *
 */
class I2cSlave : public I2cSlaveStrategy {
 public:
    I2cSlave(I2C_HandleTypeDef *i2c_handle);
    virtual ~I2cSlave();
    /**
     * @brief Thread safe, synchronous transmission over I2C.
     * @param tx_data Data array to transmit.
     * @param tx_size Data counts[bytes] to transmit. Must be smaller than 65536
     * @param transfered_count ( Currently, Just ignored) the count of the bytes transferred during the API execution.
     * @param timeout_ms Time ou [mS]. By default, there is not timeout.
     * @return Result of the processing
     * @details
     * This member function is programmed to run in the task context of RTOS. This should be internally
     * exclusive between multiple task access. In other word, it should be thread save.
     *
     * Following are the return code :
     * @li @ref murasaki::ki2csOK : All transmission completed.
     * @li @ref murasaki::ki2csNak : Transmission terminated by NAK receiving.
     * @li @ref murasaki::ki2csArbitrationLost : Transmission terminated by an arbitration error of the multi-master.
     * @li @ref murasaki::ki2csBussError : Transmission terminated by bus error
     * @li @ref murasaki::ki2csTimeOut : Transmission abort by timeout.
     * @li other value : Unhandled error. I2C device are re-initialized.
     */
    virtual murasaki::I2cStatus Transmit(
                                         const uint8_t *tx_data,
                                         unsigned int tx_size,
                                         unsigned int *transfered_count,
                                         unsigned int timeout_ms);
    /**
     * @brief Thread safe, synchronous receiving over I2C.
     * @param rx_data Data array to transmit.
     * @param rx_size Data counts[bytes] to transmit. Must be smaller than 65536
     * @param transfered_count ( Currently, Just ignored) the count of the bytes transferred during the API execution.
     * @param timeout_ms Time ou [mS]. By default, there is not timeout.
     * @return Result of the processing
     * @details
     * This member function is programmed to run in the task context of RTOS. This should be internally
     * exclusive between multiple task access. In other word, it should be thread save.
     *
     * Following are the return code :
     * @li @ref murasaki::ki2csOK : All Receive completed.
     * @li @ref murasaki::ki2csNak : Receive terminated by NAK receiving.
     * @li @ref murasaki::ki2csArbitrationLost : Receive terminated by an arbitration error of the multi-master.
     * @li @ref murasaki::ki2csBussError : Receive terminated by bus error
     * @li @ref murasaki::ki2csTimeOut : Receive abort by timeout.
     * @li other value : Unhandled error. I2C device are re-initialized.
     */
    virtual murasaki::I2cStatus Receive(
                                        uint8_t *rx_data,
                                        unsigned int rx_size,
                                        unsigned int *transfered_count,
                                        unsigned int timeout_ms);

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
     *
     * This member function have to be called from HAL_I2C_SlaveTxCpltCallback()
     *
     * @code
     * void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef * hi2c)
     * {
     *     if (murasaki::platform.i2c_slave->TransmitCompleteCallback(hi2c))
     *           return;
     * }
     * @endcode
     *
     */
    virtual bool TransmitCompleteCallback(void *ptr);
    /**
     * \brief Call back to be called for entire block transfer is complete.
     * \param ptr Pointer for generic use. Usually, points a struct of a peripheral control
     * \return true: ptr matches with peripheral and handle the call back. false : doesn't match.
     * \details
     * A call back to notify the end of entire block or byte transfer. The definition of calling timing is
     * depend on the implementation. This is called from an DMA ISR.
     *
     * Typically, an implementation may check whether the given ptr parameter matches its own device, and if matched, handle it
     * and return true. If it doesn't match, just return false.
     *
     * This function have to be called from HAL_I2C_SlaveRxCpltCallback()
     *
     * @code
     * void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef * hi2c) {
     *     if (murasaki::platform.i2c_slave->ReceiveCompleteCallback(hi2c))
     *         return;
     * }
     * @endcode
     */
    virtual bool ReceiveCompleteCallback(void *ptr);
    /**
     * @brief Error handling
     * @param ptr Pointer to I2C_HandleTypeDef struct.
     * \return true: ptr matches with device and handle the error. false : doesn't match.
     * @details
     * A handle to print out the error message.
     *
     * Checks whether handle has error and if there is, print appropriate error. Then return.
     *
     * This member function have to be called from HAL_I2C_ErrorCallback()
     *
     * @code
     * void HAL_I2C_ErrorCallback(I2C_HandleTypeDef * hi2c) {
     *     if (murasaki::platform.i2c_master->HandleError(hi2c))
     *          return;
     *     if (murasaki::platform.i2c_slave->HandleError(hi2c))
     *          return;
     * }
     * @endcode
     */
    virtual bool HandleError(void *ptr);
     private:
    /**
     * @brief Return the Platform dependent device control handle.
     * @return Handle of device.
     * @details
     * The handle is the pointer ( or some ID ) which specify the control data of
     * specific device.
     */
    virtual void* GetPeripheralHandle();
     protected:
    I2C_HandleTypeDef *const peripheral_;  // SPI peripheral handle
    Synchronizer *const sync_;  // sync between task and interrupt
    CriticalSection *const critical_section_;  // protect member function
    volatile I2cStatus interrupt_status_;  // status variable from interrupt
};

} /* namespace murasaki */

#endif /* HAL_I2C_MODULE_ENABLED */

#endif /* I2CSLAVE_HPP_ */
