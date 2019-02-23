/**
 * \file murasaki_assert.hpp
 *
 * \date 2018/01/31
 * \author takemasa
 * \brief Assertion definition
 */

#ifndef MURASAKI_ASSERT_HPP_
#define MURASAKI_ASSERT_HPP_

#include <debugger.hpp>
#include "murasaki_config.hpp"
#include "murasaki_defs.hpp"
#include <string.h>

#define MURASAKI_ASSERT_MSG "!! Assertion failure in function %s(), at line %d of file %s !!\n"
#define MURASAKI_ERROR_MSG  "Error in function %s(), at line %d of file %s : %s \n"

// Remove directory path from __FILE__
#ifndef __MURASAKI__FILE__
#define __MURASAKI__FILE__  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

/**
 * \def MURASAKI_ASSERT
 * \param COND Condition as bool type.
 * \brief Assert the COND is true.
 * \details
 * Print the COND expression to the logging port if COND is false. Do nothing if CODN is true.
 *
 * After printing the assertion failure message, currently running task is suspended.
 * If it is the interrupt context, just continue the processing.
 *
 * This assertion do nothing if programmer defines \ref MURASAKI_CONFIG_NODEBUG macro as true.
 * This macro is defined in the file \ref platform_config.hpp.
 *
 * \ingroup MURASAKI_GROUP
 */
#if MURASAKI_CONFIG_NODEBUG
#define MURASAKI_ASSERT( COND )
#else
#define MURASAKI_ASSERT( COND )\
    if ( ! (COND) )\
    {\
        murasaki::debugger->Printf("--------------------\n");\
        murasaki::debugger->Printf(MURASAKI_ASSERT_MSG,  __func__, __LINE__,__MURASAKI__FILE__ );\
        murasaki::debugger->Printf("Fail expression : %s\n", #COND);\
        if ( murasaki::IsTaskContext() )\
            vTaskSuspend(nullptr);\
    }
#endif

/**
 * \def MURASAKI_PRINT_ERROR
 * \param ERR Condition as bool type.
 * \brief Print ERR if ERR is true
 * \details
 * Print the ERR expression to the logging port if COND is true. Do nothing if ERR is true.
 *
 * This assertion do nothing if programmer defines \ref MURASAKI_CONFIG_NODEBUG macro as true.
 * This macro is defined in the file \ref platform_config.hpp.
 *
 * For example, following code is typical usage of this macro. ERROR maccro is copied from STM32Cube HAL source code.
 * @code
 * bool Uart::HandleError(void* const ptr)
 * {
 *     MURASAKI_ASSERT(nullptr != ptr)
 *
 *     if (peripheral_ == ptr) {
 *         // Check error, and print if exist.
 *         MURASAKI_PRINT_ERROR(peripheral_->ErrorCode & HAL_UART_ERROR_DMA);
 *         MURASAKI_PRINT_ERROR(peripheral_->ErrorCode & HAL_UART_ERROR_PE);
 *         MURASAKI_PRINT_ERROR(peripheral_->ErrorCode & HAL_UART_ERROR_NE);
 *         MURASAKI_PRINT_ERROR(peripheral_->ErrorCode & HAL_UART_ERROR_FE);
 *         MURASAKI_PRINT_ERROR(peripheral_->ErrorCode & HAL_UART_ERROR_ORE);
 *         MURASAKI_PRINT_ERROR(peripheral_->ErrorCode & HAL_UART_ERROR_DMA);
 *         return true;    // report the ptr matched
 *     }
 *     else {
 *         return false;   // report the ptr doesn't match
 *     }
 * }
 * @endcode
 *
 * \ingroup MURASAKI_GROUP
 */
#if MURASAKI_CONFIG_NODEBUG
#define MURASAKI_PRINT_ERROR( ERR )
#else
#define MURASAKI_PRINT_ERROR( ERR )\
    if ( (ERR) )\
    {\
        murasaki::debugger->Printf(MURASAKI_ERROR_MSG,  __func__, __LINE__,__MURASAKI__FILE__, #ERR );\
    }
#endif


namespace murasaki {
/**
 * \brief Grobal variable to provide the debugging function.
 * \details
 * This variable is declared by murasaki platform. But not instantiated. To make it happen,
 * programmer have to make an variable and initialize it explicitly. Otherwise,
 * Certain debug utility/macro may cause link error, because murasaki::debugger is refered
 * by these utility/macros.
 *
 * \ingroup MURASAKI_PLATFORM_GROUP
 *
 */

extern Debugger* debugger;
}

#endif /* MURASAKI_ASSERT_HPP_ */
