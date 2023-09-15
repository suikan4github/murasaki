// Test cases for the calc class

#include "tlv320aic3204defaultadapter.hpp"

using namespace ::testing;

// Testing SendCommand .
TEST(Tlv320aic3204DefaultAdapter, SendCommand) {
  const uint8_t device_address = 0x18;
  MockI2cMaster i2c;
  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  // Will given table be sent correctly?
  const u_int8_t table1[] = {1, 2, 3};

  EXPECT_CALL(
      i2c,                      // Mock
      Transmit(device_address,  // I2C Address
               _,               // Args<1> : Pointer to the data to send.
               sizeof(table1),  // Args<2> : Lenght of data in bytes.
               NULL,  // no variable to receive the length of transmission
               murasaki::kwmsIndefinitely  // Wait forever
               ))
      .With(Args<1, 2>(ElementsAreArray(table1)))
      .Times(1);

  adapter.SendCommand(table1, sizeof(table1));

  // Let's test with different parameter.
  const u_int8_t table2[] = {4, 5, 6, 7};

  EXPECT_CALL(
      i2c,                      // Mock
      Transmit(device_address,  // I2C Address
               _,               // Args<1> : Pointer to the data to send.
               sizeof(table2),  // Args<2> : Lenght of data in bytes.
               NULL,  // no variable to receive the length of transmission
               murasaki::kwmsIndefinitely  // Wait forever
               ))
      .With(Args<1, 2>(ElementsAreArray(table2)))
      .Times(1);

  adapter.SendCommand(table2, sizeof(table2));
}

// Testing SetPage command .
TEST(Tlv320aic3204DefaultAdapter, SetPage) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x19;

  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  EXPECT_CALL(
      i2c,                      // Mock
      Transmit(device_address,  // I2C Address
               _,               // Args<1> : Pointer to the data to send.
               2,               // Args<2> : Lenght of data in bytes.
               NULL,  // no variable to receive the length of transmission
               murasaki::kwmsIndefinitely  // Wait forever
               ))
      .With(Args<1, 2>(ElementsAreArray({0, 3})))
      .Times(1);

  adapter.SetPage(3);

  // Let's test with different page.
  EXPECT_CALL(
      i2c,                      // Mock
      Transmit(device_address,  // I2C Address
               _,               // Args<1> : Pointer to the data to send.
               2,               // Args<2> : Lenght of data in bytes.
               NULL,  // no variable to receive the length of transmission
               murasaki::kwmsIndefinitely  // Wait forever
               ))
      .With(Args<1, 2>(ElementsAreArray({0, 255})))
      .Times(1);

  adapter.SetPage(255);
}

// Testing Reset command .
TEST(Tlv320aic3204DefaultAdapter, Reset) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x20;

  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  // Must set page 0
  Expectation page_set =
      EXPECT_CALL(
          i2c,                      // Mock
          Transmit(device_address,  // I2C Address
                   _,               // Args<1> : Pointer to the data to send.
                   2,               // Args<2> : Lenght of data in bytes.
                   NULL,  // no variable to receive the length of transmission
                   murasaki::kwmsIndefinitely  // Wait forever
                   ))
          .With(Args<1, 2>(ElementsAreArray({0, 0})))
          .Times(1);

  // Then write 1 to register 1.
  EXPECT_CALL(
      i2c,                      // Mock
      Transmit(device_address,  // I2C Address
               _,               // Args<1> : Pointer to the data to send.
               2,               // Args<2> : Lenght of data in bytes.
               NULL,  // no variable to receive the length of transmission
               murasaki::kwmsIndefinitely  // Wait forever
               ))
      .With(Args<1, 2>(ElementsAreArray({1, 1})))
      .Times(1)
      .After(page_set);

  adapter.Reset();
}
