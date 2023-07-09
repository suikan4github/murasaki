/**
 * \file uartstrategy.hpp
 *
 * \date 2017/11/04
 * \author: Seiichi "Suikan" Horie
 * \brief Root class definition of the UART driver
 */

#ifndef UARTSTRATEGY_HPP_
#define UARTSTRATEGY_HPP_

#include <peripheralstrategy.hpp>
#include "murasaki_defs.hpp"

namespace murasaki {

/**
 * \ingroup MURASAKI_ABSTRACT_GROUP Abstract Classes
 * \{
 */
/**
 * \brief Definition of the root class of UART.
 * \details
 * A prototype of the UART device. Ths abstract class shows the usage of the
 * UART peripheral.
 *
 * This prototype assumes the derived class will transmit / receive data in the
 * task context on RTOS. And both method should be synchronous. That men, until the transmit / receve
 * terminates, both method doesn't return.
 *
 * Two call back methods are prepared to sync with the interrutp which tells the end of
 * Transmit/Recieve.
 */

class UartStrategy : public murasaki::PeripheralStrategy {
 public:

    /**
     * \brief Set the behavior of the hardware flow control
     * \param control The control mode.
     */
    virtual void SetHardwareFlowControl(UartHardwareFlowControl control)
                                        {
    }
    ;
    /**
     * \brief the baud rate
     * @param speed BAUD rate ( 110, 300, ... 9600,... )
     */
    virtual void SetSpeed(unsigned int speed)
                          {
    }
    ;
    /**
     * \brief buffer transmission over the UART. synchronous
     * \param data Pointer to the buffer to be sent.
     * \param size Number of the data to be sent.
     * \param timeout_ms Time out by mili Second.
     * \return Status of the IO processing
     */
    virtual murasaki::UartStatus Transmit(
                                          const uint8_t * data,
                                          unsigned int size,
                                          unsigned int timeout_ms = murasaki::kwmsIndefinitely) = 0;
    /**
     * \brief buffer receive over the UART. synchronous
     * \param data Pointer to the buffer to save the received data.
     * \param size Number of the data to be received.
     * \param transfered_count Number of bytes transfered. The nullPtr means no need to return value.
     * \param uart_timeout Specify murasaki::kutIdleTimeout, if idle line timeout is needed.
     * \param timeout_ms Time out by milli Second.
     * \return Status of the IO processing
     */
    virtual murasaki::UartStatus Receive(
                                         uint8_t * data,
                                         unsigned int size,
                                         unsigned int * transfered_count = nullptr,
                                         UartTimeout uart_timeout = murasaki::kutNoIdleTimeout,
                                         unsigned int timeout_ms = murasaki::kwmsIndefinitely) = 0;
    /**
     * \brief Call back to be called notify the transfer is complete.
     * \param ptr Pointer for generic use. Usually, points a struct of a UART device control
     * \return true: ptr matches with UART device and handle the call back. false : doesn't match.
     * \details
     * A call back to notify the end of entire block or byte transfer. The definition of calling timing is
     * depend on the implementation. This is called from an DMA ISR.
     *
     *
     * Typically, an implementation may check whether the given ptr parameter matches its own device, and if matched, handle it
     * and return true. If it doesn't match, just return false.
     */
    virtual bool TransmitCompleteCallback(void* ptr)=0;
    /**
     * \brief Call back to be called for entire block transfer is complete.
     * \param ptr Pointer for generic use. Usually, points a struct of a UART device control
     * \return true: ptr matches with UART device and handle the call back. false : doesn't match.
     * \details
     * A call back to notify the end of entire block or byte transfer. The definition of calling timing is
     * depend on the implementation. This is called from an DMA ISR.
     *
     * Typically, an implementation may check whether the given ptr parameter matches its own device, and if matched, handle it
     * and return true. If it doesn't match, just return false.
     */
    virtual bool ReceiveCompleteCallback(void* ptr) = 0;
    /**
     * @brief Handling error report of device.
     * @param ptr Pointer for generic use. Usually, points a struct of a device control
     * @return true if ptr matches with device and handle the error. false if ptr doesn't match
     * A member function to detect error.
     *
     * The error handling is depend on the implementation.
     */
    virtual bool HandleError(void * ptr)= 0;

};
/**
 * \}
 * end of add group
 */
}

#endif /* UARTSTRATEGY_HPP_ */
