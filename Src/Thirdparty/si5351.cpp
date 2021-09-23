/*
 * si5351.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: takemasa
 */

#include "Thirdparty/si5351.hpp"

#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

#include <cstring>

// Device registers
#define SI5351_STARTUS_REG 0
#define SI5351_PLL_RESET_REG 177
#define SI5351_CLK0_INITIAL_PHASE_OFFSET_REG 165
#define SI5351_CLK0_CONTROL_REG 16
#define SI5351_MULTISYNTH_NA_REG 26
#define SI5351_MULTISYNTH_0_REG 42

#define SI5351_MULTISYNTH_REG_LEN 8

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
                                      uint32_t &stage2_denom,
                                      uint32_t &div_by_4,
                                      uint32_t &r_div)
                                      {
    // Because the div_by_4 mode is tricky, keep second stage divider value here.
    uint32_t second_stage_divider;

    MURASAKI_ASSERT(output_freq >= 2500);         // must be higher than 2.5kHz.
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
        stage2_int = 0;     // This will be ignored.
        stage2_num = 0;     // This will be ignored.
        stage2_denom = 0;   // This will be ignored.

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
        stage2_denom = 1;

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
        stage2_denom = 1;

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
        MURASAKI_ASSERT(256 > r_div);  // must be lower than 256.

        // setup the divider value
        div_by_4 = 0;  // requed by Si5351 data sheet
        stage2_int = second_stage_divider;
        stage2_num = 0;
        stage2_denom = 1;
    }

    PLL_SYSLOG("r_div = %d", r_div);
    PLL_SYSLOG("stage2_int = %d", stage2_int);
    PLL_SYSLOG("stage2_num = %d", stage2_num);
    PLL_SYSLOG("stage2_denom = %d", stage2_denom);

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

void Si5351::SetPhaseOffset(unsigned int ch, uint8_t offset) {
    // Channel must be 0, 1 or 2.
    MURASAKI_ASSERT(ch < 3)
    // Register address of the phase offset to change.
    uint8_t offset_reg = SI5351_CLK0_INITIAL_PHASE_OFFSET_REG + ch;

    // offset must be 7 bits.
    MURASAKI_ASSERT(offset < 128)

    // Set the desired offset to the right register.
    SetRegister(offset_reg, offset);

}

void Si5351::SetClockConfig(unsigned int channel, union Si5351ClockControl clockConfig) {
    // Channel must be 0, 1 or 2.
    MURASAKI_ASSERT(channel < 3)

    // Register address of the integer mode to change.
    uint8_t ctrl_reg = SI5351_CLK0_CONTROL_REG + channel;

    // Set the Clock control regsiter.
    SetRegister(ctrl_reg, clockConfig.value);

}

union Si5351ClockControl Si5351::GetClockConfig(unsigned int channel) {
    // Channel must be 0, 1 or 2.
    MURASAKI_ASSERT(channel < 3)

    // Register address of the integer mode to change.
    uint8_t ctrl_reg = SI5351_CLK0_CONTROL_REG + channel;

    // Get CLK# Control register value.
    Si5351ClockControl retval;
    retval.value = GetRegister(ctrl_reg);

    return retval;
}

