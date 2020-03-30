/**
 * @file adcstrategy.hpp
 *
 * @date Feb 22, 2020
 * @author Seiichi Horie
 */

#ifndef ADCSTRATEGY_HPP_
#define ADCSTRATEGY_HPP_

#include <peripheralstrategy.hpp>

namespace murasaki {

/**
 * @brief Synchronized, blocking ADC converter Strategy.
 * \ingroup MURASAKI_ABSTRACT_GROUP
 *
 */
class AdcStrategy : public PeripheralStrategy {
 public:
    /**
     * @brief constructor
     */
    AdcStrategy()
            :
            PeripheralStrategy() {
    }

    /**
     * @brief Set the sample clocks for specified  channel.
     * @param channel Specify the ADC channel.
     * @param clocks How many clocks is applied to sample.
     * @details
     * Optional implementation dependent setting.
     */
    virtual void SetSampleClock(unsigned int channel, unsigned int clocks) = 0;

    /**
     * @brief Convert the analog input through the given channel.
     * @param channel Specify the ADC channel.
     * @param value Pointer to the variable to receive the data. Data is normalized by range [-1,1)
     * @param size Length of array if the value is array. Must be 1 if the value is a scalar.
     * @return
     */
    virtual murasaki::AdcStatus Convert(unsigned int channel, float *value, unsigned int size = 1) = 0;
    /**
     * \brief Call this function when the ADC conversion is done.
     * \param ptr Pointer for generic use. Usually, points a struct of a peripheral control 
     * \return true: ptr matches with peripheral and handle the call back. false : doesn't match.
     * \details
     * A call back to notify the end of conversion. The definition of calling timing is
     * depend on the implementation. This is called from an DMA ISR.
     *
     * Typically, an implementation may check whether the given ptr parameter matches its own device, and if matched, handle it
     * and return true. If it doesn't match, just return false.
     */
    virtual bool ConversionCompleteCallback(void *ptr) = 0;
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

#endif /* ADCSTRATEGY_HPP_ */
