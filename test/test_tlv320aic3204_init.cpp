// Test cases for the calc class

#include "tlv320aic3204_mock.hpp"

using ::testing::AtLeast;


TEST(Tlv320aic3204, init)
{
    MockI2c i2c;

    murasaki::Tlv320aic3204 codec(
        48000,      // 48kHz.
        12000000,   // 12MHz
        &i2c,       // Through I2C master controller
        0x12        // At I2C address. 
    );

    EXPECT_EQ(1.0, 1.0);
}