Si5351Status Si5351::SetFrequency(murasaki::Si5351Pll pll, unsigned int div_ch, uint32_t frequency) {
    // div must be 0, 1 or 2.
    MURASAKI_ASSERT(div_ch < 3)
    MURASAKI_ASSERT(frequency > 2);         // must be higher than 2Hz.
    MURASAKI_ASSERT(200000000 > frequency);  // must be lower than or equal to 200MHz

    // Parameter receiving variables.
    uint32_t stage1_int;
    uint32_t stage1_num;
    uint32_t stage1_denom;
    uint32_t stage2_int;
    uint32_t stage2_num;
    uint32_t stage2_denom;
    uint32_t div_by_4;
    uint32_t r_div;

    // Seek the best parameter combination for the given external frquency and output frequency.
    Si5351Status status = Si5351ConfigSeek(  //@formatter:off
                                            ext_freq_,   // external clock source frequency
                                            frequency,   // output frequency
                                            stage1_int,
                                            stage1_num,
                                            stage1_denom,
                                            stage2_int,
                                            stage2_num,
                                            stage2_denom,
                                            div_by_4,
                                            r_div);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        // @formatter:on
    MURASAKI_ASSERT(status == murasaki::ks5351Ok)

    // Construct the register field for PLL
    uint8_t pll_reg[8];
    Si5351::PackRegister(
                         stage1_int,
                         stage1_num,
                         stage1_denom,
                         0,  // div_by_4 must be zero for PLL
                         0,  // r_div must be zero for PLL
                         pll_reg);

    // calculate the offset to the PLL multisynth parameters.
    unsigned int offset = (pll == murasaki::ks5351PllA) ? 0 : SI5351_MULTISYNTH_REG_LEN;

    // set PLL multithynth parameters.
    SetRegister(SI5351_MULTISYNTH_NA_REG + offset,
                pll_reg,
                SI5351_MULTISYNTH_REG_LEN);

    // Construct the register filed for the post PLL divider
    uint8_t div_reg[8];
    Si5351::PackRegister(
                         stage2_int,
                         stage2_num,
                         stage2_denom,
                         div_by_4,
                         r_div,
                         div_reg);

    // set the post PLL multithynth divider parameters.
    SetRegister(SI5351_MULTISYNTH_0_REG + SI5351_MULTISYNTH_REG_LEN * div_ch,
                div_reg,
                SI5351_MULTISYNTH_REG_LEN);

    // Configure integer mode and disable powern down of the output.
    murasaki::Si5351ClockControl clockConfig = GetClockConfig(div_ch);
    clockConfig.fields.clk_pdn = false;
    clockConfig.fields.ms_int = (stage2_num == 0);       // set integer mode.
    SetClockConfig(div_ch, clockConfig);

    // Reset the PLL to make phase offset correct. This is required by specification.
    ResetPLL(pll);
}

Si5351Status Si5351::SetQuadratureFrequency(murasaki::Si5351Pll pll, unsigned int divI_ch, unsigned int divQ_ch, uint32_t frequency) {
    // div must be 0, 1 or 2.
    MURASAKI_ASSERT(divI_ch < 3)
    MURASAKI_ASSERT(divQ_ch < 3)
    MURASAKI_ASSERT(frequency >= 4725000);         // must be higher than 4.725MHz.
    MURASAKI_ASSERT(150000000 >= frequency);  // must be lower than or equal to 150MHz

    // Parameter receiving variables.
    uint32_t stage1_int;
    uint32_t stage1_num;
    uint32_t stage1_denom;
    uint32_t stage2_int;
    uint32_t stage2_num;
    uint32_t stage2_denom;
    uint32_t div_by_4;
    uint32_t r_div;

    // Seek the best parameter combination for the given external frquency and output frequency.
    Si5351Status status = Si5351ConfigSeek(  //@formatter:off
                                            ext_freq_,   // external clock source frequency
                                            frequency,   // output frequency
                                            stage1_int,
                                            stage1_num,
                                            stage1_denom,
                                            stage2_int,
                                            stage2_num,
                                            stage2_denom,
                                            div_by_4,
                                            r_div);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        // @formatter:on
    MURASAKI_ASSERT(status == murasaki::ks5351Ok)
    MURASAKI_ASSERT(127 >= stage2_int)
    MURASAKI_ASSERT(div_by_4 != 3);  // must not be in div by 4 mode.

    // Construct the register field for PLL
    uint8_t pll_reg[8];
    Si5351::PackRegister(
                         stage1_int,
                         stage1_num,
                         stage1_denom,
                         0,  // div_by_4 must be zero for PLL
                         0,  // r_div must be zero for PLL
                         pll_reg);

    // calculate the offset to the PLL multisynth parameters.
    unsigned int offset = (pll == murasaki::ks5351PllA) ? 0 : SI5351_MULTISYNTH_REG_LEN;

    // set PLL multithynth parameters.
    SetRegister(SI5351_MULTISYNTH_NA_REG + offset,
                pll_reg,
                SI5351_MULTISYNTH_REG_LEN);

    // Construct the register filed for the post PLL divider
    uint8_t div_reg[8];
    Si5351::PackRegister(
                         stage2_int,
                         stage2_num,
                         stage2_denom,
                         div_by_4,
                         r_div,
                         div_reg);

    // set the post PLL multithynth divider I parameters.
    SetRegister(SI5351_MULTISYNTH_0_REG + SI5351_MULTISYNTH_REG_LEN * divI_ch,
                div_reg,
                SI5351_MULTISYNTH_REG_LEN);

    // set the post PLL multithynth divider Q parameters.
    SetRegister(SI5351_MULTISYNTH_0_REG + SI5351_MULTISYNTH_REG_LEN * divQ_ch,
                div_reg,
                SI5351_MULTISYNTH_REG_LEN);

    // Configure fraction mode and disable powern down of the output.
    // Fractional mode is required by specification of phase offset.
    murasaki::Si5351ClockControl clockConfig = GetClockConfig(divI_ch);
    clockConfig.fields.clk_pdn = false;
    clockConfig.fields.clk_inv = false;
    clockConfig.fields.clk_src = murasaki::ks5351osNativeDivider;
    clockConfig.fields.ms_int = false;       // set fractional mode.
    clockConfig.fields.ms_src = pll;
    SetClockConfig(divI_ch, clockConfig);

    clockConfig = GetClockConfig(divQ_ch);
    clockConfig.fields.clk_pdn = false;
    clockConfig.fields.clk_inv = false;
    clockConfig.fields.clk_src = murasaki::ks5351osNativeDivider;
    clockConfig.fields.ms_int = false;       // set fractional mode.
    clockConfig.fields.ms_src = pll;
    SetClockConfig(divQ_ch, clockConfig);

    // Configure delay.
    // The LSB of the phase offset register is pi/2 of the PLL VCO.
    // On the other hand, the MS divider divide it by stage2_int value.
    // Thus, setting stage2_int makes pi/2 delay of output.
    SetPhaseOffset(divI_ch, 0);
    SetPhaseOffset(divQ_ch, stage2_int);

    // Reset the PLL to make phase offset correct. This is required by specification.
    ResetPLL(pll);
}

