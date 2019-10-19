/**
 * @file audioadapterstrategy.hpp
 *
 *  @date 2019/07/28
 *  @author Seiichi "Suikan" Horie
 *
 *  @brief Strategy of the Audio device adaptor.
 */

#ifndef AUDIOADAPTERSTRATEGY_HPP_
#define AUDIOADAPTERSTRATEGY_HPP_

#include "murasaki_defs.hpp"
#include "peripheralstrategy.hpp"

namespace murasaki {

/**
 * @brief Strategy of the audio device adaptor..
 * \ingroup MURASAKI_ABSTRACT_GROUP
 * @details
 * Template class of the audio device adaptor.
 */
class AudioAdapterStrategy : PeripheralStrategy {
 public:
    /**
     * @brief Destructor to enforce the virtual type function.
     */
    virtual ~AudioAdapterStrategy() {
    }
    ;

    /**
     * @brief Call this function from the error interrupt handler.
     * @param peripheral pointer to the peripheral device.
     * @return True if the peripheral matches with own peripheral which was given by constructor. Otherwise false.
     *
     * Note, we assume once this error call back is called, we can't recover.
     */
    virtual bool ErrorCallback(void * peripheral)=0;

    /**
     * @brief Kick start routine to start the TX DMA transfer.
     * @details This routine must be implemented by the derived class.
     * The task of this routine is to kick the first DMA transfer.
     * In this class, we assume DMA continuously transfer on the circular buffer
     * once after it starts.
     */
    virtual void StartTransferTx(
                                 uint8_t * tx_buffer,
                                 unsigned int channel_len
                                 ) = 0;

    /**
     * @brief Kick start routine to start the RX DMA transfer.
     * @details This routine must be implemented by the derived class.
     * The task of this routine is to kick the first DMA transfer.
     * In this class, we assume DMA continuously transfer on the circular buffer
     * once after it starts.
     */
    virtual void StartTransferRx(
                                 uint8_t * rx_buffer,
                                 unsigned int channel_len
                                 ) = 0;

    /**
     * @brief Return how many DMA phase is implemented
     * @return 2 for Double buffer, 3 for Tripple buffer.
     */
    virtual unsigned int GetNumberOfDMAPhase() = 0;

    /**
     * @brief Return how many channels are in the transfer.
     * @return 1 for Mono, 2 for stereo, 3... for multi-channel.
     */
    virtual unsigned int GetNumberOfChannelsTx() = 0;

    /**
     * @brief Return the size of the one sample.
     * @return 2 or 4. The unit is [Byte]
     */
    virtual unsigned int GetSampleWordSizeTx() = 0;

    /**
     * @brief Return how many channels are in the transfer.
     * @return 1 for Mono, 2 for stereo, 3... for multi-channel.
     */
    virtual unsigned int GetNumberOfChannelsRx() = 0;

    /**
     * @brief Return the size of the one sample.
     * @return 2 or 4. The unit is [Byte]
     */
    virtual unsigned int GetSampleWordSizeRx() = 0;

    /**
     * @brief DMA phase detector.
     * @param phase RX DMA phase : 0, 1, ...
     * @return By default, returns phase parameter.
     * @details
     * If the DMA interrupt doesn't have the explicit phase information, need to override to detect it
     * inside this function.
     *
     * By default, this function assumes the DMA phase is given though the interrupt handler. So,
     * just pass the input parameter as return value.
     */
    virtual unsigned int DetectPhase(
                                     unsigned int phase) {
        return phase;
    }
    /**
     * @brief pass the raw peripheral handler
     * @return pointer to the raw peripheral handler hidden in a class.
     */
    virtual void * GetPeripheralHandle()=0;
};

} /* namespace murasaki */

#endif /* AUDIOADAPTERSTRATEGY_HPP_ */
