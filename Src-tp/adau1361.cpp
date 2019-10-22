/*
 * adau1361.cpp
 *
 *  Created on: 2018/05/11
 *      Author: Seiichi "Suikan" Horie
 */

#include "adau1361.hpp"
#include "debugger.hpp"

#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

#include <algorithm>

// Macro for easy-to-read
#define CODEC_SYSLOG(fmt, ...)    MURASAKI_SYSLOG(kfaAudioCodec, kseDebug, fmt, ##__VA_ARGS__)

#ifdef   HAL_I2C_MODULE_ENABLED

namespace murasaki {

/*
 * Constructor. Initialize the internal variables.
 */
Adau1361::Adau1361(
                   unsigned int fs,
                   unsigned int master_clock,
                   murasaki::I2CMasterStrategy * controller,
                   unsigned int i2c_device_addr)
        :
          AudioCodecStrategy(fs),
          master_clock_(master_clock),
          i2c_(controller),
          device_addr_(i2c_device_addr) {
}

// Core clock setting
static const uint8_t init_core[] =
        { 0x40, 0x00, 0x00 };  // R0:Clock control. Core clock disabled. PLL off.

// Set core source to PLL
static const uint8_t config_core[] =
        { 0x40, 0x00, 0xff };  // R0:Clock control. Core clock enabled. Set source PLL.

// PLL Disable.
// R1 : Must write 6 byte at once.
static const uint8_t disable_pll[] =
        { 0x40, 0x02, 0x00, 0xFD, 0x00, 0x0C, 0x10, 0x00 };

// Set UMB_ADAU1361A. No mono output, No cross channel Mix, No analog path through.
static const uint8_t config_UMB_ADAU1361A[][3] = {
        // Configuration for UMB-ADAU1361-A http://dsps.shop-pro.jp/?pid=82798273
        { 0x40, 0x0a, 0x0B },  // R4: Rec Mixer Left 0,  Mixer enable, LINNG 0dB
        { 0x40, 0x0c, 0x0B },  // R6: Rec Mixer Right 0, Mixer enable, RINNG 0dB
        { 0x40, 0x15, 0x01 },  // R15:Serial Port control, Set code as Master mode I2S.
        { 0x40, 0x19, 0x63 },  // R19:ADC Control. Enable ADC, Both Cannel ADC on, HPF on
        { 0x40, 0x29, 0x03 },  // R35:Left Right Play back enable. Play back power Management
        { 0x40, 0x2a, 0x03 },  // R36:DAC Control 0. Enable DAC. Both channels on.
        { 0x40, 0x1c, 0x21 },  // R22:MIXER 3, Left DAC Mixer (set L DAC to L Mixer )
        { 0x40, 0x1e, 0x41 },  // R24:MIXER 4, Right DAC Mixer (set R DAC to R Mixer )
        { 0x40, 0x20, 0x03 },  // R26:MIXER 5, Left out mixer. L out MIX5G3 and enable
        { 0x40, 0x21, 0x09 },  // R27:MIXER 6, Right out mixer. R out MIX6G4 and enable.
        };

// Set non clock registers as default
static const uint8_t config_Adau1361[][3] = {
        // R0,1, are set by init_freq_xxx table
        { 0x40, 0x08, 0x00 },  // R2: Digital Mic
        { 0x40, 0x09, 0x00 },  // R3: Rec Power Management
        { 0x40, 0x0a, 0x00 },  // R4: Rec Mixer Left 0
        { 0x40, 0x0b, 0x00 },  // R5: Rec Mixer Left 1
        { 0x40, 0x0c, 0x00 },  // R6: Rec Mixer Right 0
        { 0x40, 0x0d, 0x00 },  // R7: Rec Mixer Right 1
        { 0x40, 0x0e, 0x00 },  // R8: Left diff input vol
        { 0x40, 0x0f, 0x00 },  // R9: Right diff input vol
        { 0x40, 0x10, 0x00 },  // R10: Rec mic bias
        { 0x40, 0x11, 0x00 },  // R11: ALC0
        { 0x40, 0x12, 0x00 },  // R12: ALC1
        { 0x40, 0x13, 0x00 },  // R13: ALC2
        { 0x40, 0x14, 0x00 },  // R14: ALC3
        { 0x40, 0x15, 0x00 },  // R15: Serial Port 0
        { 0x40, 0x16, 0x00 },  // R16: Serial Port 1
        // R17 is set by config_src_xx table
        { 0x40, 0x18, 0x00 },  // R18: Converter 1
        { 0x40, 0x19, 0x10 },  // R19:ADC Control.
        { 0x40, 0x1a, 0x00 },  // R20: Left digital volume
        { 0x40, 0x1b, 0x00 },  // R21: Rignt digital volume
        { 0x40, 0x1c, 0x00 },  // R22: Play Mixer Left 0
        { 0x40, 0x1d, 0x00 },  // R23: Play Mixer Left 1
        { 0x40, 0x1e, 0x00 },  // R24: Play Mixer Right 0
        { 0x40, 0x1f, 0x00 },  // R25: Play Mixer Right 1
        { 0x40, 0x20, 0x00 },  // R26: Play L/R mixer left
        { 0x40, 0x21, 0x00 },  // R27: Play L/R mixer right
        { 0x40, 0x22, 0x00 },  // R28: Play L/R mixer monot
        { 0x40, 0x23, 0x02 },  // R29: Play HP Left vol
        { 0x40, 0x24, 0x02 },  // R30: Play HP Right vol
        { 0x40, 0x25, 0x02 },  // R31: Line output Left vol
        { 0x40, 0x26, 0x02 },  // R32: Line output Right vol
        { 0x40, 0x27, 0x02 },  // R33: Play Mono output
        { 0x40, 0x28, 0x00 },  // R34: Pop surpress
        { 0x40, 0x29, 0x00 },  // R35: Play Power Management
        { 0x40, 0x2a, 0x00 },  // R36: DAC Control 0
        { 0x40, 0x2b, 0x00 },  // R37: DAC Control 1
        { 0x40, 0x2c, 0x00 },  // R38: DAC control 2
        { 0x40, 0x2d, 0xaa },  // R39: Seial port Pad
        { 0x40, 0x2f, 0xaa },  // R40: Control Pad 1
        { 0x40, 0x30, 0x00 },  // R41: Control Pad 2
        { 0x40, 0x31, 0x08 },  // R42: Jack detect
        { 0x40, 0x36, 0x03 }  // R67: Dejitter control
};

static const uint8_t lock_status_address[] = { 0x40, 0x02 };  // R1 : 6 byte register.

/*
 *  Send single command
 *  table : command table :
 *  size : size of table.
 */
void Adau1361::SendCommand(
                           const uint8_t table[],
                           int size) {
    CODEC_SYSLOG("Enter %p, %d", table, size)

    /*
     * Send the given table to the I2C slave device at device_addr
     */
    i2c_->Transmit(device_addr_, table, size);

    CODEC_SYSLOG("Leave.")
}

/*
 * Send entire command table
 */

void Adau1361::SendCommandTable(
                                const uint8_t table[][3],
                                int rows) {

    CODEC_SYSLOG("Enter %p, %d", table, rows)

    /*
     * Send all rows of command table.
     */
    for (int i = 0; i < rows; i++)
        SendCommand(table[i], 3);

    CODEC_SYSLOG("Leave.")
}

// loop while the PLL is not locked.
void Adau1361::WaitPllLock(void) {
    uint8_t status[6];

    CODEC_SYSLOG("Enter.")

    int count = 0;

    do {

        // Obtain PLL status
        i2c_->TransmitThenReceive(
                                  device_addr_,
                                  lock_status_address,
                                  sizeof(lock_status_address),
                                  status, sizeof(status));
        CODEC_SYSLOG(
                     "Status : %02x, %02x, %02x, %02x, %02x, %02x",
                     status[0],
                     status[1],
                     status[2],
                     status[3],
                     status[4],
                     status[5])

        // If locked, exit the loop;
        if (status[5] & 0x2)
            break;

        // If this is more than 10 times trial, that is critical error.
        count++;
        if (count > 10)
                {
            MURASAKI_SYSLOG(
                            kfaAudioCodec,
                            kseCritical,
                            "Codec at I2C address 0x%02x doesn't lock",
                            device_addr_);
            MURASAKI_ASSERT(false);
        }

        // Otherwise, sleep 100mS and then try again.
        murasaki::Sleep(
                        static_cast<murasaki::WaitMilliSeconds>(100));
    } while (true);

    CODEC_SYSLOG("Leave.")
}

// Configure PLL and start. Then, initiate the core and set the CODEC Fs.
void Adau1361::ConfigurePll(void) {
    CODEC_SYSLOG("Enter.")

    if (fs_ == 24000 || fs_ == 32000 || fs_ == 48000 || fs_ == 96000) {

        // Configure the PLL depend on the master clock frequency of the CODEC.
        switch (master_clock_) {
            case 8000000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0x7D, 0x00, 0x12, 0x31, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 12000000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0x7D, 0x00, 0x0C, 0x21, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 13000000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x06, 0x59, 0x04, 0xF5, 0x19, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 14400000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0x4B, 0x00, 0x3E, 0x33, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 19200000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0x19, 0x00, 0x03, 0x2B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 19680000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0xCD, 0x00, 0xCC, 0x23, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 19800000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x03, 0x39, 0x03, 0x1C, 0x23, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 24000000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0x7D, 0x00, 0x0C, 0x23, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 26000000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x06, 0x59, 0x04, 0xF5, 0x1B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 27000000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x04, 0x65, 0x02, 0xD1, 0x1B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 12288000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x04, 0x65, 0x02, 0xD1, 0x20, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 24576000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x04, 0x65, 0x02, 0xD1, 0x10, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            default:
                MURASAKI_SYSLOG(
                                kfaAudioCodec,
                                kseCritical,
                                "Non supported master clock %dHz",
                                master_clock_)
                MURASAKI_ASSERT(false)

        }

        // Wait for PLL lock
        WaitPllLock();

        // Enable core.
        SendCommand(config_core, sizeof(config_core));

        // Set the converter clock.
        switch (fs_) {
            case 24000: {
                // R17: Converter 0, SRC = 1/2 * core clock
                const uint8_t config_src[] = { 0x40, 0x17, 0x04 };

                SendCommand(config_src, sizeof(config_src));
                break;
            }
            case 32000: {
                // R17: Converter 0, SRC = 2/3 * core clock
                const uint8_t config_src[] = { 0x40, 0x17, 0x05 };

                SendCommand(config_src, sizeof(config_src));
                break;
            }
            case 48000: {
                // R17: Converter 0, SRC = 1 * core clock
                const uint8_t config_src[] = { 0x40, 0x17, 0x00 };

                SendCommand(config_src, sizeof(config_src));
                break;
            }
            case 96000: {
                // R17: Converter 0, SRC = 2 * core clock
                const uint8_t config_src[] = { 0x40, 0x17, 0x06 };

                SendCommand(config_src, sizeof(config_src));
                break;
            }
            default:
                MURASAKI_SYSLOG(
                                kfaAudioCodec,
                                kseCritical,
                                "Non supported fs %dHz",
                                fs_)
                MURASAKI_ASSERT(false)
        }

    }
    else if (fs_ == 22050 || fs_ == 44100 || fs_ == 88200) {

        // Configure the PLL

        switch (master_clock_) {
            case 8000000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x02, 0x71, 0x01, 0x93, 0x29, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 12000000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x02, 0x71, 0x01, 0xDD, 0x19, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 13000000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x1F, 0xBD, 0x0F, 0x09, 0x19, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 14400000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0x7D, 0x00, 0x22, 0x33, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 19200000: {
                uint8_t
                config_pll[] =
                        { 0x40, 0x02, 0x00, 0x7D, 0x00, 0x58, 0x23, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 19680000: {
                uint8_t
                config_pll[] =
                        { 0x40, 0x02, 0x04, 0x01, 0x02, 0x5C, 0x23, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 19800000: {
                uint8_t
                config_pll[] =
                        { 0x40, 0x02, 0x05, 0x5F, 0x03, 0x04, 0x23, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 24000000: {
                uint8_t
                config_pll[] =
                        { 0x40, 0x02, 0x02, 0x71, 0x01, 0xDD, 0x1B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 26000000: {
                uint8_t
                config_pll[] =
                        { 0x40, 0x02, 0x1F, 0xBD, 0x0F, 0x09, 0x1B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 27000000: {
                uint8_t
                config_pll[] =
                        { 0x40, 0x02, 0x07, 0x53, 0x02, 0x87, 0x1B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 12288000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x03, 0xE8, 0x02, 0xA3, 0x19, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 24576000: {
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x03, 0xE8, 0x02, 0xA3, 0x1B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            default:
                MURASAKI_SYSLOG(
                                kfaAudioCodec,
                                kseCritical,
                                "Non supported master clock %dHz",
                                master_clock_)
                MURASAKI_ASSERT(false)
        }

        // Waiting for the PLL lock.
        WaitPllLock();

        // Enable core.
        SendCommand(config_core, sizeof(config_core));

        // Set the converter clock.
        switch (fs_) {
            case 22050: {
                // R17: Converter 0, SRC = 1/2 * core clock
                const uint8_t config_src[] = { 0x40, 0x17, 0x04 };

                SendCommand(config_src, sizeof(config_src));
                break;
            }
            case 44100: {
                // R17: Converter 0, SRC = 1 * core clock
                const uint8_t config_src[] = { 0x40, 0x17, 0x00 };

                SendCommand(config_src, sizeof(config_src));
                break;
            }
            case 88200: {
                // R17: Converter 0, SRC = 2 * core clock
                const uint8_t config_src[] = { 0x40, 0x17, 0x06 };

                SendCommand(config_src, sizeof(config_src));
                break;
            }
            default:
                MURASAKI_SYSLOG(
                                kfaAudioCodec,
                                kseCritical,
                                "Not supported fs %dHz",
                                fs_)
                MURASAKI_ASSERT(false)
        }
    }
    else {  // if the required Fs is unknown, it is critical error.
        MURASAKI_SYSLOG(
                        kfaAudioCodec,
                        kseCritical,
                        "Not supported fs %dHz",
                        fs_)
        // then, assert.
        MURASAKI_ASSERT(false)
    }

    CODEC_SYSLOG("Leave.")
}

void Adau1361::Start(void) {
    CODEC_SYSLOG("Enter.")

    // Check if target device exist.
    // send no payload. Just enquire address.
    murasaki::I2cStatus status;
    status = i2c_->Transmit(
                            device_addr_,
                            lock_status_address,
                            0);
    if (status != murasaki::ki2csOK)
            {
        // If it I2C returns NAK, there is no CODEC device on the specific I2C address.
        // Report it
        MURASAKI_SYSLOG(
                        kfaAudioCodec,
                        kseCritical,
                        "Given audio codec at I2C address %02x doesn't response or something is wrong",
                        device_addr_);
        MURASAKI_ASSERT(false);
    }

    // then, start of configuration as register address order
    SendCommand(init_core, sizeof(init_core));
    // And then, stop PLL explicitly.
    SendCommand(disable_pll, sizeof(disable_pll));
    // Now, start PLL again as desired frequency.
    ConfigurePll();

    // Set all registers.
    SendCommandTable(
                     config_Adau1361,
                     sizeof(config_Adau1361) / 3);  // init Adau1361 as default state.

    // Set certain signal pass. If it doen't fit to your system, override it by SendCommand()
    SendCommandTable(
                     config_UMB_ADAU1361A,
                     sizeof(config_UMB_ADAU1361A) / 3);  // init UMB-ADAU1361 as default state.

    // set line input gain as 0.0dB
    // set all other input/output as muted.
    SetLineInputGain(0, 0);  // unmute
    SetAuxInputGain(0, 0, true);  // mute
    SetMicInputGain(0, 0, true);  // mute
    SetLineOutputGain(0, 0, true);  // mute
    SetHpOutputGain(0, 0, true);  // mute

    CODEC_SYSLOG("Leave.")
}

#define DATA 2  /* data payload of register */
#define ADDL 1  /* lower address of register */

#define SET_INPUT_GAIN( x ) ((x<<1)|1)

void Adau1361::SetLineInputGain(
                                float left_gain,
                                float right_gain,
                                bool mute) {

    CODEC_SYSLOG("Enter. %f4.1, %f4.1, %d", left_gain, right_gain, mute)

    uint8_t data[3];
    int left, right;

    data[0] = 0x40;  // Upper address of register

    if (mute) {
        data[ADDL] = 0x0a;
        data[DATA] = 0x01;
        i2c_->Transmit(device_addr_, data, 3);  // R4: mixer 1 enable
        data[ADDL] = 0x0c;
        data[DATA] = 0x01;
        i2c_->Transmit(device_addr_, data, 3);  // R6: mixer 2 enable
    } else {

        // set left gain
        left = (left_gain + 15) / 3;  // See table 31 LINNG
        left = std::max(left, 0);
        left = std::min(left, 7);
        data[DATA] = SET_INPUT_GAIN(left);

        data[ADDL] = 0x0a;
        i2c_->Transmit(device_addr_, data, 3);  // R4: mixer 1 enable

        // set right gain
        right = (right_gain + 15) / 3;  // See table 31 LINNG
        right = std::max(right, 0);
        right = std::min(right, 7);
        data[DATA] = SET_INPUT_GAIN(right);

        data[ADDL] = 0x0c;
        i2c_->Transmit(device_addr_, data, 3);  // R4: mixer 1 enable

    }

    CODEC_SYSLOG("Leave.")
}

void Adau1361::SetAuxInputGain(
                               float left_gain,
                               float right_gain,
                               bool mute) {
    uint8_t data[3];
    int left, right;

    CODEC_SYSLOG("Enter. %f4.1, %f4.1, %d", left_gain, right_gain, mute)

    data[0] = 0x40;  // Upper address of register

    if (mute) {
        data[ADDL] = 0x0b;
        data[DATA] = 0x00;
        i2c_->Transmit(device_addr_, data, 3);  // R5: mixer 1 L aux mute
        data[ADDL] = 0x0d;
        data[DATA] = 0x00;
        i2c_->Transmit(device_addr_, data, 3);  // R7: mixer 2 R aux mute
    } else {

        // set left gain
        left = (left_gain + 15) / 3;  // See table 31 LINNG
        left = std::max(left, 0);
        left = std::min(left, 7);
        data[DATA] = left;

        data[ADDL] = 0x0b;
        i2c_->Transmit(device_addr_, data, 3);  // R5: mixer 1 enable

        // set right gain
        right = (right_gain + 15) / 3;  // See table 31 LINNG
        right = std::max(right, 0);
        right = std::min(right, 7);
        data[DATA] = right;

        data[ADDL] = 0x0d;
        i2c_->Transmit(device_addr_, data, 3);  // R4: mixer 1 enable

    }

    CODEC_SYSLOG("Leave.")
}

#define SET_LO_GAIN( x ) ((x<<2)|2)

void Adau1361::SetLineOutputGain(
                                 float left_gain,
                                 float right_gain,
                                 bool mute) {
    uint8_t data[3];
    int left, right;

    CODEC_SYSLOG("Enter. %f4.1, %f4.1, %d", left_gain, right_gain, mute)

    data[0] = 0x40;  // Upper address of register

    if (mute) {
        data[ADDL] = 0x25;  // R31: LOUTVOL
        data[DATA] = 0x00;  // Mute, Line mode
        i2c_->Transmit(device_addr_, data, 3);
        data[ADDL] = 0x26;  // R32: ROUTVOL
        data[DATA] = 0x00;  // Mute, Line mode
        i2c_->Transmit(device_addr_, data, 3);
    } else {
        left = left_gain + 57;
        left = std::max(left, 0);
        left = std::min(left, 63);

        right = right_gain + 57;
        right = std::max(right, 0);
        right = std::min(right, 63);

        data[ADDL] = 0x25;
        data[DATA] = SET_LO_GAIN(left);
        i2c_->Transmit(device_addr_, data, 3);  // R31: LOUTVOL
        data[ADDL] = 0x26;
        data[DATA] = SET_LO_GAIN(right);
        i2c_->Transmit(device_addr_, data, 3);  // R32: ROUTVOL

    }
    CODEC_SYSLOG("Leave.")
}

#define SET_HP_GAIN( x ) ((x<<2)|3)

void Adau1361::SetHpOutputGain(
                               float left_gain,
                               float right_gain,
                               bool mute) {
    uint8_t data[3];
    int left, right;

    CODEC_SYSLOG("Enter. %f4.1, %f4.1, %d", left_gain, right_gain, mute)

    data[0] = 0x40;  // Upper address of register

    if (mute) {

        data[ADDL] = 0x23;  // R29: LHPVOL
        data[DATA] = 0x01;  // Mute, HP mode
        i2c_->Transmit(device_addr_, data, 3);
        data[ADDL] = 0x24;  // R30: RHPVOL
        data[DATA] = 0x01;  // Mute, HP mode
        i2c_->Transmit(device_addr_, data, 3);
    } else {
        left = left_gain + 57;
        left = std::max(left, 0);
        left = std::min(left, 63);

        right = right_gain + 57;
        right = std::max(right, 0);
        right = std::min(right, 63);

        data[ADDL] = 0x23;
        data[DATA] = SET_HP_GAIN(left);
        i2c_->Transmit(device_addr_, data, 3);  // R29: LHPVOL
        data[ADDL] = 0x24;
        data[DATA] = SET_HP_GAIN(right);
        i2c_->Transmit(device_addr_, data, 3);  // R30: RHPVOL

    }
    CODEC_SYSLOG("Leave.")
}

} /* namespace murasaki */

#endif //  HAL_I2C_MODULE_ENABLED
