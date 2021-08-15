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

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsInitializing() test start.")

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

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

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsInitializing() test done.")
}

void TestSi5351::TestIsLossOfLockA() {
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 5
    const int BUT = 5;   // Bit under test
    const int RUT = 0;   // Register under test

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsLossOfLockA() test start.")

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

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

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsLossOfLockA() test done.")
}

void TestSi5351::TestIsLossOfLockB() {
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 6
    const int BUT = 6;   // Bit under test
    const int RUT = 0;   // Register under test

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsLossOfLockB() test start.")

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

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

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsLossOfLockB() test done.")
}

void TestSi5351::TestIsLossOfClkin() {
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 4
    const int BUT = 4;   // Bit under test
    const int RUT = 0;   // Register under test

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsLossOfClkin() test start.")

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

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

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsLossOfClkin() test done.")

}

void TestSi5351::TestIsLossOfXtal() {
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 3
    const int BUT = 3;   // Bit under test
    const int RUT = 0;   // Register under test

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsLossOfXtal() test start.")

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    buffer[0] = 1 << BUT;   // LoL
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0xFF;   // LoL.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0x00;   // NOT LoL.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = ~(1 << BUT);   // NOT LoL.
    i2c_stub_->writeRxBuffer(buffer, 1);

    // check result.
    flag = si5351_->IsLossOfXtal();
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsLossOfXtal();
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsLossOfXtal();
    MURASAKI_ASSERT(!flag)
    flag = si5351_->IsLossOfXtal();
    MURASAKI_ASSERT(!flag)

    // check the register access
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == RUT)

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The IsLossOfXtal() test done.")

}

void TestSi5351::TestSi5351ConfigSeek(int freq_step) {
    uint32_t stage1_a, stage1_b, stage1_c;
    uint32_t stage2_a, stage2_b, stage2_c;
    uint32_t div_by_4;
    uint32_t r;
    bool error = false;

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The Si5351ConfigSeek() test start.")

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
            si5351_->Si5351ConfigSeek(xfreq, i, stage1_a, stage1_b, stage1_c, stage2_a, stage2_b, stage2_c, div_by_4, r);

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
    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The Si5351ConfigSeek() test done.")

}

void TestSi5351::TestPackRegister() {

    // Register construction test for the PLL coefficient.
    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The Si5351PackRegister() test start.")

    uint32_t inte, num, denom, div_4, r_div;
    uint8_t registers[8];
    // first test
    inte = 0x0001239F;
    denom = 0x00045678;
    num = 0x000ABCDE;
    div_4 = 3;
    r_div = 8;
    si5351_->PackRegister(inte, num, denom, div_4, r_div, registers);

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
    si5351_->PackRegister(inte, num, denom, div_4, r_div, registers);

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
    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The Si5351PackRegister() test done.")

}

void TestSi5351::TestResetPLL()
{
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 177
    const int RUT = 177;   // RESET register

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The ResetPLL() test start.")

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    // check result.
    si5351_->ResetPLL(murasaki::ks5351PllA);
    si5351_->ResetPLL(murasaki::ks5351PllB);

    // check the register access
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == RUT)
    MURASAKI_ASSERT(buffer[1] == 1 << 5)

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == RUT)
    MURASAKI_ASSERT(buffer[1] == 1 << 7)

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The ResetPLL() test done.")

}

void TestSi5351::TestSetPhaseOffset()
{
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 177
    const int CTRL_REG = 16;   // control register
    const int OFST_REG = 165;   // phase offset register

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The SetPhaseOffset() test start. ")

    // ch=0, offset=0
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    // test function
    si5351_->SetPhaseOffset(0, 0);
    // Get offset register write data.
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == OFST_REG)
    MURASAKI_ASSERT(buffer[1] == 0);    // offset value

    // ch=0, offset=11
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    // test function
    si5351_->SetPhaseOffset(0, 11);
    // Get offset register write data.
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == OFST_REG)
    MURASAKI_ASSERT(buffer[1] == 11);    // offset value

    // ch=1, offset=0
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    // test function
    si5351_->SetPhaseOffset(1, 0);
    // Get offset register write data.
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == OFST_REG + 1)
    MURASAKI_ASSERT(buffer[1] == 0);    // offset value

    // ch=2, offset=11
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    // test function
    si5351_->SetPhaseOffset(2, 11);
    // Get offset register write data.
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == OFST_REG + 2)
    MURASAKI_ASSERT(buffer[1] == 11);    // offset value

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The SetPhaseOffset() test done.")

}

