/**
 * \file murasaki_assert.hpp
 *
 * \date 2018/01/31
 * \author Seiichi "Suikan" Horie
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
 *  Print the COND expression to the logging port if COND is false. Do nothing if CODN is true.
 *
 *  After printing the assertion failure message, this aspersion triggers the Hard Fault exception.
 *  The Hard Fault Exception is caught by @ref HardFault_Handler() and eventually invoke the murasaki::debugger->DoPostMortem(), to put the system into the post mortem debug mode.
 *
 *  The following code in the macro definition calls a non-existing function located address 1.
 *  Such access causes a hard fault exception.
 *  @code
 *          { void (*foo)(void) = (void (*)())1; foo();}\
*  @endcode
 *
 *  This assertion does nothing if the programmer defines \ref URASAKI_CONFIG_NODEBUG macro as true.
 *  This macro is defined in the file \ref platform_config.hpp.
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
        { void (*foo)(void) = (void (*)())1; foo();}\
    }
#endif

/**
 * \def MURASAKI_PRINT_ERROR
 * \param ERR Condition as bool type.
 * \brief Print ERR if ERR is true
 * \details
 * Print the ERR expression to the logging port if COND is true. Do nothing if ERR is true.
 *
 * This assertion does nothing if the programmer defines \ref MURASAKI_CONFIG_NODEBUG macro as true.
 * This macro is defined in the file \ref platform_config.hpp.
 *
 * For example, the following code is a typical usage of this macro. ERROR macro is copied from STM32Cube HAL source code.
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
 * \brief A global variable to provide the debugging function.
 * \details
 * This variable is declared by murasaki platform, but not instantiated.
 * To make it happen, a programmer has to make a variable and initialize it explicitly.
 * Otherwise, some debug utility/macro may cause link error because urasaki::debugger is called from these utility/macros.

 * \ingroup MURASAKI_PLATFORM_GROUP
 *
 */

extern Debugger *debugger;
}

#endif /* MURASAKI_ASSERT_HPP_ */
