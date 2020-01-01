/*
 * spislave.cpp
 *
 *  Created on: 2018/02/14
 *      Author: Seiichi "Suikan" Horie
 */

#include <spislave.hpp>
#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

// Macro for easy-to-read
#define SPIM_SYSLOG(fmt, ...)    MURASAKI_SYSLOG(this, kfaSpiSlave, kseDebug, fmt, ##__VA_ARGS__)

// Check if CubeIDE generated SPI Module
#ifdef HAL_SPI_MODULE_ENABLED

namespace murasaki {

SpiSlave::SpiSlave(SPI_HandleTypeDef * spi_handle)
        :
          peripheral_(spi_handle),
          sync_(new murasaki::Synchronizer),
          critical_section_(new murasaki::CriticalSection),
          interrupt_status_(kspisUnknown)
{
    // Setup internal variable with given uart structure.

    MURASAKI_ASSERT(nullptr != peripheral_)
    MURASAKI_ASSERT(nullptr != sync_)
    MURASAKI_ASSERT(nullptr != critical_section_)

}

SpiSlave::~SpiSlave()
{
    // Deleting resouces
    if ( nullptr != sync_)
        delete sync_;

    if ( nullptr != critical_section_)
        delete critical_section_;

}

SpiStatus SpiSlave::TransmitAndReceive(
                                       const uint8_t* tx_data,
                                       uint8_t* rx_data,
                                       unsigned int size,
                                       unsigned int * transfered_count,
                                       unsigned int timeout_ms)
{

    SPIM_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr!= tx_data);
    MURASAKI_ASSERT(nullptr!= rx_data);
    MURASAKI_ASSERT(65536 >= size);

    // exclusive operation
    critical_section_->Enter();
    {

        // This value will be updated by TransmitAndReceiveCompleteCallback
        interrupt_status_ = murasaki::kspisTimeOut;

        // Assert the chip select for slave
        SPIM_SYSLOG("Start SPI slave transferring");

        // Keep coherence between the L2 and cache before DMA
        // No Need to invalidate for TX
        murasaki::CleanDataCacheByAddress(
                                          const_cast<uint8_t *>(tx_data),
                                          size);
        // Need to invalidate for RX
        murasaki::CleanAndInvalidateDataCacheByAddress(rx_data, size);

        // Transmit over SPI
        HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_DMA(
                                                               peripheral_,
                                                               const_cast<uint8_t *>(tx_data),
                                                               rx_data,
                                                               size);
        MURASAKI_ASSERT(HAL_OK == status);

        // wait for the completion
        sync_->Wait(timeout_ms);   // return false if timeout
        SPIM_SYSLOG("Sync released.")

        // So far it returns always 0.
        if (transfered_count != nullptr)
            *transfered_count = 0;

        // check the status from interrupt
        switch (interrupt_status_)
        {
            case murasaki::kspisOK:
                SPIM_SYSLOG("Receive complete successfully")
                break;
            case murasaki::kspisModeCRC:
                MURASAKI_SYSLOG(this, kfaSpiSlave, kseWarning, "CRC error")
                break;
            case murasaki::kspisOverflow:
                MURASAKI_SYSLOG(this, kfaSpiSlave, kseWarning, "DMA overflow / underflow")
                break;
            case murasaki::kspisFrameError:
                MURASAKI_SYSLOG(this, kfaSpiSlave, kseWarning, "Frame error")
                break;
            case murasaki::kspisDMA:
                MURASAKI_SYSLOG(this, kfaSpiSlave, kseError, "DMA error")
                // Desable SPI to try to clear the current error status. Then, re-enable.
                HAL_SPI_DeInit(peripheral_);
                HAL_SPI_Init(peripheral_);
                break;
            case murasaki::kspisErrorFlag:
                MURASAKI_SYSLOG(this, kfaSpiSlave, kseError, "Unknown error flag,  Peripheral re-initialized")
                // Desable SPI to try to clear the current error status. Then, re-enable.
                HAL_SPI_DeInit(peripheral_);
                HAL_SPI_Init(peripheral_);
                break;
            case murasaki::ki2csTimeOut:
                MURASAKI_SYSLOG(this, kfaSpiSlave, kseWarning, "Receive timeout. DMA stopped")
                // Abort on-going and not terminated transfer.
                HAL_SPI_DMAStop(peripheral_);
                break;
            default:
                MURASAKI_SYSLOG(this, kfaI2cSlave, kseEmergency, "Error is not handled.  Peripheral re-initialized")
                // Desable SPI to try to clear the current error status. Then, re-enable.
                HAL_SPI_DeInit(peripheral_);
                HAL_SPI_Init(peripheral_);
                break;

        }

        SPIM_SYSLOG("End SPI slave transferring");
    }
    critical_section_->Leave();

