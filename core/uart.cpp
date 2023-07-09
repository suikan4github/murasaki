/*
 * Uart.cpp
 *
 *  Created on: 2017/11/05
 *      Author: Seiichi "Suikan" Horie
 */

#include "uart.hpp"
#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"
#include "callbackrepositorysingleton.hpp"

// Macro for easy-to-read
#define UART_SYSLOG(fmt, ...)    MURASAKI_SYSLOG( this, kfaSerial, kseDebug, fmt, ##__VA_ARGS__)

// Check if CubeIDE generates UART module
#ifdef HAL_UART_MODULE_ENABLED

namespace murasaki {

Uart::Uart(UART_HandleTypeDef *const uart)
        :
        peripheral_(uart),
        tx_sync_(new murasaki::Synchronizer),
        rx_sync_(new murasaki::Synchronizer),
        tx_critical_section_(new murasaki::CriticalSection),
        rx_critical_section_(new murasaki::CriticalSection),
        tx_interrupt_status_(kursUnknown),
        rx_interrupt_status_(kursUnknown)
{
    // Setup internal variable with given uart structure.

    MURASAKI_ASSERT(nullptr != uart)
    MURASAKI_ASSERT(nullptr != tx_sync_)
    MURASAKI_ASSERT(nullptr != rx_sync_)
    MURASAKI_ASSERT(nullptr != tx_critical_section_)
    MURASAKI_ASSERT(nullptr != rx_critical_section_)

    rx_interrupt_status_ = murasaki::kursUnknown;
    tx_interrupt_status_ = murasaki::kursUnknown;

    // We need DMA
    MURASAKI_ASSERT(peripheral_->hdmatx != nullptr)
    MURASAKI_ASSERT(peripheral_->hdmarx != nullptr)

// Cortex M33 MCU ( SMT32H5 ) DMA driver does not have the Init.PeripheralDataAlignment field and 
// the Init.MemDataAlignment field. 
#ifndef __CORE_CM33_H_GENERIC
    // For all UART transfer, the data size have to be byte
    MURASAKI_ASSERT(peripheral_->hdmatx->Init.PeriphDataAlignment == DMA_PDATAALIGN_BYTE)
    MURASAKI_ASSERT(peripheral_->hdmarx->Init.PeriphDataAlignment == DMA_PDATAALIGN_BYTE)
    MURASAKI_ASSERT(peripheral_->hdmatx->Init.MemDataAlignment == DMA_MDATAALIGN_BYTE)
    MURASAKI_ASSERT(peripheral_->hdmarx->Init.MemDataAlignment == DMA_MDATAALIGN_BYTE)
#endif 

    // The DMA mode have to be normal
    MURASAKI_ASSERT(peripheral_->hdmatx->Init.Mode == DMA_NORMAL)
    MURASAKI_ASSERT(peripheral_->hdmarx->Init.Mode == DMA_NORMAL)

    // Register this object to the list of the interrupt handler class.
    CallbackRepositorySingleton::GetInstance()->AddPeripheralObject(this);

}

Uart::~Uart()
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

void Uart::SetHardwareFlowControl(UartHardwareFlowControl control)
                                  {
    UART_SYSLOG("Enter");

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
    // restart UART activity.
    result = HAL_UART_Init(peripheral_);
    MURASAKI_ASSERT(result == HAL_OK);

    UART_SYSLOG("Leave");
}

murasaki::UartStatus Uart::Transmit(
                                    const uint8_t *data,
                                    unsigned int size,
                                    unsigned int timeout_ms)
                                    {
    UART_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != data)
    MURASAKI_ASSERT(65536 >= size);

    // make this method re-entrant in task context.
    tx_critical_section_->Enter();
    {
        UART_SYSLOG("Start transmitting")
        // The value will be filled by interrupt.
        tx_interrupt_status_ = murasaki::kursTimeOut;
        // Keep coherence between the L2 and cache before DMA
        // No need to invalidate
        murasaki::CleanDataCacheByAddress(
                                          const_cast<uint8_t*>(data),
                                          size);

        HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(peripheral_, const_cast<uint8_t*>(data), size);
        MURASAKI_ASSERT(HAL_OK == status);

        tx_sync_->Wait(timeout_ms);
        UART_SYSLOG("Sync released");

        // check result
        switch (tx_interrupt_status_)
        {
            case murasaki::kursOK:
                UART_SYSLOG("Transmission complete successfully")
                break;
            case murasaki::kursTimeOut:
                MURASAKI_SYSLOG(this, kfaSerial, kseWarning, "Transmission timeout")
                // TODO: probably, we should think how to know the number of transmission.
                break;
            default:
                MURASAKI_SYSLOG(this, kfaSerial, kseEmergency, "Error is not handled")
                // Re-initializing device
                HAL_UART_DeInit(peripheral_);
                HAL_UART_Init(peripheral_);
        }
    }
    tx_critical_section_->Leave();

    UART_SYSLOG("Leave");
    return tx_interrupt_status_;
}

bool Uart::TransmitCompleteCallback(void *const ptr)
                                    {
    UART_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != ptr)

    if (ptr == peripheral_) {
        UART_SYSLOG("Release Sync")
        // report normal completion
        tx_interrupt_status_ = murasaki::kursOK;
        // release waiting task
        tx_sync_->Release();
        UART_SYSLOG("Return with match");
        // this interrupt is not for this device
        return true;
    }
    else {
        UART_SYSLOG("Leave");
        // this interrupt is not for this device
        return false;
    }
}

