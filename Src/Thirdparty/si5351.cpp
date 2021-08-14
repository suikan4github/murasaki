/*
 * si5351.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: takemasa
 */

#include "Thirdparty/si5351.hpp"

#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

// Device registers
#define SI5351_STARTUS_REG 0
#define SI5351_PLL_RESET_REG 177
#define SI5351_CLK0_INITIAL_PHASE_OFFSET_REG 165
#define SI5351_CLK0_CONTROL_REG 16

#define SI5351_SYS_INIT_BIT 7
#define SI5351_LOL_B_BIT 6
#define SI5351_LOL_A_BIT 5
#define SI5351_LOCLKIN_BIT 4
#define SI5351_LOXTAL_BIT 3

#define PLL_SYSLOG(fmt, ...)    MURASAKI_SYSLOG( this,  kfaPll, kseDebug, fmt, ##__VA_ARGS__)

#define R_DIV_MUSB_BE_1_TO_128_AS_POWER_OF_TWO 256

namespace murasaki {

Si5351::Si5351(
               murasaki::I2cMasterStrategy *controller,
               unsigned int addrs,
               uint32_t xtal_freq)
        :
        ext_freq_(xtal_freq),
        i2c_(controller),
        addrs_(addrs)
{
    // TODO Auto-generated constructor stub

}

Si5351::~Si5351()
{
    // TODO Auto-generated destructor stub
}

Si5351Status Si5351::Si5351ConfigSeek(
                                      const uint32_t xtal_freq,
                                      const uint32_t output_freq,
                                      uint32_t &stage1_int,
                                      uint32_t &stage1_num,
                                      uint32_t &stage1_denom,
                                      uint32_t &stage2_int,
                                      uint32_t &stage2_num,
                                      uint32_t &stage2_c,
                                      uint32_t &div_by_4,
                                      uint32_t &r_div)
                                      {
    // Because the div_by_4 mode is tricky, keep second stage divider value here.
    uint32_t second_stage_divider;

    MURASAKI_ASSERT(output_freq > 2);         // must be higher than 2Hz.
    MURASAKI_ASSERT(200000000 > output_freq);  // must be lower than or equal to 200MHz

    PLL_SYSLOG("Xtal Frequency is %d Hz", xtal_freq);
    PLL_SYSLOG("Output Frequency is %d Hz", output_freq);

    // Si5351 requires to set the div-by-4 mode if the output is higher than 150MHz
    if (output_freq > 150000000)  // If output freq is > 150MHz
            {
        PLL_SYSLOG("Frequency is higher than 150MHz");

        PLL_SYSLOG("Set the stage 2 to divid by 4 mode")
        // enforce divide by 4 mode.
        // Note that followings are required by Si5351 data sheet.
        // In case of divide by 4 mode, the second stage have to be integer mode.
        div_by_4 = 3;  // requed by Si5351 data sheet
        stage2_int = 0;
        stage2_num = 0;
        stage2_c = 1;

        // For output freq > 150MHz, r_div is set to 1;
        r_div = 1;
        second_stage_divider = 4;
    }
    else if (output_freq > 100000000)
            {
        PLL_SYSLOG("Frequency is higher than 100MHz");

        PLL_SYSLOG("Set the stage 2 to divid by 6 mode")

        // divide by 6
        div_by_4 = 0;  // requed by Si5351 data sheet
        stage2_int = 6;
        stage2_num = 0;
        stage2_c = 1;

        // r_div is set to 1;
        r_div = 1;
        second_stage_divider = 6;
    }
    else if (output_freq > 75000000)
            {
        PLL_SYSLOG("Frequency is higher than 75MHz");

        PLL_SYSLOG("Set the stage 2 to divid by 8 mode")

        // divide by 6
        div_by_4 = 0;  // requed by Si5351 data sheet
        stage2_int = 8;
        stage2_num = 0;
        stage2_c = 1;

        // r_div is set to 1;
        r_div = 1;
        second_stage_divider = 8;
    }
    else  // output freq <= 75MHz.
    {
        PLL_SYSLOG("Frequency is lower than or equal to 75MHz");

        // Initial value of the output divider
        r_div = 1;

        // Seek the appropriate multisynth divider.

        // At first, roughly calculate the dividing value. 600MHz is the lowest VCO freq.
        second_stage_divider = 600000000 / output_freq;
        // round up. Now, second_stage_divider * r_div * output_freq is bigger than 600MHz.
        second_stage_divider++;

        // The second stage divider have to be lower than or equal to 2048

        while (second_stage_divider > 2048)
        {
            r_div *= 2;
            // At first, roughly calculate the dividing value. 600MHz is the lowest VCO freq.
            second_stage_divider = 600000000 / (r_div * output_freq);
            // round up. Now, second_stage_divider * r_div * output_freq is bigger than 600MHz.
            second_stage_divider++;
        }

        // divide by 6
        div_by_4 = 0;  // requed by Si5351 data sheet
        stage2_int = second_stage_divider;
        stage2_num = 0;
        stage2_c = 1;
    }

    PLL_SYSLOG("r_div = %d", r_div);
    PLL_SYSLOG("stage2_int = %d", stage2_int);
    PLL_SYSLOG("stage2_num = %d", stage2_num);
    PLL_SYSLOG("stage2_c = %d", stage2_c);

    uint32_t vco_freq = output_freq * second_stage_divider * r_div;
    PLL_SYSLOG("VCO frequency = %d", vco_freq);

    // fvco = fxtal * a + mod
    //      = fxtal * ( a + mod / fxtal )
    //      = fxtal * ( a + b / c)

    stage1_int = vco_freq / xtal_freq;
    stage1_num = vco_freq % xtal_freq;
    stage1_denom = xtal_freq;

    // Do the Euclidean Algorithm to reduce the b and c
    int gcd, remainder;
    int x = stage1_num;
    int y = stage1_denom;

    while (1)
    {
        remainder = x % y;

        if (remainder == 0)
                {
            gcd = y;
            break;
        }
        else
        {
            x = y;
            y = remainder;
        }
    }

    stage1_num /= gcd;
    stage1_denom /= gcd;

    PLL_SYSLOG("stage1_a = %d", stage1_int);
    PLL_SYSLOG("stage1_num = %d", stage1_num);
    PLL_SYSLOG("stage1_denom = %d", stage1_denom);

    return ks5351Ok;
}

void Si5351::setRegister(unsigned int reg_addr, uint8_t value)
                         {
    murasaki::I2cStatus status;
    uint8_t regs[2];
    unsigned int transfered_count;

    regs[0] = reg_addr;     // address of regsiter
    regs[1] = value;        // value to set
    status = i2c_->Transmit(addrs_,         // Address of PLL Si5351.
            regs,      // Register # .
            2,                  // Send 1 byte to set the register to read, and 1 another as data.
            &transfered_count,  // Dummy.
            10);                // Wait 10 mSec.

    MURASAKI_ASSERT(status == murasaki::ki2csOK)

}

void Si5351::SetPhaseOffset(unsigned int ch, uint8_t offset) {
    // Channel must be 0, 1 or 2.
    MURASAKI_ASSERT(ch < 3)
    // Register address of the phase offset to change.
    uint8_t offset_reg = SI5351_CLK0_INITIAL_PHASE_OFFSET_REG + ch;

    // offset must be 7 bits.
    MURASAKI_ASSERT(offset < 128)

    // Set the desired offset to the right register.
    setRegister(offset_reg, offset);

}

void Si5351::SetClockConfig(unsigned int channel, union Si5351ClockControl clockConfig) {
    // Channel must be 0, 1 or 2.
    MURASAKI_ASSERT(channel < 3)

    // Register address of the integer mode to change.
    uint8_t ctrl_reg = SI5351_CLK0_CONTROL_REG + channel;

    // Set the Clock control regsiter.
    setRegister(ctrl_reg, clockConfig.value);

}

union Si5351ClockControl Si5351::GetClockConfig(unsigned int channel) {
    // Channel must be 0, 1 or 2.
    MURASAKI_ASSERT(channel < 3)

