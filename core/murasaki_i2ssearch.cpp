/*
 * murasaki_i2ssearch.cpp
 *
 *  Created on: 2020/03/01
 *      Author: takemasa
 */

#include "murasaki.hpp"

void murasaki::I2cSearch(murasaki::I2cMasterStrategy *master)
                         {
    uint8_t tx_buf[1];

    murasaki::debugger->Printf("\n            Probing I2C devices \n");
    murasaki::debugger->Printf("   | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
    murasaki::debugger->Printf("---+------------------------------------------------\n");

    // Search raw
    for (int raw = 0; raw < 128; raw += 16) {
        // Search column
        murasaki::debugger->Printf("%2x |", raw);
        for (int col = 0; col < 16; col++) {
            murasaki::I2cStatus result;
            // check whether device exist or not.
            result = master->Transmit(raw + col, tx_buf, 0);
            if (result == murasaki::ki2csOK)  // device acknowledged.
                murasaki::debugger->Printf(" %2X", raw + col);  // print address
            else if (result == murasaki::ki2csNak)  // no device
                murasaki::debugger->Printf(" --");
            else
                murasaki::debugger->Printf(" ??");  // unpredicted error.
        }
        murasaki::debugger->Printf("\n");
    }

}

