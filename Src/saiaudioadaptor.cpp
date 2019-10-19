/*
 * saiaudioadaptor.cpp
 *
 *  Created on: 2019/07/28
 *      Author: Seiichi Horie
 */

#include "saiaudioadaptor.hpp"
#include "murasaki_syslog.hpp"
#include "murasaki_assert.hpp"

// Macro for easy-to-read
#define SAIAUDIO_SYSLOG(fmt, ...)    MURASAKI_SYSLOG(kfaSai, kseDebug, fmt, ##__VA_ARGS__)

namespace murasaki {
#ifdef   HAL_SAI_MODULE_ENABLED

SaiAudioAdaptor::SaiAudioAdaptor(
                                 SAI_HandleTypeDef * tx_peripheral,
                                 SAI_HandleTypeDef * rx_peripheral
                                 )
        :
          tx_peripheral_(tx_peripheral),
          rx_peripheral_(rx_peripheral)

{

}

SaiAudioAdaptor::~SaiAudioAdaptor()
{
}

bool SaiAudioAdaptor::ErrorCallback(void* peripheral) {
    SAIAUDIO_SYSLOG("Enter %p", peripheral);

    MURASAKI_ASSERT(nullptr != peripheral);

    // Is this interrupt for this peripheral?
    if (peripheral == reinterpret_cast<void *>(rx_peripheral_)) {
        SAIAUDIO_SYSLOG("Pointer matched")
        uint32_t error_code = rx_peripheral_->ErrorCode;
        // Check error and display it.
        if (HAL_SAI_ERROR_OVR | error_code) {
            MURASAKI_SYSLOG(kfaSai, kseError, "HAL_SAI_ERROR_OVR")
        }
        if (HAL_SAI_ERROR_UDR | error_code) {
            MURASAKI_SYSLOG(kfaSai, kseError, "HAL_SAI_ERROR_UDR")
        }
        if (HAL_SAI_ERROR_AFSDET | error_code) {
            MURASAKI_SYSLOG(kfaSai, kseError, "HAL_SAI_ERROR_AFSDET")
        }
        if (HAL_SAI_ERROR_LFSDET | error_code) {
            MURASAKI_SYSLOG(kfaSai, kseError, "HAL_SAI_ERROR_LFSDET")
        }
        if (HAL_SAI_ERROR_CNREADY | error_code) {
            MURASAKI_SYSLOG(kfaSai, kseError, "HAL_SAI_ERROR_CNREADY")
        }
        if (HAL_SAI_ERROR_WCKCFG | error_code) {
            MURASAKI_SYSLOG(kfaSai, kseError, "HAL_SAI_ERROR_WCKCFG")
        }
        if (HAL_SAI_ERROR_TIMEOUT | error_code) {
            MURASAKI_SYSLOG(kfaSai, kseError, "HAL_SAI_ERROR_TIMEOUT")
        }
        if (HAL_SAI_ERROR_DMA | error_code) {
            MURASAKI_SYSLOG(kfaSai, kseError, "HAL_SAI_ERROR_DMA")
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

void SaiAudioAdaptor::StartTransferTx(
                                      uint8_t * tx_buffer,
                                      unsigned int channel_len
                                      ) {
    SAIAUDIO_SYSLOG("Enter. Starting SAI peripheral tx_buffer : %p, channel_len : %d", tx_buffer, channel_len)

    MURASAKI_ASSERT(nullptr != tx_peripheral_)

    // Start the Transfer and Transmit DMA.
    // Assumes CubeMX configures these setting as circular mode. That mean, for each halfway,
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

void SaiAudioAdaptor::StartTransferRx(
                                      uint8_t * rx_buffer,
                                      unsigned int channel_len
                                      ) {
    SAIAUDIO_SYSLOG("Enter. Starting SAI peripheral rx_buffer : %p, channel_len : %d", rx_buffer, channel_len)

    MURASAKI_ASSERT(nullptr != rx_peripheral_)

    // Start the Transfer and Receive DMA.
    // Assumes CubeMX configures these setting as circular mode. That mean, for each halfway,
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

unsigned int SaiAudioAdaptor::GetNumberOfChannelsRx()
{
    SAIAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(rx_peripheral_ != nullptr)

    unsigned int return_val =
            rx_peripheral_->SlotInit.SlotNumber;   // Extract number of channels from SAI setting

    SAIAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;
}

unsigned int SaiAudioAdaptor::GetSampleWordSizeRx()
{
    SAIAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(rx_peripheral_ != nullptr)
    unsigned int return_val =
            (rx_peripheral_->SlotInit.SlotSize == SAI_SLOTSIZE_32B) ? 4 : 2;

    SAIAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;
}

unsigned int SaiAudioAdaptor::GetNumberOfChannelsTx()
{
    SAIAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(tx_peripheral_ != nullptr)
    unsigned int return_val =
            tx_peripheral_->SlotInit.SlotNumber;      // Extract number of channels from SAI setting

    SAIAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;
}

unsigned int SaiAudioAdaptor::GetSampleWordSizeTx()
{
    SAIAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(tx_peripheral_ != nullptr)

    unsigned int return_val =
            (tx_peripheral_->SlotInit.SlotSize == SAI_SLOTSIZE_32B) ? 4 : 2;

    SAIAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;
}

void* SaiAudioAdaptor::GetPeripheralHandle()
{
    SAIAUDIO_SYSLOG("Enter.")

    void * return_val = rx_peripheral_;

    SAIAUDIO_SYSLOG("Exit with %p.", return_val)
    return return_val;

}
#endif //   HAL_SAI_MODULE_ENABLED

} /* namespace murasaki */

