/*
 * adau1361.cpp
 *
 *  Created on: 2018/05/11
 *      Author: takemasa
 */

#include "adau1361.hpp"
#include "debugger.hpp"

#include <algorithm>

#ifdef   HAL_I2C_MODULE_ENABLED

namespace murasaki {
Adau1361::Adau1361(unsigned int fs, murasaki::I2CMasterStrategy * controler,
		unsigned int i2c_device_addr) :
		AudioCodecStrategy(fs), i2c_(controler), device_addr_(i2c_device_addr) {
}


        // Core clock setting
static const uint8_t init_PLL[][3] =
{ { 0x40, 0x00, 0x00 }, // R0:Clock control
		{ 0x40, 0x17, 0x00 },    // R17, SRC = 1
		{ 0x40, 0x00, 0x0F }     // R0:Clock Control. Core clock. PLL Start
};




        // Set non clock registers as default
static const uint8_t init_Adau1361[][3] =
{
// R0,1, are set by init_freq_xxx table
		{ 0x40, 0x08, 0x00 },     // R2: Digital Mic
		{ 0x40, 0x09, 0x00 },     // R3: Rec Power Management
		{ 0x40, 0x0a, 0x00 },     // R4: Rec Mixer Left 0
		{ 0x40, 0x0b, 0x00 },     // R5: Rec Mixer Left 1
		{ 0x40, 0x0c, 0x00 },     // R6: Rec Mixer Right 0
		{ 0x40, 0x0d, 0x00 },     // R7: Rec Mixer Right 1
		{ 0x40, 0x0e, 0x00 },     // R8: Left diff input vol
		{ 0x40, 0x0f, 0x00 },     // R9: Right diff input vol
		{ 0x40, 0x10, 0x00 },     // R10: Rec mic bias
		{ 0x40, 0x11, 0x00 },     // R11: ALC0
		{ 0x40, 0x12, 0x00 },     // R12: ALC1
		{ 0x40, 0x13, 0x00 },     // R13: ALC2
		{ 0x40, 0x14, 0x00 },     // R14: ALC3
		{ 0x40, 0x15, 0x00 },     // R15: Serial Port 0
		{ 0x40, 0x16, 0x00 },     // R16: Serial Port 1
		// R17 is set by init_freq_xxx table
		{ 0x40, 0x18, 0x00 },     // R18: Converter 1
		{ 0x40, 0x19, 0x10 },     // R19:ADC Control.
		{ 0x40, 0x1a, 0x00 },     // R20: Left digital volume
		{ 0x40, 0x1b, 0x00 },     // R21: Rignt digital volume
		{ 0x40, 0x1c, 0x00 },     // R22: Play Mixer Left 0
		{ 0x40, 0x1d, 0x00 },     // R23: Play Mixer Left 1
		{ 0x40, 0x1e, 0x00 },     // R24: Play Mixer Right 0
		{ 0x40, 0x1f, 0x00 },     // R25: Play Mixer Right 1
		{ 0x40, 0x20, 0x00 },     // R26: Play L/R mixer left
		{ 0x40, 0x21, 0x00 },     // R27: Play L/R mixer right
		{ 0x40, 0x22, 0x00 },     // R28: Play L/R mixer monot
		{ 0x40, 0x23, 0x02 },     // R29: Play HP Left vol
		{ 0x40, 0x24, 0x02 },     // R30: Play HP Right vol
		{ 0x40, 0x25, 0x02 },     // R31: Line output Left vol
		{ 0x40, 0x26, 0x02 },     // R32: Line output Right vol
		{ 0x40, 0x27, 0x02 },     // R33: Play Mono output
		{ 0x40, 0x28, 0x00 },     // R34: Pop surpress
		{ 0x40, 0x29, 0x00 },     // R35: Play Power Management
		{ 0x40, 0x2a, 0x00 },     // R36: DAC Control 0
		{ 0x40, 0x2b, 0x00 },     // R37: DAC Control 1
		{ 0x40, 0x2c, 0x00 },     // R38: DAC control 2
		{ 0x40, 0x2d, 0xaa },     // R39: Seial port Pad
		{ 0x40, 0x2f, 0xaa },     // R40: Control Pad 1
		{ 0x40, 0x30, 0x00 },     // R41: Control Pad 2
		{ 0x40, 0x31, 0x08 },     // R42: Jack detect
		{ 0x40, 0x36, 0x03 }      // R67: Dejitter control
};

static const uint8_t lock_status_address[] = { 0x40, 0x07 };     // R2 : Byte 5.


        // Send single command
void Adau1361::send_command(const uint8_t table[], int size)
{
	i2c_->Transmit(device_addr_, table, size);
}


        // Send entire command table
void Adau1361::send_command_table(const uint8_t table[][3], int rows)
{
	for (int i = 0; i < rows; i++)
		send_command(table[i], 3);

}


