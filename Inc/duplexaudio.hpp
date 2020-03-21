/**
 * @file duplexaudio.hpp
 *
 * @date 2019/03/02
 * @author Seiichi "Suikan" Horie
 * @brief root class of the stereo audio
 */

#ifndef DUPLEXAUDIO_HPP_
#define DUPLEXAUDIO_HPP_

#include <synchronizer.hpp>
#include "audioportadapterstrategy.hpp"

namespace murasaki {

/**
 * @ingroup MURASAKI_GROUP
 * @brief Stereo Audio is served by the descendants of this class.
 * @details
 *
 * This class provides an interface to the audio data flow. Also, the internal buffer allocation, multi-phase buffering, and synchronization are provided. The features are :
 * @li Support from mono to multi-ch audio
 * @li 32bit floating-point data buffer as an interface with the application.
 * @li Data range is [-1.0, 1.0) as an interface with the application.
 * @li Blocking and synchronous API
 * @li Internal DMA operation.
 *
 * Note: This class assumes the Fs and the data size on I2S of the TX and RX are the same, and both Tx and RX are fully synchronized.
 * Also, this class assumes that the data size on I2S is more significant than the 8bit.
 *
 * Internally, this class provides a multi-buffers DMA operation between the audio peripheral and caller algorithm.
 * The critical key API is the @ref TransmitAndReceive() member function.
 * This function provides several key operations
 * @li Multiple-buffer operation to allow a background DMA transfer during caller task is processing data.
 * @li Data conversion and scaling between caller's floating-point data and DMA's integer data.
 * @li Synchronization between TransmitAndReceive() and DMA by DmaCallback().
 *
 * Thus, the user doesn't need to care about the above things.
 *
 * Because of the complicated audio data structure, there are several terminologies that a programmer must know.
 * @li Word : An atomic data of audio sample. For example, stereo sample has two word.
 * Note that in murasaki::DuplexAudio, the size of a word is given from murasaki::AudioPortAdapterStrategy.
 * @li Channel: Input / Output port of audio. For example, the stereo audio has two channels named left and right. The 5.1 surround audio has 6 channels.
 * @li Phase: State of DMA. Usually, audio DMA is configured as double or triple buffered to avoid the gap of the sound.
 * The index of the DMA buffer is called a phase. For example, the double buffer DMA can be phase 0 or 1 and
 * incremented as modulo 2.
 *
 * The number of phases is specified to the constructor, by the programmer. This phase has to be aligned with hardware.
 *
 */
class DuplexAudio {
 public:
    DuplexAudio() = delete;
    /**
     * @brief Constructor
     * @param peripheral_adapter Pointer to the audio interface peripheral class
     * @param channel_length Specify how many data are in one channel buffer.
     *
     * Initialize the internal variables and allocate the buffer based on the given parameters.
     *
     * The channel_length parameter specifies the number of the data in one channel.
     * Where channel is the independent audio data stream.
     * For example, a stereo data has 2 channels named left and right.
     */
    DuplexAudio(
                murasaki::AudioPortAdapterStrategy *peripheral_adapter,
                unsigned int channel_length
                );
    /**
     * @brief Destructor.
     */
    virtual ~DuplexAudio();

    /**
     * @brief Stereo audio transmission/receiving.
     * @param tx_left Pointer to the left channel TX buffer
     * @param tx_right Pointer to the right channel TX buffer
     * @param rx_left Pointer to the left channel RX buffer
     * @param rx_right Pointer to the right channel RX buffer
     *
     * @details
     * Synchronous API.
     * Inside this member function,
     *  -# Wait for a complete of the RX data transfer by waiting for the DmaCallback().
     *  -# Given tx_channels buffers are scaled and copied to the DMA buffer.
     *  -# Scale the data in DMA buffer and copy to rx_channels buffers.
     *
     * And then returns.
     *
     * Following is the typical usage of this function.
     *
     * @code
     * #define CH_LEN 48
     *
     * float tx_left_ch_buf[CH_LEN];
     * float tx_right_ch_buf[CH_LEN];
     * float rx_left_ch_buf[CH_LEN];
     * float rx_right_ch_buf[CH_LEN];
     *
     *
     * while(1)
     * {
     *     // prepare TX data into tx_left_ch_buf and tx_right_ch_buf
     *     ...
     *     murasaki::platform.audio->TransmitAndReceive(
     *                                          tx_left_ch_buf,
     *                                          tx_right_ch_buf,
     *                                          rx_left_ch_buf,
     *                                          rx_right_ch_buf );
     *
     *     // process RX data in rx_left_ch_buf and rx_right_ch_buf
     *     ...
     * }
     * @endcode
     *
     *
     */
    void TransmitAndReceive(
                            float *tx_left,
                            float *tx_right,
                            float *rx_left,
                            float *rx_right
                            );

