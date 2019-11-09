/*
 * duplexaudio.cpp
 *
 *  Created on: 2019/03/02
 *      Author: Seiichi "Suikan" Horie
 */

#include "duplexaudio.hpp"
#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

// Macro for easy-to-read
#define AUDIO_SYSLOG(fmt, ...)    MURASAKI_SYSLOG(kfaAudio, kseDebug, fmt, ##__VA_ARGS__)

namespace murasaki {

DuplexAudio::DuplexAudio(
                         murasaki::AudioAdapterStrategy * peripheral_adapter,
                         unsigned int channel_length
                         )
        :
          peripheral_adapter_(peripheral_adapter),
          channel_len_(channel_length),
          // Calcurate a DMA buffer size per interrupt [Byte]
          block_size_tx_(channel_len_ * peripheral_adapter_->GetNumberOfChannelsTx() * peripheral_adapter_->GetSampleWordSizeTx()),
          block_size_rx_(channel_len_ * peripheral_adapter_->GetNumberOfChannelsRx() * peripheral_adapter_->GetSampleWordSizeRx()),
          // Calcurate a entire DMA buffer size. 
          buffer_size_tx_(peripheral_adapter_->GetNumberOfDMAPhase() * block_size_tx_),
          buffer_size_rx_(peripheral_adapter_->GetNumberOfDMAPhase() * block_size_rx_),
          // Allocate DMA buffer
          tx_dma_buffer_(new uint8_t[buffer_size_tx_]),
          rx_dma_buffer_(new uint8_t[buffer_size_rx_]),
          // Initialize for the first execusion
          current_dma_phase_(0),
          // Set it true to trigger the first DMA transfer. 
          first_transfer_(true),
          // Create an sync object between interrupt and TransmitAndReceive member function. 
          sync_(new murasaki::Synchronizer())
{

    AUDIO_SYSLOG("Enter.  channel_length : %d ",
                 channel_length);

    // Check the values
    MURASAKI_ASSERT(peripheral_adapter_ != nullptr)
    MURASAKI_ASSERT(tx_dma_buffer_ != nullptr)
    MURASAKI_ASSERT(rx_dma_buffer_ != nullptr)
    MURASAKI_ASSERT(sync_ != nullptr)

    // only dual or triple buffer is acceptable.
    MURASAKI_ASSERT(2 == peripheral_adapter_->GetNumberOfDMAPhase() || 3 == peripheral_adapter_->GetNumberOfDMAPhase())

    // Initialize the DNA buffer.
    for (unsigned int i = 0; i < buffer_size_tx_; i++)
        tx_dma_buffer_[i] = 0;
    for (unsigned int i = 0; i < buffer_size_rx_; i++)
        rx_dma_buffer_[i] = 0;

    AUDIO_SYSLOG("Return")
}

DuplexAudio::~DuplexAudio() {
    AUDIO_SYSLOG("Enter.")

    // Deallocate the DMA buffer and sync object.
    delete[] tx_dma_buffer_;
    delete[] rx_dma_buffer_;
    delete sync_;

    AUDIO_SYSLOG("Return.")
}

// Stereo dedicated transfer.
void DuplexAudio::TransmitAndReceive(
                                     float* tx_left,
                                     float* tx_right,
                                     float* rx_left,
                                     float* rx_right) {
    AUDIO_SYSLOG("Enter. tx_left : %08p, tx_right : %08p, rx_left : %08p, rx_right : %08p",
                 tx_left,
                 tx_right,
                 rx_left,
                 rx_right);

// Create a TX parameter array
    tx_stereo_[0] = tx_left;
    tx_stereo_[1] = tx_right;
// Create a RX parameter array
    rx_stereo_[0] = rx_left;
    rx_stereo_[1] = rx_right;

    // Delegate the processing.
    TransmitAndReceive(
                       tx_stereo_,
                       rx_stereo_,
                       2,
                       2);
    AUDIO_SYSLOG("Return");
}

void DuplexAudio::TransmitAndReceive(
                                     float** tx_channels,
                                     float** rx_channels,
                                     unsigned int tx_num_of_channels,
                                     unsigned int rx_num_of_channels) {

    AUDIO_SYSLOG("Enter, tx_num_of_channels : %d, rx_num_of_channels : %d",
                 tx_num_of_channels,
                 rx_num_of_channels);

    MURASAKI_ASSERT(peripheral_adapter_->GetNumberOfChannelsTx() == tx_num_of_channels)
    MURASAKI_ASSERT(peripheral_adapter_->GetNumberOfChannelsRx() == rx_num_of_channels)

    if (first_transfer_) { /* Is first time transfer? Then, trigger the DMA */
        AUDIO_SYSLOG("Starting Transfer");

        // Actual DMA transfer is done by the peripheral_adapter_
        peripheral_adapter_->StartTransferTx(tx_dma_buffer_, channel_len_);
        peripheral_adapter_->StartTransferRx(rx_dma_buffer_, channel_len_);

        // Mark it to avoid the second kick. 
        first_transfer_ = false;
    }

    // Waiting for the completion of DMA transfer.
    // The sync_ is released in the DmaCallback()
    AUDIO_SYSLOG("Sync waiting");
    sync_->Wait();
    AUDIO_SYSLOG("Sync released");

    // Check whether DMA phase is OK.
    // current_dma_phase_ is updated in the DmaCallback()
    MURASAKI_ASSERT(current_dma_phase_ < peripheral_adapter_->GetNumberOfDMAPhase())

    // Processing is depend on the word size. So, get the Word size.
    // Note that we check only the TX word size. We assume the word TX and RX are identical, in the duplex audio.
    switch (peripheral_adapter_->GetSampleWordSizeRx()) {
        case 1: {
            AUDIO_SYSLOG("Case:Word size is 1");

            // Obtain the start address of the DMA buffer to transfer
            int8_t * const tx_current_dma_data = reinterpret_cast<int8_t *>(&tx_dma_buffer_[current_dma_phase_ * block_size_tx_]);
            int8_t * const rx_current_dma_data = reinterpret_cast<int8_t *>(&rx_dma_buffer_[current_dma_phase_ * block_size_rx_]);

            // Scale factor to convert between integer type on the DMA and normalized floating point data
            // Too keep the absolute maximum number as 1.0, using the _MIN value. 
            //  | INT8_MAX | +1 = | INT8_MIN |
            const float scale = -1.0 * INT8_MIN;

            AUDIO_SYSLOG("block_size_tx_ : %d", block_size_tx_);
            AUDIO_SYSLOG("block_size_rx_ : %d", block_size_rx_);

            AUDIO_SYSLOG("TX DMA BUFFER is %08p", tx_current_dma_data)
            AUDIO_SYSLOG("RX DMA BUFFER is %08p", rx_current_dma_data)

            // Data conversion and transpose the buffer
            // By scaling factor, [-1.0, 1.0] is scaled to [-INT32_MAX, INT32_MAX]
            // DMA  data order is : word0 of ch0, word 0 of ch1,... word 0 of chN-1.
            // Invalidate the DMA RX data buffer on cache. Then, ready to read.
            murasaki::CleanAndInvalidateDataCacheByAddress(rx_current_dma_data, block_size_rx_);

            // copy to TX DMA buffer.
            // The tx_current dma_data is the raw buffer to the audio device.
            // In this buffer, the data order is ch0-word0, ch0-word1, ch1-word0, so on.
            // The tx=channels are given as parameter from caller.
            // This is array of pointers. Each pointers in array point the word buffers.
            for (unsigned int ch_idx = 0; ch_idx < tx_num_of_channels; ch_idx++)
                for (unsigned int wo_idx = 0; wo_idx < channel_len_; wo_idx++)
                    tx_current_dma_data[wo_idx * tx_num_of_channels + ch_idx] = tx_channels[ch_idx][wo_idx] * scale;

            // copy from RX DMA buffer.
            // The data order in the rx_channels and rx_current_dma_data follows tx.
            for (unsigned int ch_idx = 0; ch_idx < rx_num_of_channels; ch_idx++)
                for (unsigned int wo_idx = 0; wo_idx < channel_len_; wo_idx++)
                    rx_channels[ch_idx][wo_idx] = rx_current_dma_data[wo_idx * rx_num_of_channels + ch_idx] / scale;

            // Flush the DMA TX data buffer on cache to main memory.
            murasaki::CleanDataCacheByAddress(tx_current_dma_data, block_size_tx_);

            break;
        }
        case 2: {
            AUDIO_SYSLOG("Case:Word size is 2");

            // Obtain the start address of the DMA buffer to transfer
            int16_t * const tx_current_dma_data = reinterpret_cast<int16_t*>(&tx_dma_buffer_[current_dma_phase_ * block_size_tx_]);
            int16_t * const rx_current_dma_data = reinterpret_cast<int16_t*>(&rx_dma_buffer_[current_dma_phase_ * block_size_rx_]);
            // Scale factor to convert between integer type on the DMA and normalized floating point data
            // Too keep the absolute maximum number as 1.0, using the _MIN value. 
            //  | INT16_MAX | +1 = | INT16_MIN |
            const float scale = -1.0 * INT16_MIN;

            AUDIO_SYSLOG("block_size_tx_ : %d", block_size_tx_);
            AUDIO_SYSLOG("block_size_rx_ : %d", block_size_rx_);

            AUDIO_SYSLOG("TX DMA BUFFER is %08p", tx_current_dma_data)
            AUDIO_SYSLOG("RX DMA BUFFER is %08p", rx_current_dma_data)

            // Data conversion and transpose the buffer
            // By scaling factor, [-1.0, 1.0] is scaled to [-INT32_MAX, INT32_MAX]
            // DMA  data order is : word0 of ch0, word 0 of ch1,... word 0 of chN-1.
            // Invalidate the DMA RX data buffer on cache. Then, ready to read.
            murasaki::CleanAndInvalidateDataCacheByAddress(rx_current_dma_data, block_size_rx_);

            // copy to TX DMA buffer.
            // The tx_current dma_data is the raw buffer to the audio device.
            // In this buffer, the data order is ch0-word0, ch0-word1, ch1-word0, so on.
            // The tx=channels are given as parameter from caller.
            // This is array of pointers. Each pointers in array point the word buffers.
            for (unsigned int ch_idx = 0; ch_idx < tx_num_of_channels; ch_idx++)
                for (unsigned int wo_idx = 0; wo_idx < channel_len_; wo_idx++)
                    tx_current_dma_data[wo_idx * tx_num_of_channels + ch_idx] = tx_channels[ch_idx][wo_idx] * scale;

            // copy from RX DMA buffer.
            // The data order in the rx_channels and rx_current_dma_data follows tx.
            for (unsigned int ch_idx = 0; ch_idx < rx_num_of_channels; ch_idx++)
                for (unsigned int wo_idx = 0; wo_idx < channel_len_; wo_idx++)
                    rx_channels[ch_idx][wo_idx] = rx_current_dma_data[wo_idx * rx_num_of_channels + ch_idx] / scale;

            // Flush the DMA TX data buffer on cache to main memory.
            murasaki::CleanDataCacheByAddress(tx_current_dma_data, block_size_tx_);

            break;
        }
        case 4: {
            AUDIO_SYSLOG("Case:Word size is 4");

            // Obtain the start address of the DMA buffer to transfer
            int32_t * const tx_current_dma_data = reinterpret_cast<int32_t*>(&tx_dma_buffer_[current_dma_phase_ * block_size_tx_]);
            int32_t * const rx_current_dma_data = reinterpret_cast<int32_t*>(&rx_dma_buffer_[current_dma_phase_ * block_size_rx_]);
            // Scale factor to convert between integer type on the DMA and normalized floating point data
            // Too keep the absolute maximum number as 1.0, using the _MIN value. 
            //  | INT32_MAX | +1 = | INT32_MIN |
            const float scale = -1.0 * INT32_MIN;

            AUDIO_SYSLOG("block_size_tx_ : %d", block_size_tx_);
            AUDIO_SYSLOG("block_size_rx_ : %d", block_size_rx_);

            AUDIO_SYSLOG("TX DMA BUFFER is %08p", tx_current_dma_data)
            AUDIO_SYSLOG("RX DMA BUFFER is %08p", rx_current_dma_data)

            // Data conversion and transpose the buffer
            // By scaling factor, [-1.0, 1.0] is scaled to [-INT32_MAX, INT32_MAX]
            // DMA  data order is : word0 of ch0, word 0 of ch1,... word 0 of chN-1.
            // Invalidate the DMA RX data buffer on cache. Then, ready to read.
            murasaki::CleanAndInvalidateDataCacheByAddress(rx_current_dma_data, block_size_rx_);

            // copy to TX DMA buffer.
            // The tx_current dma_data is the raw buffer to the audio device.
            // In this buffer, the data order is ch0-word0, ch0-word1, ch1-word0, so on.
            // The tx=channels are given as parameter from caller.
            // This is array of pointers. Each pointers in array point the word buffers.
            for (unsigned int ch_idx = 0; ch_idx < tx_num_of_channels; ch_idx++)
                for (unsigned int wo_idx = 0; wo_idx < channel_len_; wo_idx++)
                    tx_current_dma_data[wo_idx * tx_num_of_channels + ch_idx] = tx_channels[ch_idx][wo_idx] * scale;

            // copy from RX DMA buffer.
            // The data order in the rx_channels and rx_current_dma_data follows tx.
            for (unsigned int ch_idx = 0; ch_idx < rx_num_of_channels; ch_idx++)
                for (unsigned int wo_idx = 0; wo_idx < channel_len_; wo_idx++)
                    rx_channels[ch_idx][wo_idx] = rx_current_dma_data[wo_idx * rx_num_of_channels + ch_idx] / scale;

            // Flush the DMA TX data buffer on cache to main memory.
            murasaki::CleanDataCacheByAddress(tx_current_dma_data, block_size_tx_);

            break;
        }
        default:
            MURASAKI_SYSLOG(kfaAudio, kseError, "Unknown word size")
            MURASAKI_ASSERT(false)
    }

    AUDIO_SYSLOG("Return");
}

bool DuplexAudio::DmaCallback(
                              void* peripheral,
                              unsigned int phase) {
    AUDIO_SYSLOG("Enter with peripheral : %p, phase : %d", peripheral, phase);
    if (peripheral_adapter_->GetPeripheralHandle() == peripheral) {
        AUDIO_SYSLOG("peripheral matched");

        // Up to date the DMA phase.
        // Default implementation of the DetectPhase is, just passing through the parameter.
        // In this case, the interrupt handler have to pass the right phase.
        // In case the interrupt handler don't know the phase, The DetectPhase() ignores
        // the parameter and check hardware by itself

        // Check the validity of the given DMA phase.
        MURASAKI_ASSERT(0 <= phase && phase < peripheral_adapter_->GetNumberOfDMAPhase())

        current_dma_phase_ = peripheral_adapter_->DetectPhase(phase);

        // Check the validity of the obtained DMA phase.
        MURASAKI_ASSERT(0 <= current_dma_phase_ && current_dma_phase_ < peripheral_adapter_->GetNumberOfDMAPhase())

        // Notice the waiting task the buffer is ready.
        AUDIO_SYSLOG("Releasing Sync");
        sync_->Release();

        AUDIO_SYSLOG("Return with match");

        return true;
    }
    else {
        AUDIO_SYSLOG("Return without match");
        return false;
    }

}

bool DuplexAudio::HandleError(void* peripheral)
                              {
    AUDIO_SYSLOG("Enter, peripheral : %p", peripheral);

    bool retval = peripheral_adapter_->Match(peripheral);

    AUDIO_SYSLOG(
                 "Return with %s",
                 (retval ? "true" : "false"));

    return retval;
}
} /* namespace murasaki */
