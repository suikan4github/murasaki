/*
 * adau1361.cpp
 *
 *  Created on: 2018/05/11
 *      Author: Seiichi "Suikan" Horie
 */

#include <Thirdparty/adau1361.hpp>
#include "debugger.hpp"

#include "murasaki_assert.hpp"
#include "murasaki_syslog.hpp"

#include <algorithm>

// Macro for easy-to-read
#define CODEC_SYSLOG(fmt, ...)    MURASAKI_SYSLOG( this,  kfaAudioCodec, kseDebug, fmt, ##__VA_ARGS__)

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
        { 0x40, 0x23, 0x03 },  // R29: Play HP Left vol : Mute, Enable
        { 0x40, 0x24, 0x03 },  // R30: Play HP Right vol : Mute, HP Mode
        { 0x40, 0x25, 0x02 },  // R31: Line output Left vol : Mute, Line out mode
        { 0x40, 0x26, 0x02 },  // R32: Line output Right vol : Mute, Line out mode
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
            MURASAKI_SYSLOG( this,
                            kfaAudioCodec,
                            kseCritical,
                            "Codec at I2C address 0x%02x doesn't lock",
                            device_addr_);
            MURASAKI_ASSERT(false);
        }

        // Otherwise, sleep 100mS and then try again.
        murasaki::Sleep(
                        100);
    } while (true);

    CODEC_SYSLOG("Leave.")
}

