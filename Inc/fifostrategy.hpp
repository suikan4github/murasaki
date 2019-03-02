/**
 * @file fifostrategy.hpp
 *
 * @date 2018/02/26
 * @author Seiichi "Suikan" Horie
 * @brief Abstract class of FIFO
 */

#ifndef FIFOSTRATEGY_HPP_
#define FIFOSTRATEGY_HPP_

#include <ctype.h>
#include <cinttypes>

namespace murasaki {

/**
 * @brief Basic FIFO without thread safe.
 * @details
 * Foundemental FIFO. No blocking , not thread safe.
 *
 * The Put member function returns with "copied" data count.
 * If the internal buffer is full, it returns without copy data.
 *
 * The Get member funciton returns with "copied" data count and data.
 * If the internal buffer is empty, it returns without copy data.
 * @ingroup MURASAKI_ABSTRACT_GROUP
 */
class FifoStrategy
{
 public:
    FifoStrategy() = delete;
    /**
     * @brief Create an internal buffer
     * @param buffer_size Size of the internal buffer to be allocated [byte]
     * @details
     * Allocate the internal buffer with given buffer_size.
     * The contents is not initialized.
     */
    FifoStrategy(unsigned int buffer_size);
    /**
     * @brief Delete an internal buffer
     */
    virtual ~FifoStrategy();
    /**
     * @brief Put the data into the internal buffer.
     * @param data Data to be copied to the internal buffer
     * @param size Data count to be copied
     * @return The count of copied data. 0, if the internal buffer is full.
     */
    virtual unsigned int Put(uint8_t const data[], unsigned int size);
    /**
     * @brief Get the data from the internal buffer.
     * @param data Data buffer to receive from the internal buffer
     * @param size Size of the data parameter.
     * @return The count of copied data. 0, if the internal buffer is empty
     */
    virtual unsigned int Get(uint8_t data[], unsigned int size);
    /*
     * @brief Mark all the data inside the internal buffer as "not sent".
     */
    virtual void ReWind();
 protected:
    /**
     * @brief Size of the internal buffer [byte]
     */
    unsigned int const size_of_buffer_;
    /**
     * @brief The internal buffer. Allocated by constructor.
     */
    uint8_t * const buffer_;

 private:
    /**
     * @brief The index of where next data will be written.
     */
    unsigned int head_ = 0;
    /**
     * @brief The index of where the next data will be read.
     * @details
     * If head_ = tail_, buffer is empty.
     */
    unsigned int tail_ = 0;

};

} /* namespace murasaki */

#endif /* FIFOSTRATEGY_HPP_ */
