/**
 * @file bitoutstrategy.hpp
 *
 * @date 2018/05/07
 * @author Seiichi "Suikan" Horie
 * @brief Abstract class of GPIO bit out
 */

#ifndef BITOUTSTRATEGY_HPP_
#define BITOUTSTRATEGY_HPP_

#include <peripheralstrategy.hpp>

namespace murasaki {
/**
 * \ingroup MURASAKI_ABSTRACT_GROUP
 * \brief Definition of the root class of bit output
 * \details
 * A prototype of the general purpose bit out class
 *
 */

class BitOutStrategy: public PeripheralStrategy {
public:
	/**
	 * @brief Set a status of the output pin
	 * @param state Set "H" if the value is none zero, vice versa.
	 */
	virtual void Set( unsigned int state = 1 ) = 0;
	/**
	 * @brief Clear the status of the output pin as "L"
	 */
	virtual void Clear(void) { Set(0); };
	/**
	 * @brief Get a status of the output pin
	 * @return 1 or 0 as output state.
	 * @details
	 * The mean of "1" or "0" is system dependent.
	 *
	 * Usually, these represent "H" or "L" output state, respectively.
	 */

	virtual unsigned int Get(void) = 0;
	/**
	 * @brief Set "L" if current status is "H", vice versa.
	 */
	virtual void Toggle(void) {Set( ! Get() ); };
};

} /* namespace murasaki */

#endif /* BITOUTSTRATEGY_HPP_ */