        // loop while the PLL is not locked.
void Adau1361::wait_pll_lock(void) {
	uint8_t status[1];

	do {

		i2c_->TransmitThenReceive(
		    device_addr_,
		    lock_status_address,
				sizeof(lock_status_address),
				status,
				1);
	} while (!(status[1] & 0x2));  // read the PLL status.
}


#define DATA 2  /* data payload of register */
#define ADDL 1  /* lower address of register */
void Adau1361::start(void) {

	// then, start of configuration as register address order
	send_command_table(init_PLL, sizeof(init_PLL) / 3);


	configure_pll();

	// Set all registers.
	send_command_table(init_Adau1361, sizeof(init_Adau1361) / 3); // init Adau1361 as default state.

	configure_board();

	// set input gain
	set_line_input_gain(0, 0);        // unmute
	set_aux_input_gain(0, 0, true);   // mute
	set_mic_input_gain(0, 0, true);    // mute
	set_line_output_gain(0, 0, true); // mute
	set_hp_output_gain(0, 0, true);   // mute
}

#define SET_INPUT_GAIN( x ) ((x<<1)|1)

void Adau1361::set_line_input_gain(float left_gain, float right_gain,
		bool mute) {
	uint8_t data[3];
	int left, right;


	data[0] = 0x40; // Upper address of register

	if (mute) {
		data[ADDL] = 0x0a;
		data[DATA] = 0x01;
		i2c_->Transmit(device_addr_, data, 3);    // R4: mixer 1 enable
		data[ADDL] = 0x0c;
		data[DATA] = 0x01;
		i2c_->Transmit(device_addr_, data, 3);    // R6: mixer 2 enable
	} else {

		// set left gain
		left = (left_gain + 15) / 3;   // See table 31 LINNG
		left = std::max(left, 0);
		left = std::min(left, 7);
		data[DATA] = SET_INPUT_GAIN(left);

		data[ADDL] = 0x0a;
		i2c_->Transmit(device_addr_, data, 3);    // R4: mixer 1 enable

		// set right gain
		right = (right_gain + 15) / 3;   // See table 31 LINNG
		right = std::max(right, 0);
		right = std::min(right, 7);
		data[DATA] = SET_INPUT_GAIN(right);

		data[ADDL] = 0x0c;
		i2c_->Transmit(device_addr_, data, 3);    // R4: mixer 1 enable

	}
}


void Adau1361::set_aux_input_gain(float left_gain, float right_gain,
                                  bool mute) {
	uint8_t data[3];
	int left, right;


	data[0] = 0x40; // Upper address of register

	if (mute) {
		data[ADDL] = 0x0b;
		data[DATA] = 0x01;							//todo : should be 0
		i2c_->Transmit(device_addr_, data, 3);    // R5: mixer 1 L aux mute
		data[ADDL] = 0x0d;
		data[DATA] = 0x01;
		i2c_->Transmit(device_addr_, data, 3);    // R7: mixer 2 R aux mute
	} else {

		// set left gain
		left = (left_gain + 15) / 3;   // See table 31 LINNG
		left = std::max(left, 0);
		left = std::min(left, 7);
		data[DATA] = left;

		data[ADDL] = 0x0b;
		i2c_->Transmit(device_addr_, data, 3);    // R5: mixer 1 enable

		// set right gain
		right = (right_gain + 15) / 3;   // See table 31 LINNG
		right = std::max(right, 0);
		right = std::min(right, 7);
		data[DATA] = right;

		data[ADDL] = 0x0d;
		i2c_->Transmit(device_addr_, data, 3);    // R4: mixer 1 enable

	}
}


#define SET_LO_GAIN( x ) ((x<<2)|2)

void Adau1361::set_line_output_gain(float left_gain, float right_gain,
		bool mute) {
	uint8_t data[3];
	int left, right;

	data[0] = 0x40; // Upper address of register

	if (mute) {
		data[ADDL] = 0x25;
		data[DATA] = 0x01;
		i2c_->Transmit(device_addr_, data, 3);    // R31: LOUTVOL
		data[ADDL] = 0x26;
		data[DATA] = 0x01;
		i2c_->Transmit(device_addr_, data, 3);    // R32: LOUTVOL
	} else {
		left = left_gain + 57;
		left = std::max(left, 0);
		left = std::min(left, 63);

		right = right_gain + 57;
		right = std::max(right, 0);
		right = std::min(right, 63);

		data[ADDL] = 0x25;
		data[DATA] = SET_LO_GAIN(left);
		i2c_->Transmit(device_addr_, data, 3);    // R31: LOUTVOL
		data[ADDL] = 0x26;
		data[DATA] = SET_LO_GAIN(right);
		i2c_->Transmit(device_addr_, data, 3);    // R32: LOUTVOL

	}
}

#define SET_HP_GAIN( x ) ((x<<2)|3)

void Adau1361::set_hp_output_gain(float left_gain, float right_gain,
		bool mute) {
	uint8_t data[3];
	int left, right;

	data[0] = 0x40; // Upper address of register

	if (mute) {
		data[ADDL] = 0x23;
		data[DATA] = 0x01;
		i2c_->Transmit(device_addr_, data, 3);    // R29: LHPVOL
		data[ADDL] = 0x24;
		data[DATA] = 0x01;
		i2c_->Transmit(device_addr_, data, 3);    // R30: LHPVOL
	} else {
		left = left_gain + 57;
		left = std::max(left, 0);
		left = std::min(left, 63);

		right = right_gain + 57;
		right = std::max(right, 0);
		right = std::min(right, 63);

		data[ADDL] = 0x23;
		data[DATA] = SET_HP_GAIN(left);
		i2c_->Transmit(device_addr_, data, 3);    // R29: LHPVOL
		data[ADDL] = 0x24;
		data[DATA] = SET_HP_GAIN(right);
		i2c_->Transmit(device_addr_, data, 3);    // R30: LHPVOL

	}
}

} /* namespace murasaki */

#endif //  HAL_I2C_MODULE_ENABLED
