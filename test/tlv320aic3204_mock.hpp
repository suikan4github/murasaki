#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "tlv320aic3204.hpp"


class MockI2c : public murasaki::I2cMasterStrategy {
    public:
    MOCK_METHOD5( Transmit, murasaki::I2cStatus(
                                        unsigned int addrs,
                                        const uint8_t *tx_data,
                                        unsigned int tx_size,
                                        unsigned int *transfered_count,
                                        unsigned int timeout_ms
                                        ));
    MOCK_METHOD4( Transmit, murasaki::I2cStatus(
                                        unsigned int addrs,
                                        const uint8_t *tx_data,
                                        unsigned int tx_size,
                                        unsigned int *transfered_count
                                        ));
    MOCK_METHOD3( Transmit, murasaki::I2cStatus(
                                        unsigned int addrs,
                                        const uint8_t *tx_data,
                                        unsigned int tx_size
                                        ));

    MOCK_METHOD5( Receive, murasaki::I2cStatus(
                                        unsigned int addrs,
                                        uint8_t *rx_data,
                                        unsigned int rx_size,
                                        unsigned int *transfered_count,
                                        unsigned int timeout_ms
    ));
    MOCK_METHOD4( Receive, murasaki::I2cStatus(
                                        unsigned int addrs,
                                        uint8_t *rx_data,
                                        unsigned int rx_size,
                                        unsigned int *transfered_count
    ));
    MOCK_METHOD3( Receive, murasaki::I2cStatus(
                                        unsigned int addrs,
                                        uint8_t *rx_data,
                                        unsigned int rx_size
    ));
    
    MOCK_METHOD8(  TransmitThenReceive, murasaki::I2cStatus(
                                                    unsigned int addrs,
                                                    const uint8_t *tx_data,
                                                    unsigned int tx_size,
                                                    uint8_t *rx_data,
                                                    unsigned int rx_size,
                                                    unsigned int *tx_transfered_count,
                                                    unsigned int *rx_transfered_count,
                                                    unsigned int timeout_ms
    ));
    MOCK_METHOD7(  TransmitThenReceive, murasaki::I2cStatus(
                                                    unsigned int addrs,
                                                    const uint8_t *tx_data,
                                                    unsigned int tx_size,
                                                    uint8_t *rx_data,
                                                    unsigned int rx_size,
                                                    unsigned int *tx_transfered_count,
                                                    unsigned int *rx_transfered_count
    ));
    MOCK_METHOD6(  TransmitThenReceive, murasaki::I2cStatus(
                                                    unsigned int addrs,
                                                    const uint8_t *tx_data,
                                                    unsigned int tx_size,
                                                    uint8_t *rx_data,
                                                    unsigned int rx_size,
                                                    unsigned int *tx_transfered_count
    ));
    MOCK_METHOD5(  TransmitThenReceive, murasaki::I2cStatus(
                                                    unsigned int addrs,
                                                    const uint8_t *tx_data,
                                                    unsigned int tx_size,
                                                    uint8_t *rx_data,
                                                    unsigned int rx_size
    ));


    MOCK_METHOD1( TransmitCompleteCallback, bool(void *ptr)) ;
    MOCK_METHOD1( ReceiveCompleteCallback, bool(void *ptr)) ;
    MOCK_METHOD1( HandleError, bool(void *ptr)) ;

    protected:
    MOCK_METHOD0( GetPeripheralHandle, void *()) ;

};
