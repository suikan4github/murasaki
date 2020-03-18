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
 * \ingroup MURASAKI_HELPER_GROUP
 * @brief A structure to en-group the GPIO port and GPIO pin.
 * @details
 * This struct is used in the @ref BitIn class and @ref BitOut class.
 * THese classes returns a pointer to the variable of this type,
 * as return value of the GetPeripheralHandle() member funciton.
 */

struct GPIO_type {
    GPIO_TypeDef *port_;  //!< The port.
    uint16_t pin_;  //!< The number of pin.
};

/**
 * \ingroup MURASAKI_GROUP
 * @brief General purpose bit output.
 * \details
 * The BitOut class is the wrapper of the GPIO controller.
 * Programmer can change the output pin state by using the BitOut class.
 *
 * ## Configuration
 * To configure a bit oputput, open the Device Configuration Tool of the CubeIDE.
 * Make appropriate pins as GPIO Output pin. Then, chose GPIO section
 * from the left pane, select the output pin and configure.
 *
 * The string in the User label filed can be used in the application program.
 * In this example, that is "LD2"
 * @image html "bitout-config-1.png"
 * @image latex "bitout-config-1.png"
 *
 * ## Creating a peripheral object
 *
 * To use the BitOut class,
 * create an instance with GPIO_TypeDef * type pointer. For example, to create
 * an instance for LED control:
 * \code
 *     my_led = new murasaki::BitOut(LD2_GPIO_port, LD2_pin);
 * \endcode
 * Where "LD2" is the user label defined in the configuration.
 * The ***_GPIO_port and ***_pin are generated by CubeIDE automatically from the user label in the configuration.
 *
 * ## Writing Data
 * To write data, use the Set member function.
 * \code
 *     my_led.Set();
 * \endcode
 * The output state is set to "H".
 *
 * You can call Clear() member function to set the output to "L".
 * \code
 *     my_led.Cler();
 * \endcode
 *
 * Or you can call Toggle() member function to flip the output signal.
 * \code
 *     my_led.Toggle();
 * \endcode
 * If the previous output was "L", the output change to "H".
 * If the previous output was "H", the output change to "L".
 *


 */

class BitOut : public BitOutStrategy {
 public:
    /**
     * @brief Constructor
     * @param port Pinter to the port strict.
     * @param pin Number of the pin to output.
     */
    BitOut(GPIO_TypeDef *port, uint16_t pin);
    /**
     * @brief Set a status of the output pin
     * @param state Set "H" if the value is none zero, vice versa.
     */

    virtual void Set(unsigned int state = 1);
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

    virtual void* GetPeripheralHandle();

 private:
    const GPIO_type gpio_;
};

} /* namespace murasaki */

#endif // HAL_GPIO_MODULE_ENABLE

#endif /* BITOUT_HPP_ */

