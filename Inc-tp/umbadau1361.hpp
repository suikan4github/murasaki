/*
 * umbadau1361.hpp
 *
 *  Created on: 2018/05/11
 *      Author: Seiichi "Suikan" Horie
 */

#ifndef THIRD_PARTY_UMBADAU1361_HPP_
#define THIRD_PARTY_UMBADAU1361_HPP_

//#include "../murasaki-tp/adau1361.hpp"

#ifdef  HAL_I2C_MODULE_ENABLED

namespace murasaki {

class UmbAdau1361: public Adau1361 {
public:
	/**
	 * \brief constructor.
     * \param fs Sampling frequency.
	 * \param controler Pass the I2C controler object.
     * \param addr I2C device address. value range is from 0 to 127
	 * \details
	 *   initialize the internal variables.
	 */
	UmbAdau1361(unsigned int fs, murasaki::I2CMasterStrategy * controler,
			unsigned int addr) :
	Adau1361( fs, controler, addr ) {};
protected:
	/**
	 * \brief configuration of the PLL for the desired Fs.
	 * \details
	 *   Configure the PLL based on the given Fs and hardware clock configuration.
	 *   Fs is stored in fs member variable already. Hadrware clock have to be given
	 *   from the circuit designer. For the UMB-ADAU1361-A, the clock is external
	 *   12MHz oscillator from the clock input.
	 */
	virtual void configure_pll(void);
	/**
	 * \brief configuration of the the codec for UMB-ADAU1361-A
	 * \details
	 *   Configure Internal signal pass and parameters for UMB-ADAU1361.
	 *   The all pass-through signals are shut off. All cross channel signals are shut off.
	 *   Monoral output is disabled.
	 */
	virtual void configure_board(void);
};

} /* namespace murasaki */

#endif //  HAL_I2C_MODULE_ENABLED

#endif /* THIRD_PARTY_UMBADAU1361_HPP_ */
