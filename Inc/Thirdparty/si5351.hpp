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
 * @ingroup MURASAKI_DEFINITION_GROUP
 * @brief Status of the Si5351
 * @details
 * These enums are dedicated to the @ref Si5351 class.
 *
 */
enum Si5351Status
{
    ks5351Ok, /**< s5351Ok */
    ks5351SeekFailure/**< s5351SeekFailure */
};

/**
 * @ingroup MURASAKI_DEFINITION_GROUP
 * @brief PLL ID
 * @details
 * These enums are dedicated to the @ref Si5351 class.
 *
 */
enum Si5351Pll
{
    ks5351PllA, /**< PLLA */
    ks5351PllB /**< PLLB */
};

/**
 * @ingroup MURASAKI_DEFINITION_GROUP
 * @brief Dividor ID.
 * @details
 * These enums are dedicated to the @ref Si5351 class.
 *
 * These dividors are located at the output of PLL
 *
 */
enum Si5351Div
{
    ks5351Div0, /**< Divider 0 */
    ks5351Div1, /**< Divider 1 */
    ks5351Div2 /**< Divider 2 */
};

class TestSi5351;

/**
 * @ingroup MURASAKI_THIRDPARTY_GROUP
 * @brief Si5351 driver class.
 */
class Si5351 {
    friend TestSi5351;
    friend bool TestSi5351(int freq_step);

 public:
    /**
     * @brief Initialize an PLL control object.
     *
     * @param controller Pointer to the I2C master.
     * @param addrs 7bits address of the I2C devices. Right aligned.
     * @param xtal_freq Frequency of the Xtal oscillator [Hz]
     * @details
     * Initialize a PLL control object. The controller is the pointer to the I2C master controller.
     * This object communicate with the PLL device through this controller.
     *
     * @li <a href="https://www.silabs.com/documents/public/data-sheets/Si5351-B.pdf">Si5351A/B/C-B I2C-PROGRAMMABLE ANY-FREQUENCY CMOS CLOCKGENERATOR + VCXO</a>
     * @li <a href="https://www.silabs.com/documents/public/application-notes/AN619.pdf">AN619 Manually Generating an Si5351 Register Map for 10-MSOP and 20-QFN Devices</a>.
     */
    Si5351(murasaki::I2cMasterStrategy *controller, unsigned int addrs, uint32_t xtal_freq);

    virtual ~Si5351();

    /**
     * @brief Check whether PLL is initializing or not.
     * @return true : PLL is still under initializing. false : PLL initialization is done.
     */
    bool IsInitializing();

    /**
     * @brief Check whether PLL A is locked or not.
     * @return true : PLL A lost the lock. false : PLL A is locked.
     */
    bool IsLossOfLockA();

    /**
     * @brief Check whether PLL B is locked or not.
     * @return true : PLL B lost the lock. false : PLL B is locked.
     */
    bool IsLossOfLockB();
    /**
     * @brief Check whether Clock in is lost.
     * @return true : Clock in signal is lost. false : clock in signal is alive.
     */
    bool IsLossOfClkin();

    /**
     * @brief Check whether XTAL is lost.
     * @return true : XTAL signal is lost. false : XTAL signal is alive.
     */
    bool IsLossOfXtal();

    bool SetFrequency(
                      murasaki::Si5351Pll pll,
                      murasaki::Si5351Pll div,
                      uint32_t frequency
                      );

    bool SetQuadratureFrequency(
                                murasaki::Si5351Pll pll,
                                murasaki::Si5351Pll divI,
                                murasaki::Si5351Pll divQ,
                                uint32_t frequency
                                );

#if 0 // not yet implemented.

    void ClockEnable(int8_t mask);
#endif

 private:
    const uint32_t ext_freq_;
    murasaki::I2cMasterStrategy *const i2c_;
    unsigned int addrs_;

    // Get specified register.
    uint8_t getRegister(unsigned int reg_num);

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
    void PackRegister(
                      const uint32_t integer,
                      const uint32_t numerator,
                      const uint32_t denominator,
                      const uint32_t div_by_4,
                      const uint32_t r_div,
                      uint8_t reg[8]);

};

} /* namespace murasaki */

#endif /* THIRDPARTY_SI5351_HPP_ */
