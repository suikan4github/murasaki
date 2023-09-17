// Test cases for Tlv320aic3204AdapterStrategy.

// We use Tlv320aic320DefaultAdapter class to test Tlv320aic3204AdapterStrategy,
// Because we can not create instance of the Tlv320aic3204AdapterStrategy
#include "tlv320aic3204defaultadapter.hpp"

using namespace ::testing;

// Testing SendCommand .
TEST(Tlv320aic3204AdapterStrategy, SendCommand) {
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
TEST(Tlv320aic3204AdapterStrategy, SetPage) {
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
TEST(Tlv320aic3204AdapterStrategy, Reset) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x20;

  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  {
    InSequence dummy;

    // Must set page 0
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
        .Times(1);
  }
  adapter.Reset();
}

// Testing ConfigureClock() .
// Checking all convination of the role and PLL input,
// except the forbidden case.
TEST(Tlv320aic3204AdapterStrategy, ConfigureClock) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x21;

  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);
  // I2S Master, PLL input is MCLK
  {
    InSequence dummy;

    // Must set page 0

    EXPECT_CALL(
        i2c,                      // Mock
        Transmit(device_address,  // I2C Address
                 _,               // Args<1> : Pointer to the data to send.
                 2,               // Args<2> : Lenght of data in bytes.
                 NULL,  // no variable to receive the length of transmission
                 murasaki::kwmsIndefinitely  // Wait forever
                 ))
        .With(Args<1, 2>(ElementsAreArray({0, 0})));

    // Write to page 4, MCLCK input.

    EXPECT_CALL(
        i2c,                      // Mock
        Transmit(device_address,  // I2C Address
                 _,               // Args<1> : Pointer to the data to send.
                 2,               // Args<2> : Lenght of data in bytes.
                 NULL,  // no variable to receive the length of transmission
                 murasaki::kwmsIndefinitely  // Wait forever
                 ))
        .With(Args<1, 2>(ElementsAreArray({4, 3})));

    // Then write to register 27.
    EXPECT_CALL(
        i2c,                      // Mock
        Transmit(device_address,  // I2C Address
                 _,               // Args<1> : Pointer to the data to send.
                 2,               // Args<2> : Lenght of data in bytes.
                 NULL,  // no variable to receive the length of transmission
                 murasaki::kwmsIndefinitely  // Wait forever
                 ))
        .With(Args<1, 2>(ElementsAreArray({27, 0x3c})));
  }
  adapter.ConfigureClock(murasaki::Tlv320aic3204::kMaster,
                         murasaki::Tlv320aic3204::kMclk);

  // I2S Slave, PLL input is MCLK
  {
    InSequence dummy;

    // Must set page 0

    EXPECT_CALL(
        i2c,                      // Mock
        Transmit(device_address,  // I2C Address
                 _,               // Args<1> : Pointer to the data to send.
                 2,               // Args<2> : Lenght of data in bytes.
                 NULL,  // no variable to receive the length of transmission
                 murasaki::kwmsIndefinitely  // Wait forever
                 ))
        .With(Args<1, 2>(ElementsAreArray({0, 0})));

    // Write to page 4, MCLCK input.

    EXPECT_CALL(
        i2c,                      // Mock
        Transmit(device_address,  // I2C Address
                 _,               // Args<1> : Pointer to the data to send.
                 2,               // Args<2> : Lenght of data in bytes.
                 NULL,  // no variable to receive the length of transmission
                 murasaki::kwmsIndefinitely  // Wait forever
                 ))
        .With(Args<1, 2>(ElementsAreArray({4, 3})));

    // Then write to register 27 as I2S Slave.
    EXPECT_CALL(
        i2c,                      // Mock
        Transmit(device_address,  // I2C Address
                 _,               // Args<1> : Pointer to the data to send.
                 2,               // Args<2> : Lenght of data in bytes.
                 NULL,  // no variable to receive the length of transmission
                 murasaki::kwmsIndefinitely  // Wait forever
                 ))
        .With(Args<1, 2>(ElementsAreArray({27, 0x30})));
  }
  adapter.ConfigureClock(murasaki::Tlv320aic3204::kSlave,
                         murasaki::Tlv320aic3204::kMclk);

  // I2S Slave, PLL input is BCLK
  {
    InSequence dummy;

    // Must set page 0

    EXPECT_CALL(
        i2c,                      // Mock
        Transmit(device_address,  // I2C Address
                 _,               // Args<1> : Pointer to the data to send.
                 2,               // Args<2> : Lenght of data in bytes.
                 NULL,  // no variable to receive the length of transmission
                 murasaki::kwmsIndefinitely  // Wait forever
                 ))
        .With(Args<1, 2>(ElementsAreArray({0, 0})));

    // Write to page 4, BCLCK input.

    EXPECT_CALL(
        i2c,                      // Mock
        Transmit(device_address,  // I2C Address
                 _,               // Args<1> : Pointer to the data to send.
                 2,               // Args<2> : Lenght of data in bytes.
                 NULL,  // no variable to receive the length of transmission
                 murasaki::kwmsIndefinitely  // Wait forever
                 ))
        .With(Args<1, 2>(ElementsAreArray({4, 7})));

    // Then write to register 27 as I2S Slave.
    EXPECT_CALL(
        i2c,                      // Mock
        Transmit(device_address,  // I2C Address
                 _,               // Args<1> : Pointer to the data to send.
                 2,               // Args<2> : Lenght of data in bytes.
                 NULL,  // no variable to receive the length of transmission
                 murasaki::kwmsIndefinitely  // Wait forever
                 ))
        .With(Args<1, 2>(ElementsAreArray({27, 0x30})));
  }
  adapter.ConfigureClock(murasaki::Tlv320aic3204::kSlave,
                         murasaki::Tlv320aic3204::kBclk);
}

