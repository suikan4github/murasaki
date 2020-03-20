/**
 * @file bitinstrategy.hpp
 *
 * @date 2018/05/07
 * @author Seiichi "Suikan" Horie
 * @brief Abstract class of the GPIO bit in
 */

#ifndef BITINSTRATEGY_HPP_
#define BITINSTRATEGY_HPP_

#include <peripheralstrategy.hpp>

namespace murasaki {
/**
 * \ingroup MURASAKI_ABSTRACT_GROUP
 * \brief Definition of the root class of bit input
 * \details
 * A prototype of the general purpose bit input class
 *
 */
class BitInStrategy : public PeripheralStrategy {
 public:
    /**
     * @brief Get a status of the input pin
     * @return 1 or 0 as input state.
     * @details
     * The mean of "1" or "0" is system dependent.
     *
     * Usually, these represent "H" or "L" input state, respectively.
     */
    virtual unsigned int Get(void) = 0;
};

} /* namespace murasaki */

#endif /* BITINSTRATEGY_HPP_ */
