/**
 * @file i2cmaster.hpp
 *
 * @date 2018/02/12
 * @author: Seiichi "Suikan" Horie
 * @brief I2C master. Thread safe, synchronous IO
 */

#ifndef I2CMASTER_HPP_
#define I2CMASTER_HPP_

#include <i2cmasterstrategy.hpp>
#include <synchronizer.hpp>
#include "criticalsection.hpp"

// Check whether I2C module is enabled by CubeIDE.
#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {

/**
 * \ingroup MURASAKI_GROUP
 * @brief Thread safe, synchronous and blocking IO. Encapsulating I2C master. Based on STM32Cube HAL driver and FreeRTOS
 * \details
 * The I2cMaster class is the wrapper of the I2C controller.
 *
 * ### Configuration
 * To configure the I2C peripheral as master, chose I2C peripheral in the Device Configuration Tool
 * of the CubeIDE. Set it as I2C mode, and enable NVIC interrupt.
 *
 * @image html "i2cmaster-config-1.png"
 * @image latex "i2cmaster-config-1.png"
 *
 * Also, pay attention to the I2C Maximum Output Speed.
 * The defaul setting by CubeIDE may not be appropriate to your circuit.
 * It should be checked with oscilloscope.
 * @image html "i2cmaster-config-2.png"
 * @image latex "i2cmaster-config-2.png"
 *
 *
 * ### Creating a peripheral object
 *
 * To use the I2cMaster class,
 * create an instance with I2C_HandleTypeDef * type pointer. For example, to create
 * an instance for the I2C3 peripheral :
 * \code
 *     my_i2c3 = new murasaki::I2cMaster(&hi2c3);
 * \endcode
 * Where hi2c3 is the handle generated by CubeIDE for I2C3 peripheral.
 *
 *
 * ### Handling a interrupt
 * In addition to the instantiation, we need to prepare an interrupt callback.
 * \code
 * void HAL_I2C_TxCpltCallback(I2C_HandleTypeDef * hi2c)
 * {
 *     if (my_i2c3->TransmitCompleteCallback(hi2c))
 *         return;
 * }
 * \endcode
 * Where HAL_I2C_TxCpltCallback is a predefined name of the I2C interrupt handler.
 * This is invoked by system whenever a interrupt baed I2C transmission is complete.

 *
 * Note that above callback is invoked for any I2Cn where n is 1, 2, 3... To avoid the
 * confusion, I2cMaster::TransmitCompleteCallback() method checks whether given parameter
 * matches with its I2C_HandleTypeDef * pointer ( which was passed to constructor ).
 * And only when both matches, the member function execute the interrupt termination process
 * and return with true..
 *
 * As same as Tx, RX needs HAL_I2C_RxCpltCallback() and Error needs HAL_I2C_ErrorCallback().
 * The HAL_I2C_ErrorCallback() is essetial to implement. Otherwise, NAK response will not be
 *  handled correctly.
 * @code
 * void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
 *     if (my_i2c3->HandleError(hi2c))
 *         return;
 * }
 * @endcode
 *
 * ### Transmitting and Receiving
 * Once the instance and callback are correctly prepared, we can use the Tx/Rx member function.
 *
 * The @ref I2cMaster::Transmit() member function is a synchronous function. A programmer can specify the
 * timeout by timeout_ms parameter. By default, this parameter is set by kwmsIndefinitely
 * which specifes never time out.
 *
 * The @ref  I2cMaster::Receive() member function is a synchronous function.  A programmer can specify the
 * timeout by timeout_ms parameter. By default, this parameter is set by kwmsIndefinitely
 * which species never time out.
 *
 * The @ref I2cMaster::TransmitThenReceive() member function is synchronous function.
 * A programmer can specify the timeout by timeout_ms parameter.
 * By default, this parameter is set by kwmsIndefinitely
 * which species never time out.
 *
 * All of 3 methods can be called from only the task context. If these are called in the ISR
 * context, the result is unknown.
 *
 * Note : In case an time out occurs during transmit / receive, this implementation
 * calls HAL_I2C_MASTER_ABORT_IT(). But it is unknown whether this is right thing to do.
 * The HAL reference of the STM32F7 is not clear for this case. For example, it doesn't tell
 * what programmer do to stop the transfer at the middle. And also, it doesn't tell what's happen
 * if the HAL_I2C_MASTER_ABORT_IT() is called.
 *
 * According to the source code of the HAL_I2C_MASTER_ABORT_IT(), no interrupt will be
 * raised by this API call.
 */
