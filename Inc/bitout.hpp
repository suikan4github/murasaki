/**
 * @file bitout.hpp
 *
 * @date 2018/05/07
 * @author Seiichi "Suikan" Horie
 * @brief GPIO bit out class
 */

#ifndef BITOUT_HPP_
#define BITOUT_HPP_

#include <bitoutstrategy.hpp>

#ifdef HAL_GPIO_MODULE_ENABLED

namespace murasaki {

/**
 * \ingroup MURASAKI_GROUP
 * @brief A structure to en-group the GPIO port and GPIO pin.
 * @details
 * This struct is used in the @ref BitIn class and @ref BitOut class.
 * THese classes returns a pointer to the variable of this type,
 * as return value of the GetPeripheralHandle() member funciton.
 */

struct GPIO_type {
	GPIO_TypeDef * port_; //!< The port.
	uint16_t pin_; //!< The number of pin.
};

/**
 * \ingroup MURASAKI_GROUP
 * @brief General purpose bit output.
 * \details
 * The BitOut class is the wrapper of the GPIO controller. To use the BitOut class,
 * make an instance with GPIO_TypeDef * type pointer. For example, to create
 * an instance for the a peripheral:
 * \code
 *     my_LED = new murasaki::BitOut(LED_port, LED_pin);
 * \endcode
 * Where LED_port and LED_pin are the macro generated by CubeIDE for GPIO pin.
 * the GPIO peripheral have to be configured to be right direction.
 */

class BitOut: public BitOutStrategy {
public:
	/**
	 * @brief Constructor
	 * @param port Pinter to the port strict.
	 * @param pin Number of the pin to output.
	 */
	BitOut(GPIO_TypeDef * port, uint16_t pin);
	/**
	 * @brief Set a status of the output pin
	 * @param state Set "H" if the value is none zero, vice versa.
	 */

	virtual void Set( unsigned int state = 1 );
	/**
	 * @brief Get a status of the output pin
	 * @return 1 or 0 as output state.
	 * @details
	 * The mean of "1" or "0" is system dependent.
	 *
	 * Usually, these represent "H" or "L" output state, respectively.
	 */

	virtual unsigned int Get(void);
	/**
	 * @brief pass the raw peripheral handler
	 * @return pointer to the GPIO_type variable hidden in a class.
	 */

	virtual void * GetPeripheralHandle();

private :
	const GPIO_type gpio_;
};

} /* namespace murasaki */

#endif // HAL_GPIO_MODULE_ENABLE


#endif /* BITOUT_HPP_ */