    /**
     * @brief Multi channel audio transmission/receiving.
     * @param tx_channels Array of pointers. The number of the array element have to be same with the number of channel. Each pointer points the TX channel buffers.
     * @param rx_channels Array of pointers. The number of the array element have to be same with the number of channel. Each pointer points the RX channel buffers.
     * @param tx_num_of_channels Any number which is smaller than or equal to num_of_channels given audio peripheral adapter.
     * @param rx_num_of_channels Any number which is smaller than or equal to num_of_channels given audio peripheral adapter.
     *
     *
     * Synchronous API.
     * Inside this member function,
     *  -# wait for the complete of the RX data transfer by waiting for the DmaCallback().
     *  -# Given tx_channels buffers are scaled and copied to the DMA buffer.
     *  -# Scale the data in DMA buffer and copy to rx_channels buffers.
     *
     * And then returns.
     *
     * This function is a base for the another public TransmitAndRecieve().
     *
     * @code
     * #define NUM_CH 8
     * #define CH_LEN 48
     *
     * float * tx_channels_array[NUM_CH];
     * float * rx_channels_array[NUM_CH];
     *
     * tx_channles_array[0] = new float[CH_LEN];
     * tx_channles_array[1] = new float[CH_LEN];
     * tx_channles_array[2] = new float[CH_LEN];
     * ...
     * tx_channles_array[NUM_CH-1] = new float[CH_LEN];
     *
     * rx_channles_array[0] = new float[CH_LEN];
     * rx_channles_array[1] = new float[CH_LEN];
     * rx_channles_array[2] = new float[CH_LEN];
     * ...
     * rx_channles_array[NUM_CH-1] = new float[CH_LEN];
     *
     * while(1)
     * {
     *     // prepare TX data into rx_channlels_array.
     *     ...
     *     murasaki::platform.audio->TransmitAndReceive(
     *                                          tx_channels_array,
     *                                          rx_channels_array,
     *                                          NUM_CH,
     *                                          NUM_CH );
     *
     *     // process RX data in rx_channels_array
     *     ...
     * }
     * @endcode     */

    void TransmitAndReceive(
                            float **tx_channels,
                            float **rx_channels,
                            unsigned int tx_num_of_channels,
                            unsigned int rx_num_of_channels
                            );

