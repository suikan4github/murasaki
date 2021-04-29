/**
 * \file murasaki_0_intro.hpp
 *
 * \date 2020/03/19
 * \author Seiichi "Suikan" Horie
 * \brief Doxygen document file. No need to include.
 */

#ifndef MURASAKI_0_INTRO_HPP_
#define MURASAKI_0_INTRO_HPP_

/**
 * \page murasaki_ug_preface Preface
 * \details
 * Murasaki is a class library on the STM32Cube HAL and FreeRTOS.
 * By using Murasaki, you can program STM32 series quickly and easily. You can obtain the source code of the Murasaki Library from the <a href="https://github.com/suikan4github/murasaki">GitHub repository</a>.
 *
 * Murasaki has following design philosophies:
 *
 * \li \subpage intro_sec1
 * \li \subpage intro_sec2
 * \li \subpage intro_sec3
 * \li \subpage intro_sec4
 * \li \subpage intro_sec5
 * \li \subpage intro_sec6
 * \li \subpage intro_sec7
 * \li \subpage intro_sec8
 *
 * There are some other manuals of murasaki class library :
 *
 * \li \ref murasaki_ug_usage
 * \li \ref murasaki_pg
 * \li \ref MURASAKI_GROUP
 *
 * \page intro_sec1 Simplified IO
 * @details
 *The class types package the IO functions. For example, The murasaki::Uart class can receive a UART handle.
 *
 * \code
 * murasaki::UartStrategy * uart3 = new murasaki::Uart( &huart3 );
 * \endcode
 *
 * Where  huart3 is a UART port 3 handle, which is generated by the CubeIDE.
 *
 * The STM32Cube HAL is quite rich and flexible. On the other hand, it is quite large and complex.
 * The classes in Murasaki simplify it by letting flexibility beside.
 * For example, the murasaki::Uart class can support only the DMA transfer.
 * The polling-based transfer is not supported.
 * By giving up the flexibility, programming with Murasaki is more comfortable than using HAL directly.
 *
 * \page intro_sec2 Preemptive multi-task
 * \details
 * The Murasaki class library is built on FreeRTOS's preemptive configuration.
 * As a result, Murasaki is automatically aware of preemptive multi-task.
 * That means, Murasaki's classes don't use polling to wait for any event. Thus, a task can do some job while other tasks are waiting for the end of the IO operation.
 *
 * The multi-task programming helps to divide a big program into sub-units. This dividing is an excellent way to develop large programs easier.
 * And the more critical point, it is easier to maintain.
 *
 * \page intro_sec3 Synchronous IO
 * \details
 * The synchronous IO is one of the most critical features of Murasaki.
 * The peripheral wrapping class like murasaki::Uart provides a set of member functions to do the data transmission/receiving. Such the member functions are programmed as "synchronous" IO.
 *
 * The synchronous IO function doesn't return until each IO function finished.
 * For example, if you transmit 10bytes through the UART, the IO member function transmits the 10bytes data, and then, return.
 *
 * Note: Sometimes, the "completion" means the end of the DMA transfer session, rather than the accurate transmission of the last byte. In this case, the system generates a completion interrupt while the data is still in FIFO of the peripheral. Anyway, this is a hardware issue.
 *
 *Some member functions are restricted to use only in the task context to allow the synchronous and blocking IO.
 *
 * \page intro_sec4 Thread-safe IO
 * \details
 * The synchronous IO and the preemptive multi-task provide more accessible programming.
 * On the other hand, there is a possibility that two different tasks access one peripheral simultaneously.
 * This kind of access messes the peripheral's behavior.
 *
 * To prevent this condition, some peripheral wrapping class has an exclusive access mechanism by a mutex.
 *
 * By this mechanism, if two tasks try to transmit through one peripheral, one task is kept waiting until the other finished to transmit. This behavior is called blocking.
 *
 * \page intro_sec5 Versatile printf() logger
 * \details
 * Logging or "printf debug" is a strong tool in the embedded system development.
 * Murasaki has three levels of the printf debugging mechanism. One is the murasaki::debugger->Printf(),
 * the second is \ref MURASAKI_ASSERT macro. In addition to these two, \ref MURASAKI_SYSLOG macro is available.
 *
 * The murasaki::debugger->Printf() is flexible output mechanism which has several good
 * features :
 * \li printf() compatible parameters.
 * \li Task/interrupt bi-context operation
 * \li None-blocking logging by internal FIFO.
 * \li User configurable output port
 *
 * These features allow a programmer to do the printf() debug not only in the task context but also in the interrupt context.
 *
 * \page intro_sec6 Guard by assertion
 * \details
 * In addition to the murasaki::debugger->Printf(), programmer can use \ref MURASAKI_ASSERT macro.
 * This macro allows for easy assertion and logging. This macro uses the murasaki::debugger->Printf() internally.
 *
 * If the assertion failed, a message would be output to the debug port with the information of the source line number and file name.
 *
 * Murasaki class library is using the assertion widely.
 * As a result, the wrong context, wrong parameter, etc., will be reported to the debugger output.
 *
 * \page intro_sec7 System Logging
 * \details
 * \ref MURASAKI_SYSLOG provides the message output based on the level and filtering.
 * This mechanism is intended to help the Murasaki library development. But also the application can use this mechanism.
 *
 * \page intro_sec8 Configurable
 * \details
 * Murasaki can configure its basic parameter by customer. For example, you can change the size of the printf buffer.
 */

#endif /* MURASAKI_0_INTRO_HPP_ */
