/*
 * spimaster.cpp
 *
 *  Created on: 2018/02/14
 *      Author: Seiichi "Suikan" Horie
 */

#include <spimaster.hpp>
#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

// Macro for easy-to-read
#define SPIM_SYSLOG(fmt, ...)    MURASAKI_SYSLOG(kfaSpiMaster, kseDebug, fmt, ##__VA_ARGS__)

// Check if CubeIDE generated SPI Module
#ifdef HAL_SPI_MODULE_ENABLED

namespace murasaki {

SpiMaster::SpiMaster(SPI_HandleTypeDef * spi_handle):
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

SpiMaster::~SpiMaster()
{
    // Deleting resouces
    if ( nullptr != sync_)
        delete sync_;

    if ( nullptr != critical_section_)
        delete critical_section_;

}

SpiStatus SpiMaster::TransmitAndReceive(murasaki::SpiSlaveAdapterStrategy* spi_spec,
                         const uint8_t* tx_data,
                         uint8_t* rx_data,
                         unsigned int size,
                         unsigned int timeout_ms)
{

    SPIM_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr!= tx_data);
    MURASAKI_ASSERT(nullptr!= rx_data);
    MURASAKI_ASSERT(65536 >= size);
    MURASAKI_ASSERT(nullptr != spi_spec);

    // exclusive operation
    critical_section_->Enter();
    {
        SPIM_SYSLOG("Start re-configuring SPI master");

        // This value will be updated by TransmitAndReceiveCompleteCallback
        interrupt_status_ = murasaki::kspisTimeOut;

        // Change the clock porarity & phase
        // todo is following right?
        MURASAKI_ASSERT(HAL_SPI_DeInit(peripheral_) == HAL_OK);

        // Set clock POL of the SPI format.
        peripheral_->Init.CLKPolarity =
                (spi_spec->GetCpol() == kspoRiseThenFall) ? SPI_POLARITY_LOW : SPI_POLARITY_HIGH;
        // Set clock PHA of the SPI format.
        peripheral_->Init.CLKPhase = (spi_spec->GetCpha() == ksphLatchThenShift) ? SPI_PHASE_1EDGE : SPI_PHASE_2EDGE;

        MURASAKI_ASSERT(HAL_SPI_Init(peripheral_) == HAL_OK);

        SPIM_SYSLOG("End re-configuring SPI master");

        // Assert the chip select for slave
        spi_spec->AssertCs();
        {
            SPIM_SYSLOG("Start SPI master transferring");

            // Keep coherence between the L2 and cache before DMA
            // No need to invalidate for TX
            murasaki::CleanDataCacheByAddress(
                        const_cast<uint8_t *>(tx_data),
                        size);
            // Need to invalidate to RX
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

            // check the status from interrupt
            switch (interrupt_status_)
            {
                case murasaki::kspisOK:
                    SPIM_SYSLOG("Receive complete successfully")
                    break;
                case murasaki::kspisModeCRC:
                    MURASAKI_SYSLOG(kfaSpiMaster, kseWarning, "CRC error")
                    break;
                case murasaki::kspisOverflow:
                    MURASAKI_SYSLOG(kfaSpiMaster, kseWarning, "DMA overflow / underflow")
                    break;
                case murasaki::kspisFrameError:
                    MURASAKI_SYSLOG(kfaSpiMaster, kseWarning, "Frame error")
                    break;
                case murasaki::kspisDMA:
                    MURASAKI_SYSLOG(kfaSpiMaster, kseError, "DMA error")
                    // Desable SPI to try to clear the current error status. Then, re-enable.
                    HAL_SPI_DeInit(peripheral_);
                    HAL_SPI_Init(peripheral_);
                    break;
                case murasaki::kspisErrorFlag:
                    MURASAKI_SYSLOG(kfaSpiMaster, kseError, "Unknown error flag,  Peripheral re-initialized")
                    // Desable SPI to try to clear the current error status. Then, re-enable.
                    HAL_SPI_DeInit(peripheral_);
                    HAL_SPI_Init(peripheral_);
                    break;
                case murasaki::ki2csTimeOut:
                    MURASAKI_SYSLOG(kfaSpiMaster, kseWarning, "Receive timeout. DMA stopped")
                    // Abort on-going and not terminated transfer.
                    HAL_SPI_DMAStop(peripheral_);
                    break;
                default:
                    MURASAKI_SYSLOG(kfaI2cMaster, kseEmergency, "Error is not handled.  Peripheral re-initialized")
                    // Desable SPI to try to clear the current error status. Then, re-enable.
                    HAL_SPI_DeInit(peripheral_);
                    HAL_SPI_Init(peripheral_);
                    break;

            }

            SPIM_SYSLOG("End SPI master transferring");
        }
        // Dessert the chip select for slave
        spi_spec->DeassertCs();
    }
    critical_section_->Leave();

    SPIM_SYSLOG("Leave");
    return interrupt_status_;

}

bool SpiMaster::TransmitAndReceiveCompleteCallback(void* ptr)
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

bool SpiMaster::HandleError(void* ptr)
{
    SPIM_SYSLOG("Enter");

    MURASAKI_ASSERT(nullptr != ptr)

    if (this->Match(ptr)) {

        // Check error and halde it.
        if (peripheral_->ErrorCode & HAL_SPI_ERROR_CRC) {
            MURASAKI_SYSLOG(kfaSpiMaster, kseWarning, "HAL_SPI_ERROR_CRC");
            // This happens only in the CRC mode
            interrupt_status_ = murasaki::kspisModeCRC;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_SPI_ERROR_OVR) {
            MURASAKI_SYSLOG(kfaSpiMaster, kseWarning, "HAL_SPI_ERROR_OVR");
            // This interrupt happen when the DMA is too slow to handle the received data.
            interrupt_status_ = murasaki::kspisOverflow;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_SPI_ERROR_FRE) {
            MURASAKI_SYSLOG(kfaSpiMaster, kseWarning, "HAL_SPI_ERROR_FRE");
            // This interrupt is the frame error of the the TI frame mode
            interrupt_status_ = murasaki::kspisFrameError;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_SPI_ERROR_DMA) {
            MURASAKI_SYSLOG(kfaSpiMaster, kseWarning, "HAL_SPI_ERROR_DMA");
            // This interrupt emans something happen in DMA unit
            interrupt_status_ = murasaki::kspisDMA;
            // abort the processing
            sync_->Release();
        }
        else if (peripheral_->ErrorCode & HAL_SPI_ERROR_FLAG) {
            MURASAKI_SYSLOG(kfaSpiMaster, kseWarning, "HAL_SPI_ERROR_FLAG");
            // This interrupt emans something is recorded as error flag.
            interrupt_status_ = murasaki::kspisErrorFlag;
            // abort the processing
            sync_->Release();
        }
#ifdef HAL_SPI_ERROR_ABORT
        else if (peripheral_->ErrorCode & HAL_SPI_ERROR_ABORT) {
            MURASAKI_SYSLOG(kfaSpiMaster, kseWarning, "HAL_SPI_ERROR_ABORT");
            // This interrupt happen when program causes problem during abort process
            // No way to recover.
            interrupt_status_ = murasaki::kspisAbort;
            // abort the processing
            sync_->Release();
        }
#endif
        else
        {
            MURASAKI_SYSLOG(kfaSpiMaster, kseWarning, "Unknown error interrupt")
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

void* SpiMaster::GetPeripheralHandle() {
    SPIM_SYSLOG("Enter");
    SPIM_SYSLOG("Leave");

	return peripheral_;
}


} /* namespace murasaki */

#endif // HAL_SPI_MODULE_ENABLED