void TestSi5351::TestSetClockConfig()
{
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    // Si5351 : Register 16
    const int CTRL_REG = 16;   // control register

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The TestSetClockConfig() test start. ")

    // ch=0, offset=0
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    buffer[0] = 00;   // Set test data as register
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0xaa;   // Set test data as register
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0x55;   // Set test data as register
    i2c_stub_->writeRxBuffer(buffer, 1);

    // test function
    murasaki::Si5351ClockControl clockConfig;
    clockConfig = si5351_->GetClockConfig(0);
    // Get offset register write data.
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == CTRL_REG + 0)
    MURASAKI_ASSERT(clockConfig.value == 00)

    clockConfig = si5351_->GetClockConfig(1);
    // Get offset register write data.
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == CTRL_REG + 1)
    MURASAKI_ASSERT(clockConfig.value == 0xaa)

    clockConfig = si5351_->GetClockConfig(2);
    // Get offset register write data.
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == CTRL_REG + 2)
    MURASAKI_ASSERT(clockConfig.value == 0x55)

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The TestSetClockConfig() test done. ")
}

void TestSi5351::TestGetClockConfig()
{
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    // Si5351 : Register 16
    const int CTRL_REG = 16;   // control register

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The TestGetClockConfig() test start. ")

    // ch=0, offset=0
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    // test function
    murasaki::Si5351ClockControl clockConfig;
    clockConfig.value = 0x55;
    si5351_->SetClockConfig(0, clockConfig);
    // Get offset register write data.
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == CTRL_REG + 0)
    MURASAKI_ASSERT(buffer[1] == 0x55);    // offset value

    clockConfig.value = 0xAA;
    si5351_->SetClockConfig(1, clockConfig);
    // Get offset register write data.
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == CTRL_REG + 1)
    MURASAKI_ASSERT(buffer[1] == 0xAA);    // offset value

    clockConfig.value = 0xDE;
    si5351_->SetClockConfig(2, clockConfig);
    // Get offset register write data.
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == CTRL_REG + 2)
    MURASAKI_ASSERT(buffer[1] == 0xDE);    // offset value

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The TestGetClockConfig() test done. ")
}

void TestSi5351::TestSi5351ClockControl()
{
    murasaki::Si5351ClockControl clockConfig;

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The TestSi5351ClockControl() test start. ")

    MURASAKI_ASSERT(sizeof(clockConfig) == 1)
    clockConfig.value = 0;
    MURASAKI_ASSERT(clockConfig.fields.clk_idrv == murasaki::ks5351od2mA)

    clockConfig.value = 1;
    MURASAKI_ASSERT(clockConfig.fields.clk_idrv == murasaki::ks5351od4mA)
    clockConfig.value = 2;
    MURASAKI_ASSERT(clockConfig.fields.clk_idrv == murasaki::ks5351od6mA)
    clockConfig.value = 3;
    MURASAKI_ASSERT(clockConfig.fields.clk_idrv == murasaki::ks5351od8mA)

    clockConfig.value = 3 << 2;
    MURASAKI_ASSERT(clockConfig.fields.clk_src == murasaki::ks5351osNativeDivider)

    clockConfig.value = 1 << 4;
    MURASAKI_ASSERT(clockConfig.fields.clk_inv)

    clockConfig.value = 1 << 5;
    MURASAKI_ASSERT(clockConfig.fields.ms_src == murasaki::ks5351PllB)

    clockConfig.value = 1 << 6;
    MURASAKI_ASSERT(clockConfig.fields.ms_int)

    clockConfig.value = 1 << 7;
    MURASAKI_ASSERT(clockConfig.fields.clk_pdn)

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The TestSi5351ClockControl() test done. ")

}

void TestSi5351Driver(int freq_step) {

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

    dut->TestSi5351ClockControl();
    dut->TestIsInitializing();
    dut->TestIsLossOfLockA();
    dut->TestIsLossOfLockB();
    dut->TestIsLossOfXtal();
    dut->TestIsLossOfClkin();
    dut->TestResetPLL();
    dut->TestSetClockConfig();
    dut->TestGetClockConfig();
    dut->TestSetPhaseOffset();
    dut->TestSetFrequency();
    dut->TestPackRegister();
    dut->TestSi5351ConfigSeek(freq_step);
}

