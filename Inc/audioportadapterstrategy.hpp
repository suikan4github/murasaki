/**
 * @file audioportadapterstrategy.hpp
 *
 *  @date 2019/07/28
 *  @author Seiichi "Suikan" Horie
 *
 *  @brief Strategy of the Audio device adaptor.
 */

#ifndef AUDIOPORTADAPTERSTRATEGY_HPP_
#define AUDIOPORTADAPTERSTRATEGY_HPP_

#include "murasaki_defs.hpp"
#include "peripheralstrategy.hpp"

namespace murasaki {

/**
 * @brief Strategy of the audio device adaptor..
 * \ingroup MURASAKI_ABSTRACT_GROUP
 * @details
 * Template class of the audio device adaptor.
 */
class AudioPortAdapterStrategy : public murasaki::PeripheralStrategy {
 public:

    /**
     * @brief Kick start routine to start the TX DMA transfer.
     * @details This routine must be implemented by the derived class.
     * The task of this routine is to kick the first DMA transfer.
     * In this class, we assume DMA continuously transfer on the circular buffer
     * once after it starts.
     */
    virtual void StartTransferTx(
                                 uint8_t *tx_buffer,
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
                                 uint8_t *rx_buffer,
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
     * @brief Return the bit count to shift the 1.31 fixed point data to the DMA required format.
     * @return 0..32. The unit is [bit]
     * @details
     * This is needed because of the variation of the DMA format.
     *
     * Let's assume the 24bit data I2S format. Some peripheral places the data as
     * right aligned in 32bit DMA data ( as integer ), some peripheral places the data
     * as left aligned in 32bit DMA data ( as fixed point ).
     *
     * This kind of the mismatch will be aligned by audio frame work. This member function returns
     * how many bits have to be shifted from left aligned data to the DMA required format..
     *
     * If peripheral requires the left aligned format, this function should return 0.
     */
    virtual unsigned int GetSampleShiftSizeTx() = 0;

    /**
     * @brief Return the size of the one sample on memory for Tx channel
     * @return 2 or 4. The unit is [Byte]
     * @details
     * This function returns the size of the word which should be
     * allocated on the memory.
     */
    virtual unsigned int GetSampleWordSizeTx() = 0;

    /**
     * @brief Return how many channels are in the transfer.
     * @return 1 for Mono, 2 for stereo, 3... for multi-channel.
     */
    virtual unsigned int GetNumberOfChannelsRx() = 0;

    /**
     * @brief Return the bit count to shift the DMA format to the left aligned 1.31 format. .
     * @return 0..32. The unit is [bit]
     * @details
     * This is needed because of the variation of the DMA format.
     *
     * Let's assume the 24bit data I2S format. Some peripheral places the data as
     * right aligned in 32bit DMA data ( as integer ), some peripheral places the data
     * as left aligned in 32bit DMA data ( as fixed point ).
     *
     * This kind of the mismatch will be aligned by audio frame work. This member function returns
     * how many bits have to be shifted from the DMA format to the left aligned data.
     *
     * If peripheral DMA uses the left aligned format, this function should return 0.
     */
    virtual unsigned int GetSampleShiftSizeRx() = 0;

    /**
     * @brief Return the size of the one sample on memory for Rx channel
     * @return 2 or 4. The unit is [Byte]
     * @details
     * This function returns the size of the word which should be
     * allocated on the memory.
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
     * @brief Handling error report of device.
     * @param ptr Pointer for generic use. Usually, points a struct of a device control
     * @return true if ptr matches with device and handle the error. false if ptr doesn't match
     * @details
     * A member function to detect error.
     *
     * Note, we assume once this error call back is called, we can't recover.
     */
    virtual bool HandleError(void *ptr)= 0;

    /**
     * @details Check if peripheral handle matched with given handle.
     * @param peripheral_handle
     * @return true if match, false if not match.
     */
    virtual bool Match(void *peripheral_handle) = 0;

    /**
     * @brief pass the raw peripheral handler
     * @return pointer to the raw peripheral handler hidden in a class.
     */
    virtual void* GetPeripheralHandle()=0;

    /**
     * @brief Display half word swap is required. .
     * @return true : half word swap required. false :harlf word swap is not required.
     * @details
     * Display whether the half word (int16_t) swap is required or not.
     *
     * Certain architecture requires to swap the upper half word and lower half word inside a word (int32_t ).
     * In case this is required before copying to TX DMA buffer or after copying from RX DMA buffer,
     * return true. Otherwise, return false.
     *
     * The return value doesn't affect to the endian inside half word.
     * This display will be ignored if the audio sample size is half word (int16_t) or byte (int8_t).
     */
    virtual bool IsInt16SwapRequired() = 0;

};

} /* namespace murasaki */

#endif /* AUDIOPORTADAPTERSTRATEGY_HPP_ */
