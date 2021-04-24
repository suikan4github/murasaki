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

#define TESTXTAL 25000000

namespace murasaki {


bool TestSi5351()
{
	// Create an test stub peripheral
	murasaki::I2cMasterStubSi5351 *i2c_stub = new murasaki::I2cMasterStubSi5351();

	// Create an object under test
	murasaki::Si5351 * si5351 = new murasaki::Si5351(i2c_stub, TESTXTAL);

	return true;
}

} /* namespace murasaki */
