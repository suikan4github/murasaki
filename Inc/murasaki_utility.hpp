/*
 * murasaki_utility.hpp
 *
 *  Created on: 2020/03/01
 *      Author: takemasa
 */

#ifndef MURASAKI_UTILITY_HPP_
#define MURASAKI_UTILITY_HPP_

#include "i2cmasterstrategy.hpp"

namespace murasaki {

/**
 * @brief I2C device serach function
 * @ingroup MURASAKI_FUNCTION_GROUP
 * @param master Pointer to the I2C master controller object.
 * @details
 * Poll all device address and check the response. If no response(NAK),
 * there is no device.
 *
 */
void I2cSearch(murasaki::I2CMasterStrategy *master);

}

#endif /* MURASAKI_UTILITY_HPP_ */