class I2cMaster : public I2CMasterStrategy
{
 public:
    /**
     * @brief Constructor
     * @param i2c_handle Peripheral handle created by CubeMx
     */
    I2cMaster(I2C_HandleTypeDef *i2c_handle);
    /**
     * @brief Destructor
     */
    virtual ~I2cMaster();
    /**
     * @brief Thread safe, synchronous transmission over I2C.
     * @param addrs 7bit address of the I2C device.
     * @param tx_data Data array to transmit.
     * @param tx_size Data counts[bytes] to transmit. Must be smaller than 65536
     * @param transfered_count ( Currently, Just ignored) the count of the bytes transfered during the API execution.
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
                                         unsigned int addrs,
                                         const uint8_t *tx_data,
                                         unsigned int tx_size,
                                         unsigned int *transfered_count,
                                         unsigned int timeout_ms);

    /**
     * @brief Thread safe, synchronous receiving over I2C.
     * @param addrs 7bit address of the I2C device.
     * @param rx_data Data array to transmit.
     * @param rx_size Data counts[bytes] to transmit. Must be smaller than 65536
     * @param transfered_count ( Currently, Just ignored) the count of the bytes transfered during the API execution.
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
                                        unsigned int addrs,
                                        uint8_t *rx_data,
                                        unsigned int rx_size,
                                        unsigned int *transfered_count,
                                        unsigned int timeout_ms);
    /**
     * @brief Thread safe, synchronous transmission and then receiving over I2C.
     * @param addrs 7bit address of the I2C device.
     * @param tx_data Data array to transmit.
     * @param tx_size Data counts[bytes] to transmit. Must be smaller than 65536
     * @param rx_data Data array to transmit.
     * @param rx_size Data counts[bytes] to transmit. Must be smaller than 65536
     * @param tx_transfered_count ( Currently, Just ignored) the count of the bytes transmitted during the API execution.
     * @param rx_transfered_count ( Currently, Just ignored) the count of the bytes received during the API execution.
     * @param timeout_ms Time ou [mS]. By default, there is not timeout.
     * @return Result of the processing
     * @details
     * First, this member function transmit the data, and the, by repeated start function,
     * it receives data. The transmission device address and receiving device address is same.
     *
     * This member function is programmed to run in the task context of RTOS. This should be internally
     * exclusive between multiple task access. In other word, it should be thread save.
     *
     * Following are the return code :
     * @li @ref murasaki::ki2csOK : All transmission and receive completed.
     * @li @ref murasaki::ki2csNak : Transmission or receive terminated by NAK receiving.
     * @li @ref murasaki::ki2csArbitrationLost : Transmission or receive terminated by an arbitration error of the multi-master.
     * @li @ref murasaki::ki2csBussError : Transmission or receive terminated by bus error
     * @li @ref murasaki::ki2csTimeOut : Transmission or receive abort by timeout.
     * @li other value : Unhandled error. I2C device are re-initialized.
     */
    virtual murasaki::I2cStatus TransmitThenReceive(
                                                    unsigned int addrs,
                                                    const uint8_t *tx_data,
                                                    unsigned int tx_size,
                                                    uint8_t *rx_data,
                                                    unsigned int rx_size,
                                                    unsigned int *tx_transfered_count,
                                                    unsigned int *rx_transfered_count,
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
     * This function have to be called from HAL_I2C_MasterTxCpltCallback()
     * @code
     * void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c)
     * {
     *     if (murasaki::platform.i2c_master->TransmitCompleteCallback(hi2c))
     *         return;
     * }
     * @endcode
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
     * This member function have to be called from HAL_I2C_MasterRxCpltCallback()
     *
     * @code
     * void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef * hi2c) {
     *     if (murasaki::platform.i2c_master->ReceiveCompleteCallback(hi2c))
     *         return;
     * }
     * @endcode
     *
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
     * This member function have to be called from　HAL_I2C_ErrorCallback()
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
    CriticalSection *const critical_section_;  // protect memberfunction
    volatile I2cStatus interrupt_status_;  // status variable from interrupt
};

} /* namespace murasaki */

#endif // HAL_I2C_MODULE_ENABLED

#endif /* I2CMASTER_HPP_ */
