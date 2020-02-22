/*
 * adc.cpp
 *
 *  Created on: Feb 22, 2020
 *      Author: Seiichi Horie
 */

#include "adc.hpp"

#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

// Macro for easy-to-read
#define ADC_SYSLOG(fmt, ...)    MURASAKI_SYSLOG( this,  kfaAdc, kseDebug, fmt, ##__VA_ARGS__)

namespace murasaki {

#ifdef HAL_ADC_MODULE_ENABLED

Adc::Adc(ADC_HandleTypeDef *peripheral)
        :
        AdcStrategy(),
        peripheral_(peripheral),
        sync_(new murasaki::Synchronizer),
        critical_section_(new murasaki::CriticalSection),
        status_(murasaki::kasOK),
        channel_array_len_(32),     // Most of the STM32 ADC has smaller channel than 32.
        channels_(new unsigned int[channel_array_len_]),
        clocks_(new unsigned int[channel_array_len_]),
        data_count_(0)

{
    ADC_SYSLOG("Enter %p", peripheral)

    MURASAKI_ASSERT(nullptr != peripheral_)
    MURASAKI_ASSERT(nullptr != sync_)
    MURASAKI_ASSERT(nullptr != critical_section_)

    ADC_SYSLOG("Exit")
}

Adc::~Adc()
{
    delete[] channels_;
    delete[] clocks_;
}

murasaki::AdcStatus murasaki::Adc::Convert(
                                           unsigned int channel,
                                           float *value,
                                           const unsigned int size)
                                           {
    unsigned int ret_val;
    HAL_StatusTypeDef api_status;

    ADC_SYSLOG("Enter %d, %p, %d", channel, value, size)

// Wrap by critical section to gurantee the exclussive access between different channels.
    critical_section_->Enter();
    {
        MURASAKI_ASSERT(nullptr != value)
        MURASAKI_ASSERT(size == 1)

        // Before conversion, configure the channel
        ADC_ChannelConfTypeDef ch_config = { 0 };
        ch_config.Channel = channel;
        ch_config.Rank = 1;     // this is not important in the single conversion.
        ch_config.SamplingTime = GetSampleClocks(channel);

        api_status = HAL_ADC_ConfigChannel(peripheral_, &ch_config);
        MURASAKI_ASSERT(api_status == HAL_OK)

        // Start conversion.
        api_status = HAL_ADC_Start_IT(peripheral_);
        MURASAKI_ASSERT(api_status == HAL_OK)

        // Wait for interrupt.
        status_ = murasaki::kasOK;

        ADC_SYSLOG("Sync waiting")
        sync_->Wait();
        ADC_SYSLOG("Sync released")

        switch (status_)
        {
            case kasOK:
                ret_val = HAL_ADC_GetValue(peripheral_);
                // normalize to [-1.0, 1.0)
                *value = (-1.0f * ret_val) / INT16_MIN;
                break;
            case kasOverrun:
                case kasDMA:
                case kasUnknown:
                case kasInternal:
                // There is nothing we can do.
                *value = 0;
                break;
            default:
                // This is coding error. All api_status have to be recognized.
                MURASAKI_SYSLOG(this, kfaAdc, kseError, "Unknown Status")
                break;
        }
    }
    critical_section_->Leave();

    ADC_SYSLOG("Exit with %d", status_)
    return status_;

}

bool murasaki::Adc::ConversionCompleteCallback(void *ptr)
                                               {
    ADC_SYSLOG("Enter %d,", ptr)

    if (peripheral_ == ptr) {
        sync_->Release();

        ADC_SYSLOG("Sync releasing")

        ADC_SYSLOG("Exit with true")
        return true;
    }
    else
    {
        ADC_SYSLOG("Exit with false")
        return false;
    }

}

bool murasaki::Adc::HandleError(void *ptr)
                                {
    unsigned int error_code;

    ADC_SYSLOG("Enter.")

    if (peripheral_ == ptr) {

        error_code = HAL_ADC_GetError(peripheral_);

        if (error_code & HAL_ADC_ERROR_INTERNAL)
        {
            MURASAKI_SYSLOG(this, kfaAdc, kseWarning, "HAL_ADC_ERROR_INTERNAL")
            status_ = kasInternal;
        }
        else if (error_code & HAL_ADC_ERROR_OVR)
        {
            MURASAKI_SYSLOG(this, kfaAdc, kseWarning, "HAL_ADC_ERROR_OVR")
            status_ = kasOverrun;
        }
        else if (error_code & HAL_ADC_ERROR_DMA)
        {
            MURASAKI_SYSLOG(this, kfaAdc, kseWarning, "HAL_ADC_ERROR_DMA")
            status_ = kasDMA;
        }
        else
        {
            MURASAKI_SYSLOG(this, kfaAdc, kseError, "Unknown error code")
            status_ = kasUnknown;
        }

        sync_->Release();
        ADC_SYSLOG("Sync releasing")

        ADC_SYSLOG("Exit with true")
        return true;
    }
    else
    {
        ADC_SYSLOG("Exit with false")
        return false;
    }
}

void* Adc::GetPeripheralHandle()
{
    void *ret_val;
    ADC_SYSLOG("Enter.")

    ret_val = reinterpret_cast<void*>(peripheral_);

    ADC_SYSLOG("Exit with %p.", ret_val)

    return ret_val;
}

void Adc::SetSampleClock(unsigned int channel, unsigned int clocks)
                         {
    ADC_SYSLOG("Enter %d, %d.", channel, clocks);

    InstallSampleClocks(channel, clocks);

    ADC_SYSLOG("Exit.")

}

/**
 * @brief Store the given channel / clocks pare to the arrays.
 * @param channel ADC channel.
 * @param clocks Sample Clocks
 *
 * First, search for the channels_ with given channel. If there is,
 * over write its clocks element. If there is not, append.
 */
void Adc::InstallSampleClocks(unsigned int channel, unsigned int clocks)
                              {
    ADC_SYSLOG("Enter %d, %d.", channel, clocks);

    for (unsigned int index = 0; index < data_count_; index++)
        if (channels_[index] == channel) {
            ADC_SYSLOG("Found in the history . Index : %d", index);

            // Overwrite the old data.
            clocks_[index] = clocks;

            ADC_SYSLOG("Overwrote and Exit.")
            return;
        }

    // Here, we cound not find the channel in the history.

    // Be sure, we have place to store.
    MURASAKI_ASSERT(data_count_ < channel_array_len_)

    if (data_count_ < channel_array_len_) {
        // Store the data
        channels_[data_count_] = channel;
        clocks_[data_count_] = clocks;

        // update the size of the history.
        data_count_++;
        ADC_SYSLOG("Stored")
    }
    else
    MURASAKI_SYSLOG(this, kfaAdc, kseError, "Channel history over flow")

    ADC_SYSLOG("Exit.")
}

/**
 * @brief Retrieve the sample clock data.
 * @param channel Spefify the channel to found
 * @return If the channel found in the history, return its clock. If not, return ADC_SAMPLETIME_3CYCLES
 */
unsigned int Adc::GetSampleClocks(unsigned int channel)
                                  {
    unsigned int ret_val;

    ADC_SYSLOG("Enter %d", channel);

    // Search for the history
    for (unsigned int index = 0; index < data_count_; index++)
        if (channels_[index] == channel) {
            ADC_SYSLOG("Found in the history . Index : %d", index);

            ret_val = clocks_[index];

            ADC_SYSLOG("Exit with %d.", ret_val)

            return ret_val;
        }

    // We can't find. Return the ADC_SAMPLETIME_3CYCLES as default value.

    ADC_SYSLOG("Exit with ADC_SAMPLETIME_3CYCLES.")
    return ADC_SAMPLETIME_3CYCLES;
}

#endif // HAL_ADC_MODULE_ENABLED
}

/* namespace murasaki */