// Configure PLL and start. Then, initiate the core and set the CODEC Fs.
void Adau1361::ConfigurePll(void) {
    CODEC_SYSLOG("Enter.")

    if (fs_ == 24000 || fs_ == 32000 || fs_ == 48000 || fs_ == 96000) {

        // Configure the PLL. Target PLL out is 49.152MHz = 1024xfs
        // Regarding X, R, M, N, check ADAU1361 Datasheet register R1.
        // In the following code, the config_pll[0] and config_pll[1] contains regsiter address of R1.

        switch (master_clock_) {
            case 8000000: {
                /**
                 * X : 1
                 * R : 6
                 * M : 125
                 * N : 18
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 49.152MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0x7D, 0x00, 0x12, 0x31, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 12000000: {
                /**
                 * X : 1
                 * R : 4
                 * M : 125
                 * N : 12
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 49.152MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0x7D, 0x00, 0x0C, 0x21, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 13000000: {
                /**
                 * X : 1
                 * R : 3
                 * M : 1625
                 * N : 1269
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 49.152MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x06, 0x59, 0x04, 0xF5, 0x19, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 14400000: {
                /**
                 * X : 2
                 * R : 6
                 * M : 75
                 * N : 62
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 49.152MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0x4B, 0x00, 0x3E, 0x33, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 19200000: {
                /**
                 * X : 2
                 * R : 4
                 * M : 25
                 * N : 3
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 49.152MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0x19, 0x00, 0x03, 0x2B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 19680000: {
                /**
                 * X : 2
                 * R : 4
                 * M : 205
                 * N : 204
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 49.152MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0xCD, 0x00, 0xCC, 0x23, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 19800000: {
                /**
                 * X : 2
                 * R : 4
                 * M : 825
                 * N : 796
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 49.152MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x03, 0x39, 0x03, 0x1C, 0x23, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 24000000: {
                /**
                 * X : 2
                 * R : 4
                 * M : 125
                 * N : 12
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 49.152MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0x7D, 0x00, 0x0C, 0x23, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 26000000: {
                /**
                 * X : 2
                 * R : 3
                 * M : 1625
                 * N : 1269
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 49.152MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x06, 0x59, 0x04, 0xF5, 0x1B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 27000000: {
                /**
                 * X : 2
                 * R : 3
                 * M : 1125
                 * N : 721
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 49.152MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x04, 0x65, 0x02, 0xD1, 0x1B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 12288000: {
                /**
                 * X : 1
                 * R : 4
                 * M : Don't care
                 * N : Don't care
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 49.152MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x04, 0x65, 0x02, 0xD1, 0x20, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 24576000: {
                /**
                 * X : 1
                 * R : 4
                 * M : Don't care
                 * N : Don't care
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 49.152MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x04, 0x65, 0x02, 0xD1, 0x10, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            default:
                MURASAKI_SYSLOG( this,
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
                MURASAKI_SYSLOG( this,
                                kfaAudioCodec,
                                kseCritical,
                                "Non supported fs %dHz",
                                fs_)
                MURASAKI_ASSERT(false)
        }

    }
    else if (fs_ == 22050 || fs_ == 44100 || fs_ == 88200) {

        // Configure the PLL. Target PLL out is 45.1584MHz = 1024xfs

        switch (master_clock_) {
            case 8000000: {
                /**
                 * X : 1
                 * R : 5
                 * M : 625
                 * N : 403
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 45.1584MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x02, 0x71, 0x01, 0x93, 0x29, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 12000000: {
                /**
                 * X : 1
                 * R : 3
                 * M : 625
                 * N : 477
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 45.1584MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x02, 0x71, 0x01, 0xDD, 0x19, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 13000000: {
                /**
                 * X : 1
                 * R : 3
                 * M : 8125
                 * N : 3849
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 45.1584MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x1F, 0xBD, 0x0F, 0x09, 0x19, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 14400000: {
                /**
                 * X : 2
                 * R : 6
                 * M : 125
                 * N : 34
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 45.1584MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x00, 0x7D, 0x00, 0x22, 0x33, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 19200000: {
                /**
                 * X : 2
                 * R : 4
                 * M : 125
                 * N : 88
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 45.1584MHz
                 */
                uint8_t
                config_pll[] =
                        { 0x40, 0x02, 0x00, 0x7D, 0x00, 0x58, 0x23, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 19680000: {
                /**
                 * X : 2
                 * R : 4
                 * M : 1025
                 * N : 604
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 45.1584MHz
                 */
                uint8_t
                config_pll[] =
                        { 0x40, 0x02, 0x04, 0x01, 0x02, 0x5C, 0x23, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 19800000: {
                /**
                 * X : 2
                 * R : 4
                 * M : 1375
                 * N : 772
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 45.1584MHz
                 */
                uint8_t
                config_pll[] =
                        { 0x40, 0x02, 0x05, 0x5F, 0x03, 0x04, 0x23, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 24000000: {
                /**
                 * X : 2
                 * R : 3
                 * M : 625
                 * N : 477
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 45.1584MHz
                 */
                uint8_t
                config_pll[] =
                        { 0x40, 0x02, 0x02, 0x71, 0x01, 0xDD, 0x1B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 26000000: {
                /**
                 * X : 1
                 * R : 3
                 * M : 8125
                 * N : 3849
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 45.1584MHz
                 */
                uint8_t
                config_pll[] =
                        { 0x40, 0x02, 0x1F, 0xBD, 0x0F, 0x09, 0x1B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 27000000: {
                /**
                 * X : 2
                 * R : 3
                 * M : 1875
                 * N : 647
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 45.1584MHz
                 */
                uint8_t
                config_pll[] =
                        { 0x40, 0x02, 0x07, 0x53, 0x02, 0x87, 0x1B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 12288000: {
                /**
                 * X : 1
                 * R : 3
                 * M : 1000
                 * N : 675
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 45.1584MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x03, 0xE8, 0x02, 0xA3, 0x19, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            case 24576000: {
                /**
                 * X : 2
                 * R : 3
                 * M : 1000
                 * N : 675
                 * PLL = ( MCLK / X ) * ( R + N/M ) = 45.1584MHz
                 */
                uint8_t config_pll[] =
                        { 0x40, 0x02, 0x03, 0xE8, 0x02, 0xA3, 0x1B, 0x01 };

                SendCommand(config_pll, sizeof(config_pll));
                break;
            }

            default:
                MURASAKI_SYSLOG( this,
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
                MURASAKI_SYSLOG( this,
                                kfaAudioCodec,
                                kseCritical,
                                "Not supported fs %dHz",
                                fs_)
                MURASAKI_ASSERT(false)
        }
    }
    else {  // if the required Fs is unknown, it is critical error.
        MURASAKI_SYSLOG( this,
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
        MURASAKI_SYSLOG( this,
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

    // Before calling mute() function, set the gain as appropriate value.
    // The value is not problem if it is in the appropriate range.
    // This value is not be able to set by SetGain() member function, because at this moment,
    // Mute is not clealy specified.
    line_input_left_gain_ = 0.0;
    line_input_right_gain_ = 0.0;
    aux_input_left_gain_ = 0.0;
    aux_input_right_gain_ = 0.0;
    line_output_left_gain_ = 0.0;
    line_output_right_gain_ = 0.0;
    hp_output_left_gain_ = 0.0;     // headphone
    hp_output_right_gain_ = 0.0;
    
    // Mute all channels.
    Mute(murasaki::kccLineInput);
    Mute(murasaki::kccAuxInput);
    Mute(murasaki::kccLineOutput);
    Mute(murasaki::kccHeadphoneOutput);


    CODEC_SYSLOG("Leave.")
}

/*
 * Set gain of the specific input channel.
 * The mute status is not affected.
 */
void Adau1361::SetGain(murasaki::CodecChannel channel, float left_gain, float right_gain) {

    CODEC_SYSLOG("Enter. %d, %f, %f ", channel, left_gain, right_gain)

    switch (channel)
    {
        case murasaki::kccLineInput:
            line_input_left_gain_ = left_gain;
            line_input_right_gain_ = right_gain;
            SetLineInputGain(line_input_left_gain_, line_input_right_gain_, line_input_mute_);
            break;
        case murasaki::kccAuxInput:
            aux_input_left_gain_ = left_gain;
            aux_input_right_gain_ = right_gain;
            SetAuxInputGain(aux_input_left_gain_, aux_input_right_gain_, aux_input_mute_);
            break;
        case murasaki::kccLineOutput:
            line_output_left_gain_ = left_gain;
            line_output_right_gain_ = right_gain;
            SetLineOutputGain(line_output_left_gain_, line_output_right_gain_, line_output_mute_);
            break;

        case murasaki::kccHeadphoneOutput:
            hp_output_left_gain_ = left_gain;
            hp_output_right_gain_ = right_gain;
            SetHpOutputGain(hp_output_left_gain_, hp_output_right_gain_, hp_output_mute_);
            break;
        default:
            MURASAKI_SYSLOG(this, kfaAudioCodec, kseCritical, "Unknown value in parameter channel : %d ", channel)
            MURASAKI_ASSERT(false)
            break;
    }
    CODEC_SYSLOG("Leave.")
}

/*
 * Set mute status of specific channels.
 * Channel gains are not affected.
 */
void Adau1361::Mute(murasaki::CodecChannel channel, bool mute) {

    CODEC_SYSLOG("Enter. %d, %s ", channel, mute ? "true" : "false")

    switch (channel)
    {
        case murasaki::kccLineInput:
            line_input_mute_ = mute;
            SetLineInputGain(line_input_left_gain_, line_input_right_gain_, line_input_mute_);
            break;
        case murasaki::kccAuxInput:
            aux_input_mute_ = mute;
            SetAuxInputGain(aux_input_left_gain_, aux_input_right_gain_, aux_input_mute_);
            break;
        case murasaki::kccLineOutput:
            line_output_mute_ = mute;
            SetLineOutputGain(line_output_left_gain_, line_output_right_gain_, line_output_mute_);
            break;

        case murasaki::kccHeadphoneOutput:
            hp_output_mute_ = mute;
            SetHpOutputGain(hp_output_left_gain_, hp_output_right_gain_, hp_output_mute_);
            break;
        default:
            MURASAKI_SYSLOG(this, kfaAudioCodec, kseCritical, "Unknown value in parameter channel : %d ", channel)
            MURASAKI_ASSERT(false)
        break;
    }
    CODEC_SYSLOG("Leave.")
}





#define DATA 2  /* data payload of register */
#define ADDL 1  /* lower address of register */
#define ADDH 0  /* upper address of register */

#define SET_INPUT_GAIN( x, mute ) ((mute)? 0x00 : (x<<1))
/*
 * This function assumes the single-end input. The gain control is LINNG.
 * See Figure 31 "Record Signal Path" in the ADAU1361 data sheet
 *
 */
void Adau1361::SetLineInputGain(
                                float left_gain,
                                float right_gain,
                                bool mute) {
    uint8_t txbuf[3];
    uint8_t rxbuf[1];
    int left, right;

    CODEC_SYSLOG("Enter. %d, %d, %d", (int )left_gain, (int )right_gain, mute)

    // set left gain
    left = std::max(left, -12);
    left = std::min(left, 6);
    left = (left_gain + 15) / 3;  // See table 31 LINNG

    // set right gain
    right = std::max(right, -12);
    right = std::min(right, 6);
    right = (right_gain + 15) / 3;  // See table 31 LINNG

    /*
     *  *************** Read Modify light the Left Channel Gain *********************
     */
    // R4 : Record Mixer Left
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x0a;

    // Obtain the Register
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte  */
                              rxbuf, /* Retrieve data buffer */
                              1); /* retrieve a register */
    CODEC_SYSLOG("R4 : 0x%02x", rxbuf[0]);
    // Create a register value
    txbuf[DATA] = (rxbuf[0] & 0xF1) | SET_INPUT_GAIN(left, mute);
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);

    // Set the R4.
    i2c_->Transmit(device_addr_, txbuf, 3);  // R4: Record Mixer Left

    /*
     *  *************** Read Modify light the Right Channel Gain *********************
     */
    // R6 : Record Mixer 2
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x0c;

    // Obtain the Register
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte  */
                              rxbuf, /* Retrieve data buffer */
                              1); /* retrieve a register */
    CODEC_SYSLOG("R6 : 0x%02x", rxbuf[0]);
    // Create a register value
    txbuf[DATA] = (rxbuf[0] & 0xF1) | SET_INPUT_GAIN(right, mute);
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);

    // Set the R4.
    i2c_->Transmit(device_addr_, txbuf, 3);  // R4:  Record Mixer Right

    CODEC_SYSLOG("Leave.")
}

#define SET_AUX_GAIN( x, mute ) ((mute)? 0x00 : x )

/*
 * This function assumes the input is the single end. Then,
 */
void Adau1361::SetAuxInputGain(
                               float left_gain,
                               float right_gain,
                               bool mute) {
    uint8_t txbuf[3];
    uint8_t rxbuf[1];
    int left, right;

    CODEC_SYSLOG("Enter. %d, %d, %d", (int )left_gain, (int )right_gain, mute)

    // set left gain LDBOOST is muted.
    left = std::max(left, -12);
    left = std::min(left, 6);
    left = (left_gain + 15) / 3;  // See table 32 MX1AUGXG
    // set right gain. LDBOOST is muted.
    right = std::max(right, -12);
    right = std::min(right, 6);
    right = (right_gain + 15) / 3;  // See table 34 MX1AUGXG

    /*
     *  *************** Read Modify light the Left Channel Gain *********************
     */
    // R5 : Record Mixer Left Control 1
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x0b;

    // Obtain the Register
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte  */
                              rxbuf, /* Retrieve data buffer */
                              1); /* retrieve a register */
    CODEC_SYSLOG("Obtain R5 : 0x%02x", rxbuf[0]);
    // Create a register value
    txbuf[DATA] = (rxbuf[0] & 0xF8) | SET_AUX_GAIN(left, mute);
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);
    // Set the R5.
    i2c_->Transmit(device_addr_, txbuf, 3);  // R5:

    /*
     *  *************** Read Modify light the Right Channel Gain *********************
     */
    // R7 : Record Mixer Right Control 1
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x0d;

    // Obtain the Register
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte  */
                              rxbuf, /* Retrieve data buffer */
                              1); /* retrieve a register */
    CODEC_SYSLOG("Obtain R7 : 0x%02x", rxbuf[0]);
    // Create a register value
    txbuf[DATA] = (rxbuf[0] & 0xF8) | SET_AUX_GAIN(left, mute);
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);
    // Set the R7.
    i2c_->Transmit(device_addr_, txbuf, 3);  // R7:

    CODEC_SYSLOG("Leave.")
}

#define SET_LO_GAIN( x, unmute, headphone ) ((x<<2)|(unmute <<1) | (headphone))

// Read modify right the R31 and R32
void Adau1361::SetLineOutputGain(
                                 float left_gain,
                                 float right_gain,
                                 bool mute) {
    uint8_t txbuf[3];
    uint8_t rxbuf[1];
    int left, right;

    CODEC_SYSLOG("Enter. %d, %d, %d", (int )left_gain, (int )right_gain, mute)

    // set 0 if mute, set 1 if unmute;
    int unmute_flag = mute ? 0 : 1;

    // Calc left gain setting.
    // LOUTVOL[5:0] = 0  ; -57dB
    // LOUTVOL[5:0] = 63 ; 6dB
    left = left_gain + 57;
    left = std::max(left, 0);
    left = std::min(left, 63);

    // Calc left gain setting.
    // ROUTVOL[5:0] = 0  ; -57dB
    // ROUTVOL[5:0] = 63 ; 6dB
    right = right_gain + 57;
    right = std::max(right, 0);
    right = std::min(right, 63);

    /*
     *  *************** Read Modify light the Left Channel Gain *********************
     */
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x25;  // R31: LOUTVOL

    // Obtain the R31
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte : R31 */
                              rxbuf, /* Retrieve R31 data */
                              1); /* retrieve a register */
    CODEC_SYSLOG("R31 : 0x%02x", rxbuf[0]);

    // Set line out of Left
    txbuf[DATA] = SET_LO_GAIN(
                              left, /* GAIN */
                              unmute_flag, /* UNMUTE */
                              rxbuf[0] & 1); /* LOMODE of R31*/
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);
    // Set LOUTVOL : R31
    i2c_->Transmit(device_addr_, txbuf, 3);

    /*
     *  *************** Read Modify light the Right Channel Gain *********************
     */
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x26;  // R32: ROUTVOL

    // Obtain the R32
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte : R31 */
                              rxbuf, /* Retrieve R32 data */
                              1); /* retrieve a register */
    CODEC_SYSLOG("R32 : 0x%02x", rxbuf[0]);

    txbuf[DATA] = SET_LO_GAIN(
                              right, /* GAIN */
                              unmute_flag, /* UNMUTE */
                              rxbuf[0] & 1); /* ROMODE of R32 */
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);
    // Set ROUTVOL : R32
    i2c_->Transmit(device_addr_, txbuf, 3);

    CODEC_SYSLOG("Leave.")
}

#define SET_HP_GAIN( x, unmute, headphone ) ((x<<2)|( unmute <<1) | (headphone))

// Read modify right the R29 and R30

void Adau1361::SetHpOutputGain(
                               float left_gain,
                               float right_gain,
                               bool mute) {
    uint8_t txbuf[3];
    uint8_t rxbuf[1];
    int left, right;

    CODEC_SYSLOG("Enter. %d, %d, %d", (int )left_gain, (int )right_gain, mute)

    // set 0 if mute, set 1 if unmute;
    int unmute_flag = mute ? 0 : 1;

    // Calc left gain setting.
    // LHPVOL[5:0] = 0  ; -57dB
    // LHPVOL[5:0] = 63 ; 6dB
    left = left_gain + 57;
    left = std::max(left, 0);
    left = std::min(left, 63);

    // Calc left gain setting.
    // RHPVOL[5:0] = 0  ; -57dB
    // RHPVOL[5:0] = 63 ; 6dB
    right = right_gain + 57;
    right = std::max(right, 0);
    right = std::min(right, 63);

    /*
     *  *************** Read Modify light the Left Channel Gain *********************
     */
    // R29: LHPVOL
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x23;

    // Obtain R29
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte : R29 */
                              rxbuf, /* Retrieve R29 data */
                              1); /* retrieve a register1 */
    CODEC_SYSLOG("R29 : 0x%02x", rxbuf[0]);

    // HP left out control data
    txbuf[DATA] = SET_HP_GAIN(
                              left, /* GAIN */
                              unmute_flag, /* UNMUTE */
                              rxbuf[0] & 1); /* HPEN of R29*/
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);
    // SET LHPVOL : R29
    i2c_->Transmit(device_addr_, txbuf, 3);

    /*
     *  *************** Read Modify light the Left Channel Gain *********************
     */
    // R30 : RHPVOL address
    txbuf[ADDH] = 0x40;  // Upper address of register
    txbuf[ADDL] = 0x24;

    // Obtain R30
    i2c_->TransmitThenReceive(
                              device_addr_, /*  CODEC device address */
                              txbuf, /*  CODEC regsiter address*/
                              2, /* Address are 2byte : R30 */
                              rxbuf, /* Retrieve R30 data */
                              1); /* retrieve a register1 */
    CODEC_SYSLOG("R30 : 0x%02x", rxbuf[0]);

    // HP right out control data
    txbuf[DATA] = SET_HP_GAIN(
                              right, /* GAIN */
                              unmute_flag, /* UNMUTE */
                              rxbuf[0] & 1); /* HPMODE of R30*/
    CODEC_SYSLOG("Transmitting %02x, %02x, %02x", txbuf[0], txbuf[1], txbuf[2]);
    // SET RHPVOL : R30
    i2c_->Transmit(device_addr_, txbuf, 3);

    CODEC_SYSLOG("Leave.")
}

} /* namespace murasaki */


#endif //  HAL_I2C_MODULE_ENABLED
