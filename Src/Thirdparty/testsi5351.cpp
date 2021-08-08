/*
 * testsi5351.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: takemasa
 */

#include <Thirdparty/testsi5351.hpp>

/// Fixed frequency for the Si5351 object test.
#define TESTXTAL 25000000

/// End frequency of the scan test of the Si5351::Si5351ConfigSeek()
#define ENDFREQ 200000000
/// Start frequency of the scan test of the Si5351::Si5351ConfigSeek()
#define STARTFREQ 3

#define SI5351_TEST_BUFFER_LEN 40
#define SI5351_TEST_BUFFER_NUM 8

namespace murasaki {

TestSi5351::TestSi5351(
                       murasaki::I2cMasterStub *i2c_stub
                       )
        :
        i2c_stub_(i2c_stub),
        si5351_(new murasaki::Si5351(i2c_stub, 1, TESTXTAL))
{

}

void TestSi5351::TestIsInitializing() {
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 7
    const int BUT = 7;   // Bit under test
    const int RUT = 0;   // Register under test

    // ****************************************************************************
    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsInitializing() test ")
    buffer[0] = 1 << BUT;   // System is initializing.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0xFF;   // System is initializing.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0x00;   // System is NOT initializing.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = ~(1 << BUT);   // System is NOT initializing.
    i2c_stub_->writeRxBuffer(buffer, 1);

    // check result.
    flag = si5351_->IsInitializing();   // status 0x80
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsInitializing();    // status 0xFF
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsInitializing();    // status 0x00
    MURASAKI_ASSERT(!flag)
    flag = si5351_->IsInitializing();    // status 0x7F
    MURASAKI_ASSERT(!flag)

    // check the register access
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == RUT)

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();
}

void TestSi5351::TestIsLossOfLockA() {
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 5
    const int BUT = 5;   // Bit under test
    const int RUT = 0;   // Register under test

    // ****************************************************************************
    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsLossOfLockA() test ")
    buffer[0] = 1 << BUT;   // LoL
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0xFF;   // LoL.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0x00;   // NOT LoL.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = ~(1 << BUT);   // NOT LoL.
    i2c_stub_->writeRxBuffer(buffer, 1);

    // check result.
    flag = si5351_->IsLossOfLockA();   // status 0x80
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsLossOfLockA();    // status 0xFF
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsLossOfLockA();    // status 0x00
    MURASAKI_ASSERT(!flag)
    flag = si5351_->IsLossOfLockA();    // status 0x7F
    MURASAKI_ASSERT(!flag)

    // check the register access
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == RUT)

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();
}

void TestSi5351::TestIsLossOfLockB() {
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 6
    const int BUT = 6;   // Bit under test
    const int RUT = 0;   // Register under test

    // ****************************************************************************
    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsLossOfLockB() test ")
    buffer[0] = 1 << BUT;   // LoL
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0xFF;   // LoL.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0x00;   // NOT LoL.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = ~(1 << BUT);   // NOT LoL.
    i2c_stub_->writeRxBuffer(buffer, 1);

    // check result.
    flag = si5351_->IsLossOfLockB();
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsLossOfLockB();
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsLossOfLockB();
    MURASAKI_ASSERT(!flag)
    flag = si5351_->IsLossOfLockB();
    MURASAKI_ASSERT(!flag)

    // check the register access
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == RUT)

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();
}

void TestSi5351::TestIsLossOfClkin() {
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 4
    const int BUT = 4;   // Bit under test
    const int RUT = 0;   // Register under test

    // ****************************************************************************
    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsLossOfClkin() test ")
    buffer[0] = 1 << BUT;   // LoL
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0xFF;   // LoL.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0x00;   // NOT LoL.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = ~(1 << BUT);   // NOT LoL.
    i2c_stub_->writeRxBuffer(buffer, 1);

    // check result.
    flag = si5351_->IsLossOfClkin();
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsLossOfClkin();
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsLossOfClkin();
    MURASAKI_ASSERT(!flag)
    flag = si5351_->IsLossOfClkin();
    MURASAKI_ASSERT(!flag)

    // check the register access
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == RUT)

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

}

