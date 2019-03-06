/**
 * @file audiostrategy.hpp
 *
 * @date 2019/03/02
 * @author Seiichi "Suikan" Horie
 * @brief root class of the stereo audio
 */

#ifndef AUDIOSTRATEGY_HPP_
#define AUDIOSTRATEGY_HPP_

#include <peripheralstrategy.hpp>
#include <synchronizer.hpp>

namespace murasaki {

/**
 * @brief Stereo Audio is served by the descendants of this class.
 * @details
 *
 * This class provides an interface to the audio peripheral. Also the internal buffer allocation, multi-phase buffering, and synchronization are provided. The features are :
 * \li Stereo to multi-ch audio
 * \li 32bit floating point buffer as interface with application.
 * @li data range is [-1.0, 1.0) as interface with application.
 * @li blocking and synchronous API
 * @li Internal DMA operation.
 *
 * Internally, this class provide a multi-buffer DMA operation between the audio peripheral and caller algorithm.
 * The key API is the TransmitAndReceive member function.
 * This function provide the several key operations
 * @li Multiple-buffer operation to allow a background DMA transfer during caller is processing data.
 * @li Data conversion and scaling between caller's floating point data and DMA's integer data.
 * @li Synchronization between TransmitAndReceive() and ReceiveCallback()
 * @li Exclusive access to peripheral
 *
 * Thus, user doesn't need these things.
 *
 * The multi-buffer DMA transfer is so called double-buffer or ripple buffer transfer.
 * In Murasaki, "double", "triple" are named as phase.
 * Programmer can specify the phase freely through the constructor.
 *
 */
class AudioStrategy : public PeripheralStrategy {
 public:
    /**
     * @brief Constructor
     * @param peripheral The pointer to the peripheral management variable.
     * @param channel_length Specify how many data are in one channel buffer.
     * @param num_phases 2 for double-buffers,  3 for triple-bufferers.
     * @param num_channnels 2 for stereo data. 6 for 5.1ch data. Must be aligned with CODEC configuration.
     *
     * Initialize the internal variables and allocate the buffer based on the given parameters.
     *
     * The channel_length peripheral specify the number of the data in one channel.
     * Where channel is the independent audio data stream.
     * For example, a stereo data has 2 channel named left and right.
     */
    AudioStrategy(
                        void * peripheral,
                        unsigned int channel_length,
                        unsigned int num_phases,
                        unsigned int num_channnels);
    /**
     * @brief Destructor.
     */
    virtual ~AudioStrategy();

    /**
     * @brief Multi channel audio transmission/receiving.
     * @param tx_channels Array of pointers. The number of the array element have to be same with the number of channel. Each pointer points the TX channel buffers.
     * @param rx_channels Array of pointers. The number of the array element have to be same with the number of channel. Each pointer points the RX channel buffers.
     *
     * Blocking and synchronous API. Given tx_channels buffers are scaled and copied to the DMA buffer.
     * Inside this member function, wait for the complete of the RX data transfer by waiting for the ReceiveCallback().
     * And then, scale the data in DMA buffer and copy to rx_channels buffers.
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
     *     murasaki::platform.audio->TransmitAndReceive(
     *                                          tx_channels_array,
     *                                          rx_channels_array );
     *
     *     // process RX data in rx_channels_array
     *     ...
     *     // prepare TX data into rx_channlels_array.
     *     ...
     * }
     * @endcode
     *
     */

    void TransmitAndReceive(
                            float ** tx_channels,
                            float ** rx_channels
    );

    /**
     * @brief Stereo audio transmission/receiving.
     * @param tx_left Pointer to the left channel TX buffer
     * @param tx_right Pointer to the right channel TX buffer
     * @param rx_left Pointer to the left channel RX buffer
     * @param rx_right Pointer to the right channel RX buffer
     *
     * @details
     * Blocking and synchronous API. Given tx_channels buffers are scaled and copied to the DMA buffer.
     * Inside this member function, wait for the complete of the RX data transfer by waiting for the ReceiveCallback().
     * And then, scale the data in DMA buffer and copy to rx_channels buffers.
     *
     * @code
     * #define CH_LEN 48
     *
     * float tx_left_ch_buf[NUM_CH];
     * float tx_right_ch_buf[NUM_CH];
     * float rx_left_ch_buf[NUM_CH];
     * float rx_right_ch_buf[NUM_CH];
     *
     *
     * while(1)
     * {
     *     murasaki::platform.audio->TransmitAndReceive(
     *                                          tx_left_ch_buf,
     *                                          tx_right_ch_buf,
     *                                          rx_left_ch_buf,
     *                                          rx_right_ch_buf );
     *
     *     // process RX data in rx_left_ch_buf and rx_right_ch_buf
     *     ...
     *     // prepare TX data into tx_left_ch_buf and tx_right_ch_buf
     *     ...
     * }
     * @endcode
     *
     */
    void TransmitAndReceive(
                            float * tx_left,
                            float * tx_right,
                            float * rx_left,
                            float * rx_right
                            );

    bool ReceiveCallback(void * peripheral, unsigned int phase);
    virtual bool ErrorCallback(void * peripheral) = 0;

 protected:
    virtual unsigned int DetectPhase(unsigned int phase);
    virtual void StartTransfer() = 0;

    const unsigned int channel_len_;
    const unsigned int num_phases_;
    const unsigned int num_channels_;
    const murasaki::Synchronizer * sync;
    bool runnning = false;
    const int32_t ** tx_buffer;  // pointers to tx_buffer[num_phases_][channlel_len_][num_channels_]. each pointer points each phase.
    const int32_t ** rx_buffer;  // rx_buffer[num_phases_][channlel_len_][num_channels_]
};

} /* namespace murasaki */

#endif /* AUDIOSTRATEGY_HPP_ */