//@formatter:off
inline uint32_t synth_p1(uint32_t a, uint32_t b, uint32_t c) { return 128 * a + (128 * b) / c - 512; }
inline uint32_t synth_p2(uint32_t a, uint32_t b, uint32_t c) { return 128 * b - c*((128 * b) / c); }
inline uint32_t synth_p3(uint32_t a, uint32_t b, uint32_t c) { return c; }

inline uint8_t msynth_reg0(uint32_t p1, uint32_t p2, uint32_t p3) { return (p3 >> 8) & 0xFF; }
inline uint8_t msynth_reg1(uint32_t p1, uint32_t p2, uint32_t p3) { return p3 & 0xFF; }
inline uint8_t msynth_reg2(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t r_div, uint32_t divby4) { return ((r_div & 0x07) << 4) | ((divby4 & 0x03) << 2) | ((p1 >> 16) & 0x03); }
inline uint8_t msynth_reg3(uint32_t p1, uint32_t p2, uint32_t p3) { return (p1 >> 8) & 0xFF; }
inline uint8_t msynth_reg4(uint32_t p1, uint32_t p2, uint32_t p3) { return p1 & 0xFF; }
inline uint8_t msynth_reg5(uint32_t p1, uint32_t p2, uint32_t p3) { return (((p3 >> 16) & 0x0F ) << 4 ) | ((p2 >> 16) & 0x0F); }
inline uint8_t msynth_reg6(uint32_t p1, uint32_t p2, uint32_t p3) { return (p2 >> 8) & 0xFF; }
inline uint8_t msynth_reg7(uint32_t p1, uint32_t p2, uint32_t p3) { return p2 & 0xFF; }
//@formatter:on

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

    uint32_t p1;
    uint32_t p2;
    uint32_t p3;

    if (div_by_4 == 0) {
        // Set up the internal variable. See AN619 of the Si5351
        p1 = synth_p1(integer, numerator, denominator);
        p2 = synth_p2(integer, numerator, denominator);
        p3 = synth_p3(integer, numerator, denominator);
    }
    else    // if div by 4 mode.
    {
        p1 = 0;
        p2 = 0;
        p3 = 1;
    }

