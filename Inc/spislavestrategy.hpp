/**
 * @file spislavestrategy.hpp
 *
 * @date 2018/02/11
 * @author: Seiichi "Suikan" Horie
 * @brief SPI slave root class.
 */

#ifndef SPISLAVESTRATEGY_HPP_
#define SPISLAVESTRATEGY_HPP_

#include <spicallbackstrategy.hpp>

namespace murasaki {

/**
 * \brief Root class of the SPI slave
 * \details
 * This class provides a thread safe, synchronous SPI transfer.
 *
 * \ingroup MURASAKI_ABSTRACT_GROUP
 */
class SpiSlaveStrategy : public murasaki::SpiCallbackStrategy
{
 public:
    /**
     * @brief Thread safe, synchronous SPI transfer
     * @param tx_data Data to be transmitted
     * @param rx_data Data buffer to receive data
     * @param size Transfer data size [byte] for each way. Must be smaller than 65536
     * @param transfered_count The transfered number of bytes during API.
     * @param timeout_ms Timeout limit [mS]
     * @return true if transfer complete, false if timeout
     */
    virtual SpiStatus TransmitAndReceive(
                                         const uint8_t *tx_data,
                                         uint8_t *rx_data,
                                         unsigned int size,
                                         unsigned int *transfered_count = nullptr,
                                         unsigned int timeout_ms = murasaki::kwmsIndefinitely)=0;

};

} /* namespace murasaki */

#endif /* SPISLAVESTRATEGY_HPP_ */
