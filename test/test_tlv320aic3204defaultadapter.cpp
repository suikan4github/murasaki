// Test cases for Tlv320aic320Default4Adapter.

#include "tlv320aic3204defaultadapter.hpp"

using namespace ::testing;

// Testing Constructor.
// The constructor paths its controller and device_address parameter
// to the constructor of the parents class.
// We can test it by calling SendCommand.
TEST(Tlv320aic3204DefaultAdapter, Constructor) {
  const uint8_t device_address = 0x18;
  MockI2cMaster i2c;
  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  // Will given table be sent correctly?
  const u_int8_t table1[] = {1, 2, 3};

  EXPECT_CALL(
      i2c,                      // Mock
      Transmit(device_address,  // I2C Address
               _,               // Args<1> : Pointer to the data to send.
               sizeof(table1),  // Args<2> : Length of data in bytes.
               NULL,  // no variable to receive the length of transmission
               murasaki::kwmsIndefinitely  // Wait forever
               ))
      .With(Args<1, 2>(ElementsAreArray(table1)))
      .Times(1);

  adapter.SendCommand(table1, sizeof(table1));
}
