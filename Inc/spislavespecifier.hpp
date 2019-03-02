/**
 * @file spislavespecifier.hpp
 *
 * @date 2018/02/17
 * @author Seiichi "Suikan" Horie
 * @brief STM32 SPI slave speifire
 */

#ifndef SPISLAVESPECIFIER_HPP_
#define SPISLAVESPECIFIER_HPP_

#include <spislavespecifierstrategy.hpp>

// Check if CubeMx geenrates the SPI modeule
#ifdef HAL_SPI_MODULE_ENABLED

namespace murasaki {

/**
 * @brief A speficier of SPI slave.
 * @details
 * This class describes how ths slave is. The description is clock POL and PHA for the
 * speicific slave device.
 *
 * In addition to the clock porality, the instans of this class works as salogate of the
 * chip select control.
 *
 * The instans will be passed to the @ref SpiMaster class.
 *
 * @ingroup MURASAKI_GROUP
 */
class SpiSlaveSpecifier : public SpiSlaveSpecifierStrategy
{
 public:
    SpiSlaveSpecifier();
    /**
     * @brief Constructor
     * @param pol Polarity setting
     * @param pha Phase setting
     * @param port GPIO port of the chip select
     * @param pin GPIO pin of the chip select
     * @details
     * The port and pin parameters are passed to the HAL_GPIO_WritePin().
     * The port and pin have to be configured by CubeMX correctly.
     *
     */
    SpiSlaveSpecifier(murasaki::SpiClockPolarity pol, murasaki::SpiClockPhase pha, ::GPIO_TypeDef * port, uint16_t pin);
    /**
     * @brief Constructor
     * @param pol Polarity setting
     * @param pha Phase setting
     * @param port GPIO port of the chip select
     * @param pin GPIO pin of the chip select
     * @details
     * The port and pin parameters are passed to the HAL_GPIO_WritePin().
     * The port and pin have to be configured by CubeMX correctly.
     */
    SpiSlaveSpecifier(unsigned int pol, unsigned int pha, ::GPIO_TypeDef * const port, uint16_t pin);
    /**
     * @brief Chip select assertion
     * @details
     * This member function asset the output line to select the slave chip.
     */
    virtual void AssertCs();
    /**
     * @brief Chip select deassertoin
     * @details
     * This member function deasset the output line to de-select the slave chip.
     */
    virtual void DeassertCs();

 private:
    GPIO_TypeDef * const port_;
    uint16_t const pin_;
};

} /* namespace murasaki */

#endif //HAL_SPI_MODULE_ENABLED

#endif /* SPISLAVESPECIFIER_HPP_ */
