/*
 * i2cmasterstub.cpp
 *
 *  Created on: Feb 27, 2021
 *      Author: takemasa
 */

#include <Thirdparty/i2cmasterstub.hpp>
#include "murasaki.hpp"

namespace murasaki {

I2cMasterStub::I2cMasterStub(int buf_count, int buf_size)
{
    // Create internal buffers.
    tx_data_buffer_ = newBuffers(buf_count, buf_size);
    MURASAKI_ASSERT(tx_data_buffer_ != nullptr)

    rx_data_buffer_ = newBuffers(buf_count, buf_size);
    MURASAKI_ASSERT(rx_data_buffer_ != nullptr)
}

I2cMasterStub::~I2cMasterStub()
{
    deleteBuffers(rx_data_buffer_);
    deleteBuffers(tx_data_buffer_);
}

murasaki::I2cStatus I2cMasterStub::Transmit(
                                            unsigned int addrs,
                                            const uint8_t *tx_data,
                                            unsigned int tx_size,
                                            unsigned int *transfered_count,
                                            unsigned int timeout_ms)
                                                    {
    // print address and write sign
    murasaki::debugger->Printf("%02x W ", addrs);
    // print data
    for (unsigned int i = 0; i < tx_size; i++)
        murasaki::debugger->Printf("0x02 ", tx_data[i]);
    // then, line feed
    murasaki::debugger->Printf("\n");

    return( murasaki::ki2csOK);

}

murasaki::I2cStatus I2cMasterStub::Receive(
                                    unsigned int addrs,
                                    uint8_t *rx_data,
                                    unsigned int rx_size,
                                    unsigned int *transfered_count,
                                    unsigned int timeout_ms )
{
	MURASAKI_ASSERT(false);
   return( murasaki::ki2csUnknown);
}

I2cMasterStub::DataBuffer* I2cMasterStub::newBuffers(int buf_count, int buf_size)
                                                     {
    // create a new Buffer structure.
    DataBuffer *buffers = new DataBuffer;

    // Initializing the buffer to store the tx data.
    buffers->buffer_num_ = buf_count;
    buffers->buffer_size_ = buf_size;
    buffers->buffers_ = new uint8_t*[buffers->buffer_num_];         // allocate pointer array.
    for (int i = 0; i < buffers->buffer_num_; i++)                  // allocate buffers
        buffers->buffers_[i] = new uint8_t[buffers->buffer_size_];
    buffers->data_sizes_ = new int[buffers->buffer_num_];           // Allocate the data size variables.
    buffers->write_index_ = 0;                                      // No buffer is used.
    buffers->read_index_ = 0;                                      // Next buffer to read.

    return buffers;
}

int I2cMasterStub::getNumTxBuffer()
{
    return tx_data_buffer_->write_index_;
}

void I2cMasterStub::clearTxBuffer()
{
    tx_data_buffer_->write_index_ = 0;
    tx_data_buffer_->read_index_ = 0;

}

void I2cMasterStub::clearRxBuffer()
{
    rx_data_buffer_->write_index_ = 0;
    rx_data_buffer_->read_index_ = 0;
}

void I2cMasterStub::writeRxBuffer(uint8_t *data, int length)
                                  {

    int widx = rx_data_buffer_->write_index_;

    // Overflow check
    MURASAKI_ASSERT(widx < rx_data_buffer_->buffer_num_)
    // Overflow check inside a buffer
    MURASAKI_ASSERT(length <= rx_data_buffer_->buffer_size_)
    // And then copy from data to buffer.
    memcpy(rx_data_buffer_->buffers_[widx],
           data,
           length
           );
    rx_data_buffer_->write_index_++;
}

void I2cMasterStub::readTxBuffer(uint8_t *data, int max_length, int *length)
                                 {
    int ridx = tx_data_buffer_->read_index_;
    int widx = tx_data_buffer_->write_index_;

    // Underflow check
    MURASAKI_ASSERT(ridx < widx)

    *length = tx_data_buffer_->data_sizes_[ridx];
    // Overflow inside a check
    MURASAKI_ASSERT(max_length <= *length)

    // And then copy from buffer to data
    memcpy(data,
           tx_data_buffer_->buffers_[ridx],
           *length
           );
    tx_data_buffer_->read_index_++;
}

void I2cMasterStub::deleteBuffers(DataBuffer *buffers)
                                  {
    // release buffers.
    for (int i = 0; i < buffers->buffer_num_; i++)
        delete[] (buffers->buffers_[i]);
    // release pointer array.
    delete[] (buffers->buffers_);
    // release counter array.
    delete[] (buffers->data_sizes_);

}

murasaki::I2cStatus I2cMasterStub::TransmitThenReceive(
                                                unsigned int addrs,
                                                const uint8_t *tx_data,
                                                unsigned int tx_size,
                                                uint8_t *rx_data,
                                                unsigned int rx_size,
                                                unsigned int *tx_transfered_count,
                                                unsigned int *rx_transfered_count,
                                                unsigned int timeout_ms )
{
	MURASAKI_ASSERT(false);
   return( murasaki::ki2csUnknown);

}

void I2cMasterStub::writeTxBuffer(uint8_t *data, int length)
                                  {

    int widx = tx_data_buffer_->write_index_;

    // Overflow check
    MURASAKI_ASSERT(widx < tx_data_buffer_->buffer_num_)
    // Overflow check inside a buffer
    MURASAKI_ASSERT(length <= tx_data_buffer_->buffer_size_)
    // And then copy from data to buffer
    memcpy(tx_data_buffer_->buffers_[widx],
           data,
           length
           );
    tx_data_buffer_->write_index_++;
}

void I2cMasterStub::readRxBuffer(uint8_t *data, int max_length, int *length)
                                 {
    int ridx = rx_data_buffer_->read_index_;
    int widx = rx_data_buffer_->write_index_;

    // Underflow check
    MURASAKI_ASSERT(ridx < widx)

    *length = rx_data_buffer_->data_sizes_[ridx];
    // Overflow check inside a buffer
    MURASAKI_ASSERT(max_length <= *length)

    // And then copy from buffer to data
    memcpy(data,
           rx_data_buffer_->buffers_[ridx],
           *length
           );
    rx_data_buffer_->read_index_++;
}

} /* namespace murasaki */
