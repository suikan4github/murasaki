/**
 * \file debugger.hpp
 *
 * \date 2018/01/03
 * \author takemasa
 * \brief Debug print class. For both ISR and task.
 * \details
 * This class serves printf function for both task context and ISR context.
 */

#ifndef DEBUGGER_HPP_
#define DEBUGGER_HPP_

#include <FreeRTOS.h>
#include <loggerstrategy.hpp>
#include <task.h>
#include <semphr.h>

#include "murasaki_config.hpp"
#include "debuggerfifo.hpp"
#include "debuggertxtask.hpp"
#include "debuggerautoreprinttask.hpp"



namespace murasaki {

/**
 * \ingroup MURASAKI_GROUP
 * \brief Debug class. Provides printf() style output for both task and ISR context.
 * \details
 * Wrapper class to help the printf debug. The printf() method can be called from both
 * task context and ISR context.
 *
 * There are several configurable parameters of this class:
 * \li \ref PLATFORM_CONFIG_DEBUG_BUFFER_SIZE
 * \li \ref PLATFORM_CONFIG_DEBUG_LINE_SIZE
 * \li \ref PLATFORM_CONFIG_DEBUG_TASK_STACK_SIZE
 * \li \ref PLATFORM_CONFIG_DEBUG_TASK_PRIORITY
 * \li @ref PLATFORM_CONFIG_DEBUG_SERIAL_TIMEOUT
 *
 * See \ref MURASAKI_PLATFORM_GROUP as example this class.
 *
 */
class Debugger
{
 public:
    /**
     * \brief Constructor. Create internal variable.
     * \param logger The pointer to the \ref LoggerStrategy wrapper class variable.
     */
    Debugger(LoggerStrategy* logger);
    /**
     * \brief Deconstructor. Delete internal variable.
     */
    virtual ~Debugger();

    /**
     * \brief Debug output function.
     * \param fmt Format string
     * \param ... optional parameters
     * \details
     * The printf() compatible method. This method can be called from both task context and ISR context.
     * This method internally calls sprintf() variant. So, the parameter processing is fully compatible with
     * with printf().
     *
     * The formatted string is stored in the internal circular buffer. And data inside buffer is
     * transmitted through the uart which is passed by constructor. If the buffer is overflowed,
     * this method streos as possible, and discard the rest of string. That mean, this method  is
     * not blocking.
     *
     * This member function is non-blocking, thread safe and re-entrant.
     *
     * Be careful, this is non-blocking while the Debug::getchFromTask() is blocking.
     *
     * At 2018/Jan/14 measurement, task stack was consumed 49bytes.
     */
    void Printf(const char * fmt, ...);

    /**
     * \brief Receive one character from serial port.
     * \return Received character.
     * \details
     * A blcoking function which returns received character. The receive is done on the
     * UART which is passed to the constructor.
     *
     * This is thread safe and task context dedicated function. Never call from ISR.
     *
     * Becareful, this is blocking while the Debug::Printf() non-blocking.
     */
    char GetchFromTask();
    /**
     * @brief Print the old data again.
     * @details
     * Must call from task context.
     * For each time this member function is called, old data in the buffer is re-sent again.
     *
     * The data to be re-setn is the one in the data in side circular buffer.
     * Then, the resent size is same as  \ref PLATFORM_CONFIG_DEBUG_BUFFER_SIZE .
     *
     */
    void RePrint();
    /**
     * @brief Print history automatically.
     * @details
     * Once this member function is called, internally new task is created.
     * This new task watches input by @ref GetchFromTask() and for each input char is
     * recevied, trigger the @ref RePrint().
     *
     * This auto reprint funciton is exclusive and irreversible.
     * Once auto reprint is triggered, there is no way to stop the auto reprint.
     * The second call for the AutoHistory  may be ignored
     *
     * This member function have to be called from task context.
     */
    void AutoRePrint();
    /**
     * \brief Start the post mortem mprocessing. Never return
     */
    void DoPostMortem();


 protected:
    const murasaki::LoggingHelpers helpers_;
    /**
     * \brief Handle to the transmission control task.
     */
    murasaki::DebuggerTxTask * const tx_task_;

    /**
     * @brief Handle to the auto reprint task.
     */
    murasaki::DebuggerAutoRePrintTask * auto_reprint_task;

    /**
     * @brief For protecting from double enabled.
     */
    bool auto_reprint_enable_;

    /**
     * @brief as receiver for the snprintf()
     * @details
     * This variable can be local variable of the printf() member function.
     * In thiss case, the implementation of the printf() is much easier.
     * In the other hand, each task must has enough depth on its task stack.
     *
     * Probably, having bigger task for each task doesn't pay, and it may cuase stack overflow bug
     * at the debug or assertion. This is not preferable.
     */
    char line_[PLATFORM_CONFIG_DEBUG_LINE_SIZE];

    /**
     * @brief Syslog severity threshold
     * @details
     * All seveirity level lower than this value will be ignored by Syslog() function. Note that
     * murasaki::kseEmergency is the highest and murasaki::kseDebug is the lowerest seveirty.
     *
     * By default, the severity level threshold is murasaki::kseError. That mean, the weaker severity
     * than kseError is ignored.
     */
    murasaki::SyslogSeverity severity_;
    /**
     * @brief Syslog facility filter mask
     * @details
     * If certain bit is "1", the corresponding Syslog facility is allowed to output. By default
     * the value is 0xFFFF ( equivalent to SyslogAllowAllFacilities(0xFFFFFFFF) )
     */
    uint32_t facility_mask_;
};

extern Debugger * debugger;


} /* namespace murasaki */



#endif /* DEBUGGER_HPP_ */