    /**
     * @brief Callback function on the RX DMA interrupt.
     * @param peripheral pointer to the peripheral device.
     * @param phase 0 or 1, ..., numPhase-1. The index of the buffer in the muli-buffer DMA.
     * @return True if the peripheral matches with own peripheral which was given by constructor. Otherwise false.
     * @details
     * For each time RX DMA finish the transfer, interrupt should raised. This callback is
     * designed to be called from that interrupt hander.
     *
     * The interrupt must have phase. For example, for the double buffer DMA, it should have phase 0 and 1.
     * For the triple buffer, it should have phase 0, 1, and 2. The maximum phase is defined by the num_dma_phases - 1,
     * where num_dma_phases are given through the constructor parameter.
     *
     * In some system, the interrupts have explicit phase information. For example, there are
     * half-way-interrupt and end-of-buffer interrupt. In such the system, interrupt should
     * give the phase parameter.
     *
     * In certain system, the interrupts don't have explicit phase information.
     * For example, only one interrupt happens on both half way and end of buffer. In this case,
     * @ref AudioPortAdapterStrategy::DetectPhase of the derived class must detect the phase. So, interrupt doesn't need to give
     * the meaningful phase through this member function..
     *
     * This function returns if peripheral parameter is match with the one passed by the constructor.
     *
     * This member function have to be call from the HAL call backs of the
     * SAI/I2S. In case of the SAI :
     *
     * @code
     * // Halfway
     * void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef * hsai) {
     *     if (murasaki::platform.audio->DmaCallback(hsai, 0))  // second parameter is 0 for the halfway
     *         return;
     * }
     *
     * // Complete
     * void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef * hsai) {
     *     if (murasaki::platform.audio->DmaCallback(hsai, 1))  // second parameter is 1 for the complete
     *         return;
     * }
     * @endcode
     *
     * And in case of I2S :
     *
     * @code
     * // Half way
     * void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
     *     if (murasaki::platform.audio->DmaCallback(hi2s, 0))  // second parameter is 0 for the halfway
     *         return;
     * }
     *
     * // Complete
     * void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
     *     if (murasaki::platform.audio->DmaCallback(hi2s, 1))  // second parameter is 1 for the complete
     *         return;
     * }
     *
     * @endcode
     */
    bool DmaCallback(void *peripheral, unsigned int phase);

    /**
     * @brief Call this function from the interrupt handler.
     * @param peripheral pointer to the peripheral device.
     * @return True if the peripheral matches with own peripheral which was given by constructor. Otherwise false.
     * @details
     * This function calls the @ref AudioPortAdapterStrategy::HandleError() which knows how to handle.
     * Usually, this error call back is unable to recover. So, assertion may be triggered.
     *
     * This member function have to be called from the error call back of SAI/I2S HAL
     *
     * @code
     * void HAL_SAI_ErrorCallback(SAI_HandleTypeDef * hsai) {
     *     if (murasaki::platform.audio->HandleError(hsai))
     *         return;
     * }
     * @endcode
     * @code
     * void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s) {
     *     if (murasaki::platform.audio->HandleError(hi2s))
     *         return;
     * }
     * @endcode
     */
    virtual bool HandleError(void *peripheral);

 private:

    murasaki::AudioPortAdapterStrategy *const peripheral_adapter_;
    /**
     * @brief Length of a audio channel by one DMA transfer. The unit is [audio word].
     */
    const unsigned int channel_len_;

    /**
     * @brief Size of DMA buffer by one interrupt period [Byte]
     */
    const unsigned int block_size_tx_;

    /**
     * @brief Size of DMA buffer by one interrupt period [Byte]
     */
    const unsigned int block_size_rx_;

    /**
     * @brief Size of entire DMA buffer [Byte]
     */
    const unsigned int buffer_size_tx_;

    /**
     * @brief Size of entire DMA buffer [Byte]
     */
    const unsigned int buffer_size_rx_;

    /**
     * @brief pointer to dma buffer [num_dma_phases * num_channels_ * channlel_len_* word_size_].
     */
    uint8_t *const tx_dma_buffer_;
    /**
     * @brief pointer to dma buffer [num_dma_phases  * num_channels_ * channlel_len_* word_size_]
     */
    uint8_t *const rx_dma_buffer_;

    /**
     * @brief
     * Phase number of the DMA buffer phase which is avairable to CPU.
     * @details
     * 0, 1 for double buffer.
     * 0, 1, 2 for triple buffer.
     */
    unsigned int current_dma_phase_;
    /**
     * @brief flat to kick start the transfer.
     */
    bool first_transfer_;
    /**
     * @brief Synchronization between DMA interrupt and audio transfer.
     */
    murasaki::Synchronizer *const sync_;

    /**
     * @brief Scratch pad for the Stereo usage.
     */
    float *tx_stereo_[2];
    /**
     * @brief Scratch pad for the Stereo usage.
     */
    float *rx_stereo_[2];

};

} /* namespace murasaki */

#endif /* DuplexAudio_HPP_ */
