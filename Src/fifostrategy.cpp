/*
 * fifostrategy.cpp
 *
 *  Created on: 2018/02/26
 *      Author: takemasa
 */

#include <fifostrategy.hpp>
#include <murasaki_assert.hpp>
#include <algorithm>
#include <string.h>

namespace murasaki {

FifoStrategy::FifoStrategy(unsigned int buffer_size):
          size_of_buffer_(buffer_size),
          buffer_(new uint8_t[size_of_buffer_])
{
    MURASAKI_ASSERT(size_of_buffer_ != 0);        // very explicit check
    MURASAKI_ASSERT(buffer_ != nullptr);

    // set both pointer same value ( empty buffer ).
    tail_ = head_ = 0;
}


FifoStrategy::~FifoStrategy()
{
    if (buffer_ != nullptr)
        delete[] buffer_;
}

unsigned int FifoStrategy::Put(uint8_t const data[], unsigned int size)
{
    unsigned int avairable;

    // check the avairable area size. Data is append to the head
    if (head_ > tail_)  // there is no wrap around
        avairable = size_of_buffer_ - (head_ - tail_);
    else if (tail_ > head_)
        avairable = tail_ - head_;
    else
        avairable = size_of_buffer_;

    // If we fill up buffer entirely, we can not distinguish full and empty from the
    // comparing tail_ and head_.
    // This can be fixed if we add new variable "length", but it get complex.
    // To save this problem, we clip the vacant size by buffsersize-1.
    if (avairable > 0)
        avairable--;

    // clip the line size by available room in the buffer.
    unsigned int copy_size = std::min(size, avairable);

    // Now, we can copy from line to buffer.
    for (unsigned int i = 0; i < copy_size; i++) {
        // newest data is appended to the point of head_
        buffer_[head_] = data[i];
        head_++;

        // wrap around the head index;
        if (head_ >= size_of_buffer_)
            head_ -= size_of_buffer_;
    }  // end for loop.

    return copy_size;

}

unsigned int FifoStrategy::Get(uint8_t data[], unsigned int size)
{
    unsigned int copy_size = 0;

    // To avoid race condition, Take a snapshot of the head_ and tail_ pointer.
    // head_ could be modified during this task is running but no problem.
    // check whether the data area is wrapped araound at the end of buffer. Note : Data is added to the head.

    if (head_ == tail_)
        copy_size = 0;  // nothing to copy
    else {
        if (head_ > tail_)
            copy_size = std::min(size, head_ - tail_);
        else
            // tail_ > head_
            copy_size = std::min(size, size_of_buffer_ - tail_);    // stop once, at the end of buffer.

        // copy data to output buffer
        ::memcpy(data, &buffer_[tail_], copy_size);
        // update tail.
        tail_ += copy_size;

        // if tail_ reaches the end, wrap around.
        if (tail_ >= size_of_buffer_)
            tail_ -= size_of_buffer_;
    }


    return copy_size;
}

void FifoStrategy::ReWind()
{
    // by setting tail as head+1, the entire buffer is marked as "not sent"
    tail_ = head_ + 1;

    // wrap arround;
    if (tail_ >= size_of_buffer_)
        tail_ -= size_of_buffer_;
}

} /* namespace murasaki */
