/*
 * debuggerart.cpp
 *
 *  Created on: 2018/09/23
 *      Author: Seiichi "Suikan" Horie
 */

#include "debuggeruart.hpp"
#include "murasaki_defs.hpp"
#include "murasaki_assert.hpp"

// Check if CubeMX generates UART module
#ifdef HAL_UART_MODULE_ENABLED

namespace murasaki {

DebuggerUart::DebuggerUart(UART_HandleTypeDef * const uart)
        : peripheral_(uart),
          tx_sync_(new murasaki::Synchronizer),
          rx_sync_(new murasaki::Synchronizer),
          tx_critical_section_(new murasaki::CriticalSection),
          rx_critical_section_(new murasaki::CriticalSection)
{
    // Setup internal variable with given uart structure.

    MURASAKI_ASSERT(nullptr != uart)
    MURASAKI_ASSERT(nullptr != tx_sync_)
    MURASAKI_ASSERT(nullptr != rx_sync_)
    MURASAKI_ASSERT(nullptr != tx_critical_section_)
    MURASAKI_ASSERT(nullptr != rx_critical_section_)

}

DebuggerUart::~DebuggerUart()
{

    if (nullptr != tx_sync_)
        delete tx_sync_;

    if (nullptr != rx_sync_)
        delete rx_sync_;

    if (nullptr != tx_critical_section_)
        delete tx_critical_section_;

    if (nullptr != tx_critical_section_)
        delete rx_critical_section_;
}

void DebuggerUart::SetHardwareFlowControl(UartHardwareFlowControl control)
                                          {
    // stop UART activity. This is required by UART HAL specification.
    int result = HAL_UART_DeInit(peripheral_);
    MURASAKI_ASSERT(result == HAL_OK);

    // Change the Hardware flow control
    switch (control) {
        case kuhfcCts:  // Control CTS only ( Flow control on TX )
            peripheral_->Init.HwFlowCtl = UART_HWCONTROL_CTS;
            break;
        case kuhfcRts:  // Control RTS only ( Flow control on RX )
            peripheral_->Init.HwFlowCtl = UART_HWCONTROL_RTS;
            break;
        case kuhfcCtsRts:  // Control CTS and RTS ( Flow control on TX and RX )
            peripheral_->Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
            break;
        default:  // Nor hardware flow control.
            peripheral_->Init.HwFlowCtl = UART_HWCONTROL_NONE;
            break;

    }
    // restart UART acitivty.
    result = HAL_UART_Init(peripheral_);
    MURASAKI_ASSERT(result == HAL_OK);
}

murasaki::UartStatus DebuggerUart::Transmit(
                                            const uint8_t * data,
                                            unsigned int size,
                                            WaitMilliSeconds timeout_ms)
                                            {
    MURASAKI_ASSERT(nullptr != data)
    MURASAKI_ASSERT(65536 >= size);

    // make this methold re-entrant in task context.
    tx_critical_section_->Enter();
    {
        // Keep coherency between the L2 and cache before DMA
        // No need to invalidate
        murasaki::CleanDataCacheByAddress(
                                          const_cast<uint8_t *>(data),
                                          size);

        HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(peripheral_,
                                                         const_cast<uint8_t *>(data),
                                                         size);
        MURASAKI_ASSERT(HAL_OK == status);

        tx_sync_->Wait(timeout_ms);
    }
    tx_critical_section_->Leave();

    return murasaki::kursOK;  // always return OK in this class.
}

bool DebuggerUart::TransmitCompleteCallback(void* const ptr)
                                            {
    MURASAKI_ASSERT(nullptr != ptr)

    if (ptr == peripheral_) {
        tx_sync_->Release();
        return true;
    }
    else {
        return false;
    }
}

murasaki::UartStatus DebuggerUart::Receive(
                                           uint8_t * data,
                                           unsigned int size,
                                           unsigned int * transfered_count,
                                           UartTimeout uart_timeout,
                                           WaitMilliSeconds timeout_ms)
                                           {

    MURASAKI_ASSERT(nullptr != data);
    MURASAKI_ASSERT(65536 > size);

    // make this methold re-entrant in task context.
    rx_critical_section_->Enter();
    {
        // Keep coherency between the L2 and cache before DMA
        // Need to invalidate
        murasaki::CleanAndInvalidateDataCacheByAddress(data, size);

        HAL_StatusTypeDef status = HAL_UART_Receive_DMA(peripheral_, data, size);
        MURASAKI_ASSERT(HAL_OK == status);

        rx_sync_->Wait(timeout_ms);
    }
    rx_critical_section_->Leave();

    return murasaki::kursOK;  // always return OK in this class.
}

void DebuggerUart::SetSpeed(unsigned int baud_rate)
                            {
    // stop UART activity. This is required by UART HAL specification.
    int result = HAL_UART_DeInit(peripheral_);
    MURASAKI_ASSERT(result == HAL_OK);

    // Change the Speed
    peripheral_->Init.BaudRate = baud_rate;

    // restart UART acitivty.
    result = HAL_UART_Init(peripheral_);
    MURASAKI_ASSERT(result == HAL_OK);

}

bool DebuggerUart::ReceiveCompleteCallback(void* const ptr)
                                           {
    MURASAKI_ASSERT(nullptr != ptr)

    if (peripheral_ == ptr) {
        rx_sync_->Release();
        return true;
    }
    else {
        return false;
    }
}

bool DebuggerUart::HandleError(void* const ptr)
                               {
    MURASAKI_ASSERT(nullptr != ptr)

    if (this->Match(ptr)) {
        // Check error, and print if exist.
        MURASAKI_PRINT_ERROR(peripheral_->ErrorCode & HAL_UART_ERROR_DMA);
        MURASAKI_PRINT_ERROR(peripheral_->ErrorCode & HAL_UART_ERROR_PE);
        MURASAKI_PRINT_ERROR(peripheral_->ErrorCode & HAL_UART_ERROR_NE);
        MURASAKI_PRINT_ERROR(peripheral_->ErrorCode & HAL_UART_ERROR_FE);
        MURASAKI_PRINT_ERROR(peripheral_->ErrorCode & HAL_UART_ERROR_ORE);
        MURASAKI_PRINT_ERROR(peripheral_->ErrorCode & HAL_UART_ERROR_DMA);

        // Force exception by any error.
        MURASAKI_ASSERT(false);
        return true;  // report the ptr matched
    }
    else {
        return false;  // report the ptr doesn't match
    }
}

void* DebuggerUart::GetPeripheralHandle() {
    return peripheral_;
}

} /* namespace platform */

#endif // HAL_UART_MODULE_ENABLED
