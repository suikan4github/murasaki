/**
 * \file uart.hpp
 *
 * \date 2017/11/05
 * \author Seiichi "Suikan" Horie
 * @brief UART. Thread safe and synchronous IO
 */

#ifndef UART_HPP_
#define UART_HPP_

#include <synchronizer.hpp>
#include <uartstrategy.hpp>
#include "criticalsection.hpp"

// Check if CubeIDE generates UART module
#ifdef HAL_UART_MODULE_ENABLED

namespace murasaki {

/**
 * \ingroup MURASAKI_GROUP
 * \brief Thread safe, synchronous and blocking IO. Concrete implementation of UART controller. Based on the STM32Cube HAL DMA Transfer.
 * \details
 *
 * The Uart class is a wrapper of the UART controller.
 *
 * ### Configuration
 * To configure the UART peripheral, chose UART/USART peripheral in the Device Configuration Tool
 * of the CubeIDE. Set it as Asynchronous mode.
 *
 * @image html "uart-config-1.png"
 * @image latex "uart-config-1.png" width=5.19in
 *
 * Make sure setting direction to Receive and Transmit. Other parameters are up to the application.
 *
 * The DMA have to be enabled for both TX and RX. The data size is 8bit for both Peripheral and memory.
 *
 * @image html "uart-config-2.png"
 * @image latex "uart-config-2.png" width=3.66in
 *
 * And then, enable the interrupt.
 * @image html "uart-config-3.png"
 * @image latex "uart-config-3.png" width=3.69in
 *
 *
 * ### Creating a peripheral object
 * To use the Uart class,
 * create an instance with UART_HandleTypeDef * type pointer. For example, to create
 * an instance for the UART3 peripheral :
 * \code
 *     my_uart3 = new murasaki::Uart(&huart3);
 * \endcode
 * Where huart3 is the handle generated by CubeIDE for UART3 peripheral. To use this class,
 * the UART peripheral have to be configured to use the DMA functionality. The baud rate,
 * length and flow control should be configured by the CubeIDE.
 *
 * ### Handling an interrupt
 *
 * Interrupt is handled automatically. Programmer doesn't need to care.
 *
 * ### Transmitting and Receiving
 * Once the instance and callbacks are correctly prepared, we can use the Tx/Rx member function.
 *
 * The @ref Uart::Transmit() member function is a synchronous function. A programmer can specify the
 * timeout by timeout_ms parameter. By default, this parameter is set by murasaki::kwmsIndefinitely
 * which specifes never time out.
 *
 * The @ref Uart::Receive() member function is a synchronous function.  A programmer can specify the
 * timeout by timeout_ms parameter. By default, this parameter is set by murasaki::kwmsIndefinitely
 * which specifes never time out.
 *
 * Both methods can be called from only the task context. If these are called in the ISR
 * context, the result is unknown.
 */
class Uart : public UartStrategy
{
 public:
    /**
     * \brief Constructor
     * \param uart Pointer to a UART control struct. This device have to be configured to use DMA and interrupt for both Tx and Rx.
     * \details
     * Store the given uart pointer into the internal variable. This pointer is passed to the STM32Cube HAL UART functions when needed.
     *
     */
    Uart(UART_HandleTypeDef *uart);
    /**
     * \brief Destructor. Delete internal variables.
     */
    virtual ~Uart();
    /**
     * \brief Set the behavior of the hardware flow control
     * \param control The control mode.
     * \details
     * Before calling this method, all transmission and recevie activites have to be finished.
     * This is responsibility of the programmer.
     *
     * Note this method is NOT re-etnrant.
     * In other word, this member function can be called from both task and interrupt context.
     */
    virtual void SetHardwareFlowControl(UartHardwareFlowControl control);
    /**
     * @brief Set the BAUD rate
     * @param baud_rate BAUD rate ( 110, 300,... 57600,... )
     * \details
     * Before calling this method, all transmission and recevie activites have to be finished.
     * This is responsibility of the programmer.
     *
     * Note this method is NOT re-etnrant.
     * In other word, this member function can be called from both task and interrupt context.
     */
    virtual void SetSpeed(unsigned int baud_rate);

