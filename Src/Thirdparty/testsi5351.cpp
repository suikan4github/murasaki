/*
 * testsi5351.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: takemasa
 */

#include <Thirdparty/testsi5351.hpp>

/// Fixed frequency for the Si5351 object test.
#define TESTXTAL 25000000
/// Fixed Test I2C address of Si5351.
#define TESTI2CADDRESS 0x60

/// End frequency of the scan test of the Si5351::Si5351ConfigSeek()
/// 200MHz is the Highest frequency in Si5351 specification
#define ENDFREQ 200000000
/// Start frequency of the scan test of the Si5351::Si5351ConfigSeek()
/// 2.5kHz is the lowest frequency in Si5351 specification
#define STARTFREQ 2500

#define SI5351_TEST_BUFFER_LEN 40
#define SI5351_TEST_BUFFER_NUM 12

// Parameter configuration macro for Si5351. See datasheet for details of P1, P2, P3
#define P1(a,b,c) (128*a+(128*b/c)-512)
#define P2(a,b,c) (128*b-c*(128*b/c))
#define P3(a,b,c) (c)

// To test, calculate the P1, P2, P3 from the packed registers.
#define PACKED_P1(reg) (((reg[2] & 0x03) << 16)+ (reg[3]<<8) + reg[4])
#define PACKED_P2(reg) (( (reg[5] & 0x0F) << 16) + (reg[6] << 8 ) + reg[7] )
#define PACKED_P3(reg) (((reg[5]>>4)<<16) + (reg[0]<<8) + reg[1])
#define PACKED_DIVBY4(reg) ((reg[2] & 0x0C) >> 2)
#define PACKED_RDIV(reg) ((reg[2] & 0x70) >> 4)

namespace murasaki {

TestSi5351::TestSi5351(
                       murasaki::I2cMasterStub *i2c_stub
                       )
        :
        i2c_stub_(i2c_stub),
        si5351_(new murasaki::Si5351(i2c_stub, TESTI2CADDRESS, TESTXTAL))
{

}

void TestSi5351::TestIsInitializing() {
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 7
    const int BUT = 7;   // Bit under test
    const int RUT = 0;   // Register under test

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start.")

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    buffer[0] = 1 << BUT;   // System is initializing.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0xFF;   // System is initializing.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0x00;   // System is NOT initializing.
    i2c_stub_->writeRxBuffer(buffer, 1);
    buffer[0] = 0xFF & ~(1 << BUT);   // System is NOT initializing.
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

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done.")
}

void TestSi5351::TestIsLossOfLock() {

    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 5
    int BUT = 5;   // Bit under test
    int RUT = 0;   // Register under test

    // Test PLL A

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start.")

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
    flag = si5351_->IsLossOfLock(murasaki::ks5351PllA);   // status 0x80
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsLossOfLock(murasaki::ks5351PllA);    // status 0xFF
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsLossOfLock(murasaki::ks5351PllA);    // status 0x00
    MURASAKI_ASSERT(!flag)
    flag = si5351_->IsLossOfLock(murasaki::ks5351PllA);    // status 0x7F
    MURASAKI_ASSERT(!flag)

    // check the register access
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == RUT)

    // Test PLL B

    // Si5351 : Register 0, bit 6
    BUT = 6;   // Bit under test
    RUT = 0;   // Register under test

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
    flag = si5351_->IsLossOfLock(murasaki::ks5351PllB);
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsLossOfLock(murasaki::ks5351PllB);
    MURASAKI_ASSERT(flag)
    flag = si5351_->IsLossOfLock(murasaki::ks5351PllB);
    MURASAKI_ASSERT(!flag)
    flag = si5351_->IsLossOfLock(murasaki::ks5351PllB);
    MURASAKI_ASSERT(!flag)

    // check the register access
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == RUT)

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done.")
}

void TestSi5351::TestIsLossOfClkin() {
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 4
    const int BUT = 4;   // Bit under test
    const int RUT = 0;   // Register under test

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start.")

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

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done.")

}

void TestSi5351::TestIsLossOfXtal() {
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 0, bit 3
    const int BUT = 3;   // Bit under test
    const int RUT = 0;   // Register under test

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start.")

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

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done.")

}

