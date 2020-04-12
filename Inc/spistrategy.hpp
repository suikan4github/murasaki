/**
 * @file spistrategy.hpp
 *
 * @date 2020/04/11
 * @author: Seiichi "Suikan" Horie
 * @brief SPI root class.
 */

#ifndef SPISTRATEGY_HPP_
#define SPISTRATEGY_HPP_

#include <peripheralstrategy.hpp>

namespace murasaki {

/**
 * \brief Root class of the SPI
 * \ingroup MURASAKI_ABSTRACT_GROUP
 */
class SpiStrategy : public murasaki::PeripheralStrategy
{
 public:
    /**
     * @brief Callback to notifiy the end of transfer
     * @param ptr Pointer to the control object.
     * @return true if no error.
     */
    virtual bool TransmitAndReceiveCompleteCallback(void *ptr) = 0;
    /**
     * @brief Handling error report of device.
     * @param ptr Pointer for generic use. Usually, points a struct of a device control
     * @return true if ptr matches with device and handle the error. false if ptr doesn't match
     * A member function to detect error.
     *
     * The error handling is depend on the implementation.
     */
    virtual bool HandleError(void *ptr)= 0;

};

} /* namespace murasaki */

#endif /* SPISTRATEGY_HPP_ */
