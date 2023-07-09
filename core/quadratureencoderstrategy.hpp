/**
 * @file quadratureencoderstrategy.hpp
 *
 * @date 2020/02/19
 * @author Seiichi Horie
 */

#ifndef THIRDPARTY_QUADRATUREENCODERSTRATEGY_HPP_
#define THIRDPARTY_QUADRATUREENCODERSTRATEGY_HPP_

#include <peripheralstrategy.hpp>

namespace murasaki {

/**
 * @brief Strategy class for the quadrature encoder.
 * @details
 * The strategy class of the quadrature encoder peripheral.
 * The inherited class will control the encoder.
 *
 * @ingroup MURASAKI_ABSTRACT_GROUP
 *
 */
class QuadratureEncoderStrategy : public PeripheralStrategy {
 public:
    QuadratureEncoderStrategy()
            :
            PeripheralStrategy() {
    }

    /**
     * @brief Get the internal counter value
     * @return The value of the internal counter.
     *
     * Return the encoder internal value.
     */
    virtual unsigned int Get() = 0;

    /**
     * @brief Set the internal counter
     * @param value The value to set.
     *
     * The encoder internal counter is set to the value.
     */
    virtual void Set(unsigned int value) = 0;
};

} /* namespace murasaki */

#endif /* THIRDPARTY_QUADRATUREENCODERSTRATEGY_HPP_ */
