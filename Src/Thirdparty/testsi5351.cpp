/*
 * testsi5351.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: takemasa
 */

#include <Thirdparty/i2cmasterstubsi5351.hpp>
#include "Thirdparty/si5351.hpp"
#include "murasaki.hpp"

#include <Thirdparty/testsi5351.hpp>

namespace murasaki {


bool TestSi5351()
{
	murasaki::I2cMasterStrategy *i2c_stub = new murasaki::I2cMasterStubSi5351();
	murasaki::Si5351 * si5351 = new murasaki::Si5351(i2c_stub, 25000000);

}

} /* namespace murasaki */
