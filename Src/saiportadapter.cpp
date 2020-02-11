/*
 * saiportadapter.cpp
 *
 *  Created on: 2019/07/28
 *      Author: Seiichi Horie
 */

#include "saiportadapter.hpp"
#include "murasaki_syslog.hpp"
#include "murasaki_assert.hpp"

// Macro for easy-to-read
#define SAIAUDIO_SYSLOG(fmt, ...)    MURASAKI_SYSLOG( this , kfaSai, kseDebug, fmt, ##__VA_ARGS__)

namespace murasaki {
#ifdef   HAL_SAI_MODULE_ENABLED

SaiPortAdapter::SaiPortAdapter(
                                 SAI_HandleTypeDef * tx_peripheral,
                                 SAI_HandleTypeDef * rx_peripheral
                                 )
        :
          tx_peripheral_(tx_peripheral),
          rx_peripheral_(rx_peripheral)

{
    // At least one of two peripheral have to be not null.
    MURASAKI_ASSERT(tx_peripheral_ != nullptr || rx_peripheral_ != nullptr)

    // Is TX peripheral is correctly configureed as TX?
    if (tx_peripheral_ != nullptr) {
        MURASAKI_ASSERT(tx_peripheral_->Init.AudioMode == SAI_MODEMASTER_TX ||
                        tx_peripheral_->Init.AudioMode == SAI_MODESLAVE_TX)
    }

    // Is RX peripheral is correctly configureed as RX?
    if (rx_peripheral_ != nullptr) {
        MURASAKI_ASSERT(rx_peripheral_->Init.AudioMode == SAI_MODEMASTER_RX ||
                        rx_peripheral_->Init.AudioMode == SAI_MODESLAVE_RX)
    }

}

SaiPortAdapter::~SaiPortAdapter()
{
}

bool SaiPortAdapter::HandleError(void *ptr) {
    SAIAUDIO_SYSLOG("Enter %p", ptr);

    MURASAKI_ASSERT(nullptr != ptr);

    // Is this interrupt for this peripheral?
    if (this->Match(ptr)) {
        SAIAUDIO_SYSLOG("Pointer matched")
        uint32_t error_code = rx_peripheral_->ErrorCode;
        // Check error and display it.
        if (HAL_SAI_ERROR_OVR | error_code) {
            MURASAKI_SYSLOG( this, kfaSai, kseError, "HAL_SAI_ERROR_OVR")
        }
        if (HAL_SAI_ERROR_UDR | error_code) {
            MURASAKI_SYSLOG( this, kfaSai, kseError, "HAL_SAI_ERROR_UDR")
        }
        if (HAL_SAI_ERROR_AFSDET | error_code) {
            MURASAKI_SYSLOG( this, kfaSai, kseError, "HAL_SAI_ERROR_AFSDET")
        }
        if (HAL_SAI_ERROR_LFSDET | error_code) {
            MURASAKI_SYSLOG( this, kfaSai, kseError, "HAL_SAI_ERROR_LFSDET")
        }
        if (HAL_SAI_ERROR_CNREADY | error_code) {
            MURASAKI_SYSLOG( this, kfaSai, kseError, "HAL_SAI_ERROR_CNREADY")
        }
        if (HAL_SAI_ERROR_WCKCFG | error_code) {
            MURASAKI_SYSLOG( this, kfaSai, kseError, "HAL_SAI_ERROR_WCKCFG")
        }
        if (HAL_SAI_ERROR_TIMEOUT | error_code) {
            MURASAKI_SYSLOG( this, kfaSai, kseError, "HAL_SAI_ERROR_TIMEOUT")
        }
        if (HAL_SAI_ERROR_DMA | error_code) {
            MURASAKI_SYSLOG( this, kfaSai, kseError, "HAL_SAI_ERROR_DMA")
        }
        // This is fatal condition.
        SAIAUDIO_SYSLOG("Device matched.")
        MURASAKI_ASSERT(false)
        return true;
    }
    else {
        SAIAUDIO_SYSLOG("Return without match")
        return false;
    }

}

void SaiPortAdapter::StartTransferTx(
                                      uint8_t * tx_buffer,
                                      unsigned int channel_len
                                      ) {
    SAIAUDIO_SYSLOG("Enter. Starting SAI peripheral tx_buffer : %p, channel_len : %d", tx_buffer, channel_len)

    MURASAKI_ASSERT(nullptr != tx_peripheral_)

    // Start the Transfer and Transmit DMA.
    // Assumes CubeIDE configures these setting as circular mode. That mean, for each halfway,
    // interrupt is raised, in addition to the end of buffer interrupt.
    // Note, STM32 HAL requirement of the size parameter is ambiguous. There is no description whether
    // It should be in Byte or Word.
    HAL_SAI_Transmit_DMA(
                         tx_peripheral_,
                         tx_buffer,
                         GetNumberOfDMAPhase() * GetNumberOfChannelsTx() * channel_len
                                 );

    SAIAUDIO_SYSLOG("Return")
}

void SaiPortAdapter::StartTransferRx(
                                      uint8_t * rx_buffer,
                                      unsigned int channel_len
                                      ) {
    SAIAUDIO_SYSLOG("Enter. Starting SAI peripheral rx_buffer : %p, channel_len : %d", rx_buffer, channel_len)

    MURASAKI_ASSERT(nullptr != rx_peripheral_)

    // Start the Transfer and Receive DMA.
    // Assumes CubeIDE configures these setting as circular mode. That mean, for each halfway,
    // Interrupt is raised, in addition to the end of buffer interrupt.
    // Note, HAL requirement of the size parameter is ambiguous. There is no description whether
    // It should be in Byte or Word.

    HAL_SAI_Receive_DMA(
                        rx_peripheral_,
                        rx_buffer,
                        GetNumberOfDMAPhase() * GetNumberOfChannelsRx() * channel_len  // Assume the unit is word, not byte.
                                );
    SAIAUDIO_SYSLOG("Return")
}

unsigned int SaiPortAdapter::GetNumberOfChannelsRx()
{
    SAIAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(rx_peripheral_ != nullptr)

    unsigned int return_val =
            rx_peripheral_->SlotInit.SlotNumber;  // Extract number of channels from SAI setting

    SAIAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;
}

unsigned int SaiPortAdapter::GetSampleWordSizeRx()
{
    SAIAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(rx_peripheral_ != nullptr)

    unsigned int return_val;

    switch (rx_peripheral_->Init.DataSize) {
        case SAI_DATASIZE_16:
            return_val = 2;
            break;
        case SAI_DATASIZE_24:
            case SAI_DATASIZE_32:
            return_val = 4;
            break;
        default:
            MURASAKI_SYSLOG(this, kfaSai, kseError, "Unexpected data size")
            MURASAKI_ASSERT(false)
            // force assertion.
    }

    SAIAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;
}

unsigned int SaiPortAdapter::GetNumberOfChannelsTx()
{
    SAIAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(tx_peripheral_ != nullptr)
    unsigned int return_val =
            tx_peripheral_->SlotInit.SlotNumber;  // Extract number of channels from SAI setting

    SAIAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;
}

unsigned int SaiPortAdapter::GetSampleWordSizeTx()
{
    SAIAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(tx_peripheral_ != nullptr)

    unsigned int return_val;

    switch (rx_peripheral_->Init.DataSize) {
        case SAI_DATASIZE_16:
            return_val = 2;
            break;
        case SAI_DATASIZE_24:
            case SAI_DATASIZE_32:
            return_val = 4;
            break;
        default:
            MURASAKI_SYSLOG(this, kfaSai, kseError, "Unexpected data size")
            MURASAKI_ASSERT(false)
            // force assertion.
    }

    SAIAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;
}

bool SaiPortAdapter::Match(void *peripheral_handle) {
    SAIAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(peripheral_handle != nullptr)

    bool return_val;

    if (rx_peripheral_ != nullptr)
        return_val = rx_peripheral_ == peripheral_handle;
    else if (tx_peripheral_ != nullptr)
        return_val = tx_peripheral_ == peripheral_handle;

    SAIAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;

}

void* SaiPortAdapter::GetPeripheralHandle()
{
    SAIAUDIO_SYSLOG("Enter.")

    void * return_val = rx_peripheral_;

    SAIAUDIO_SYSLOG("Exit with %p.", return_val)
    return return_val;

}

bool SaiPortAdapter::IsInt16SwapRequired()
{
    SAIAUDIO_SYSLOG("Enter.")

    bool return_val = false;

    SAIAUDIO_SYSLOG("Exit with %s.", return_val ? "true" : "false")

    return return_val;

}


#endif //   HAL_SAI_MODULE_ENABLED

} /* namespace murasaki */

