// Test cases for the calc class

#include "tlv320aic3204_mock.hpp"

using ::testing::AtLeast;
using ::testing::_;


TEST(Tlv320aic3204, init)
{
    MockI2c i2c;
    const uint8_t table[] = {1,2,3,4};


    murasaki::Tlv320aic3204 codec(
        48000,      // 48kHz.
        12000000,   // 12MHz
        &i2c,       // Through I2C master controller
        0x12        // At I2C address. 
    );

    EXPECT_CALL(i2c, Transmit(0x12,
                                table, 
                                sizeof(table),
                                NULL,
                                murasaki::kwmsIndefinitely)).Times(1);

    codec.SendCommand(table, sizeof(table));

}