    // Register address of the integer mode to change.
    uint8_t ctrl_reg = SI5351_CLK0_CONTROL_REG + channel;

    // Get CLK# Control register value.
    Si5351ClockControl retval;
    retval.value = getRegister(ctrl_reg);

    return retval;
}

void Si5351::PackRegister(
                          const uint32_t integer,
                          const uint32_t numerator,
                          const uint32_t denominator,
                          const uint32_t div_by_4,
                          const uint32_t r_div,
                          uint8_t reg[8])
                          {
// integer part must be no bigger than 18bits.
    MURASAKI_ASSERT((integer & 0xFFFC0000) == 0)
// Numerator and denominator part must be no bigger than 20bits.
    MURASAKI_ASSERT((numerator & 0xFFF00000) == 0)
    MURASAKI_ASSERT((denominator & 0xFFF00000) == 0)
// Right value of div by four must be 0 or 3
    MURASAKI_ASSERT((div_by_4 == 0) || (div_by_4 == 3))

    reg[0] = (denominator >> 8) & 0x00FF;    // extract [15:8]
    reg[1] = (denominator) & 0x00FF;    // extract [7:0]
    reg[3] = (integer >> 8) & 0x00FF;    // extract [15:8]
    reg[4] = (integer) & 0x00FF;    // extract [7:0]
    reg[5] = ((((denominator) >> 16) & 0x0F) << 4) |    // extract [19:16]
            ((numerator >> 16) & 0x0F);    // extract [19:16]
    reg[6] = (numerator >> 8) & 0x00FF;    // extract [15:8]
    reg[7] = (numerator) & 0x00FF;    // extract [7:0]

    reg[2] = 0;

    reg[2] |= (integer >> 16) & 0x03;
    reg[2] |= div_by_4 << 2;

// Calculate the field value for the r_div.
// The r_div is true divider value. That is the value of the 2^x.
// The field value mast be this "x". Following code seeks the x for r_div.
    uint32_t field = R_DIV_MUSB_BE_1_TO_128_AS_POWER_OF_TWO;    // 256 is an error value.

    unsigned int value = 1;
    for (unsigned int i = 0; i < 8; i++) {
        if (value == r_div) {
            field = i;
            break;
        }
        else
            value <<= 1;
    }

    MURASAKI_ASSERT(field != R_DIV_MUSB_BE_1_TO_128_AS_POWER_OF_TWO)
// Make sure the field value is not error.
    reg[2] |= field << 4;
}

uint8_t
Si5351::getRegister(unsigned int reg_addr)
                    {
    murasaki::I2cStatus status;
    uint8_t reg, register_num;
    unsigned int transfered_count;

    register_num = reg_addr;
    status = i2c_->Transmit(addrs_,         // Address of PLL Si5351.
            &register_num,         // Register # .
            1,         // Send 1 byte to set the register to read
            &transfered_count,         // Dummy.
            10);         // Wait 10 mSec.

    MURASAKI_ASSERT(status == murasaki::ki2csOK)

    status = i2c_->Receive(addrs_,         // Address of PLL Si5351.
            &reg,         // received Register  .
            1,         // Receive 1 byte.
            &transfered_count,         // Dummy.
            10);         // Wait 10 mSec.

    MURASAKI_ASSERT(status == murasaki::ki2csOK)

    return reg;
}

// Return true if still initializing.
bool Si5351::IsInitializing()
{
    uint8_t reg = getRegister(SI5351_STARTUS_REG);       // Get Device Status.
    return (((1 << SI5351_SYS_INIT_BIT) & reg) != 0);       // is SI5351_LOL_A_BIT  H?
}

bool Si5351::IsLossOfLockA()
{
    uint8_t reg = getRegister(SI5351_STARTUS_REG);       // Get Device Status.
    return (((1 << SI5351_LOL_A_BIT) & reg) != 0);       // is SI5351_LOL_A_BIT H?
}

bool Si5351::IsLossOfLockB()
{
    uint8_t reg = getRegister(SI5351_STARTUS_REG);       // Get Device Status.
    return (((1 << SI5351_LOL_B_BIT) & reg) != 0);       // is SI5351_LOL_B_BIT H?
}

bool Si5351::IsLossOfClkin()
{
    uint8_t reg = getRegister(SI5351_STARTUS_REG);       // Get Device Status.
    return (((1 << SI5351_LOCLKIN_BIT) & reg) != 0);       // is SI5351_LOCLKIN_BIT H?
}

bool Si5351::IsLossOfXtal()
{
    uint8_t reg = getRegister(SI5351_STARTUS_REG);       // Get Device Status.
    return (((1 << SI5351_LOXTAL_BIT) & reg) != 0);       // is SI5351_LOXTAL_BIT H?
}

void Si5351::ResetPLL(murasaki::Si5351Pll pll)
                      {
    switch (pll) {
        case murasaki::ks5351PllA:
            setRegister(SI5351_PLL_RESET_REG, 1 << 5);  // PLLB_RST
            break;
        case murasaki::ks5351PllB:
            setRegister(SI5351_PLL_RESET_REG, 1 << 7);  // PLLB_RST
            break;
        default:
            // illegal value
            MURASAKI_ASSERT(false)
    }
}

} /* namespace murasaki */
