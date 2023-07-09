/*
 * spislaveadapter.cpp
 *
 *  Created on: 2018/02/17
 *      Author: Seiichi "Suikan" Horie
 */

#include <spislaveadapter.hpp>
#include "murasaki_assert.hpp"

// Check if CubeIDE generated SPI modules.
#ifdef HAL_SPI_MODULE_ENABLED

namespace murasaki {

SpiSlaveAdapter::SpiSlaveAdapter(murasaki::SpiClockPolarity pol,
                                     murasaki::SpiClockPhase pha,
                                     ::GPIO_TypeDef* port,
                                     uint16_t pin)
        : SpiSlaveAdapterStrategy(pol, pha),
          port_(port),
          pin_(pin)
{
    MURASAKI_ASSERT(nullptr != port);
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);   // deassert at first.
}

SpiSlaveAdapter::SpiSlaveAdapter(unsigned int pol, unsigned int pha, ::GPIO_TypeDef* port, uint16_t pin)
        : SpiSlaveAdapterStrategy(pol, pha),
          port_(port),
          pin_(pin)
{
    MURASAKI_ASSERT(nullptr != port);
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);   // deassert at first
}

void SpiSlaveAdapter::AssertCs()
{
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);
}

void SpiSlaveAdapter::DeassertCs()
{
    HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
}

} /* namespace murasaki */

#endif // HAL_SPI_MODULE_ENABLED
