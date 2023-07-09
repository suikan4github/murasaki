/**
 * @file spimasterstrategy.hpp
 *
 * @date 2018/02/11
 * @author: Seiichi "Suikan" Horie
 * @brief SPI master root class.
 */

#ifndef SPIMASTERSTRATEGY_HPP_
#define SPIMASTERSTRATEGY_HPP_

#include <spicallbackstrategy.hpp>
#include <spislaveadapterstrategy.hpp>

namespace murasaki {

/**
 * \brief Root class of the SPI master
 * \details
 * This class provides a thread safe, synchronous SPI transfer.
 *
 * \ingroup MURASAKI_ABSTRACT_GROUP
 */
class SpiMasterStrategy : public murasaki::SpiCallbackStrategy
{
 public:
    /**
     * @brief Thread safe, synchronous SPI transfer
     * @param spi_spec Pointer to the SPI slave adapter which has clock configuraiton and chip select handling.
     * @param tx_data Data to be transmitted
     * @param rx_data Data buffer to receive data
     * @param size Transfer data size [byte] for each way. Must be smaller than 65536
     * @param timeout_ms Timeout limit [mS]
     * @return true if transfer complete, false if timeout
     */
    virtual SpiStatus TransmitAndReceive(
                                         murasaki::SpiSlaveAdapterStrategy *spi_spec,
                                         const uint8_t *tx_data,
                                         uint8_t *rx_data,
                                         unsigned int size,
                                         unsigned int timeout_ms = murasaki::kwmsIndefinitely)=0;

};

} /* namespace murasaki */

#endif /* SPIMASTERSTRATEGY_HPP_ */
