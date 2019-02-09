/**
 * @file spislavestrategy.hpp
 *
 * @date 2018/02/11
 * @author: takemasa
 * @brief SPI master root class.
 */

#ifndef SPISLAVESTRATEGY_HPP_
#define SPISLAVESTRATEGY_HPP_

#include <peripheralstrategy.hpp>

namespace murasaki {

/**
 * \brief Root class of the SPI slave
 * \details
 * This class provides a thread safe, blocking SPI transfer.
 *
 * \ingroup MURASAKI_ABSTRACT_GROUP
 */
class SpiSlaveStrategy : public murasaki::PeripheralStrategy
{
 public:
    /**
     * @brief Thread safe, blocking SPI transfer
     * @param tx_data Data to be transmitted
     * @param rx_data Data buffer to receive data
     * @param size Transfer data size [byte] for each way. Must be smaller than 65536
     * @param transfered_count The transfered number of bytes during API.
     * @param timeout_ms Timeout limit [mS]
     * @return true if transfer complete, false if timeout
     */
    virtual SpiStatus TransmitAndReceive(
                                         const uint8_t * tx_data,
                                         uint8_t * rx_data,
                                         unsigned int size,
                                         unsigned int * transfered_count = nullptr,
                                         murasaki::WaitMilliSeconds timeout_ms = murasaki::kwmsIndefinitely)=0;
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

#endif /* SPISLAVESTRATEGY_HPP_ */