void TestSi5351::TestEnableOutput()
{
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    bool flag;
    // Si5351 : Register 3 for OE
    const int RUT = 3;   // Register under test

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start.")

    // ************** Enable to enable, bit 0
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    buffer[0] = 0x00;   // Set value to be read.
    i2c_stub_->writeRxBuffer(buffer, 1);

    si5351_->EnableOutput(0);   // enable;

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    // must be 1byte transmission to specify the register address.
    MURASAKI_ASSERT(transffered_len == 1)
    // must be OE register address.
    MURASAKI_ASSERT(buffer[0] == RUT)

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    // must be 1byte transmission to specify the register address.
    MURASAKI_ASSERT(transffered_len == 2)
    // must be OE register address.
    MURASAKI_ASSERT(buffer[0] == RUT)
    // if enable, it must be 0 in bit 0.
    MURASAKI_ASSERT(buffer[1] == 0)

    // ************** Disable to enable, bit 0
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    buffer[0] = 0x07;   // Set value to be read.
    i2c_stub_->writeRxBuffer(buffer, 1);

    si5351_->EnableOutput(0);   // enable;

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    // must be 1byte transmission to specify the register address.
    MURASAKI_ASSERT(transffered_len == 1)
    // must be OE register address.
    MURASAKI_ASSERT(buffer[0] == RUT)

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    // must be 1byte transmission to specify the register address.
    MURASAKI_ASSERT(transffered_len == 2)
    // must be OE register address.
    MURASAKI_ASSERT(buffer[0] == RUT)
    // if enable, it must be 0 in bit 0.
    MURASAKI_ASSERT(buffer[1] == 0x06)

    // ************** Enable to disable, bit 0
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    buffer[0] = 0x00;   // Set value to be read.
    i2c_stub_->writeRxBuffer(buffer, 1);

    si5351_->EnableOutput(0, false);   // enable;

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    // must be 1byte transmission to specify the register address.
    MURASAKI_ASSERT(transffered_len == 1)
    // must be OE register address.
    MURASAKI_ASSERT(buffer[0] == RUT)

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    // must be 1byte transmission to specify the register address.
    MURASAKI_ASSERT(transffered_len == 2)
    // must be OE register address.
    MURASAKI_ASSERT(buffer[0] == RUT)
    // if enable, it must be 0 in bit 0.
    MURASAKI_ASSERT(buffer[1] == 1)

    // ************** Disable to disable, bit 0
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    buffer[0] = 0x07;   // Set value to be read.
    i2c_stub_->writeRxBuffer(buffer, 1);

    si5351_->EnableOutput(0, false);   // enable;

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    // must be 1byte transmission to specify the register address.
    MURASAKI_ASSERT(transffered_len == 1)
    // must be OE register address.
    MURASAKI_ASSERT(buffer[0] == RUT)

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    // must be 1byte transmission to specify the register address.
    MURASAKI_ASSERT(transffered_len == 2)
    // must be OE register address.
    MURASAKI_ASSERT(buffer[0] == RUT)
    // if enable, it must be 0 in bit 0.
    MURASAKI_ASSERT(buffer[1] == 0x7)

    // ************** Disable to enable, bit 1
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    buffer[0] = 0x07;   // Set value to be read.
    i2c_stub_->writeRxBuffer(buffer, 1);

    si5351_->EnableOutput(1);   // enable;

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    // must be 1byte transmission to specify the register address.
    MURASAKI_ASSERT(transffered_len == 1)
    // must be OE register address.
    MURASAKI_ASSERT(buffer[0] == RUT)

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    // must be 1byte transmission to specify the register address.
    MURASAKI_ASSERT(transffered_len == 2)
    // must be OE register address.
    MURASAKI_ASSERT(buffer[0] == RUT)
    // if enable, it must be 1 in bit 0.
    MURASAKI_ASSERT(buffer[1] == 0x05)

    // ************** Enable to disable, bit 2
    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    buffer[0] = 0x00;   // Set value to be read.
    i2c_stub_->writeRxBuffer(buffer, 1);

    si5351_->EnableOutput(2, false);   // enable;

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    // must be 1byte transmission to specify the register address.
    MURASAKI_ASSERT(transffered_len == 1)
    // must be OE register address.
    MURASAKI_ASSERT(buffer[0] == RUT)

    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    // must be 1byte transmission to specify the register address.
    MURASAKI_ASSERT(transffered_len == 2)
    // must be OE register address.
    MURASAKI_ASSERT(buffer[0] == RUT)
    // if enable, it must be 2 in bit 0.
    MURASAKI_ASSERT(buffer[1] == 0x04)

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done.")
}