void TestSi5351::TestSetFrequency()
{
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    const int CLKCTRL = 16;   // RESET register
    const int PLLA = 26;   // RESET register
    const int MSDIV0 = 42;   // RESET register

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The TestSetFrequency() test start.")

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    buffer[0] = 00;   // Set test data as register for CLK_CTRL register.
    i2c_stub_->writeRxBuffer(buffer, 1);

    // check result.

    si5351_->SetFrequency(    //@formatter:off
                          murasaki::ks5351PllA,     // PLL A
                          0,                        // output port 0
                          50490000                      // 50.490 MHz
                          );            //@formatter:on

            // check the PLL register configuration
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 9)
    MURASAKI_ASSERT(buffer[0] == PLLA)
    MURASAKI_ASSERT((((buffer[3] & 0x03) << 16) + (buffer[4] << 8) + buffer[5]) == 24);    // PLL INTEGER part
    MURASAKI_ASSERT((((buffer[6] & 0x03) << 16) + (buffer[7] << 8) + buffer[8]) == 147);    // PLL NUM part
    MURASAKI_ASSERT((((buffer[6] & 0xF0) << 12) + (buffer[1] << 8) + buffer[2]) == 625);    // PLL DENOM part
    MURASAKI_ASSERT((buffer[3] & 0x0C) >> 2 == 0);    // PLL DIV_BY_4
    MURASAKI_ASSERT((buffer[3] & 0x70) >> 4 == 0);    // PLL RDIV

    // check the DIVIDER register configuration
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 9)
    MURASAKI_ASSERT(buffer[0] == MSDIV0)
    MURASAKI_ASSERT((((buffer[3] & 0x03) << 16) + (buffer[4] << 8) + buffer[5]) == 12);    // MS INTEGER part
    MURASAKI_ASSERT((((buffer[6] & 0x03) << 16) + (buffer[7] << 8) + buffer[8]) == 0);    // MS NUM part
    MURASAKI_ASSERT((((buffer[6] & 0xF0) << 12) + (buffer[1] << 8) + buffer[2]) == 1);    // MS DENOM part
    MURASAKI_ASSERT((buffer[3] & 0x0C) >> 2 == 0);    // MS DIV_BY_4
    MURASAKI_ASSERT((buffer[3] & 0x70) >> 4 == 0);    // MS RDIV

    // check the CLKCTRL address set
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == CLKCTRL)

    // check the CLKCTRL read-modify-write
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == CLKCTRL)
    MURASAKI_ASSERT((buffer[1] & (1 << 6)) && !(buffer[1] & (1 << 7)))
    // power down disable, integer mode enable.

    // Sepcific test freq > 150MHz ( div by 4 )
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    buffer[0] = 00;   // Set test data as register for CLK_CTRL register.
    i2c_stub_->writeRxBuffer(buffer, 1);

    // check result.

    si5351_->SetFrequency(   //@formatter:off
            murasaki::ks5351PllA,// PLL A
            0,// output port 0
            161234000// 161.234 MHz
    );
                                                    //@formatter:on

            // check the PLL register configuration
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 9)
    MURASAKI_ASSERT(buffer[0] == PLLA)
    MURASAKI_ASSERT((((buffer[3] & 0x03) << 16) + (buffer[4] << 8) + buffer[5]) == 25);    // PLL INTEGER part
    MURASAKI_ASSERT((((buffer[6] & 0x03) << 16) + (buffer[7] << 8) + buffer[8]) == 2492);    // PLL NUM part
    MURASAKI_ASSERT((((buffer[6] & 0xF0) << 12) + (buffer[1] << 8) + buffer[2]) == 3125);    // PLL DENOM part
    MURASAKI_ASSERT((buffer[3] & 0x0C) >> 2 == 0);    // PLL DIV_BY_4
    MURASAKI_ASSERT((buffer[3] & 0x70) >> 4 == 0);    // PLL RDIV

    // check the DIVIDER register configuration
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 9)
    MURASAKI_ASSERT(buffer[0] == MSDIV0)
    MURASAKI_ASSERT((((buffer[3] & 0x03) << 16) + (buffer[4] << 8) + buffer[5]) == 0);    // MS INTEGER part
    MURASAKI_ASSERT((((buffer[6] & 0x03) << 16) + (buffer[7] << 8) + buffer[8]) == 0);    // MS NUM part
    MURASAKI_ASSERT((((buffer[6] & 0xF0) << 12) + (buffer[1] << 8) + buffer[2]) == 1);    // MS DENOM part
    MURASAKI_ASSERT((buffer[3] & 0x0C) >> 2 == 3);    // MS DIV_BY_4
    MURASAKI_ASSERT((buffer[3] & 0x70) >> 4 == 0);    // MS RDIV

    // check the CLKCTRL address set
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == CLKCTRL)

    // check the CLKCTRL read-modify-write
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == CLKCTRL)
    MURASAKI_ASSERT((buffer[1] & (1 << 6)) && !(buffer[1] & (1 << 7)))
    // power down disable, integer mode enable.

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The TestSetFrequency() test done.")
}

} /* namespace murasaki */