murasaki::UartStatus Uart::Receive(
                                   uint8_t *data,
                                   unsigned int size,
                                   unsigned int *transfered_count,
                                   UartTimeout uart_timeout,
                                   unsigned int timeout_ms)
                                   {
    UART_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != data);
    MURASAKI_ASSERT(65536 > size);

    // make this method re-entrant in task context.
    rx_critical_section_->Enter();
    {
        UART_SYSLOG("Start receiving")

        rx_interrupt_status_ = murasaki::kursTimeOut;
        // Keep coherence between the L2 and cache before DMA
        // Need to invalidate
        murasaki::CleanAndInvalidateDataCacheByAddress(data, size);

        HAL_StatusTypeDef status = HAL_UART_Receive_DMA(peripheral_, data, size);
        MURASAKI_ASSERT(HAL_OK == status);

        rx_sync_->Wait(timeout_ms);
        UART_SYSLOG("Sync released")

        // check result
        switch (rx_interrupt_status_)
        {
            case murasaki::kursOK:
                UART_SYSLOG("Receiving complete successfully")
                break;
            case murasaki::kursTimeOut:
                MURASAKI_SYSLOG(this, kfaSerial, kseWarning, "Receiving timeout")
                // return without resetting device.
                break;
            case murasaki::kursFrame:
                case murasaki::kursParity:
                case murasaki::kursNoise:
                MURASAKI_SYSLOG(this, kfaSerial, kseWarning, "Receiving error by frame, parity or noise error")
                // return without resetting device.
                break;
            case murasaki::kursOverrun:
                MURASAKI_SYSLOG(this, kfaSerial, kseError, "Overrun error on transmission. ")
                break;
            case murasaki::kursDMA:
                MURASAKI_SYSLOG(this, kfaSerial, kseError, "Un-recoverable DMA error. Peripheral re-initialized")
                // Re-initializing device
                HAL_UART_DeInit(peripheral_);
                HAL_UART_Init(peripheral_);
                break;
            default:
                MURASAKI_SYSLOG(this, kfaSerial, kseEmergency, "Error is not handled. Peripheral re-initialized.")
                // Re-initializing device
                HAL_UART_DeInit(peripheral_);
                HAL_UART_Init(peripheral_);
                break;
        }
    }
    rx_critical_section_->Leave();

    UART_SYSLOG("Leave");
    return rx_interrupt_status_;
}

void Uart::SetSpeed(unsigned int baud_rate)
                    {
    UART_SYSLOG("Enter");
    // stop UART activity. This is required by UART HAL specification.
    int result = HAL_UART_DeInit(peripheral_);
    MURASAKI_ASSERT(result == HAL_OK);

    // Change the Speed
    peripheral_->Init.BaudRate = baud_rate;

    // restart UART acitivty.
    result = HAL_UART_Init(peripheral_);
    MURASAKI_ASSERT(result == HAL_OK);

    UART_SYSLOG("Leave");
}

bool Uart::ReceiveCompleteCallback(void *const ptr)
                                   {
    UART_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != ptr)

    if (peripheral_ == ptr) {
        UART_SYSLOG("Release Sync")
        // report normal communication
        rx_interrupt_status_ = murasaki::kursOK;
        // releasing sync
        rx_sync_->Release();
        UART_SYSLOG("Return with match");
        // This interrupt was for this device.
        return true;
    }
    else {
        UART_SYSLOG("Leave");
        // This interrupt was not for this device
        return false;
    }
}

bool Uart::HandleError(void *const ptr)
                       {
    UART_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != ptr)

    if (this->Match(ptr)) {
        // Check error and halde it.
        if (peripheral_->ErrorCode & HAL_UART_ERROR_PE) {
            MURASAKI_SYSLOG(this, kfaSerial, kseWarning, "HAL_UART_ERROR_PE");
            // This interrupt happen when RX cause parity error.
            rx_interrupt_status_ = murasaki::kursParity;
            // abort the processing
            rx_sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_UART_ERROR_FE) {
            MURASAKI_SYSLOG(this, kfaSerial, kseWarning, "HAL_UART_ERROR_FE");
            // This interrupt happen when rx detect the character frame is wrong.
            rx_interrupt_status_ = murasaki::kursFrame;
            // abort the processing
            rx_sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_UART_ERROR_NE) {
            MURASAKI_SYSLOG(this, kfaSerial, kseWarning, "HAL_UART_ERROR_NE");
            // This interrupt happen when rx detect the problem by noise.
            rx_interrupt_status_ = murasaki::kursNoise;
            // abort the processing
            rx_sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_UART_ERROR_ORE) {
            MURASAKI_SYSLOG(this, kfaSerial, kseWarning, "HAL_UART_ERROR_ORE");
            // This interrupt happen when rx register is full and next data comes.
            rx_interrupt_status_ = murasaki::kursOverrun;
            // abort the processing
            rx_sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_UART_ERROR_DMA) {
            MURASAKI_SYSLOG(this, kfaSerial, kseWarning, "HAL_UART_ERROR_DMA");
            // This interrupt happen when something problem happen in DMA. This is fatal.
            rx_interrupt_status_ = murasaki::kursDMA;
            // abort the processing
            rx_sync_->Release();
        }
        else {
            MURASAKI_SYSLOG(this, kfaSerial, kseEmergency, "Unknown error");
            // Unknown interrupt. Must be updated this program by the newest HAL spec.
            rx_interrupt_status_ = murasaki::kursUnknown;
            // abort the processing
            rx_sync_->Release();
        }

        UART_SYSLOG("Leave");
        return true;  // report the ptr matched
    }
    else {
        UART_SYSLOG("Return with match");
        return false;  // report the ptr doesn't match
    }
}

void* Uart::GetPeripheralHandle() {
    UART_SYSLOG("Enter");
    UART_SYSLOG("Leave");

    return peripheral_;
}

} /* namespace platform */

#endif // HAL_UART_MODULE_ENABLED