void TestSi5351::TestSi5351ConfigSeek(int freq_step) {
    uint32_t stage1_a, stage1_b, stage1_c;
    uint32_t stage2_a, stage2_b, stage2_c;
    uint32_t div_by_4;
    uint32_t r;
    bool error = false;
    // Xtal frequency test range. Note that Si5351A allows only Xtal and its 25 and 27MHz
    int xfreq_range[] = {
            25000000,  // 25MHz Si5351 Xtal frequency
            27000000,  // 27MHz Si5351 Xtal frequency
            10000000,  // 10MHz
            15000000,  // 15MHz
            20000000,  // 20MHz
            30000000,  // 30MHz
            35000000,  // 35MHz
            40000000 };  // 40MHz

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start.")

    // Massive test for the basic coefficient calculation.
    // From input frequency 10Mhz to 40MHz by 5MHz step, and 27MHz
    for (const auto &xfreq : xfreq_range)
    {
        MURASAKI_SYSLOG(this, kfaPll, kseNotice, "xfreq     %dHz ", xfreq)

        // test on the given range.
        // Be careful, the ENDFREQ must be tested/
        for (int frequency = STARTFREQ; frequency <= ENDFREQ; frequency += freq_step) {

            // Configure PLL by given frequency.
            si5351_->Si5351ConfigSeek(xfreq, frequency, stage1_a, stage1_b, stage1_c, stage2_a, stage2_b, stage2_c, div_by_4, r);

            // Calculate the synthesized frequency
            double output, fvco;
            // To reduce the round error, original a + b/c is transformed.
            fvco = xfreq * (stage1_a * double(stage1_c) + stage1_b) / stage1_c;

            // VCO frequency must be between 900MHz and 600MHz
            // To allow the error, upper boundary is +1Hz.
            if ((fvco > 900000001) or (600000000 > fvco))
                    {
                MURASAKI_SYSLOG(this, kfaPll, kseError, "VCO error fvco = %d ", int(fvco))
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
                        MURASAKI_SYSLOG(this, kfaPll, kseError, "Divider error stage2_a = %d ", stage2_a)
                        error = true;
                }

            }
            else {
                debugger->Printf("logic error \n");
                error = true;
            }

            // The mantissa of the double is 15digit. Accept 2digit error.
            int delta = frequency - output;
            delta = (delta > 0) ? delta : -delta;
            if (delta > double(frequency) * 1e-13) {
                MURASAKI_SYSLOG(this, kfaPll, kseError, "Error at %dHz, difference is %dHz ", frequency, int(output - frequency));
                error = true;
            }

            if (error) {

                MURASAKI_SYSLOG(this, kfaPll, kseError, "frequency %d ", frequency);
                MURASAKI_SYSLOG(this, kfaPll, kseError, "stage1_a %d  ", stage1_a);
                MURASAKI_SYSLOG(this, kfaPll, kseError, "stage1_b %d  ", stage1_b);
                MURASAKI_SYSLOG(this, kfaPll, kseError, "stage1_c %d  ", stage1_c);
                MURASAKI_SYSLOG(this, kfaPll, kseError, "stage2_a %d  ", stage2_a);
                MURASAKI_SYSLOG(this, kfaPll, kseError, "stage2_b %d  ", stage2_b);
                MURASAKI_SYSLOG(this, kfaPll, kseError, "stage2_c %d  ", stage2_c);
                MURASAKI_SYSLOG(this, kfaPll, kseError, "div by 4  %d ", div_by_4);
                MURASAKI_SYSLOG(this, kfaPll, kseError, "r         %d ", r);
                MURASAKI_ASSERT(false);

            }
        }
    }   // end of for.
    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done.")

}

void TestSi5351::TestPackRegister() {

    // Register construction test for the PLL coefficient.
    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start.")

    uint8_t registers[8];
    for (int i = 8; i < 2048; i += 100)  // integer part
        for (int k = 1; k < (128 * 1024); k += 1000)  // denominator part
            for (int j = 0; j < k; j += 1000)  // numerator part
                for (int l = 1; l < 256; l <<= 1)
                        {
                    si5351_->PackRegister(i, j, k, 0, l, registers);
                    int p = P1(i, j, k);
                    int pp = PACKED_P1(registers);
                    if (p != pp) {
                        murasaki::debugger->Printf("j,j,k : %d, %d, %d", i, j, k);
                        MURASAKI_ASSERT(false)
                    }
                    p = P2(i, j, k);
                    pp = PACKED_P2(registers);
                    if (p != pp) {
                        murasaki::debugger->Printf("j,j,k : %d, %d, %d", i, j, k);
                        MURASAKI_ASSERT(false)
                    }
                    p = P3(i, j, k);
                    pp = PACKED_P3(registers);
                    if (p != pp) {
                        murasaki::debugger->Printf("j,j,k : %d, %d, %d", i, j, k);
                        MURASAKI_ASSERT(false)
                    }
                }

    si5351_->PackRegister(1024, 1023, 1024, 3, 4, registers);
    MURASAKI_ASSERT(PACKED_P1(registers) == 0)
    MURASAKI_ASSERT(PACKED_P2(registers) == 0)
    MURASAKI_ASSERT(PACKED_P3(registers) == 1)
    MURASAKI_ASSERT(PACKED_DIVBY4(registers) == 3)

// Now, testing the basic functions.
    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done.")

}

