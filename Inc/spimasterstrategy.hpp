/**
 * @file spimasterstrategy.hpp
 *
 * @date 2018/02/11
 * @author: Seiichi "Suikan" Horie
 * @brief SPI master root class.
 */

#ifndef SPIMASTERSTRATEGY_HPP_
#define SPIMASTERSTRATEGY_HPP_

#include <peripheralstrategy.hpp>
#include <spislaveadapterstrategy.hpp>

namespace murasaki {

/**
 * \brief Root class of the SPI master
 * \details
 * This class provides a thread safe, blocking SPI transfer.
 *
 * \ingroup MURASAKI_ABSTRACT_GROUP
 */
class SpiMasterStrategy : public murasaki::PeripheralStrategy
{
 public:
    /**
     * @brief Thread safe, blocking SPI transfer
     * @param spi_spec Pointer to the SPI slave adapter which has clock configuraiton and chip select handling.
     * @param tx_data Data to be transmitted
     * @param rx_data Data buffer to receive data
     * @param size Transfer data size [byte] for each way. Must be smaller than 65536
     * @param timeout_ms Timeout limit [mS]
     * @return true if transfer complete, false if timeout
     */
    virtual SpiStatus TransmitAndReceive(
                                         murasaki::SpiSlaveAdapterStrategy * spi_spec,
                                         const uint8_t * tx_data,
                                         uint8_t * rx_data,
                                         unsigned int size,
                                         unsigned int timeout_ms = murasaki::kwmsIndefinitely)=0;
    /**
     * @brief Callback to notifiy the end of transfer
     * @param ptr Pointer to the control object.
     * @return true if no error.
     */
    virtual bool TransmitAndReceiveCompleteCallback(void * ptr) = 0;
    /**
     * @brief Handling error report of device.
     * @param ptr Pointer for generic use. Usually, points a struct of a device control
     * @return true if ptr matches with device and handle the error. false if ptr doesn't match
     * A member function to detect error.
     *
     * The error handling is depend on the implementation.
     */
    virtual bool HandleError(void * ptr)= 0;

};

} /* namespace murasaki */

#endif /* SPIMASTERSTRATEGY_HPP_ */
