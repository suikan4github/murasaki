/*
 * i2cmaster.cpp
 *
 *  Created on: 2018/02/12
 *      Author: Seiichi "Suikan" Horie
 */

#include <i2cmaster.hpp>
#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"
#include "callbackrepositorysingleton.hpp"

// Macro for easy-to-read
#define I2C_SYSLOG(fmt, ...)    MURASAKI_SYSLOG( this,  kfaI2cMaster, kseDebug, fmt, ##__VA_ARGS__)

// check if I2C module is enabled by CubeIDE
#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {

I2cMaster::I2cMaster(I2C_HandleTypeDef *i2c_handle)
        :
        peripheral_(i2c_handle),
        sync_(new Synchronizer),
        critical_section_(new CriticalSection),
        interrupt_status_(ki2csUnknown)

{
    // setup peripheral handle
    MURASAKI_ASSERT(nullptr != i2c_handle);

    // synchronization setup.
    MURASAKI_ASSERT(nullptr != sync_)

    MURASAKI_ASSERT(nullptr != critical_section_)

    // Register this object to the list of the interrupt handler class.
    CallbackRepositorySingleton::GetInstance()->AddPeripheralObject(this);
}

I2cMaster::~I2cMaster() {
    if (nullptr != sync_)
        delete sync_;

    if (nullptr != critical_section_)
        delete critical_section_;
}

murasaki::I2cStatus I2cMaster::Transmit(
                                        unsigned int addrs,
                                        const uint8_t *tx_data,
                                        unsigned int tx_size,
                                        unsigned int *transfered_count,
                                        unsigned int timeout_ms) {

    I2C_SYSLOG("Enter");

    MURASAKI_ASSERT(128 > addrs);
    MURASAKI_ASSERT(nullptr != tx_data);
    MURASAKI_ASSERT(65536 >= tx_size);

    // exclusive operation
    critical_section_->Enter();
    {
        I2C_SYSLOG("Start transmitting");

        // the value will be updated by interrupt.
        interrupt_status_ = murasaki::ki2csTimeOut;
        // Transmit over I2C. Address shift is required by HAL specification.
        HAL_StatusTypeDef status = HAL_I2C_Master_Transmit_IT(
                                                              peripheral_,
                                                              addrs << 1,
                                                              const_cast<uint8_t*>(tx_data),
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
                MURASAKI_SYSLOG(this, kfaI2cMaster, kseWarning, "Transmission timeout. I2C aborted")
                // Abort on-going and not terminated transfer.
                HAL_I2C_Master_Abort_IT(peripheral_, addrs << 1);
                break;
            default:
                MURASAKI_SYSLOG(this, kfaI2cMaster, kseEmergency, "Error is not handled. Peripheral re-initialized")
                // Re-initialize device
                HAL_I2C_DeInit(peripheral_);
                HAL_I2C_Init(peripheral_);
                break;
        }
    }
    critical_section_->Leave();

    I2C_SYSLOG("Leave");
    return interrupt_status_;
}

murasaki::I2cStatus I2cMaster::Receive(
                                       unsigned int addrs,
                                       uint8_t *rx_data,
                                       unsigned int rx_size,
                                       unsigned int *transfered_count,
                                       unsigned int timeout_ms) {

    I2C_SYSLOG("Enter");

    MURASAKI_ASSERT(128 > addrs);
    MURASAKI_ASSERT(nullptr != rx_data);
    MURASAKI_ASSERT(65536 >= rx_size);

    // exclusive operation
    critical_section_->Enter();
    {
        I2C_SYSLOG("Start receiving");

        // this variable will be updated by interrupt.
        interrupt_status_ = murasaki::ki2csTimeOut;

        // receive. Address shift is required by HAL specification.
        HAL_StatusTypeDef status = HAL_I2C_Master_Receive_IT(
                                                             peripheral_,
                                                             addrs << 1,
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
                I2C_SYSLOG("Transmission abort by bus error")
                break;
            case murasaki::ki2csTimeOut:
                MURASAKI_SYSLOG(this, kfaI2cMaster, kseWarning, "Receive timeout. I2C aborted")
                // Abort on-going and not terminated transfer.
                HAL_I2C_Master_Abort_IT(peripheral_, addrs << 1);
                break;
            default:
                MURASAKI_SYSLOG(this, kfaI2cMaster, kseEmergency, "Error is not handled. Peripheral re-initialized")
                // Re-initialize device
                HAL_I2C_DeInit(peripheral_);
                HAL_I2C_Init(peripheral_);
                break;
        }

    }
    critical_section_->Leave();

    I2C_SYSLOG("Leave");
    return interrupt_status_;
}