void TestSi5351::TestResetPLL()
{
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
// Si5351 : Register 177
    const int RUT = 177;   // RESET register

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start.")

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

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done.")

}

void TestSi5351::TestSetPhaseOffset()
{
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    const int OFST_REG = 165;   // phase offset register

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start. ")

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

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done.")

}

void TestSi5351::TestSetClockConfig()
{
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
// Si5351 : Register 16
    const int CTRL_REG = 16;   // control register

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start. ")

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

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done. ")
}

void TestSi5351::TestGetClockConfig()
{
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
// Si5351 : Register 16
    const int CTRL_REG = 16;   // control register

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start. ")

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

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done. ")
}

void TestSi5351::TestSi5351ClockControl()
{
    murasaki::Si5351ClockControl clockConfig;

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start. ")

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

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done. ")

}

void TestSi5351::TestSetFrequency()
{
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    const int CLKCTRL = 16;   // RESET register
    const int PLLA = 26;   // RESET register
    const int MSDIV0 = 42;   // RESET register

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start.")

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
    MURASAKI_ASSERT((((buffer[3] & 0x03) << 16) + (buffer[4] << 8) + buffer[5]) == 2590);    // P1
    MURASAKI_ASSERT((((buffer[6] & 0x03) << 16) + (buffer[7] << 8) + buffer[8]) == 66);    // P2
    MURASAKI_ASSERT((((buffer[6] & 0xF0) << 12) + (buffer[1] << 8) + buffer[2]) == 625);    // P3
    MURASAKI_ASSERT((buffer[3] & 0x0C) >> 2 == 0);    // PLL DIV_BY_4
    MURASAKI_ASSERT((buffer[3] & 0x70) >> 4 == 0);    // PLL RDIV

// check the DIVIDER register configuration
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 9)
    MURASAKI_ASSERT(buffer[0] == MSDIV0)
    MURASAKI_ASSERT((((buffer[3] & 0x03) << 16) + (buffer[4] << 8) + buffer[5]) == 1024);    // MS INTEGER part
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
    MURASAKI_ASSERT((((buffer[3] & 0x03) << 16) + (buffer[4] << 8) + buffer[5]) == 2790);    // PLL INTEGER part
    MURASAKI_ASSERT((((buffer[6] & 0x03) << 16) + (buffer[7] << 8) + buffer[8]) == 226);    // PLL NUM part
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

// Check PLL reset
    const int PLL_RESET_REG = 177;   // RESET register
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == PLL_RESET_REG)
    MURASAKI_ASSERT(buffer[1] == 1 << 5)

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done.")
}

void TestSi5351::TestSetQuadratureFrequency()
{
    uint8_t buffer[SI5351_TEST_BUFFER_LEN];
    unsigned int transffered_len;
    const int CLKCTRL = 16;   // RESET register
    const int PLLA = 26;   // RESET register
    const int MSDIV0 = 42;   // RESET register
    const int OFST_REG = 165;   // phase offset register

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test start.")

    i2c_stub_->clearRxBuffer();
    i2c_stub_->clearTxBuffer();

    buffer[0] = 0xFF;   // Set test data as register for CLK_CTRL register.
    i2c_stub_->writeRxBuffer(buffer, 1);    // for I
    i2c_stub_->writeRxBuffer(buffer, 1);    // for Q

// check result.

    si5351_->SetQuadratureFrequency(    //@formatter:off
                          murasaki::ks5351PllB,     // PLL A
                          1,                        // output port 1
                          2,                        // output port 2
                          50490000                      // 50.490 MHz
                          );            //@formatter:on

            // check the PLL register configuration
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 9)
    MURASAKI_ASSERT(buffer[0] == PLLA + 8)
    MURASAKI_ASSERT((((buffer[3] & 0x03) << 16) + (buffer[4] << 8) + buffer[5]) == 2590);    // PLL INTEGER part
    MURASAKI_ASSERT((((buffer[6] & 0x03) << 16) + (buffer[7] << 8) + buffer[8]) == 66);    // PLL NUM part
    MURASAKI_ASSERT((((buffer[6] & 0xF0) << 12) + (buffer[1] << 8) + buffer[2]) == 625);    // PLL DENOM part
    MURASAKI_ASSERT((buffer[3] & 0x0C) >> 2 == 0);    // PLL DIV_BY_4
    MURASAKI_ASSERT((buffer[3] & 0x70) >> 4 == 0);    // PLL RDIV