    /**
     * \brief Transmit raw data through an UART by synchronous mode.
     * \param data Data buffer to be transmitted.
     * \param size The count of the data ( byte ) to be transfered. Must be smaller than 65536
     * \param timeout_ms Time out limit by milliseconds.
     * \return  True if all data transfered completely. False if time out happen.
     * \details
     * Transmit given data buffer through an UART device.
     *
     * The transmission mode is synchronous. That means, function returns when all data has been transmitted, except timeout.
     * Passing \ref murasaki::kwmsIndefinitely to the parameter timeout_ms orders not to return until complete transmission. Other value of
     * timeout_ms parameter specifies the time out by millisecond. If time out happen, function returns false. If not happen, it returns true.
     *
     * This function is exclusive. Internally the function is guarded by mutex. Then this function is thread safe.
     * This function is forbiddedn to call from ISR.
     */
    virtual murasaki::UartStatus Transmit(
                                          const uint8_t *data,
                                          unsigned int size,
                                          unsigned int timeout_ms);
    /**
     * \brief Receive raw data through an UART by synchronous mode.
     * \param data Data buffer to place the received data..
     * \param count The count of the data ( byte ) to be transfered. Must be smaller than 65536
     * \param transfered_count ( Currently, Just ignored) Number of bytes transfered. The nullPtr means no need to return value.
     * \param uart_timeout Specify murasaki::kutIdleTimeout, if idle line timeout is needed.
     * \param timeout_ms Time out limit by milliseconds.
     * \return  True if all data transfered completely. False if time out happen.
     * \details
     * Receive to given data buffer through an UART device.
     *
     * The receiving mode is synchronous. That means, function returns when specified number of data has been received, except timeout.
     * Passing \ref murasaki::kwmsIndefinitely to the parameter timeout_ms orders not to return until complete receiving. Other value of
     * timeout_ms parameter specifies the time out by millisecond.
     * If time out happen, function returns false. If not happen, it returns true.
     *
     * This function is exclusive. Internally this function is guarded by mutex. Then this function is thread safe.
     * This function is forbidden to call from ISR.
     *
     * The retun values are:
     * @li @ref murasaki::kursOK : Transmit complete.
     * @li @ref murasaki::kursTimeOut : Time out occur.
     * @li @ref murasaki::kursOverrun : Next char was written to TX register. This is fatal problem in HAL. Periperal is re-initialized internally.
     * @li @ref murasaki::kursDMA : This is fatal problem in HAL. Peripheral is re-initialized internally.
     * @li other : This is fatal problem in HAL. Peripheral is re-initialized internally.
     */
    virtual murasaki::UartStatus Receive(
                                         uint8_t *data,
                                         unsigned int count,
                                         unsigned int *transfered_count,
                                         UartTimeout uart_timeout,
                                         unsigned int timeout_ms);
    /**
     * \brief Call back for entire block transfer completion.
     * \param ptr Pointer to UART_HandleTypeDef struct.
     * \return true: ptr matches with UART device and handle the call back. false : doesn't match.
     * \details
     * A call back to notify the end of entire block transfer.
     * This is considered as the end of DMA based transmission.
     * The context have to be interrupt.
     *
     * This member function checks whether the given ptr parameter matches its own device, and if matched,
     * Release the waiting task and return true. If it doesn't match, just return false.
     *
     * The retun values are:
     * @li @ref murasaki::kursOK : Received complete.
     * @li @ref murasaki::kursTimeOut : Time out occur.
     * @li @ref murasaki::kursFrame : Receive error by wrong word size configuration.
     * @li @ref murasaki::kursParity : Parity error.
     * @li @ref murasaki::kursNoise : Error by noise.
     * @li @ref murasaki::kursDMA : This is fatal problem in HAL. Peripheral is re-initialized internally.
     * @li other : This is fatal problem in HAL. Peripheral is re-initialized internally.
     *
     * This method have to be called from HAL_UART_TxCpltCallback(). See STM32F7 HAL manual for detail
     *
     * @code
     * void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
     * {
     *     if (murasaki::platform.uart_console->TransmitCompleteCallback(huart))
     *         return;
     * }
     * @endcode
     */

    virtual bool TransmitCompleteCallback(void *const ptr);
    /**
     * \brief Call back for entire block transfer completion.
     * \param ptr Pointer to UART_HandleTypeDef struct.
     * \return true: ptr matches with UART device and handle the call back. false : doesn't match.
     * \details
     * A call back to notify the end of entire block transfer.
     * This is considered as the end of DMA based receiving.
     * The context have to be interrupt.
     *
     * This member function checks whether the given ptr parameter matches its own device, and if matched,
     * Release the waiting task and return true. If it doesn't match, just return false.
     *
     * This method have to be called from HAL_UART_RxCpltCallback(). See STM32F7 HAL manual for detail
     * @code
     * void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
     *  {
     *     if (murasaki::platform.uart_console->ReceiveCompleteCallback(huart))
     *         return;
     * }
     * @endcode
     *
     * */

    virtual bool ReceiveCompleteCallback(void *const ptr);
    /**
     * @brief Error handling
     * @param ptr Pointer to UART_HandleTypeDef struct.
     * \return true: ptr matches with UART device and handle the error. false : doesn't match.
     * @details
     * A handle to print out the error message.
     *
     * Checks whether handle has error and if there is, print appropriate error. Then return.
     *
     * This function have to be coalled from().
     * @code
     * void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
     *     if (murasaki::platform.uart_console->HandleError(huart))
     *         return;
     * }
     * @endcode
     *
     */
    virtual bool HandleError(void *const ptr);

 protected:
    UART_HandleTypeDef *const peripheral_;

    Synchronizer *const tx_sync_;
    Synchronizer *const rx_sync_;

    CriticalSection *const tx_critical_section_;
    CriticalSection *const rx_critical_section_;

 private:
    murasaki::UartStatus tx_interrupt_status_, rx_interrupt_status_;
    /**
     * @brief Return the Platform dependent device control handle.
     * @return Handle of device.
     * @details
     * The handle is the pointer ( or some ID ) which specify the control data of
     * specific device.
     */
    virtual void* GetPeripheralHandle();

};

} /* namespace platform */

#endif // HAL_UART_MODULE_ENABLED

#endif /* UART_HPP_ */
