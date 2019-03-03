/**
 * \file spimaster.hpp
 *
 * @date 2018/02/14
 * @author Seiichi "Suikan" Horie
 * @brief SPI Master. Thread safe and blocking IO
 */

#ifndef SPIMASTER_HPP_
#define SPIMASTER_HPP_

#include <spimasterstrategy.hpp>
#include <synchronizer.hpp>
#include "criticalsection.hpp"

// Check if CubeMX genrated SPI module
#ifdef HAL_SPI_MODULE_ENABLED

namespace murasaki {
/**
 * @brief Thread safe, blocking IO. Encapsulating SPI master. Based on STM32Cube HAL driver and FreeRTOS
 * \details
 * The SpiMaster class is the wrapper of the SPI controller. To use the SpiMaster class,
 * make an instance with SPI_HandleTypeDef * type pointer. For example, to create
 * an instance for the SPI3 peripheral :
 * \code
 *     my_spi3 = new murasaki::SpiMaster(&hspi3);
 * \endcode
 * Where hspi3 is the handle generated by CubeMX for SPI3 peripheral. To use this class,
 * the SPI peripheral have to be configured to use the interrupt and DMA.
 * The bitrate should be configured by the CubeMX.
 *
 * In addition to the instantiation, we need to prepare an interrupt callback.
 * \code
 * void HAL_SPI_TxRxCpltCallback (SPI_HandleTypeDef * hspi)
 * {
 *     my_spi3->TransmitAndReceiveCompleteCallback(hspi);
 * }
 * \endcode
 *
 * Where HAL_SPI_TxRxCpltCallback is a predefined name of the SPI interrupt handler.
 * This is invoked by system whenever a interrupt baed SPI transmission is complete.
 * Becuase the default function is weakly bound, above definition will overwride the
 * default one.
 *
 * Note that above callback is invoked for any SPIn where n is 1, 2, 3... To avoid the
 * confusion, SpiMaster::TransferCompleteCallback() method chckes whether given parameter
 * matches with its SPI_HandleTypeDef * pointer ( which was passed to constructor ).
 * And only when both matches, the member function execute the interrupt termination process.
 *
 * Once the instance and callbacks are correctly prepared, we can use the Transfer member function.
 *
 * The @ref SpiMaster::TransmitAndReceive() member function is a blocking function. A programmer can specify the
 * timeout by timeout_ms parameter. By default, this parameter is set by kwmsIndefinitely
 * which specifes never time out.
 *
 *
 * Both methods can be called from only the task context. If these are called in the ISR
 * context, the result is unknown.
 *
 * Note : The behavior of when the timeout happen is not tested. Actually, it should not happen
 * because DMA is taken in SPI transmission. Murasaki stpos internal DMA, interrupt and SPI processing
 * internally then, return.
 *
 * Other error will cause the re-initializing of the SPI master. Murasaki doesn't support
 * any of CRC detection, TI frame mode or Multi-master SPI.
 * @ingroup MURASAKI_GROUP
 */
class SpiMaster : public SpiMasterStrategy
{
 public:
    /**
     * @brief Constractor
     * @param spi_handle Handle to the SPI peripheral. This have to be configured to use DMA by CubeMX.
     */
    SpiMaster(SPI_HandleTypeDef * spi_handle);
    virtual ~SpiMaster();
    /**
     * @brief Data transfer to/from SPI slave.
     * @param spi_spec A pointer to the AbstractSpiSpecification to specify the slave device.
     * @param tx_data Data to be transmitted
     * @param rx_data Data buffer to receive data
     * @param size Transfer data size [byte] for each way.
     * @param timeout_ms Timeout limit [mS]
     * @return true if transfer complete, false if timeout
     * @details
     * Transfer the data to/from SPI slave specified by parameter spi_spec.
     *
     * This member funciton re-initialize the SPI peripheral based on the clock information
     * from the spi_spec. And then, assert the chips elect through the spi_spec during the
     * data transfer.
     *
     * Following are the return codes:
     * @li @ref murasaki::kspisOK : The transfer complete without error.
     * @li @ref murasaki::kspisModeCRC : CRC error was detected.
     * @li @ref murasaki::kspisOverflow : SPI overflow or underflow was detected.
     * @li @ref murasaki::kspisFrameError Frame error in TI mode.
     * @li @ref murasaki::kspisDMA : Some DMA error was detected in HAL. SPI re-initialized.
     * @li @ref murasaki::kspisErrorFlag : Unhandled flags. SPI re-initialized.
     * @li @ref murasaki::ki2csTimeOut : Timeout detected. DMA stopped.
     * @li Other : Unhandled error . SPI re-initialized.
     */
    virtual SpiStatus TransmitAndReceive(
                                         murasaki::SpiSlaveAdapterStrategy * spi_spec,
                                         const uint8_t * tx_data,
                                         uint8_t * rx_data,
                                         unsigned int size,
                                         murasaki::WaitMilliSeconds timeout_ms = murasaki::kwmsIndefinitely);
    /**
     * @brief Callback to notify the end of transfer
     * @param ptr Pointer to the control object.
     * @return true if no error.
     */
    virtual bool TransmitAndReceiveCompleteCallback(void * ptr);
    /**
     * @brief Error handling
     * @param ptr Pointer to I2C_HandleTypeDef struct.
     * \return true: ptr matches with device and handle the error. false : doesn't match.
     * @details
     * A handle to print out the error message.
     *
     * Checks whether handle has error and if there is, print appropriate error. Then return.
     */
    virtual bool HandleError(void * ptr);
 private:
    /**
     * @brief Return the Platform dependent device control handle.
     * @return Handle of device.
     * @details
     * The handle is the pointer ( or some ID ) which specify the control data of
     * specific device.
     */
    virtual void * GetPeripheralHandle();
 protected:
    SPI_HandleTypeDef * const peripheral_;        // SPI peripheral handler.
    Synchronizer * const sync_;          // sync between task and interrupt
    CriticalSection * const critical_section_;    // protect memberfunction
 private:
    SpiStatus interrupt_status_;
};

} /* namespace murasaki */

#endif // HAL_SPI_MODULE_ENABLED

#endif /* SPIMASTER_HPP_ */
