// Test cases for the calc class

#include "tlv320aic3204_mock.hpp"

using namespace ::testing;

TEST(Tlv320aic3204, init) {
  MockI2c i2c;
  const uint8_t table[] = {1, 2, 3, 4};

  murasaki::Tlv320aic3204 codec(48000,     // 48kHz.
                                12000000,  // 12MHz
                                &i2c,      // Through I2C master controller
                                0x12       // At I2C address.
  );

#if 0
    EXPECT_CALL(i2c, Transmit(0x12,
                                table, 
                                sizeof(table),
                                NULL,
                                murasaki::kwmsIndefinitely)).Times(1);
#else
  EXPECT_CALL(
      i2c,            // Mock
      Transmit(0x12,  // I2C Address
               _,     // Args<1> : Pointer to the data to send.
               4,     // Args<2> : Lenght of data in bytes.
               NULL,  // no variable to receive the length of transmission
               murasaki::kwmsIndefinitely  // Wait forever
               ))
      .With(Args<1, 2>(ElementsAreArray({1, 2, 3, 4})))
      .Times(1);

#endif

  codec.SendCommand(table, sizeof(table));
}
