/**
 * @file testsi5351.hpp
 * @
 *  Created on: 2021/04/24
 *      Author: takemasa
 */

#ifndef THIRDPARTY_TESTSI5351_HPP_
#define THIRDPARTY_TESTSI5351_HPP_

#include "Thirdparty/i2cmasterstub.hpp"
#include "Thirdparty/si5351.hpp"

#include "murasaki.hpp"

namespace murasaki {

/**
 * @ingroup MURASAKI_THIRDPARTY_TESTER
 * @brief Test driver of the @ref Si5351 class.
 * @param freq_step Step of the frequency during the test
 * @details
 *
 */
void TestSi5351Driver(int freq_step);

class TestSi5351 {
 public:
    TestSi5351(
               murasaki::I2cMasterStub *i2c_stub
               );

    void TestIsInitializing();
    void TestIsLossOfLockA();
    void TestIsLossOfLockB();
    void TestIsLossOfClkin();
    void TestIsLossOfXtal();
    void TestResetPLL();
    void TestSetClockConfig();
    void TestGetClockConfig();
    void TestSi5351ClockControl();
    void TestSetPhaseOffset();
    void TestSi5351ConfigSeek(int freq_step);
    void TestPackRegister();
    void TestSetFrequency();
    void TestSetQuadratureFrequency();

 private:
    murasaki::I2cMasterStub *const i2c_stub_;
    murasaki::Si5351 *const si5351_;
};

}
// namespace murasaki ;

#endif /* THIRDPARTY_TESTSI5351_HPP_ */
