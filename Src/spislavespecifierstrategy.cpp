/*
 * abstractspimasterdecorator.cpp
 *
 *  Created on: 2018/02/11
 *      Author: Seiichi "Suikan" Horie
 */

#include <spislavespecifierstrategy.hpp>
#include "murasaki_assert.hpp"

namespace murasaki {



SpiSlaveSpecifierStrategy::~SpiSlaveSpecifierStrategy()
{
    // Do nothing.
}

void SpiSlaveSpecifierStrategy::AssertCs()
{
    // Do nothing here
    // Must be overriden
}

void SpiSlaveSpecifierStrategy::DeassertCs()
{
    // Do nothing here
    // Must be overriden
}

murasaki::SpiClockPhase SpiSlaveSpecifierStrategy::GetCpha()
{
    return cpha_;
}

murasaki::SpiClockPolarity SpiSlaveSpecifierStrategy::GetCpol()
{
    return cpol_;
}


SpiSlaveSpecifierStrategy::SpiSlaveSpecifierStrategy(murasaki::SpiClockPolarity pol, murasaki::SpiClockPhase pha)
        : cpol_(pol),
          cpha_(pha)
{
}

SpiSlaveSpecifierStrategy::SpiSlaveSpecifierStrategy(unsigned int pol, unsigned int pha)
        : cpol_(static_cast<murasaki::SpiClockPolarity>(pol)),
          cpha_(static_cast<murasaki::SpiClockPhase>(pha))
{
}


} /* namespace murasaki */
