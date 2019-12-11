/*
 * saiaudioadaptor.hpp
 *
 *  Created on: 2019/07/28
 *      Author: takemasa
 */

#ifndef SAIAUDIOADAPTOR_HPP_
#define SAIAUDIOADAPTOR_HPP_

#include <audioadapterstrategy.hpp>

namespace murasaki {

#ifdef   HAL_SAI_MODULE_ENABLED
class SaiAudioAdaptor : public AudioAdapterStrategy {
 public:
    SaiAudioAdaptor(
                    SAI_HandleTypeDef * tx_peripheral,
                    SAI_HandleTypeDef * rx_peripheral
                    );

    virtual ~SaiAudioAdaptor();

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
                                 );

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
    virtual bool HandleError(void * ptr);
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
    virtual bool Match(void * peripheral_handle);

    /**
     * @brief pass the raw peripheral handler
     * @return pointer to the raw peripheral handler hidden in a class.
     */
    virtual void * GetPeripheralHandle();

 private:
    SAI_HandleTypeDef * const tx_peripheral_;
    SAI_HandleTypeDef * const rx_peripheral_;
};
#endif // HAL_SAI_MODULE_ENABLED

}
/* namespace murasaki */

#endif /* SAIAUDIOADAPTOR_HPP_ */
