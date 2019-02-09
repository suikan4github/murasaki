/**
 * \file murasaki_config.hpp
 *
 * \date 2018/01/03
 * \author takemasa
 * \brief Configuration file for platform.
 * \details To override the configuration, define the same name macro inside
 * application_config.hpp
 */

#ifndef MURASAKI_CONFIG_HPP_
#define MURASAKI_CONFIG_HPP_

#include <FreeRTOS.h>

// Read the overriding definitions by programmer.
#include <platform_config.hpp>

/**
 * \ingroup MURASAKI_DEFINITION_GROUP
 * \{
 */
// For debug.h **********************************************************
/**
 * \def PLATFORM_CONFIG_DEBUG_LINE_SIZE
 * \brief Size of one line[byte] in the debug printf.
 * \details
 * The array length to store the formatted string. Note that this array is a private
 * instance variable. Then, it will occupy the memory where the class is instantiated.
 * For example, if an object is instantiated in the heap, this line buffer will be
 * reserved in the heap.
 *
 * If the class is instantiated on the stack, the buffer will be reserved in the stack.
 *
 * To override the definition here, define same macro inside @ref platform_config.hpp.
 */
#ifndef PLATFORM_CONFIG_DEBUG_LINE_SIZE
#define PLATFORM_CONFIG_DEBUG_LINE_SIZE  256
#endif

/**
 * \def PLATFORM_CONFIG_DEBUG_BUFFER_SIZE
 * \brief Size[byte] of the circular buffer to be transmitted through the serial port.
 * \details
 * The circular buffer array length to copy the formatted strings before transmitting through the uart.
 *
 * To override the definition here, define same macro inside @ref platform_config.hpp.
 */
#ifndef PLATFORM_CONFIG_DEBUG_BUFFER_SIZE
#define PLATFORM_CONFIG_DEBUG_BUFFER_SIZE  4096
#endif

/**
 * \def PLATFORM_CONFIG_DEBUG_SERIAL_TIMEOUT
 * \brief Timeout of the serial port to transmit the string through the Debug class.
 * \details
 * By default, there is no timeout. Wait for eternally.
 *
 * To override the definition here, define same macro inside @ref platform_config.hpp.
 */
#ifndef PLATFORM_CONFIG_DEBUG_SERIAL_TIMEOUT
#define PLATFORM_CONFIG_DEBUG_SERIAL_TIMEOUT  (murasaki::kwmsIndefinitely)
#endif

// For task stack depth ************************************************
/**
 * \def PLATFORM_CONFIG_DEBUG_TASK_STACK_SIZE
 * \brief Size[Byte] of the task inside Debug class.
 * \details
 * The murasaki::Debuger class has internal task to handle its FIFO buffer.
 *
 * To override the definition here, define same macro inside @ref platform_config.hpp.
 */
#ifndef PLATFORM_CONFIG_DEBUG_TASK_STACK_SIZE
#define PLATFORM_CONFIG_DEBUG_TASK_STACK_SIZE  256
#endif

// For task priority ***************************************************
// Higher value is the higher priority.
/**
 * \def PLATFORM_CONFIG_DEBUG_TASK_PRIORITY
 * \brief The task proiority of the debug task.
 * \details
 * The priority of the murasaki::Debuger internal task. To output the logging data as fast as possible,
 * the debug taks have to have relatively high priority. In other hand, to yield the CPU to the
 * critical tasks, it's priority have to be smaller than the max priority.
 *
 * To override the definition here, define same macro inside @ref platform_config.hpp.
 */
#ifndef PLATFORM_CONFIG_DEBUG_TASK_PRIORITY
#define PLATFORM_CONFIG_DEBUG_TASK_PRIORITY  (( configMAX_PRIORITIES-1 > 0 ) ? configMAX_PRIORITIES-1 : 0)
#endif

// For assertion ******************************************************
/**
 * \def MURASAKI_CONFIG_NODEBUG
 * \brief Surpress \ref MURASAKI_ASSERT macro.
 * \details
 * Set this macro to true, to discard the assertion \ref MURASAKI_ASSERT. Set this macro false, to use the assertion.
 *
 *
 * To override the definition here, define same macro inside @ref platform_config.hpp.
 */
#ifndef MURASAKI_CONFIG_NODEBUG
#define MURASAKI_CONFIG_NODEBUG false
#endif

// For syslog ******************************************************
/**
 * \def MURASAKI_CONFIG_NOSYSLOG
 * \brief Surpress \ref MURASAKI_SYSLOG macro.
 * \details
 * Set this macro to true, to discard the  \ref MURASAKI_SYSLOG. Set this macro false, to use the syslog.
 *
 *
 * To override the definition here, define same macro inside @ref platform_config.hpp.
 */
#ifndef MURASAKI_CONFIG_NOSYSLOG
#define MURASAKI_CONFIG_NOSYSLOG false
#endif


/**
 * \}
 * end of the MURASAKI_PLATFORM_CONFIGURATION
 */

#endif /* MURASAKI_CONFIG_HPP_ */
