/**
 * @file adau1361.hpp
 *
 * @date 2018/05/11
 * @author: takemasa
 */

#ifndef MURASAKI_TP_ADAU1361_HPP_
#define MURASAKI_TP_ADAU1361_HPP_

#include <audiocodecstrategy.hpp>
#include "i2cmaster.hpp"


#ifdef HAL_I2C_MODULE_ENABLED

namespace murasaki {

/**
 * @ingroup MURASAKI_THIRDPARTY_GROUP
 * \brief Audio Codec LSI class
 */

class Adau1361: public AudioCodecStrategy {
public:
    /**
    * \brief constructor.
     * \param fs Sampling frequency.
    * \param controler Pass the I2C controler object.
     * \param i2c_device_addr I2C device address. value range is from 0 to 127
    * \details
    *   initialize the internal variables.
    */
	Adau1361(unsigned int fs, murasaki::I2CMasterStrategy * controler,
			unsigned int i2c_device_addr);

    /**
    * \brief Set up the ADAU1361 codec,  and then, start the codec.
    * \details
    *   This method starts the ADAU1361 AD/DA conversion and I2S communication.
    *
    *   The line in is configured to use the Single-End negative input. This is funny but ADAU1361 datasheet
    *   specifies to do it. The positive in and diff in are killed. All biases are set as "normal".
    *
    *   The CODEC is configured as master mode. That mean, bclk and WS are given from ADAU1361 to the micro processor.
    */
	virtual void start(void);

    /**
    * \brief Set the line input gain and enable the relevant mixer.
    * \param left_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
    * \param right_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
    * \param mute set true to mute
    * \details
    *   As same as start(), this gain control function uses the single-end negative input only. Other
    *   input signal of the line in like positive signal or diff signal are killed.
    *
    *   Other input line like aux are not killed. To kill it, user have to mute them explicitly.
    */
	virtual void set_line_input_gain(float left_gain, float right_gain,
			bool mute = false);

    /**
    * \brief Set the aux input gain and enable the relevant mixer.
    * \param left_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
    * \param right_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
    * \param mute set true to mute
    * \details
    *   Other input lines are not killed. To kill it, user have to mute them explicitly.
    */
	virtual void set_aux_input_gain(float left_gain, float right_gain,
			bool mute = false);

    /**
    * \brief Set the line output gain and enable the relevant mixer.
    * \param left_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
    * \param right_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
    * \param mute set true to mute
    * \details
    *   Other output lines are not killed. To kill it, user have to mute them explicitly.
    *
    */
	virtual void set_line_output_gain(float left_gain, float right_gain,
			bool mute = false);

    /**
    * \brief Set the headphone output gain and enable the relevant mixer.
    * \param left_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
    * \param right_gain Gain by dB. The gain value outside of the acceptable range will be saturated.
    * \param mute set true to mute
    * \details
    *   Other out line like line in are not killed. To kill it, user have to mute them explicitly.
    */
	virtual void set_hp_output_gain(float left_gain, float right_gain,
			bool mute = false);

protected:
    murasaki::I2CMasterStrategy * const i2c_;
    const unsigned int device_addr_;

    /**
    * \brief configuration of PLL for the desired core clock
    * \details
    * A pure virutal function.
    *
    * This member function must be overriden by inherited class. Before the call of this function,
    * R0 is initialized as 0 and then, set the clock source is PLL.
    *
    * This member funciton must configure the PLL correctly, confirm the PLL lock status. And then
    * set the SRC.
    *
    * Note that the setting SRC before PLL lock may fail.
    */
	virtual void configure_pll(void)=0;

    /**
    * \brief configuration of the ADAU1361 for the codec board
    * \details
    * A pure virutal function.
    *
    * This member function must be overriden by inherited class. Before the calling of this function,
    * the codec is initialized as default state except PLL. PLL is set by configure_pll() method
    * before calling this function.
    *
    * This member funciton must configure the ADAU1361 registered based on the board circuit. For example,
    * internal signal pass or bias.
    *
    */
	virtual void configure_board(void)=0;
    /**
    *  Service function for the ADAu1361 board implementer.
    *
    * \brief send one command to ADAU1361.
    * \param command command data array. It have to have register addess of ADAU1361 in first two bytes.
    * \param size number of bytes in the command, including the regsiter address.
    * \details
    *   Send one complete command to ADAU3161 by I2C.
    */
	virtual void send_command(const uint8_t command[], int size);
    /**
    * \brief send one command to ADAU1361.
    * \param table command table. All commands are stored in one row. Each row has only 1 byte data after reg address.
    * \param rows number of the rows in the table.
    * \details
    *   Service function for the ADAu1361 board implementer.
    *
    *   Send a list of command to ADAU1361. All commands has 3 bytes length. That mean, after two byte register
    *   address, only 1 byte data payload is allowed. Commadns are sent by I2C
    */
	virtual void send_command_table(const uint8_t table[][3], int rows);

    /**
    * \brief wait until PLL locks.
    * \details
    *   Service function for the ADAu1361 board implementer.
    *
    *   Read the PLL status and repeat it until the PLL locks.
    */
	virtual void wait_pll_lock(void);
};

} /* namespace murasaki */

#endif //  HAL_I2C_MODULE_ENABLED

#endif /* MURASAKI_TP_ADAU1361_HPP_ */
