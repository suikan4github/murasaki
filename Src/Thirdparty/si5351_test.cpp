/*
 * si5351_test.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: takemasa
 */

#include <Thirdparty/i2cmasterstubsi5351.hpp>
#include "Thirdparty/si5351.hpp"
#include "murasaki.hpp"

namespace murasaki {

murasaki::I2cMasterStrategy *i2c_stub = new murasaki::I2cMasterStubSi5351();

} /* namespace murasaki */
