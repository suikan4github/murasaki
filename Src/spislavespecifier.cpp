/*
 * spislavespecifier.cpp
 *
 *  Created on: 2018/02/17
 *      Author: takemasa
 */

#include <spislavespecifier.hpp>
#include "murasaki_assert.hpp"

// Check if CubeMX generated SPI modules.
#ifdef HAL_SPI_MODULE_ENABLED

namespace murasaki {

SpiSlaveSpecifier::SpiSlaveSpecifier(murasaki::SpiClockPolarity pol,
                                     murasaki::SpiClockPhase pha,
                                     ::GPIO_TypeDef* port,
                                     uint16_t pin)
        : SpiSlaveSpecifierStrategy(pol, pha),
          port_(port),
          pin_(pin)
{
    MURASAKI_ASSERT(nullptr != port);
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);   // deassert at first.
}

SpiSlaveSpecifier::SpiSlaveSpecifier(unsigned int pol, unsigned int pha, ::GPIO_TypeDef* port, uint16_t pin)
        : SpiSlaveSpecifierStrategy(pol, pha),
          port_(port),
          pin_(pin)
{
    MURASAKI_ASSERT(nullptr != port);
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);   // deassert at first
}

void SpiSlaveSpecifier::AssertCs()
{
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);
}

void SpiSlaveSpecifier::DeassertCs()
{
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
}

} /* namespace murasaki */

#endif // HAL_SPI_MODULE_ENABLED
