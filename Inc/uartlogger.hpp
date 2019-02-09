/**
 * \file uartlogger.hpp
 *
 * \date 2018/01/20
 * \author: takemasa
 * \brief Logging to Uart
 */

#ifndef UARTLOGGER_HPP_
#define UARTLOGGER_HPP_

#include <loggerstrategy.hpp>
#include <uartstrategy.hpp>

// Check if CubeMX generates UART module
#ifdef HAL_UART_MODULE_ENABLED


namespace murasaki {
/**
 * \brief Logging through an UART port.
 * \details
 * This is a standard logging class through the UART port. The instance of this
 * class can be passed to the murasaki::Debugger constructor.
 *
 * See \ref MURASAKI_PLATFORM_GROUP as usage example.
 *
 * \ingroup MURASAKI_GROUP
 */
class UartLogger : public LoggerStrategy
{
 public:
    /**
     * \brief Constructor
     * \param uart Pointer to the uart object.
     */
    UartLogger(UartStrategy * uart);
    /**
     * \brief Message output member function.
     * \param message Non null terminated character array. This data is stored or output to the logger.
     * @param size Size of the message[bytes].  Must be smaller than 65536
     */
    virtual void putMessage(char message[], unsigned int size);
    virtual char getCharacter();
    /**
     * \brief Start post mortem process
     * \param debugger_fifo Pointer to the DebuggerFifo class object. The data inside this FIFO will be sent to UART
     * This member function read the data in given FIFO, and then do the auto history.
     *
     * This funciton call the DebuggerFifo::SetPostMortem() intenally. Then, output the data inside
     * FIFO through the given UART.
     *
     * Once all the data is output, this function wait for a receive data. Once data received,
     * this funciton rewind the FIFO and then, start to transmit the data again.
     */
    virtual void DoPostMortem( void * debugger_fifo);

 protected:
    UartStrategy * const uart_;
};

} /* namespace murasaki */

#endif // HAL_UART_MODULE_ENABLED


#endif /* UARTLOGGER_HPP_ */
