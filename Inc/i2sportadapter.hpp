/**
 * @file i2sportadapter.hpp
 *
 *  @date 2020/01/15
 *  @author takemasa
 */

#ifndef I2SPORTADAPTER_HPP_
#define I2SPORTADAPTER_HPP_

#include <audioportadapterstrategy.hpp>

namespace murasaki {

#ifdef   HAL_I2S_MODULE_ENABLED
/**
 * @brief Adapter as I2S audio port.
 * @details
 * Dedicated adapter for the @ref murasaki::DuplexAudio.
 * By passing this adapter, the DuplexAudio class can handle audio through the I2S port.
 *
 * Caution : Regardless of the size of the data in I2S frame, set DMA data size as 16bit.
 *
 * Following table summarizes the data size in I2S signal frame vs Configurator settings.
 *
 * | I2S Data Size | I2S Data and Frame Format    | I2S DMA Data Size |
 * |:-------------:|:----------------------------:|:-----------------:|
 * |     16 bits   | 16bits data and 16bits frame |  Half Word        |
 * |     16 bits   | 16bits data and 32bits frame |  Half Word        |
 * |     24 bits   | 24bits data and 32bits frame |       Word        |
 * |     32 bits   | 32bits data and 32bits frame |       Word        |
 *
 * \ingroup MURASAKI_GROUP
 *
 */
class I2sPortAdapter : public AudioPortAdapterStrategy {
 public:
    /**
     * @brief Constructor.
     * @param tx_peripheral I2S_HandleTypeDef type peripheral for TX. This is defined in main.c.
     * @param rx_peripheral I2S_HandleTypeDef type peripheral for RX. This is defined in main.c.
     * @details
     * Receives handle of the I2S block peripherals.
     *
     * This class assumes one is the TX and the other is RX.
     * In case of a programmer use I2S as simplex audio, the unused block must be passed as nullptr.
     */
    I2sPortAdapter(
                   I2S_HandleTypeDef *tx_peripheral,
                   I2S_HandleTypeDef *rx_peripheral
                   );

    virtual ~I2sPortAdapter();

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
     * @return Always return 2 for STM32 I2S, becuase the cyclic DMA has halfway and complete interrupt.
     */
    virtual unsigned int GetNumberOfDMAPhase() {
        return 2;
    }
    ;
    /**
     * @brief Return how many channels are in the transfer.
     * @return always 2
     */
    virtual unsigned int GetNumberOfChannelsTx() {
        return 2;
    }
    ;

    /**
     * @brief Return the bit count to shift to make the DMA data to right align in TX I2S frame.
     * @return Always 0.
     * @details
     * This is needed because of the mismatch in the DMA buffer data formant and I2S format.
     *
     * Let's assume the 24bit data I2S format. Some peripheral place the data as
     * right aligned in 32bit DMA data ( as integer ), some peripheral places the data
     * as left aligned in 32bit DMA data ( as fixed point ).
     *
     * This kind of the mismatch will be aligned by audio frame work. This member function returns
     * how many bits have to be shifted to right in TX.
     *
     * If peripheral requires left align format, this function shuld return 0.
     *
     * The STM32 I2S DMA format is left aligned. So, always return 0.
     */
    virtual unsigned int GetSampleShiftSizeTx() {
        return 0;
    }
    ;

    /**
     * @brief Return the size of the one sample on memory for Tx channel
     * @return 2 or 4. The unit is [Byte]
     * @details
     * This function returns the size of the word which should be
     * allocated on the memory.
     */
    virtual unsigned int GetSampleWordSizeTx();

    /**
     * @brief Return how many channels are in the transfer.
     * @return always 2
     */
    virtual unsigned int GetNumberOfChannelsRx() {
        return 2;
    }
    ;

    /**
     * @brief Return the bit count to shift to make the DMA data to right align in RX I2S frame.
     * @return 0 The unit is [bit]
     * @details
     * This is needed because of the mismatch in the DMA buffer data formant and I2S format.
     *
     * Let's assume the 24bit data I2S format. Some peripheral place the data as
     * right aligned in 32bit DMA data ( as integer ), some peripheral places the data
     * as left aligned in 32bit DMA data ( as fixed point ).
     *
     * This kind of the mismatch will be aligned by audio frame work. This member function returns
     * how many bits have to be shifted to left in RX.
     *
     * If peripheral requires left align format, this function shuld return 0.
     *
     * The STM32 I2S DMA format is left aligned. So, always return 0.
     */
    virtual unsigned int GetSampleShiftSizeRx() {
        return 0;
    }
    ;

    /**
     * @brief Return the size of the one sample on memory for Rx channel
     * @return 2 or 4. The unit is [Byte]
     * @details
     * This function returns the size of the word which should be
     * allocated on the memory.
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

    /**
     * @brief Display half word swap is required. .
     * @return Always ture.
     * @details
     * Display whether the half word (int16_t) swap is required or not.
     *
     * I2S DMA requires the half word swap inside word.
     * Thus, always returns true.
     */
    virtual bool IsInt16SwapRequired()
    {
        return true;
    }
    ;

 private:
    I2S_HandleTypeDef *const tx_peripheral_;
    I2S_HandleTypeDef *const rx_peripheral_;
};
#endif // HAL_I2S_MODULE_ENABLED

}
/* namespace murasaki */

#endif /* I2SPORTADAPTER_HPP_ */