    SPIM_SYSLOG("Leave");
    return interrupt_status_;

}

bool SpiSlave::TransmitAndReceiveCompleteCallback(void* ptr)
{
    SPIM_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != ptr)

    // if matches, release task
    if (peripheral_ == ptr) {
        SPIM_SYSLOG("Release sync");
        // report normal completion
        interrupt_status_ = murasaki::kspisOK;
        // release waiting task
        sync_->Release();
        SPIM_SYSLOG("Return with match");
        // This interrupt is for this device.
        return true;
    }
    else {
        SPIM_SYSLOG("Return without match");
        // This interrupt is not for this device.
        return false;
    }
}

bool SpiSlave::HandleError(void* ptr)
{
    SPIM_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != ptr)

    if (this->Match(ptr)) {

        // Check error and halde it.
        if (peripheral_->ErrorCode & HAL_SPI_ERROR_CRC) {
            MURASAKI_SYSLOG(this, kfaSpiSlave, kseWarning, "HAL_SPI_ERROR_CRC");
            // This happens only in the CRC mode
            interrupt_status_ = murasaki::kspisModeCRC;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_SPI_ERROR_OVR) {
            MURASAKI_SYSLOG(this, kfaSpiSlave, kseWarning, "HAL_SPI_ERROR_OVR");
            // This interrupt happen when the DMA is too slow to handle the received data.
            interrupt_status_ = murasaki::kspisOverflow;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_SPI_ERROR_FRE) {
            MURASAKI_SYSLOG(this, kfaSpiSlave, kseWarning, "HAL_SPI_ERROR_FRE");
            // This interrupt is the frame error of the the TI frame mode
            interrupt_status_ = murasaki::kspisFrameError;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_SPI_ERROR_DMA) {
            MURASAKI_SYSLOG(this, kfaSpiSlave, kseWarning, "HAL_SPI_ERROR_DMA");
            // This interrupt emans something happen in DMA unit
            interrupt_status_ = murasaki::kspisDMA;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_SPI_ERROR_FLAG) {
            MURASAKI_SYSLOG(this, kfaSpiSlave, kseWarning, "HAL_SPI_ERROR_FLAG");
            // This interrupt emans something is recorded as error flag.
            interrupt_status_ = murasaki::kspisErrorFlag;
            // abort the processing
            sync_->Release();
        }
#ifdef HAL_SPI_ERROR_ABORT
        else if (peripheral_->ErrorCode & HAL_SPI_ERROR_ABORT) {
            MURASAKI_SYSLOG(this, kfaSpiSlave, kseWarning, "HAL_SPI_ERROR_ABORT");
            // This interrupt happen when program causes problem during abort process
            // No way to recover.
            interrupt_status_ = murasaki::kspisAbort;
            // abort the processing
            sync_->Release();
        }
#endif
        else
        {
            MURASAKI_SYSLOG(this, kfaSpiSlave, kseWarning, "Unknown error interrupt")
            // Unknown error. This program must be updated by the newest specificaiton.
            interrupt_status_ = murasaki::kspisUnknown;
            // abort the processing
            sync_->Release();
        }
        SPIM_SYSLOG("Return with match");
        return true;    // report the ptr matched
    }
    else {
        SPIM_SYSLOG("Return without match");
        return false;   // report the ptr doesn't match
    }
}

void* SpiSlave::GetPeripheralHandle() {
    SPIM_SYSLOG("Enter");
    SPIM_SYSLOG("Leave");

	return peripheral_;
}


} /* namespace murasaki */

#endif // HAL_SPI_MODULE_ENABLED