// check the DIVIDER I register configuration
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 9)
    MURASAKI_ASSERT(buffer[0] == MSDIV0 + 8)
    MURASAKI_ASSERT((((buffer[3] & 0x03) << 16) + (buffer[4] << 8) + buffer[5]) == 1024);    // MS INTEGER part
    MURASAKI_ASSERT((((buffer[6] & 0x03) << 16) + (buffer[7] << 8) + buffer[8]) == 0);    // MS NUM part
    MURASAKI_ASSERT((((buffer[6] & 0xF0) << 12) + (buffer[1] << 8) + buffer[2]) == 1);    // MS DENOM part
    MURASAKI_ASSERT((buffer[3] & 0x0C) >> 2 == 0);    // MS DIV_BY_4
    MURASAKI_ASSERT((buffer[3] & 0x70) >> 4 == 0);    // MS RDIV

// check the DIVIDER Q register configuration
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 9)
    MURASAKI_ASSERT(buffer[0] == MSDIV0 + 8 * 2)
    MURASAKI_ASSERT((((buffer[3] & 0x03) << 16) + (buffer[4] << 8) + buffer[5]) == 1024);    // MS INTEGER part
    MURASAKI_ASSERT((((buffer[6] & 0x03) << 16) + (buffer[7] << 8) + buffer[8]) == 0);    // MS NUM part
    MURASAKI_ASSERT((((buffer[6] & 0xF0) << 12) + (buffer[1] << 8) + buffer[2]) == 1);    // MS DENOM part
    MURASAKI_ASSERT((buffer[3] & 0x0C) >> 2 == 0);    // MS DIV_BY_4
    MURASAKI_ASSERT((buffer[3] & 0x70) >> 4 == 0);    // MS RDIV

// check the CLKCTRL address set
// I
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == CLKCTRL + 1)
// check the CLKCTRL read-modify-write
// I
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == CLKCTRL + 1)
    MURASAKI_ASSERT(!(buffer[1] & 0xC0));    // power down disable, integer mode disable.

// check the CLKCTRL address set
// Q
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 1)
    MURASAKI_ASSERT(buffer[0] == CLKCTRL + 2)
// check the CLKCTRL read-modify-write
// Q
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == CLKCTRL + 2)
    MURASAKI_ASSERT(!(buffer[1] & 0xC0));    // power down disable, integer mode disable.

// PHASE CONTROL for I
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == OFST_REG + 1)
    MURASAKI_ASSERT(buffer[1] == 0);    // must be zero

// PHASE CONTROL for Q
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == OFST_REG + 2)
    MURASAKI_ASSERT(buffer[1] == 12);    // = MS INTEGER PART

// Check PLL reset
    const int PLL_RESET_REG = 177;   // RESET register
    i2c_stub_->readTxBuffer(buffer, SI5351_TEST_BUFFER_LEN, &transffered_len);
    MURASAKI_ASSERT(transffered_len == 2)
    MURASAKI_ASSERT(buffer[0] == PLL_RESET_REG)
    MURASAKI_ASSERT(buffer[1] == 1 << 7)

    MURASAKI_SYSLOG(this, kfaPll, kseNotice, "Test done.")
}

void TestSi5351Driver(int freq_step) {

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "Test start : %dHz step.", freq_step)

// Create an Device Under Test.
// @formatter:off
    TestSi5351 *dut = new TestSi5351(
                                     new murasaki::I2cMasterStub(
                                             SI5351_TEST_BUFFER_NUM,  // Number of the TX RX test buffers.
                                             SI5351_TEST_BUFFER_LEN,  // LENGTH of the TX RX test buffers.
                                             TESTI2CADDRESS,  // DUT I2C address
                                             true)  // Address filtering is on
                                             );
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               // @formatter:on

    dut->TestSi5351ClockControl();
    dut->TestIsInitializing();
    dut->TestIsLossOfLock();
    dut->TestIsLossOfXtal();
    dut->TestIsLossOfClkin();
    dut->TestEnableOutput();
    dut->TestResetPLL();
    dut->TestSetClockConfig();
    dut->TestGetClockConfig();
    dut->TestSetPhaseOffset();
    dut->TestSetFrequency();
    dut->TestSetQuadratureFrequency();
    dut->TestPackRegister();
    dut->TestSi5351ConfigSeek(freq_step);

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "Test done.")
}

} /* namespace murasaki */
