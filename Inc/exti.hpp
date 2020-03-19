/**
 * @file exti.hpp
 *
 * @date Feb 29, 2020
 * @author takemasa
 */

#ifndef EXTI_HPP_
#define EXTI_HPP_

#include "interruptstrategy.hpp"
#include "synchronizer.hpp"

#ifdef HAL_EXTI_MODULE_ENABLED

namespace murasaki {

/**
 * @brief EXTI wrapper class.
 * @details
 * This class allow to enable / disable EXTI interrupt from GPIO.
 * Only EXTI_0 to EXTI_16 are supported.
 *
 * Beside of the disabling /enabling interrupt, this class prive a simple
 * synchronization between ISR and task, by Release and Wait function.
 *
 * Because each instance of this class can handle only one line, to control the
 * 16 input, you need to create 16 instance of this class.
 *
 * ## Configuration
 * To configure an EXTI, select the GPIO  and line to use.
 * And then, set it as external interrupt.
 * In this example, we configure the EXTI13.
 * You can give your own name to this likne by User Label field.
 * In this exmple, we name this EXTI as B1.
 *
 * @image html "exti-config-1.png"
 * @image latex "exti-config-1.png" width=5.12in
 *
 * Next, you must enable the relevant NVIC interrupt line. In this example, we enable the NIVC of EXTI15:10 ( for EXTI 13 ).
 * @image html "exti-config-1.png"
 * @image latex "exti-config-1.png" width=5.12in
 * ## Creating a peripheral object
 * To create an EXTI object, create a class variable from Exti type. The parameter of
 * the constructor should be a macro constant for the EXTI line, which is generated by CubeIDE.
 * \code
 *     my_exti = new murasaki::Exti(B1_pin);
 * \endcode
 *
 * ## Waiting interrupt
 * To wait an interrupt, use Exti::Wait() member function in a task.
 *
 * \code
 *     my_exti.Wait();
 * \endcode
 *
 * ## Interrupt handling
 * In the interrupt callback, you can reelase the waiting task by calling Exti::Release().
 * The parameter must be parameter of the HAL_GPIO_EXTI_Callback.
 * If the given parameter is same with its EXTI line, the Exti.Release() function release the
 * waiting task and return with truth.
 * \code
 * void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 * {
 *     // Check whether appropriate interrupt or not
 *     if (my_exti.Release(GPIO_Pin))
 *         return;
 * }
 *  \endcode
 *
 * \ingroup MURASAKI_GROUP
 */
class Exti : public InterruptStrategy {
 public:
    // Forbidden default constructor.
    Exti() = delete;

    /**
     * @brief Constructor.
     * @param line EXTI line identifier. Use CubeMX/IDE created value (***_pin) or EXTI_LINE_**
     * @details
     * The parameter passing should be like following. The B1_Pin is the pin identifier of the EXTI generated by CubeIDE.
     * @code
     * murasaki::platform.exti_b1 = new murasaki::Exti(B1_Pin);
     * @endcode
     * Or, pre-defined EXTI line identifire can be passed.
     * @code
     * murasaki::platform.exti_b1 = new murasaki::Exti(EXTI_LINE_13);
     * @endcode
     */
    Exti(unsigned int line);

    virtual ~Exti();
    /**
     * @brief Enable interrupt
     */
    virtual void Enable();

    /**
     * @brief Disable interrupt
     */
    virtual void Disable();

    /**
     * @details Wait for an interrupt from task.
     * @param timeout time out parameter
     * @return kisOK : Released by interrupt. kisTimeOut : released by timeout.
     */
    virtual murasaki::InterruptStatus Wait(unsigned int timeout = murasaki::kwmsIndefinitely);

    /**
     * @details Release the waiting task
     * @param line Interrupt line bit map given from the HAL_GPIO_EXTI_Callback()
     * @return true if line is matched with this EXTI. false if not matched.
     * @details
     * Should be called from the interrupt call back.
     *
     * The typical usage is to call from the HAL_GPIO_EXTI_Callback() function which
     * will be called from the HAL interrupt handler.
     * @code
     * void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
     * {
     *     // Check whether appropriate interrupt or not
     *     if (murasaki::platform.exti_b1->Release(GPIO_Pin))
     *         return;
     * }
     * @endcode
     */
    virtual bool Release(unsigned int line);

 private:
    // EXTI hande for HAL
    EXTI_HandleTypeDef hexti_;
    // EXTI configuration type to record.
    EXTI_ConfigTypeDef hconfig_;
    // bit pattern of line
    unsigned int line_;
    // For the synchronization between task and interrupt
    Synchronizer *const sync_;

};

}
;
/* namespace murasaki */

#endif // HAL_EXTI_MODULE_ENABLED

#endif /* EXTI_HPP_ */
