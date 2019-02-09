/**
 * @file spislavespecifierstrategy.hpp
 *
 * @date 2018/02/11
 * @author: takemasa
 * @brief Abstract class of SPI slave specification.
 */

#ifndef SPISLAVESPECIFIERSTRATEGY_HPP_
#define SPISLAVESPECIFIERSTRATEGY_HPP_

#include "murasaki_defs.hpp"

namespace murasaki {
/**
 * \brief Definition of the root class of SPI slave specifier.
 * \details
 * A prototype of the SPI slave device specifier.
 *
 * The specifier adds the following SPI attributes :
 * @li CPOL
 * @li CPHA
 * @li Chip select control for slave.
 *
 * Because SPI slave has different setting device by device, this specifier should be
 * passed to the each transactions.
 *
 * AssetCs() and DeassertCs() have to be overriden to control the chip select output.
 * These member functions will be called from the AbstractSpiMaster.
 * @ingroup MURASAKI_ABSTRACT_GROUP
 */
class SpiSlaveSpecifierStrategy
{
 public:
    /**
     * @brief Constructor
     * @param pol Polarity setting
     * @param pha Phase setting
     */
    SpiSlaveSpecifierStrategy(murasaki::SpiClockPolarity pol, murasaki::SpiClockPhase pha);
    /**
     * @brief Constructor
     * @param pol Polarity setting
     * @param pha Phase setting
     */
    SpiSlaveSpecifierStrategy(unsigned int pol, unsigned int pha);
    /**
     * @brief Destructor
     */
    virtual ~SpiSlaveSpecifierStrategy();
    /**
     * @brief Chip select assertion
     * @details
     * This member function asset the output line to select the slave chip.
     *
     * This have to be overriden.
     */
    virtual void AssertCs();
    /**
     * @brief Chip select deassertoin
     * @details
     * This member function deasset the output line to de-select the slave chip.
     *
     * This have to be overriden.
     */
    virtual void DeassertCs();
    /**
     * @brief Getter of the CPHA
     * @return CPHA setting
     */
    murasaki::SpiClockPhase GetCpha();
    /**
     * @brief Getter of the CPOL
     * @return CPOL setting
     */
    murasaki::SpiClockPolarity GetCpol();
 protected:

    /**
     * @brief Setting of CPOL
     */
    murasaki::SpiClockPolarity const cpol_;
    /**
     * @brief Setting of CPHA
     */
    murasaki::SpiClockPhase const cpha_;
};

} /* namespace murasaki */

#endif /* SPISLAVESPECIFIERSTRATEGY_HPP_ */