murasaki::I2cStatus I2cMaster::TransmitThenReceive(
                                                   unsigned int addrs,
                                                   const uint8_t *tx_data,
                                                   unsigned int tx_size,
                                                   uint8_t *rx_data,
                                                   unsigned int rx_size,
                                                   unsigned int *tx_transfered_count,
                                                   unsigned int *rx_transfered_count,
                                                   unsigned int timeout_ms) {

    I2C_SYSLOG("Enter");

    MURASAKI_ASSERT(128 > addrs);
    MURASAKI_ASSERT(nullptr != tx_data);
    MURASAKI_ASSERT(65536 >= tx_size);
    MURASAKI_ASSERT(nullptr != rx_data);
    MURASAKI_ASSERT(65536 >= rx_size);
    critical_section_->Enter();
    {
        // Transmit without Stop status. Address shift is required by HAL specification.
        HAL_StatusTypeDef status;

        I2C_SYSLOG("Start transmission");

        // This value will be updated by interrupt handler
        interrupt_status_ = murasaki::ki2csTimeOut;


        status = HAL_I2C_Master_Sequential_Transmit_IT(
                                                       peripheral_,
                                                       addrs << 1,
                                                       const_cast<uint8_t*>(tx_data),
                                                       tx_size,
                                                       I2C_FIRST_FRAME);
        MURASAKI_ASSERT(HAL_OK == status);

        // wait for the completion
        sync_->Wait(timeout_ms);
        I2C_SYSLOG("Sync released.")

        // retrieve the count of transfer
        // XfereCount is initialized in HAL. The initial value is XferSize which is tx_size in this function.
        // And then XferCunt is decremented for each interrupt.
        if (tx_transfered_count != nullptr)
            *tx_transfered_count = tx_size - peripheral_->XferCount;

        // In case of the TX problem, the RX may be skipped.
        // To save that situation, initialize the value.
        *rx_transfered_count = 0;
        // check the status from interrupt
        switch (interrupt_status_)
        {
            case murasaki::ki2csOK:
                I2C_SYSLOG("Transmission complete successfully")

                I2C_SYSLOG("Repeated start receiving")

                // This value will be updated by interrupt handler
                interrupt_status_ = murasaki::ki2csTimeOut;

                // receive with restart condition. Address shift is required by HAL specification.

                status = HAL_I2C_Master_Sequential_Receive_IT(
                                                              peripheral_,
                                                              addrs << 1,
                                                              rx_data,
                                                              rx_size,
                                                              I2C_LAST_FRAME);
                MURASAKI_ASSERT(HAL_OK == status)
                ;

                sync_->Wait(timeout_ms);
                I2C_SYSLOG("Sync released.")

                // retrieve the count of transfer
                // XfereCount is initialized in HAL. The initial value is XferSize which is tx_size in this function.
                // And then XferCunt is decremented for each interrupt.
                if (rx_transfered_count != nullptr)
                    *rx_transfered_count = rx_size - peripheral_->XferCount;

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
                        I2C_SYSLOG("Transmission abort by bus error")
                        break;
                    case murasaki::ki2csTimeOut:
                        MURASAKI_SYSLOG(this, kfaI2cMaster, kseWarning, "Receive timeout. I2C aborted.")
                        // Abort on-going and not terminated transfer.
                        HAL_I2C_Master_Abort_IT(peripheral_, addrs << 1);
                        break;
                    default:
                        MURASAKI_SYSLOG(this, kfaI2cMaster, kseEmergency, "Error is not handled. Peripheral re-initialized")
                        // Re-initialize device
                        HAL_I2C_DeInit(peripheral_);
                        HAL_I2C_Init(peripheral_);
                        break;
                }

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
                MURASAKI_SYSLOG(this, kfaI2cMaster, kseWarning, "Transmission timeout. I2C aborted.")
                // Abort on-going and not terminated transfer.
                HAL_I2C_Master_Abort_IT(peripheral_, addrs << 1);
                break;
            default:
                MURASAKI_SYSLOG(this, kfaI2cMaster, kseEmergency, "Error is not handled.  Peripheral re-initialized")
                // Re-initialize device
                HAL_I2C_DeInit(peripheral_);
                HAL_I2C_Init(peripheral_);
                break;
        }

    }
    critical_section_->Leave();

    I2C_SYSLOG("Leave");
    return interrupt_status_;  // return false if timeout
}

