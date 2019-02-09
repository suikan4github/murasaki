/*
 * umbadau1361.cpp
 *
 *  Created on: 2018/05/11
 *      Author: takemasa
 */

#include "umbadau1361.hpp"

#ifdef  HAL_I2C_MODULE_ENABLED

namespace murasaki {


// 48kHz PLL setting for 12MHz input
// Denominator : 0x7D = 125
// Nunerator : 0x0c = 12
// Fractional PLL, *4 multiplying
// PLL Enable : Freq is 12Mhz * ( 4 + 12/125 ) = 49.125Mhz (1024 core clock)
static const uint8_t init_pll_48[] = { 0x40, 0x02, 0x00, 0x7D, 0x00, 0x0C, 0x21,
		0x01 };
// R17: Converter 0, SRC = 1 * core clock
static const uint8_t init_src_48[] = { 0x40, 0x17, 0x00 };

// 96kHz PLL setting for 12MHz input
#define init_pll_96 init_pll_48
// R17: Converter 0, SRC = 1 * core clock
static const uint8_t init_src_96[] = { 0x40, 0x17, 0x06 };

// 32kHz PLL setting for 12MHz input
#define init_pll_32 init_pll_48
// R17: Converter 0, SRC = 1 * core clock
static const uint8_t init_src_32[] = { 0x40, 0x17, 0x05 };

// 44.1kHz PLL setting for 12MHz input
// Denominator : 0x271 = 625
// Nunerator : 0x1dd = 447
// Fractional PLL, *3 multiplying
// PLL Enable : Freq is 12Mhz * ( 3 + 477/625 ) = 45.1584Mhz (1024 core clock)
static const uint8_t init_pll_441[] = { 0x40, 0x02, 0x02, 0x71, 0x01, 0xDD,
		0x19,
		0x01 };
// R17: Converter 0, SRC = 1 * core clock
#define init_src_441 init_src_48

// Set UMB_ADAU1361A. No mono output, No cross channel Mix, No analog path through.
static const uint8_t config_UMB_ADAU1361A[][3] = {
// Configuration for UMB-ADAU1361-A http://dsps.shop-pro.jp/?pid=82798273
		{ 0x40, 0x0a, 0x0B },  // R4: Rec Mixer Left 0,  Mixer enable, LINNG 0dB
		{ 0x40, 0x0c, 0x0B },  // R6: Rec Mixer Right 0, Mixer enable, RINNG 0dB
		{ 0x40, 0x15, 0x01 }, // R15:Serial Port control, Set code as Master mode I2S.
		{ 0x40, 0x19, 0x63 }, // R19:ADC Control. Enable ADC, Both Cannel ADC on, HPF on
		{ 0x40, 0x29, 0x03 }, // R35:Left Right Play back enable. Play back power Management
		{ 0x40, 0x2a, 0x03 }, // R36:DAC Control 0. Enable DAC. Both channels on.
		{ 0x40, 0x1c, 0x21 }, // R22:MIXER 3, Left DAC Mixer (set L DAC to L Mixer )
		{ 0x40, 0x1e, 0x41 }, // R24:MIXER 4, Right DAC Mixer (set R DAC to R Mixer )
		{ 0x40, 0x20, 0x03 }, // R26:MIXER 5, Left out mixer. L out MIX5G3 and enable
		{ 0x40, 0x21, 0x09 }, // R27:MIXER 6, Right out mixer. R out MIX6G4 and enable.
		};

// Clear PLL regsiter and then, set up PLL registers for given Fs.
void UmbAdau1361::configure_pll(void) {
	// set Fs ( xternal clock in is 12MHz )
	switch (fs_)  // fs is member variable.
	{
	case 96000:
		send_command( init_pll_96, sizeof(init_pll_96));
		wait_pll_lock();
		send_command(init_src_96, sizeof(init_src_96));
		break;
	case 44100:
		send_command(init_pll_441, sizeof(init_pll_441));
		wait_pll_lock();
		send_command( init_src_441, sizeof(init_src_441));
		break;
	case 32000:
		send_command( init_pll_32, sizeof(init_pll_32));
		wait_pll_lock();
		send_command(init_src_32, sizeof(init_src_32));
		break;
	case 48000:
	default:
		send_command(init_pll_48, sizeof(init_pll_48));
		wait_pll_lock();
		send_command(init_src_48, sizeof(init_src_48));
		break;
	}

}

void UmbAdau1361::configure_board(void) {
	send_command_table(config_UMB_ADAU1361A, sizeof(config_UMB_ADAU1361A) / 3);
}

} /* namespace murasaki */
#endif //  HAL_I2C_MODULE_ENABLED

