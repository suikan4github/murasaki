/**
 * \file i2ccallbackstrategy.hpp
 *
 *  \date  2018/02/11
 *  \author: Seiichi "Suikan" Horie
 *  \brief Root class definition of the I2C Master.
 */

#ifndef I2CCALLBACKSTRATEGY_HPP_
#define I2CCALLBACKSTRATEGY_HPP_

#include <peripheralstrategy.hpp>

namespace murasaki {
/**
 * \ingroup MURASAKI_ABSTRACT_GROUP
 * \brief Definition of the root class of I2C .
 * \details
 * This class specify the unified callbacks between the master and slave dvices.
 */

class I2cCallbackStrategy : public murasaki::PeripheralStrategy
{
 public:

    /**
     * \brief Call back to be called notify the transfer is complete.
     * \param ptr Pointer for generic use. Usually, points a struct of a peripheral control
     * \return true: ptr matches with peripheral and handle the call back. false : doesn't match.
     * \details
     * A call back to notify the end of entire block or byte transfer. The definition of calling timing is
     * depend on the implementation. This is called from an DMA ISR.
     *
     *
     * Typically, an implementation may check whether the given ptr parameter matches its own device, and if matched, handle it
     * and return true. If it doesn't match, just return false.
     */
    virtual bool TransmitCompleteCallback(void *ptr)=0;
    /**
     * \brief Call back to be called for entire block transfer is complete.
     * \param ptr Pointer for generic use. Usually, points a struct of a peripheral control
     * \return true: ptr matches with peripheral and handle the call back. false : doesn't match.
     * \details
     * This member function is a callback to notify the end of the entire block or byte transfer.
     * The definition of calling timing depends on the implementation.
     * You must call this function from an ISR.

     * Typically, an implementation may check whether the given ptr parameter matches its device,
     * and if matched, handle it and return true. If it doesn't match, return false.
     */
    virtual bool ReceiveCompleteCallback(void *ptr) = 0;
    /**
     * @brief Handling error report of device.
     * @param ptr Pointer for generic use. Usually, points a struct of a device control
     * @return true if ptr matches with device and handle the error. false if ptr doesn't match
     * This member function is a callback to detect error.
     *
     * The error handling is depend on the implementation.
     */
    virtual bool HandleError(void *ptr)= 0;

};

} /* namespace murasaki */

#endif /* I2CCALLBACKSTRATEGY_HPP_ */