bool I2cMaster::TransmitCompleteCallback(void *ptr) {
    I2C_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != ptr)

    // if matches, release task
    if (peripheral_ == ptr) {
        I2C_SYSLOG("Release sync");
        // report normal completion
        interrupt_status_ = murasaki::ki2csOK;
        // release waiting task
        sync_->Release();
        I2C_SYSLOG("Return wih match");
        // This interrupt was for this device.
        return true;
    } else {
        I2C_SYSLOG("Leave");
        // This interrupt is not for this device
        return false;
    }
}

bool I2cMaster::ReceiveCompleteCallback(void *ptr) {
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

bool I2cMaster::HandleError(void *ptr) {
    I2C_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != ptr)

    if (this->Match(ptr)) {
        // Check error and halde it.
        if (peripheral_->ErrorCode & HAL_I2C_ERROR_AF) {
            MURASAKI_SYSLOG(this, kfaI2cMaster, kseWarning, "HAL_I2C_ERROR_AF");
            // This interrupt happen when device doesn't respond or return NAK.
            interrupt_status_ = murasaki::ki2csNak;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_I2C_ERROR_BERR) {
            MURASAKI_SYSLOG(this, kfaI2cMaster, kseWarning, "HAL_I2C_ERROR_BERR");
            // This interrupt happen when device detected STOP/START condition at illegal place.
            interrupt_status_ = murasaki::ki2csBussError;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_I2C_ERROR_ARLO) {
            MURASAKI_SYSLOG(this, kfaI2cMaster, kseWarning, "HAL_I2C_ERROR_ARLO");
            // This interrupt happen when device causes Arbitration lost against other master
            interrupt_status_ = murasaki::ki2csArbitrationLost;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_I2C_ERROR_OVR) {
            MURASAKI_SYSLOG(this, kfaI2cMaster, kseWarning, "HAL_I2C_ERROR_OVR");
            // This interrupt happen when device causes Overrun or Underrun. Slave only
            interrupt_status_ = murasaki::ki2csOverrun;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_I2C_ERROR_DMA) {
            MURASAKI_SYSLOG(this, kfaI2cMaster, kseWarning, "HAL_I2C_ERROR_DMA");
            // This interrupt happen when some error is detected in DMA.
            interrupt_status_ = murasaki::ki2csDMA;
            // abort the processing
            sync_->Release();
        }
        #ifdef         HAL_I2C_ERROR_SIZE
        else if (peripheral_->ErrorCode & HAL_I2C_ERROR_SIZE) {
            MURASAKI_SYSLOG(this, kfaI2cMaster, kseWarning, "HAL_I2C_ERROR_SIZE");
            // This interrupt happen when.
            interrupt_status_ = murasaki::ki2csSizeError;
            // abort the processing
            sync_->Release();
        }
        #endif
        else {
            MURASAKI_SYSLOG(this, kfaI2cMaster, kseEmergency, "Unknown error");
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

void* I2cMaster::GetPeripheralHandle() {
    I2C_SYSLOG("Enter");
    I2C_SYSLOG("Leave");

    return peripheral_;
}

} /* namespace murasaki */

#endif //HAL_I2C_MODULE_ENABLED
