/**
 * @file si5351.hpp
 *
 *  Created on: Feb 27, 2021
 *      Author: takemasa
 */

#ifndef THIRDPARTY_SI5351_HPP_
#define THIRDPARTY_SI5351_HPP_

#include <murasaki_defs.hpp>
#include <i2cmasterstrategy.hpp>

namespace murasaki {

/**
 * @ingroup MURASAKI_THIRDPARTY_GROUP
 * @brief Si5351 driver class.
 */
class Si5351 {
 public:
    Si5351(murasaki::I2cMasterStrategy *controller, uint32_t xtal_freq);

    virtual ~Si5351();

 private:
    const uint32_t ext_freq_;
    murasaki::I2cMasterStrategy *const i2c_;

    /**
     * @brief Seek the appropriate configuration of the Si5351.
     * @param xtal_freq [IN] : Xtal frequency in Hz. Must be from 10MHz to 40Mhz.
     * @param output_freq [IN] : Output frequency in Hz. Must be from 2.5kHz 200MHz.
     * @param stage1_int [OUT] : Parameter "a" of the PLL feedback divider. [15, 90]
     * @param stage1_num [OUT] : Parameter "b" of the PLL feedback divider. [0, 1048575]
     * @param stage1_denom [OUT] : Parameter "c" of the PLL feedback divider. [0, 1048575]
     * @param stage2_int [OUT] : Parameter "a" of the stage 2 divider. [8, 2048]
     * @param stage2_num [OUT] : Parameter "b" of the stage 2 divider. [0, 1048575]
     * @param stage2_denom [OUT] : Parameter "c" of the stage 2 divider. [0, 1048575]
     * @param div_by_4 [OUT] : 0b11 : div by 4 mode. 0b00 : div by other mode.
     * @param r_div [OUT] : Output divider
     * @details
     * Calculate an appropriate set of the PLL parameters for Si5351. See AN619 for detail of Si5351.
     *
     * This function calculate the formula only. So, the atuall register
     * configuration have to be compiled from the given parameters.
     *
     * The configuration seek is done for the Multisynth(TM) dividers of the
     * Si5351. In this function, we assume following configuration.
     * @li No prescalor in front of PLL
     * @li The first stage is PLL
     * @li The second stage is Multisynth divider.
     * @li Output Divider.
     *
     * In this configuration, we have two Multisynth Dividers. One is the
     * second stage. And the other is the feedback divider of the PLL.
     *
     * This function seek the appropriate setting based on the following restriction:
     * @li Keep second stage as integer division
     * @li Keep the PLL VCO frequency between 600 to 900MHz.
     *
     *
     * The output frequency is :
     *  Fout = xtal_freq * ( stage1_int + stage1_num/stage1_denom) /( (stage2_int + stage2_num/stage2_denom) * r_div )
     *
     * Also, there are several restriction by silicon.
     * @li If the output is below 500kHz, use r_div divider 1, 2, 4, 8, ... 128.
     * @li If the output is over 150MHz, stage 2 is a=0, b=0, c=0, div_by_4=0b11
     */
    Si5351Status Si5351ConfigSeek(
                                  const uint32_t xtal_freq,
                                  const uint32_t output_freq,
                                  uint32_t &stage1_int,
                                  uint32_t &stage1_num,
                                  uint32_t &stage1_denom,
                                  uint32_t &stage2_int,
                                  uint32_t &stage2_num,
                                  uint32_t &stage2_denom,
                                  uint32_t &div_by_4,
                                  uint32_t &r_div);

    /**
     * @brief Pack the given data for the Si5351 dividers.
     * @param integer [IN] The integer part of the divider. See following details.
     * @param numerator [IN] The numerator part of the divider. See following details.
     * @param denominator [IN] The denominator part of the divider. See following details.
     * @param div_by_4 [IN] 0 for normal mode, 3 for div by 4 mode. Must be 0 for the PLL multiplier.
     * @param r_div [IN] The output divider value. Must be zero for the PLL multiplier.
     * @param reg [OUT] The packed register.
     * @details
     * Data packing for the Si5351 divider. The data will be packed as following :
     * @li reg[0] = denominator[15:8]
     * @li reg[1] = denominator[7:0]
     * @li reg[2] = formated(r_div)[2:0] << 4 | div_by_4[1:0] << 2| integer[17:16]
     * @li reg[3] = integer[15:8]
     * @li reg[4] = integer[7:0]
     * @li reg[5] = denominator[19:16] << 4 |  numerator[19:16]
     * @li reg[6] = numerator[15:8]
     * @li reg[7] = numerator[7:0]
     *
     * Note that the r_div is formated before packed. The format from r_div to the formatted value is as followings :
     * @li 1 -> 0
     * @li 2 -> 1
     * @li N -> log2(N)
     * @li 128 -> 7
     *
     * So the r_div must be the power of 2.
     */
    void Si5351PackRegister(
                            const uint32_t integer,
                            const uint32_t numerator,
                            const uint32_t denominator,
                            const uint32_t div_by_4,
                            const uint32_t r_div,
                            uint8_t reg[8]);

};

} /* namespace murasaki */

#endif /* THIRDPARTY_SI5351_HPP_ */
