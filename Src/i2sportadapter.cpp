/*
 * i2sportadapter.cpp
 *
 *  Created on: 2020/01/15
 *      Author: Seiichi Horie
 */

#include <i2sportadapter.hpp>
#include "murasaki_syslog.hpp"
#include "murasaki_assert.hpp"

// Macro for easy-to-read
#define I2SAUDIO_SYSLOG(fmt, ...)    MURASAKI_SYSLOG( this , kfaI2s, kseDebug, fmt, ##__VA_ARGS__)

namespace murasaki {
#ifdef   HAL_I2S_MODULE_ENABLED

I2sPortAdapter::I2sPortAdapter(
                               I2S_HandleTypeDef *tx_peripheral,
                               I2S_HandleTypeDef *rx_peripheral
                                 )
        :
          tx_peripheral_(tx_peripheral),
          rx_peripheral_(rx_peripheral)

{
    MURASAKI_ASSERT(tx_peripheral_ != nullptr || rx_peripheral_ != nullptr)
}

I2sPortAdapter::~I2sPortAdapter()
{
}

bool I2sPortAdapter::HandleError(void *ptr) {
    I2SAUDIO_SYSLOG("Enter %p", ptr);

    MURASAKI_ASSERT(nullptr != ptr);

    // Is this interrupt for this peripheral?
    if (this->Match(ptr)) {
        I2SAUDIO_SYSLOG("Pointer matched")
        uint32_t error_code = rx_peripheral_->ErrorCode;
        // Check error and display it.
        if (HAL_I2S_ERROR_OVR | error_code) {
            MURASAKI_SYSLOG(this, kfaSai, kseError, "HAL_I2S_ERROR_OVR")
        }
        if (HAL_I2S_ERROR_UDR | error_code) {
            MURASAKI_SYSLOG(this, kfaSai, kseError, "HAL_I2S_ERROR_UDR")
        }
        if (HAL_I2S_ERROR_PRESCALER | error_code) {
            MURASAKI_SYSLOG(this, kfaSai, kseError, "HAL_I2S_ERROR_PRESCALER")
        }
        if (HAL_I2S_ERROR_TIMEOUT | error_code) {
            MURASAKI_SYSLOG(this, kfaSai, kseError, "HAL_I2S_ERROR_TIMEOUT")
        }
        if (HAL_I2S_ERROR_DMA | error_code) {
            MURASAKI_SYSLOG(this, kfaSai, kseError, "HAL_I2S_ERROR_DMA")
        }
        // This is fatal condition.
        I2SAUDIO_SYSLOG("Device matched.")
        MURASAKI_ASSERT(false)
        return true;
    }
    else {
        I2SAUDIO_SYSLOG("Return without match")
        return false;
    }

}

void I2sPortAdapter::StartTransferTx(
                                      uint8_t * tx_buffer,
                                      unsigned int channel_len
                                      ) {
    I2SAUDIO_SYSLOG("Enter. Starting I2S peripheral tx_buffer : %p, channel_len : %d", tx_buffer, channel_len)

    MURASAKI_ASSERT(nullptr != tx_peripheral_)

    // Start the Transfer and Transmit DMA.
    // Assumes CubeIDE configures these setting as circular mode. That mean, for each halfway,
    // interrupt is raised, in addition to the end of buffer interrupt.
    // Note, STM32 HAL requirement of the size parameter is ambiguous. There is no description whether
    // It should be in Byte or Word.
    HAL_I2S_Transmit_DMA(
                         tx_peripheral_,
                         reinterpret_cast<uint16_t*>(tx_buffer),
                         GetNumberOfDMAPhase() * GetNumberOfChannelsTx() * channel_len
                                 );

    I2SAUDIO_SYSLOG("Return")
}

void I2sPortAdapter::StartTransferRx(
                                      uint8_t * rx_buffer,
                                      unsigned int channel_len
                                      ) {
    I2SAUDIO_SYSLOG("Enter. Starting I2S peripheral rx_buffer : %p, channel_len : %d", rx_buffer, channel_len)

    MURASAKI_ASSERT(nullptr != rx_peripheral_)

    // Start the Transfer and Receive DMA.
    // Assumes CubeIDE configures these setting as circular mode. That mean, for each halfway,
    // Interrupt is raised, in addition to the end of buffer interrupt.
    // Note, HAL requirement of the size parameter is ambiguous. There is no description whether
    // It should be in Byte or Word.

    HAL_I2S_Receive_DMA(
                        rx_peripheral_,
                        reinterpret_cast<uint16_t*>(rx_buffer),
                        GetNumberOfDMAPhase() * GetNumberOfChannelsRx() * channel_len  // Assume the unit is word, not byte.
                                );
    I2SAUDIO_SYSLOG("Return")
}

unsigned int I2sPortAdapter::GetNumberOfChannelsRx()
{
    I2SAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(rx_peripheral_ != nullptr)

    // Channel number of I2S peripheral is always 2.
    unsigned int return_val = 2;

    I2SAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;
}

unsigned int I2sPortAdapter::GetSampleWordSizeRx()
{
    I2SAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(rx_peripheral_ != nullptr)
    unsigned int return_val =
            (rx_peripheral_->Init.DataFormat == I2S_DATAFORMAT_16B) ? 2 : 4;

    I2SAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;
}

unsigned int I2sPortAdapter::GetNumberOfChannelsTx()
{
    I2SAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(tx_peripheral_ != nullptr)

    // Channel number of I2S peripheral is always 2.
    unsigned int return_val = 2;

    I2SAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;
}

unsigned int I2sPortAdapter::GetSampleWordSizeTx()
{
    I2SAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(tx_peripheral_ != nullptr)

    unsigned int return_val =
            (tx_peripheral_->Init.DataFormat == I2S_DATAFORMAT_16B) ? 2 : 4;

    I2SAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;
}

bool I2sPortAdapter::Match(void *peripheral_handle) {
    I2SAUDIO_SYSLOG("Enter.")
    MURASAKI_ASSERT(peripheral_handle != nullptr)

    bool return_val;

    if (rx_peripheral_ != nullptr)
        return_val = rx_peripheral_ == peripheral_handle;
    else if (tx_peripheral_ != nullptr)
        return_val = tx_peripheral_ == peripheral_handle;

    I2SAUDIO_SYSLOG("Exit with %d.", return_val)
    return return_val;

}

void* I2sPortAdapter::GetPeripheralHandle()
{
    I2SAUDIO_SYSLOG("Enter.")

    void * return_val = rx_peripheral_;

    I2SAUDIO_SYSLOG("Exit with %p.", return_val)
    return return_val;

}
#endif //   HAL_I2S_MODULE_ENABLED

} /* namespace murasaki */

