// Test cases for the calc class

#include "tlv320aic3204.hpp"

using namespace ::testing;

// Testing SetPage command .
TEST(Tlv320aic3204, SetPage) {
  MockI2cMaster i2c;

  murasaki::Tlv320aic3204 codec(48000,     // 48kHz.
                                12000000,  // 12MHz
                                &i2c,      // Through I2C master controller
                                0x18       // At I2C address.
  );

  EXPECT_CALL(
      i2c,            // Mock
      Transmit(0x18,  // I2C Address
               _,     // Args<1> : Pointer to the data to send.
               2,     // Args<2> : Lenght of data in bytes.
               NULL,  // no variable to receive the length of transmission
               murasaki::kwmsIndefinitely  // Wait forever
               ))
      .With(Args<1, 2>(ElementsAreArray({0, 3})))
      .Times(1);

  codec.SetPage(3);

  EXPECT_CALL(
      i2c,            // Mock
      Transmit(0x18,  // I2C Address
               _,     // Args<1> : Pointer to the data to send.
               2,     // Args<2> : Lenght of data in bytes.
               NULL,  // no variable to receive the length of transmission
               murasaki::kwmsIndefinitely  // Wait forever
               ))
      .With(Args<1, 2>(ElementsAreArray({0, 255})))
      .Times(1);

  codec.SetPage(255);
}
