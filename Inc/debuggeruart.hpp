/**
 * \file debuggeruart.hpp
 *
 * \date 2018/09/23
 * \author Seiichi "Suikan" Horie
 * @brief UART. Thread safe and synchronous IO
 */

#ifndef DEBUGGER_UART_HPP_
#define DEBUGGER_UART_HPP_

#include <synchronizer.hpp>
#include <uartstrategy.hpp>
#include "criticalsection.hpp"

// Check if CubeIDE generates UART module
#ifdef HAL_UART_MODULE_ENABLED

namespace murasaki {

/**
 * @ingroup MURASAKI_HELPER_GROUP
 * \brief Logging dedicated UART class
 * \details
 *
 * The Uart class is the wrapper of the UART controller. To use the DebuggerUart class,
 * make an instance with UART_HandleTypeDef * type pointer. For example, to create
 * an instance for the UART3 peripheral :
 * \code
 *     my_uart3 = new murasaki::DebuggerUart(&huart3);
 * \endcode
 * Where huart3 is the handle generated by CubeIDE for UART3 peripheral. To use this class,
 * the UART peripheral have to be configured to use the DMA functionality. The baud rate,
 * length and flow control should be configured by the CubeIDE.
 *
 *
 * The @ref Uart::Transmit() member function is a synchronous function. A programmer can specify the
 * timeout by timeout_ms parameter. By default, this parameter is set by kwmsIndefinitely
 * which specifies never time out.
 *
 * The @ref Uart::Receive() member function is a synchronous function.  A programmer can specify the
 * timeout by timeout_ms parameter. By default, this parameter is set by kwmsIndefinitely
 * which specifies never time out.
 *
 * Both methods can be called from only the task context. If these are called in the ISR
 * context, the result is unknown.
 */
class DebuggerUart : public UartStrategy
{
 public:
    /**
     * \brief Constructor
     * \param uart Pointer to a UART control struct. This device have to be configured to use DMA and interrupt for both Tx and Rx.
     * \details
     * Store the given uart pointer into the internal variable. This pointer is passed to the STM32Cube HAL UART functions when needed.
     *
     */
    DebuggerUart(UART_HandleTypeDef *uart);
    /**
     * \brief Destructor. Delete internal variables.
     */
    virtual ~DebuggerUart();
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
     * \return  Always returns OK
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
     * \param transfered_count This parameter is ignored.
     * \param uart_timeout This parameter is ignored
     * \param timeout_ms Time out limit by milliseconds.
     * \return  Always returns OK
     * \details
     * Receive to given data buffer through an UART device.
     *
     * The receiving mode is synchronous. That means, function returns when specified number of data has been received, except timeout.
     * Passing \ref murasaki::kwmsIndefinitely to the parameter timeout_ms orders not to return until complete receiving. Other value of
     * timeout_ms parameter specifies the time out by millisecond.
     * If time out happen, function returns false. If not happen, it returns true.
     *
     * This function is exclusive. Internally this function is guarded by mutex. Then this function is thread safe.
     * This function is forbiddedn to call from ISR.
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
     * This method have to be called from HAL_UART_TxCpltCallback(). See STM32F7 HAL manual for detail
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
     * This method have to be called from HAL_UART_RxCpltCallback(). See STM32F7 HAL manual for detail      */

    virtual bool ReceiveCompleteCallback(void *const ptr);
    /**
     * @brief Error handling
     * @param ptr Pointer to UART_HandleTypeDef struct.
     * \return true: ptr matches with UART device and handle the error. false : doesn't match.
     * @details
     * A handle to print out the error message.
     *
     * Checks whether handle has error and if there is, print appropriate error.
     * Then trigger an assertion.
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

#endif /* DEBUGGER_UART_HPP_ */
