/*
 * uartlogger.cpp
 *
 *  Created on: 2018/01/20
 *      Author: takemasa
 */

#include "uartlogger.hpp"
#include "murasaki_config.hpp"
#include "murasaki_assert.hpp"

// Check if CubeMX generates UART module
#ifdef HAL_UART_MODULE_ENABLED


namespace murasaki {

UartLogger::UartLogger(UartStrategy * uart):
		uart_ ( uart)
{
    MURASAKI_ASSERT(nullptr != uart)

}


void UartLogger::putMessage(char message[], unsigned int size)
{
    MURASAKI_ASSERT(nullptr != message)
    MURASAKI_ASSERT(65536 > size);
    MURASAKI_ASSERT(IsTaskContext());
    uart_->Transmit(reinterpret_cast<uint8_t *>(message),        // Message to send
                    size,                     // length of message by byte.
                    static_cast<WaitMilliSeconds>( PLATFORM_CONFIG_DEBUG_SERIAL_TIMEOUT)  // wait eternaly
                    );

}

char UartLogger::getCharacter()
{
    char buf;

    MURASAKI_ASSERT(IsTaskContext());
    uart_->Receive(
            reinterpret_cast<uint8_t *>(&buf),  // buffer to receive
            1                                   // receive one char
    );
    return buf;
}

void UartLogger::DoPostMortem(void* debugger_fifo) {
	// Set the peripheral handle of UART.
	UART_HandleTypeDef * const huart = static_cast<UART_HandleTypeDef * > (GetPeripheralHandle(uart_));
	// Set the FIFO.
	DebuggerFifo * const fifo = reinterpret_cast<DebuggerFifo *> (debugger_fifo);

	// Set FIFO mode to post mortem. Now, FIFO is not synchronizing.
	fifo->SetPostMortem();

    // Stop all UART activity.
    HAL_UART_DeInit(huart);
    // Then, restart it.
    HAL_UART_Init(huart);

	do {
		uint16_t transfered_num;
		uint8_t data[10];		// data transfer buffer.

		do {
			// retrieve data from FIFO.
			transfered_num = fifo->Get(data, sizeof(data));

			HAL_UART_Transmit(huart, data, transfered_num, HAL_MAX_DELAY);	// no time out.
		} while ( transfered_num != 0);

        HAL_UART_Receive(huart, data, 1, HAL_MAX_DELAY);		// wait any type in from UART

		fifo->ReWind();		// then, rewind the FIFO.
	} while (true);

}

} /* namespace murasaki */

#endif // HAL_UART_MODULE_ENABLED
