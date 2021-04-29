/*
 * testsi5351.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: takemasa
 */

#include <Thirdparty/i2cmasterstub.hpp>
#include "Thirdparty/si5351.hpp"
#include "murasaki.hpp"

#include <Thirdparty/testsi5351.hpp>

/// Fixed frequency for the Si5351 object test.
#define TESTXTAL 25000000

/// End frequency of the scan test of the Si5351::Si5351ConfigSeek()
#define ENDFREQ 200000000
/// Start frequency of the scan test of the Si5351::Si5351ConfigSeek()
#define STARTFREQ 3

namespace murasaki {

bool TestSi5351(int freq_step)
                {
    bool error = false;

    // Create an test stub peripheral
    murasaki::I2cMasterStub *i2c_stub = new murasaki::I2cMasterStub(5, 40, 1, true);

    // Create an object under test. Address can be any.
    murasaki::Si5351 *si5351 = new murasaki::Si5351(i2c_stub, 1, TESTXTAL);

    uint32_t stage1_a, stage1_b, stage1_c;
    uint32_t stage2_a, stage2_b, stage2_c;
    uint32_t div_by_4;
    uint32_t r;

    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The Si5351ConfigSeek() test starts")

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
//                MURASAKI_ASSERT(false);

            }
        }
    }
    MURASAKI_SYSLOG(nullptr, kfaPll, kseNotice, "The Si5351ConfigSeek() test finished")

    return !error;
}

} /* namespace murasaki */