// Testing ConfigureClock() .
// Checking all convination of the role and PLL input,
// except the forbidden case.
TEST(Tlv320aic3204AdapterStrategy, ConfigurePll) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x23;

  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  // Test all combination of the PLL parameters except D paraemter.
  // D parameter is too heavy to check all.
  for (uint8_t r = 1; r < 5; r++)
    for (uint8_t j = 4; j < 64; j++)
      for (uint32_t d = 0; d < 10000; d += 99)
        for (uint8_t p = 1; p < 9; p++) {
          {
            InSequence dummy;

            // Must set page 0
            EXPECT_CALL(
                i2c,  // Mock
                Transmit(
                    device_address,  // I2C Address
                    _,               // Args<1> : Pointer to the data to send.
                    2,               // Args<2> : Lenght of data in bytes.
                    NULL,  // no variable to receive the length of transmission
                    murasaki::kwmsIndefinitely  // Wait forever
                    ))
                .With(Args<1, 2>(ElementsAreArray({0, 0})));

            // Write to page 5,6,7 and 8.
            EXPECT_CALL(
                i2c,  // Mock
                Transmit(
                    device_address,  // I2C Address
                    _,               // Args<1> : Pointer to the data to send.
                    5,               // Args<2> : Lenght of data in bytes.
                    NULL,  // no variable to receive the length of transmission
                    murasaki::kwmsIndefinitely  // Wait forever
                    ))
                .With(Args<1, 2>(ElementsAreArray({
                    (uint8_t)5,                      // reg number.
                    (uint8_t)(0x80 | (p << 4) | r),  // reg5
                    j,                               // reg6
                    (uint8_t)(d >> 8),               // reg7
                    (uint8_t)(d & 0xff)              // reg8
                })));
          }
          adapter.ConfigurePll(r, j, d, p);
        }
}

