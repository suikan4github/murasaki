/**
 * @file debuggerfifo.hpp
 *
 * @date 2018/03/01
 * @author takemasa
 * @brief Dedicated FIFO to logging the debug message.
 */

#ifndef DEBUGGERFIFO_HPP_
#define DEBUGGERFIFO_HPP_

#include <fifostrategy.hpp>
#include <loggerstrategy.hpp>
#include "synchronizer.hpp"

namespace murasaki {

/**
 * @brief FIFO with thread safe.
 * @details
 * Non blocking , thread safe FIFO
 *
 * The Put member function returns with "copied" data count.
 * If the internal buffer is full, it returns without copy data.
 * This is thread safe and ISR/Task bi-modal.
 *
 * The Get member funciton returns with "copied" data count and data.
 * If the internal buffer is empty, it returns without copy data.
 *
 * @ingroup MURASAKI_HELPER_GROUP
 */
class DebuggerFifo : public FifoStrategy
{
 public:
    /**
     * @brief Create an internal buffer
     * @param buffer_size Size of the internal buffer to be allocated [byte]
     * @details
     * Allocate the internal buffer with given buffer_size.
     * The buffer contents is initialized by blank.
     */
    DebuggerFifo(unsigned int buffer_size);
    /**
     * @brief Delete an internal buffer
     */
    virtual ~DebuggerFifo();
    /**
     * @brief Notyify new data is in the buffer, to the receiver task.
     */
    virtual void NotifyData();
    /**
     * @brief Get the data from the internal buffer. This is thread safe function. Do not call from ISR.
     * @param data Data buffer to receive from the internal buffer
     * @param size Size of the data parameter.
     * @return The count of copied data. 0, if the internal buffer is empty
     */
    virtual unsigned int Get(uint8_t data[], unsigned int size);
    /**
     * @brief Mark all the data inside the internal buffer as "not sent". Thread safe.
     */
    virtual void ReWind();
    /**
     * @brief Transit to the post mortem mode.
     * @details
     * In this mode, FIFO doesn't sync between the put and get method.
     * Actually, this mode assumes nobody send messayge by Put()
     *
     */
    virtual void SetPostMortem();
 private:
    // Alias to call the parent member function.
    typedef FifoStrategy inherited;
    // For the communication between generator / consumer.
    Synchronizer * const sync_;
    bool post_mortem_;
};

/**
 * @brief A stracture to engroup the logging tools.
 * @ingroup MURASAKI_HELPER_GROUP
 */
struct LoggingHelpers
{
    murasaki::DebuggerFifo * fifo;
    murasaki::LoggerStrategy * logger;

};
}
;
/* namespace murasaki */

#endif /* DEBUGGERFIFO_HPP_ */