void TestSi5351::TestIsLossOfXtal() {
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 3
    const int BUT = 3;   // Bit under test
    const int RUT = 0;   // Register under test

    // ****************************************************************************
    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsLossOfXtal() test ")
    buffer[0] = 1 << BUT;   // LoL
    i2c_stub->writeRxBuffer(buffer, 1);
    buffer[0] = 0xFF;   // LoL.
    i2c_stub->writeRxBuffer(buffer, 1);
    buffer[0] = 0x00;   // NOT LoL.
    i2c_stub->writeRxBuffer(buffer, 1);
    buffer[0] = ~(1 << BUT);   // NOT LoL.
    i2c_stub->writeRxBuffer(buffer, 1);

    // check result.
    flag = si5351->IsLossOfXtal();
    MURASAKI_ASSERT(flag)
    flag = si5351->IsLossOfXtal();
    MURASAKI_ASSERT(flag)
    flag = si5351->IsLossOfXtal();
    MURASAKI_ASSERT(!flag)
    flag = si5351->IsLossOfXtal();
    MURASAKI_ASSERT(!flag)

    // check the register access
    i2c_stub->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == RUT)

    i2c_stub->clearRxBuffer();
    i2c_stub->clearTxBuffer();
}

bool TestSi5351Driver(int freq_step)
                      {
    bool error = false;

    // Create an Device Under Test.
    // @formatter:off
    TestSi5351 *dut = new TestSi5351(
                                     new murasaki::I2cMasterStub(
                                             SI5351_TEST_BUFFER_NUM,  // Number of the TX RX test buffers.
                                             SI5351_TEST_BUFFER_LEN,  // LENGTH of the TX RX test buffers.
                                             1,  // DUT I2C address
                                             true)  // Address filtering is on
                                             );
                                                                                                        // @formatter:on

    uint32_t stage1_a, stage1_b, stage1_c;
    uint32_t stage2_a, stage2_b, stage2_c;
    uint32_t div_by_4;
    uint32_t r;

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The Si5351ConfigSeek() test ")

    // Massive test for the basic coefficient calculation.
    // From input frequency 10Mhz to 30MHz by 5MHz step
    for (int xfreq = 10000000; xfreq < 35000000; xfreq += 5000000)
            {
        MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "xfreq     %dHz ", xfreq)

        // initial step of the frequency test is 1Hz.
        int step = 1;

        // test from 3Hz to 200MHz
        for (int i = STARTFREQ; i < ENDFREQ; i += step) {

            if (i > freq_step)
                step = freq_step;
            // Configure PLL by given frequency.
            si5351->Si5351ConfigSeek(xfreq, i, stage1_a, stage1_b, stage1_c, stage2_a, stage2_b, stage2_c, div_by_4, r);

            // Calculate the synthesized frequency
            double output, fvco;
            fvco = xfreq * (stage1_a + double(stage1_b) / stage1_c);

            // VCO frequency must be between 900MHz and 600MHz
            if ((fvco > 900000000) or (600000000 > fvco))
                    {
                MURASAKI_SYSLOG(nullptr, kfaPll, kseError, "VCO error fvco = %d ", int(fvco))
                error = true;
            }

            // See data sheet to understand the restriction of the div_by_4
            if (div_by_4 == 3)
                output = fvco / 4;
            else if (div_by_4 == 0)
                    {
                output = fvco / (stage2_a) / r;

                // Test divider restriction
                switch (stage2_a)
                {
                    case 1:
                        case 2:
                        case 3:
                        case 5:
                        case 7:
                        MURASAKI_SYSLOG(nullptr, kfaPll, kseError, "Divider error stage2_a = %d ", stage2_a)
                        error = true;
                }

            }
            else {
                debugger->Printf("logic error \n");
                error = true;
            }

            // The mantissa of the double is 15digit. Accept 2digit error.
            int delta = i - output;
            delta = (delta > 0) ? delta : -delta;
            if (delta > double(i) * 1e-13) {
                MURASAKI_SYSLOG(nullptr, kfaPll, kseError, "Error at %dHz, difference is %dHz ", i, int(output - i));
                error = true;
            }

            if (error) {

                MURASAKI_SYSLOG(nullptr, kfaPll, kseError, "starge1_a %d ", stage1_a);
                MURASAKI_SYSLOG(nullptr, kfaPll, kseError, "starge1_b %d ", stage1_b);
                MURASAKI_SYSLOG(nullptr, kfaPll, kseError, "starge1_c %d ", stage1_c);
                MURASAKI_SYSLOG(nullptr, kfaPll, kseError, "starge2_a %d ", stage2_a);
                MURASAKI_SYSLOG(nullptr, kfaPll, kseError, "starge2_b %d ", stage2_b);
                MURASAKI_SYSLOG(nullptr, kfaPll, kseError, "starge2_c %d ", stage2_c);
                MURASAKI_SYSLOG(nullptr, kfaPll, kseError, "div by 4  %d ", div_by_4);
                MURASAKI_SYSLOG(nullptr, kfaPll, kseError, "r         %d ", r);
                MURASAKI_ASSERT(false);

            }
        }
    }   // end of for.

    // Register construction test for the PLL coefficient.
    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The Si5351PackRegister() test")

    uint32_t inte, num, denom, div_4, r_div;
    uint8_t registers[8];
    // first test
    inte = 0x0001239F;
    denom = 0x00045678;
    num = 0x000ABCDE;
    div_4 = 3;
    r_div = 8;
    si5351->Si5351PackRegister(inte, num, denom, div_4, r_div, registers);

    MURASAKI_ASSERT(registers[0] == 0x56)
    MURASAKI_ASSERT(registers[1] == 0x78)
    MURASAKI_ASSERT(registers[3] == 0x23)
    MURASAKI_ASSERT(registers[4] == 0x9F)
    MURASAKI_ASSERT(registers[5] == 0x4A)
    MURASAKI_ASSERT(registers[6] == 0xBC)
    MURASAKI_ASSERT(registers[7] == 0xDE)

    MURASAKI_ASSERT((registers[2] & 0x03) == 0x01)
    MURASAKI_ASSERT(((registers[2] >> 2) & 0x03) == 0x03)
    MURASAKI_ASSERT(((registers[2] >> 4) & 0x07) == 0x03)

    // Second test as negative bit pattern of first one.
    inte = 0x0003FFFF & ~inte;
    denom = 0x000FFFFF & ~denom;
    num = 0x000FFFFF & ~num;
    div_4 = 0;
    r_div = 128;
    si5351->Si5351PackRegister(inte, num, denom, div_4, r_div, registers);

    MURASAKI_ASSERT(registers[0] == (0xFF & ~0x56))
    MURASAKI_ASSERT(registers[1] == (0xFF & ~0x78))
    MURASAKI_ASSERT(registers[3] == (0xFF & ~0x23))
    MURASAKI_ASSERT(registers[4] == (0xFF & ~0x9F))
    MURASAKI_ASSERT(registers[5] == (0xFF & ~0x4A))
    MURASAKI_ASSERT(registers[6] == (0xFF & ~0xBC))
    MURASAKI_ASSERT(registers[7] == (0xFF & ~0xDE))

    MURASAKI_ASSERT((registers[2] & 0x03) == (0x03 & ~0x01))
    MURASAKI_ASSERT(((registers[2] >> 2) & 0x03) == 0x00)
    MURASAKI_ASSERT(((registers[2] >> 4) & 0x07) == 0x07)

    // Now, testing the basic functions.

    return !error;
}

} /* namespace murasaki */
