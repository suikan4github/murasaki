/*
 * abstractspimasterdecorator.cpp
 *
 *  Created on: 2018/02/11
 *      Author: Seiichi "Suikan" Horie
 */

#include <spislaveadapterstrategy.hpp>
#include "murasaki_assert.hpp"

namespace murasaki {



SpiSlaveAdapterStrategy::~SpiSlaveAdapterStrategy()
{
    // Do nothing.
}

void SpiSlaveAdapterStrategy::AssertCs()
{
    // Do nothing here
    // Must be overriden
}

void SpiSlaveAdapterStrategy::DeassertCs()
{
    // Do nothing here
    // Must be overriden
}

murasaki::SpiClockPhase SpiSlaveAdapterStrategy::GetCpha()
{
    return cpha_;
}

murasaki::SpiClockPolarity SpiSlaveAdapterStrategy::GetCpol()
{
    return cpol_;
}


SpiSlaveAdapterStrategy::SpiSlaveAdapterStrategy(murasaki::SpiClockPolarity pol, murasaki::SpiClockPhase pha)
        : cpol_(pol),
          cpha_(pha)
{
}

SpiSlaveAdapterStrategy::SpiSlaveAdapterStrategy(unsigned int pol, unsigned int pha)
        : cpol_(static_cast<murasaki::SpiClockPolarity>(pol)),
          cpha_(static_cast<murasaki::SpiClockPhase>(pha))
{
}


} /* namespace murasaki */
