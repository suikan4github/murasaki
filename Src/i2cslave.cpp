/*
 * i2cslave.cpp
 *
 *  Created on: 2018/10/07
 *      Author: Seiichi "Suikan" Horie
 */

#include <i2cslave.hpp>
#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

// Macro for easy-to-read
#define I2C_SYSLOG(fmt, ...)    MURASAKI_SYSLOG( this, kfaI2cSlave, kseDebug, fmt, ##__VA_ARGS__)
// check if I2C module is enabled by CubeIDE
#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {

I2cSlave::I2cSlave(I2C_HandleTypeDef * i2c_handle)
        : peripheral_(i2c_handle),
          sync_(new Synchronizer),
          critical_section_(new CriticalSection),
          interrupt_status_(ki2csUnknown)
{
    // setup peripheral handle
    MURASAKI_ASSERT(nullptr != i2c_handle);

    // synchronization setup.
    MURASAKI_ASSERT(nullptr != sync_)

    MURASAKI_ASSERT(nullptr != critical_section_)

}

I2cSlave::~I2cSlave()
{
    if (nullptr != sync_)
        delete sync_;

    if (nullptr != critical_section_)
        delete critical_section_;
}

murasaki::I2cStatus I2cSlave::Transmit(
                                       const uint8_t* tx_data,
                                       unsigned int tx_size,
                                       unsigned int * transfered_count,
                                       unsigned int timeout_ms)
                                       {
    I2C_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != tx_data);
    MURASAKI_ASSERT(65536 >= tx_size);

    // exclusive operation
    critical_section_->Enter();
    {
        I2C_SYSLOG("Start transmitting");

        // the value will be updated by interrupt.
        interrupt_status_ = murasaki::ki2csTimeOut;
        // Transmit over I2C. Address shift is required by HAL specification.
        HAL_StatusTypeDef status = HAL_I2C_Slave_Transmit_IT(
                                                             peripheral_,
                                                             const_cast<uint8_t *>(tx_data),
                                                             tx_size);
        MURASAKI_ASSERT(HAL_OK == status);

        // wait for the completion
        sync_->Wait(timeout_ms);
        I2C_SYSLOG("Sync released.")

        // retrieve the count of transfer
        // XfereCount is initialized in HAL. The initial value is XferSize which is tx_size in this function.
        // And then XferCunt is decremented for each interrupt.
        if (transfered_count != nullptr)
            *transfered_count = tx_size - peripheral_->XferCount;

        switch (interrupt_status_)
        {
            case murasaki::ki2csOK:
                I2C_SYSLOG("Transmission complete successfully")
                break;
            case murasaki::ki2csNak:
                I2C_SYSLOG("Transmission abort by NAK or no response")
                break;
            case murasaki::ki2csArbitrationLost:
                I2C_SYSLOG("Transmission abort by arbitration Lost")
                break;
            case murasaki::ki2csBussError:
                I2C_SYSLOG("Transmission abort by bus error")
                break;
            case murasaki::ki2csTimeOut:
                MURASAKI_SYSLOG( this, kfaI2cSlave, kseWarning, "Transmission timeout")
                // Abort on-going and not terminated transfer.
                HAL_I2C_DeInit(peripheral_);
                HAL_I2C_Init(peripheral_);
                break;
            default:
                MURASAKI_SYSLOG( this, kfaI2cSlave, kseEmergency, "Error is not handled")
                // Re-initialize device
                HAL_I2C_DeInit(peripheral_);
                HAL_I2C_Init(peripheral_);

        }
    }
    critical_section_->Leave();

    I2C_SYSLOG("Leave");
    return interrupt_status_;
}

murasaki::I2cStatus I2cSlave::Receive(
                                      uint8_t* rx_data,
                                      unsigned int rx_size,
                                      unsigned int * transfered_count,
                                      unsigned int timeout_ms)
                                      {
    I2C_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != rx_data);
    MURASAKI_ASSERT(65536 >= rx_size);

    // exclusive operation
    critical_section_->Enter();
    {
        I2C_SYSLOG("Start receiving");

        // this variable will be updated by interrupt.
        interrupt_status_ = murasaki::ki2csTimeOut;

        // receive. Address shift is required by HAL specification.
        HAL_StatusTypeDef status = HAL_I2C_Slave_Receive_IT(
                                                            peripheral_,
                                                            rx_data,
                                                            rx_size);
        MURASAKI_ASSERT(HAL_OK == status);

        // wait for the completion
        sync_->Wait(timeout_ms);
        I2C_SYSLOG("Sync released.")

        // retrieve the count of transfer
        // XfereCount is initialized in HAL. The initial value is XferSize which is tx_size in this function.
        // And then XferCunt is decremented for each interrupt.
        if (transfered_count != nullptr)
            *transfered_count = rx_size - peripheral_->XferCount;

        // check the status from interrupt
        switch (interrupt_status_)
        {
            case murasaki::ki2csOK:
                I2C_SYSLOG("Receive complete successfully")
                break;
            case murasaki::ki2csNak:
                I2C_SYSLOG("Receive abort by NAK or no response")
                break;
            case murasaki::ki2csArbitrationLost:
                I2C_SYSLOG("Transmission abort by arbitration Lost")
                break;
            case murasaki::ki2csBussError:
                MURASAKI_SYSLOG( this, kfaI2cSlave, kseWarning, "Receive abort by bus error")
                break;
            case murasaki::ki2csTimeOut:
                MURASAKI_SYSLOG( this, kfaI2cSlave, kseWarning, "Receive timeout")
                // Re-initialize device
                HAL_I2C_DeInit(peripheral_);
                HAL_I2C_Init(peripheral_);
                break;
            default:
                MURASAKI_SYSLOG( this, kfaI2cSlave, kseEmergency, "Error is not handled")
                // Re-initialize device
                HAL_I2C_DeInit(peripheral_);
                HAL_I2C_Init(peripheral_);

        }

    }
    critical_section_->Leave();

    I2C_SYSLOG("Leave");
    return interrupt_status_;
}

