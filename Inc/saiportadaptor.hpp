/**
 * @file saiportadaptor.hpp
 *
 *  @date 2019/07/28
 *  @author takemasa
 */

#ifndef SAIPORTADAPTOR_HPP_
#define SAIPORTADAPTOR_HPP_

#include <audioportadapterstrategy.hpp>

namespace murasaki {

#ifdef   HAL_SAI_MODULE_ENABLED
/**
 * @brief Adapter as SAI audio port.
 * @details
 * Dedicated adapter for the @ref murasaki::DuplexAudio.
 * By passing this adapter, the DuplexAudio class can handle audio through the SAI port.
 *
 * Caution : The size of the data in SAI and the width of the data in DMA must be aligned.
 * This is responsibility of the programmer. The misaligned configuration gives broken audio.
 *
 * \ingroup MURASAKI_GROUP
 *
 */
class SaiPortAdaptor : public AudioPortAdapterStrategy {
 public:
    /**
     * @brief Constructor.
     * @param tx_peripheral SAI_HandleTypeDef type peripheral for TX. This is defined in main.c.
     * @param rx_peripheral SAI_HandleTypeDef type peripheral for RX. This is defined in main.c.
     * @details
     * Receives handle of the SAI block peripherals.
     *
     * SAI has two block internally.
     * This class assumes one is the TX and the other is RX.
     * In case of a programmer use SAI as simplex audio, the unused block must be passed as nullptr.
     */
    SaiPortAdaptor(
                   SAI_HandleTypeDef *tx_peripheral,
                   SAI_HandleTypeDef *rx_peripheral
                   );

    virtual ~SaiPortAdaptor();

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
                                 );

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
                                 );
    /**
     * @brief Return how many DMA phase is implemented
     * @return Always return 2 for STM32 SAI, becuase the cyclic DMA has halfway and complete interrupt.
     */
    virtual unsigned int GetNumberOfDMAPhase() {
        return 2;
    }
    ;
    /**
     * @brief Return how many channels are in the transfer.
     * @return 1 for Mono, 2 for stereo, 3... for multi-channel.
     */
    virtual unsigned int GetNumberOfChannelsTx();

    /**
     * @brief Return the size of the one sample.
     * @return 2 or 4. The unit is [Byte]
     */
    virtual unsigned int GetSampleWordSizeTx();
    /**
     * @brief Return how many channels are in the transfer.
     * @return 1 for Mono, 2 for stereo, 3... for multi-channel.
     */
    virtual unsigned int GetNumberOfChannelsRx();

    /**
     * @brief Return the size of the one sample.
     * @return 2 or 4. The unit is [Byte]
     */
    virtual unsigned int GetSampleWordSizeRx();
    /**
     * @brief Handling error report of device.
     * @param ptr Pointer for generic use. Usually, points a struct of a device control
     * @return true if ptr matches with device and handle the error. false if ptr doesn't match
     * A member function to detect error.
     *
     * The error handling is depend on the implementation.
     */
    virtual bool HandleError(void *ptr);
    /**
     * @details Check if peripheral handle matched with given handle.
     * @param peripheral_handle
     * @return true if match, false if not match.
     * @details
     * The SaiAudioAdapter type has two peripheral. TX and RX. This function checks RX paripheral
     * and return with this value. That means, if RX is not nullptr, TX is not checked.
     *
     * TX is checked only when, RX is nullptr.
     */
    virtual bool Match(void *peripheral_handle);

    /**
     * @brief pass the raw peripheral handler
     * @return pointer to the raw peripheral handler hidden in a class.
     */
    virtual void* GetPeripheralHandle();

 private:
    SAI_HandleTypeDef *const tx_peripheral_;
    SAI_HandleTypeDef *const rx_peripheral_;
};
#endif // HAL_SAI_MODULE_ENABLED

}
/* namespace murasaki */

#endif /* SAIPORTADAPTOR_HPP_ */