// Calculate the field value for the r_div.
// The r_div is true divider value. That is the value of the 2^x.
// The field value mast be this "x". Following code seeks the x for r_div.
    uint32_t rdiv_field = R_DIV_MUSB_BE_1_TO_128_AS_POWER_OF_TWO;    // 256 is an error value.

    switch (r_div)
    {
        case 0:  // r_div 0 is allowed only for the PLL setting.
        case 1:
            rdiv_field = 0;
            break;
        case 2:
            rdiv_field = 1;
            break;
        case 4:
            rdiv_field = 2;
            break;
        case 8:
            rdiv_field = 3;
            break;
        case 16:
            rdiv_field = 4;
            break;
        case 32:
            rdiv_field = 5;
            break;
        case 64:
            rdiv_field = 6;
            break;
        case 128:
            rdiv_field = 7;
            break;
        default:    // illegal case.
            rdiv_field = R_DIV_MUSB_BE_1_TO_128_AS_POWER_OF_TWO;
    }

    // Check wether the r_div is regular value or not.
    MURASAKI_ASSERT(rdiv_field != R_DIV_MUSB_BE_1_TO_128_AS_POWER_OF_TWO)

    // Pack the register for PLL.
    reg[0] = msynth_reg0(p1, p2, p3);
    reg[1] = msynth_reg1(p1, p2, p3);
    reg[2] = msynth_reg2(p1, p2, p3, rdiv_field, div_by_4);
    reg[3] = msynth_reg3(p1, p2, p3);
    reg[4] = msynth_reg4(p1, p2, p3);
    reg[5] = msynth_reg5(p1, p2, p3);
    reg[6] = msynth_reg6(p1, p2, p3);
    reg[7] = msynth_reg7(p1, p2, p3);

}

uint8_t
Si5351::GetRegister(unsigned int reg_addr)
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

void Si5351::SetRegister(unsigned int reg_addr, uint8_t value)
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

void Si5351::SetRegister(unsigned int reg_num, uint8_t *values, uint8_t length) {
    murasaki::I2cStatus status;
    uint8_t buf[9];
    unsigned int transfered_count;

    MURASAKI_ASSERT(length <= 8)

    buf[0] = reg_num;     // address of register
    std::memcpy(            // copy the transmit data to the buffer.
            &buf[1],
            values,
            length);
    status = i2c_->Transmit(addrs_,         // Address of PLL Si5351.
            buf,      // Register # .
            length + 1,                  // Send 1 byte to set the register to read, and subsequent data.
            &transfered_count,  // Dummy.
            10);                // Wait 10 mSec.

    MURASAKI_ASSERT(status == murasaki::ki2csOK)

}

// Return true if still initializing.
bool Si5351::IsInitializing()
{
    uint8_t reg = GetRegister(SI5351_STARTUS_REG);       // Get Device Status.
    return (((1 << SI5351_SYS_INIT_BIT) & reg) != 0);       // is SI5351_LOL_A_BIT  H?
}

bool Si5351::IsLossOfLockA()
{
    uint8_t reg = GetRegister(SI5351_STARTUS_REG);       // Get Device Status.
    return (((1 << SI5351_LOL_A_BIT) & reg) != 0);       // is SI5351_LOL_A_BIT H?
}

bool Si5351::IsLossOfLockB()
{
    uint8_t reg = GetRegister(SI5351_STARTUS_REG);       // Get Device Status.
    return (((1 << SI5351_LOL_B_BIT) & reg) != 0);       // is SI5351_LOL_B_BIT H?
}

bool Si5351::IsLossOfClkin()
{
    uint8_t reg = GetRegister(SI5351_STARTUS_REG);       // Get Device Status.
    return (((1 << SI5351_LOCLKIN_BIT) & reg) != 0);       // is SI5351_LOCLKIN_BIT H?
}

bool Si5351::IsLossOfXtal()
{
    uint8_t reg = GetRegister(SI5351_STARTUS_REG);       // Get Device Status.
    return (((1 << SI5351_LOXTAL_BIT) & reg) != 0);       // is SI5351_LOXTAL_BIT H?
}

void Si5351::ResetPLL(murasaki::Si5351Pll pll)
                      {
    switch (pll) {
        case murasaki::ks5351PllA:
            SetRegister(SI5351_PLL_RESET_REG, 1 << 5);  // PLLB_RST
            break;
        case murasaki::ks5351PllB:
            SetRegister(SI5351_PLL_RESET_REG, 1 << 7);  // PLLB_RST
            break;
        default:
            // illegal value
            MURASAKI_ASSERT(false)
    }
}

} /* namespace murasaki */
