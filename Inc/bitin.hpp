/**
 * @file bitin.hpp
 *
 * @date 2018/05/07
 * @author Seiichi "Suikan" Horie
 * @brief GPIO bit in class
 */

#ifndef BITIN_HPP_
#define BITIN_HPP_

#include <bitinstrategy.hpp>
#include "bitout.hpp"

#ifdef HAL_GPIO_MODULE_ENABLED
namespace murasaki {
/**
 * \ingroup MURASAKI_GROUP
 * @brief General purpose bit input.
 * \details
 * The BitIn class is the wrapper of the GPIO controller.
 * Programmer can read the state of a signal by using BitIn class.
 *
 * ### Configuration
 * To configure a bit input, open the Device Configuration Tool of the CubeIDE.
 * Make appropriate pins as GPIO Input pin. Then, chose GPIO section
 * from the left pane, select the input pin and configure.
 *
 * The string in the User Label filed can be used in the application program.
 * In this example, that is "SIG1"
 * @image html "bitin-config-1.png"
 * @image latex "bitin-config-1.png" width=5.77in
 *
 * ### Creating a peripheral object
 *
 * To use the BitIn class,
 * create an instance with GPIO_TypeDef * type pointer. For example, to create
 * an instance for a switch peripheral:
 * \code
 *     my_signal = new murasaki::BitIn(SIG1_GPIO_port, SIG1_pin);
 * \endcode
 * Where "SIG1" is the user label defined in the configuration.
 * The ***_GPIO_port and ***_pin are generated by CubeIDE automatically from the user label in the configuration.
 *
 * ### Reading Data
 * To read data, use the Get member function.
 * \code
 *     unsigned int state = my_signal.Get();
 * \endcode
 * The return value is 1 for "H" input, 0 for "L" input.
 *
 */

class BitIn : public BitInStrategy {
 public:
    /**
     * @brief Constructor
     * @param port Pinter to the port strict.
     * @param pin Number of the pin to input.
     */
    BitIn(GPIO_TypeDef *port, uint16_t pin);
    /**
     * @brief Get a status of the input pin
     * @return 1 or 0 as output state.
     * @details
     * Mean of "1" or "0" is system dependent.
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

#endif // HAL_GPIO_ENABLED

#endif /* BITIN_HPP_ */