// Testing Reset command .
TEST(Tlv320aic3204AdapterStrategy, ShutdownPll) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x32;

  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  {
    InSequence dummy;

    // Must set page 0
    EXPECT_CALL(
        i2c,                      // Mock
        Transmit(device_address,  // I2C Address
                 _,               // Args<1> : Pointer to the data to send.
                 2,               // Args<2> : Lenght of data in bytes.
                 NULL,  // no variable to receive the length of transmission
                 murasaki::kwmsIndefinitely  // Wait forever
                 ))
        .With(Args<1, 2>(ElementsAreArray({0, 0})));

    // Then write 1 to register 1.
    EXPECT_CALL(
        i2c,                      // Mock
        Transmit(device_address,  // I2C Address
                 _,               // Args<1> : Pointer to the data to send.
                 2,               // Args<2> : Lenght of data in bytes.
                 NULL,  // no variable to receive the length of transmission
                 murasaki::kwmsIndefinitely  // Wait forever
                 ))
        .With(Args<1, 2>(ElementsAreArray({4, 0})));
  }
  adapter.ShutdownPll();
}

// Testing assertion of ConfigureClock() .
// The convination of the I2S master mode and the PLL input from
// BCLK is not allowed, because BCLK is output in the master mode.
// In this test, we check whether assertion works correctly or not.
TEST(Tlv320aic3204AdapterStrategyDeathTest, ConfigureClock) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x22;
  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  ASSERT_DEATH(adapter.ConfigureClock(murasaki::Tlv320aic3204::kMaster,
                                      murasaki::Tlv320aic3204::kBclk),
               "pll_source == Tlv320aic3204::kBclk && role == "
               "Tlv320aic3204::kMaster");
}

// Assertion test when r == 0
TEST(Tlv320aic3204AdapterStrategyDeathTest, ConfigurePll_r_0) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x22;
  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  ASSERT_DEATH(adapter.ConfigurePll(0,  // r
                                    1,  // j
                                    1,  // d
                                    1   // p
                                    ),
               "1 <= r && r <= 4");
}

// Assertion test when r == 5
TEST(Tlv320aic3204AdapterStrategyDeathTest, ConfigurePll_r_5) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x22;
  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  ASSERT_DEATH(adapter.ConfigurePll(5,  // r
                                    1,  // j
                                    1,  // d
                                    1   // p
                                    ),
               "1 <= r && r <= 4");
}

// Assertion test when j == 0
TEST(Tlv320aic3204AdapterStrategyDeathTest, ConfigurePll_j_0) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x22;
  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  ASSERT_DEATH(adapter.ConfigurePll(1,  // r
                                    0,  // j
                                    1,  // d
                                    1   // p
                                    ),
               "4 <= j && j <= 63");
}

// Assertion test when j == 64
TEST(Tlv320aic3204AdapterStrategyDeathTest, ConfigurePll_j_64) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x22;
  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  ASSERT_DEATH(adapter.ConfigurePll(1,   // r
                                    64,  // j
                                    1,   // d
                                    1    // p
                                    ),
               "4 <= j && j <= 63");
}

// Assertion test when d == 10000
TEST(Tlv320aic3204AdapterStrategyDeathTest, ConfigurePll_d_10000) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x22;
  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  ASSERT_DEATH(adapter.ConfigurePll(1,      // r
                                    4,      // j
                                    10000,  // d
                                    1       // p
                                    ),
               "d <= 9999");
}

// Assertion test when p == 0
TEST(Tlv320aic3204AdapterStrategyDeathTest, ConfigurePll_p_0) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x22;
  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  ASSERT_DEATH(adapter.ConfigurePll(1,  // r
                                    4,  // j
                                    1,  // d
                                    0   // p
                                    ),
               "1 <= p && p <= 8");
}

// Assertion test when p == 9
TEST(Tlv320aic3204AdapterStrategyDeathTest, ConfigurePll_p_9) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x22;
  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  ASSERT_DEATH(adapter.ConfigurePll(1,  // r
                                    4,  // j
                                    1,  // d
                                    9   // p
                                    ),
               "1 <= p && p <= 8");
}

// Assertion test when fs is wrong.
TEST(Tlv320aic3204AdapterStrategyDeathTest, ConfigureCODEC) {
  MockI2cMaster i2c;
  const uint8_t device_address = 0x23;
  murasaki::Tlv320aic3204DefaultAdapter adapter(&i2c, device_address);

  ASSERT_DEATH(adapter.ConfigureCODEC(32000),
               "fs == 44100 || fs == 88200 || fs == 176400 || fs == 48000 || "
               "fs == 96000 || fs == 192000");
}
