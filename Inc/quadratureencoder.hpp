/**
 * @file quadratureencoder.hpp
 *
 * @date 2020/02/19
 * @author takemasa
 */

#ifndef QUADRATUREENCODER_HPP_
#define QUADRATUREENCODER_HPP_

#include <quadratureencoderstrategy.hpp>

// Check if CubeIDE generated SPI Module
#ifdef HAL_TIM_MODULE_ENABLED

namespace murasaki {
/**
 * @brief Quadrature Encoder class.
 * @details
 * Dedicated class for the STM32 Timer encoder. The STM32 timer encoder can accept the 2 signal from the quadrature pulse encoder.
 * This timer encoer allow to up / down the counter by CW/CCW rotation. Basic parameter have to be set by CubeIDE/CubeMX
 *
 * \ingroup MURASAKI_GROUP
 *
 */
class QuadratureEncoder : public QuadratureEncoderStrategy {
 public:
    /**
     * Suppress the default constructor.
     */
    QuadratureEncoder() = delete;

    /**
     * @brief Constructor
     * @param htim The pointer to the peripheral control structure.
     * @details
     * The given parameter is stored to the peripheral_ variable.
     */
    QuadratureEncoder(TIM_HandleTypeDef *htim);
    /**
     * @brief Get the internal counter value
     * @return The value of the internal counter.
     *
     * Return the encoder internal value.
     */
    virtual unsigned int Get();

    /**
     * @brief Set the internal counter
     * @param value The value to set.
     *
     * The encoder internal counter is set to the value.
     */
    virtual void Set(unsigned int value);

 protected:
    TIM_HandleTypeDef *const peripheral_;    // Handle to the peripheral.

 private:
    /**
     * @brief Return the Platform dependent device control handle.
     * @return Handle of device.
     * @details
     * The handle is the pointer ( or some ID ) which specify the control data of
     * specific device.
     */
    virtual void* GetPeripheralHandle();

};

} /* namespace murasaki */

#endif /* HAL_TIM_MODULE_ENABLED */

#endif /* QUADRATUREENCODER_HPP_ */