bool I2cSlave::TransmitCompleteCallback(void* ptr)
                                        {
    I2C_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != ptr)

    // if matches, release task
    if (peripheral_ == ptr) {
        I2C_SYSLOG("Release sync");
        // report normal completion
        interrupt_status_ = murasaki::ki2csOK;
        // release waiting task
        sync_->Release();
        I2C_SYSLOG("Return with match");
        // This interrupt was for this device.
        return true;
    } else {
        I2C_SYSLOG("Leave");
        // This interrupt is not for this device
        return false;
    }
}

bool I2cSlave::ReceiveCompleteCallback(void* ptr)
                                       {
    I2C_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != ptr)

    // if matches, release task
    if (peripheral_ == ptr) {
        I2C_SYSLOG("Release sync");
        // report normal completion
        interrupt_status_ = murasaki::ki2csOK;
        // release waiting task
        sync_->Release();
        I2C_SYSLOG("Return with match");
        // This interrupt was for this device.
        return true;
    } else {
        I2C_SYSLOG("Leave");
        // This interrupt is not for this device
        return false;
    }
}

bool I2cSlave::HandleError(void* ptr)
                           {
    I2C_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != ptr)

    if (this->Match(ptr)) {
        // Check error and halde it.
        if (peripheral_->ErrorCode & HAL_I2C_ERROR_AF) {
            MURASAKI_SYSLOG( this, kfaI2cSlave, kseWarning, "HAL_I2C_ERROR_AF");
            // This interrupt happen when device doesn't respond or return NAK.
            interrupt_status_ = murasaki::ki2csNak;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_I2C_ERROR_BERR) {
            MURASAKI_SYSLOG( this, kfaI2cSlave, kseWarning, "HAL_I2C_ERROR_BERR");
            // This interrupt happen when device detected STOP/START condition at illegal place.
            interrupt_status_ = murasaki::ki2csBussError;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_I2C_ERROR_ARLO) {
            MURASAKI_SYSLOG( this, kfaI2cSlave, kseWarning, "HAL_I2C_ERROR_ARLO");
            // This interrupt happen when device causes Arbitration lost against other master
            interrupt_status_ = murasaki::ki2csArbitrationLost;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_I2C_ERROR_OVR) {
            MURASAKI_SYSLOG( this, kfaI2cSlave, kseWarning, "HAL_I2C_ERROR_OVR");
            // This interrupt happen when device causes Overrun or Underrun. Slave only
            interrupt_status_ = murasaki::ki2csOverrun;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_I2C_ERROR_DMA) {
            MURASAKI_SYSLOG( this, kfaI2cSlave, kseWarning, "HAL_I2C_ERROR_DMA");
            // This interrupt happen when some error is detected in DMA.
            interrupt_status_ = murasaki::ki2csDMA;
            // abort the processing
            sync_->Release();
        }
        #ifdef         HAL_I2C_ERROR_SIZE
        else if (peripheral_->ErrorCode & HAL_I2C_ERROR_SIZE) {
            MURASAKI_SYSLOG( this, kfaI2cSlave, kseWarning, "HAL_I2C_ERROR_SIZE");
            // This interrupt happen when.
            interrupt_status_ = murasaki::ki2csSizeError;
            // abort the processing
            sync_->Release();
        }
        #endif
        else {
            MURASAKI_SYSLOG( this, kfaI2cSlave, kseEmergency, "Unknown error");
            // Unknown interrupt. Must be updated this program by the newest HAL spec.
            interrupt_status_ = murasaki::ki2csUnknown;
            // abort the processing
            sync_->Release();
        }
        I2C_SYSLOG("Return with match");
        return true;  // report the ptr matched
    } else {
        I2C_SYSLOG("Leave");
        return false;  // report the ptr doesn't match
    }

}

void* I2cSlave::GetPeripheralHandle()
{
    I2C_SYSLOG("Enter");
    I2C_SYSLOG("Leave");

    return peripheral_;
}

} /* namespace murasaki */

#endif
