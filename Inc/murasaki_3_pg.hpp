/**
 * @file murasaki_3_pg.hpp
 *
 * @date May 25, 2018
 * @author Seiichi "Suikan" Horie
 * @brief Porting Guide
 */

#ifndef MURASAKI_3_PG_HPP_
#define MURASAKI_3_PG_HPP_

/**
 * \page murasaki_pg Porting guide
 * \brief This porting guide introduces murasaki class library porting step by step.
 * \details
 *
 * In this guide, user will study the library porting to the STM32 microcomputer
 * system working with STM32Cube HAL.
 *
 * Followings are the contents of this porting guide :
 *
 * \li \subpage murasaki_pg_directory
 * \li \subpage murasaki_pg_cubemx
 * \li \subpage murasaki_pg_configuration
 * \li \subpage murasaki_pg_task_priority_and_stack
 * \li \subpage murasaki_pg_heap
 * \li \subpage murasaki_pg_platform_varialbe
 * \li \subpage murasaki_pg_interrupt
 * \li \subpage murasaki_pg_error
 * \li \subpage murasaki_pg_summary
 *
 * There are some other manuals of murasaki class library :
 *
 * \li \ref murasaki_ug_preface
 * \li \ref murasaki_ug_usage
 * \li \ref MURASAKI_GROUP
 *
 */


/**
 * \page murasaki_pg_directory Directory Structure
 * \brief Murasaki has four main directory and several user-modifiable files.
 * \details
 *
 * This page describes these directories and files.
 *
 * \section sec_ds_1 Src directory
 * Almost files of the Murasaki source code are stored in this directory. Basically,
 * there is no need to edit the files inside this directory, except the development of
 * Murasaki itself. The project setting must
 * refer this directory as the source directory.
 *
 * \section sec_ds_1_2 Inc directory
 * This directory contains the include files, the project setting must
 * refer this directory as an include directory.
 *
 * \section sec_ds_1_5 Src-tp and Inc-tp directory
 * The class collection of the third party peripherals. The "third party" means, the
 * outside of the microprocessor.
 *
 * Currently these directories are not utilized.
 *
 * \section sec_ds_2 murasaki.hpp
 * Usually, the \ref murasaki.hpp include file is the only one to include from an application program.
 * By including this file, an application can refer all the definition of the Murasaki
 *
 * This file is stored in the Inc directory.
 *
 * \section sec_ds_3 template directory
 * \subsection sec_ds_3_1 platform_config.hpp
 * The \ref platform_config.hpp file is a collection of the build configuration.
 * By defining a macro, a programmer can change the behavior of the Murasaki.
 *
 * There are mainly two types of the configuration in this file.
 *
 * One type of configuration is to override the \ref murasaki_config.hpp file.
 * All contents of the murasaki_config.hpp are macros.
 * These macros are defined to control  the Murasaki, for example:
 * the task priority, the task stack size or the timeout period,
 * described in the \ref MURASAKI_DEFINITION_GROUP.
 *
 * The other configuration type is the assertion inside Murasaki.
 * See \ref MURASAKI_CONFIG_NODEBUG for details.
 *
 * The platform_config.hpp is better to be copied in the /Inc directory of the application.
 * The @ref sec_ds_4 will copy this file to /Src directory of application for programmer.
 *
 * \subsection sec_ds_3_2 platform_defs.hpp
 * As same as \ref platform_config.hpp, the \ref platform_defs.hpp is not the core part
 * of the Murasaki class library.
 * This include file has a definition of the \ref murasaki::platform
 * which provide "nice looking" aggregation of the class objects.
 *
 * The application programmer can define the \ref murasaki::Platform type freely.
 * There is no limitation or requirement what you put into unless compiler reports an error message.
 *
 * On the other hand, a programmer may find that adding the peripheral-based class variables and
 * middleware based class variables into the murasaki::Platform type is reasonable.
 * Actually, the independent devices ( ie:I2C connected LCD controller ) may be
 * better to be a member variable of the mruasaki::Platform type.
 *
 * The platform_defs.hpp is better to be copied in the /Inc directory of the application.
 * The @ref sec_ds_4 will copy this file to /Src directory of application for programmer.
 *
 * See \ref MURASAKI_PLATFORM_GROUP as usage sample.
 *
 * @subsection sec_ds_3_3 murasaki_platform.hpp
 * A header file of the @ref murasaki_platform.cpp. This file is better to be copied in the
 * /Inc directory of the application.
 * The @ref sec_ds_4 will copy this file to /Src directory of application for programmer.
 *
 * @subsection sec_ds_3_4 murasaki_platform.cpp
 * The murasaki_platform.cpp is the interface between the application and the HAL/RTOS.
 * This file has variables / functions which user needs to program at porting time.
 * @li @ref murasaki::platform variable
 * @li @ref murasaki::debugger variable
 * @li \ref InitPlatform() to initialize the platform variable
 * \li \ref ExecPlatform() to execute the platform algorithm
 * \li \link murasaki_pg_interrupt Interrupt routing functions \endlink
 * \li \link murasaki_pg_error HAL assertion function and Custome default exception handler \endlink
 *
 * The murasaki_platform.cpp is better to be copied in the /Src directory of the application.
 * The @ref sec_ds_4 will copy this file to /Src directory of application for programmer.
 *
 * \section sec_ds_4 install script
 * The install script have mainly 4 tasks.
 * @li Copy template files to the appropriate application directories from @ref sec_ds_3
 * @li Modify main.c to call the @ref InitPlatform() and @ref ExecPlatform() from the default task.
 * @li Modify main.c to call the @ref CustomAssertFailed() from the HAL assertion
 * @li Modify the hard fault handler to call the CustomDefaultHandler()
 * @li Generate @ref murasaki_include_stub.h to let the Murasaki library to include HAL headers.
 *
 * Last one is little tricky to do it manually. Refer @ref murasaki_include_stub.h for details.
 */



/**
 * \page murasaki_pg_cubemx CubeMX setting
 * \brief There is several required CubeMX setting.
 * \details
 *
 * \li @subpage sec_cm_1
 * \li @subpage sec_cm_2
 * \li @subpage sec_cm_3
 * \li @subpage sec_cm_4
 * \li @subpage sec_cm_5
 * \li @subpage sec_cm_6
 * \li @subpage sec_cm_7
 * \li @subpage sec_cm_8
 *
 *
 * @page sec_cm_1 Heap Size
 * @brief Heap is very important in the application with murasaki.
 *
 * First, class instances are created inside heap region by new operator often.
 * And second, murasaki::Debugger allocates a huge size of FIFO buffer.
 * This  buffer stays in between the murasaki::Debugger::Printf() function and the logger task.
 * The size of this FIFO buffer is defined by @ref PLATFORM_CONFIG_DEBUG_BUFFER_SIZE. The default is 4KB.
 *
 * Usually, the heap is simply called "heap", without precise definition of terminology.
 * But let's call it "system heap" here.
 * The system heap is the one which is managed by new and delete operators by default.
 *
 * In addition to the system heap, FreeRTOS has its own heap.
 * This heap is managed separately from the system heap.
 * This management includes the heap size watching and returning error.
 * And this heap is thread safe while the system heap is not.
 *
 * Using two heap is not easy.
 * And definitely, the FreeRTOS heap is better than the system heap in the embedded application.
 * So, in murasaki, the new and the delete operators are overloaded and redirected to the FreeRTOS heap.
 * See @ref murasaki_pg_heap for detail.
 *
 * To avoid the heap allocation problem, it is better to have more than 8kB FreeRTOS heap.
 * The FreeRTOS heap size can be changed by CubeMX :
 * @code
 * Tab => Pinout & Configuration => Middleware => FreeRTOS => Config Parameters Tab => TOTAL_HEAP_SIZE
 * @endcode
 *
 * On the other hand, the system heap size can be smaller like 128 Byte because we don't use it..
 *
 * Note that to know the minimum requirement of the system heap size, you must investigate how much allocations are done before entering FreeRTOS.
 * Because murasaki application doesn't use any system heap, only very small management memory should be required in system heap.
 *
 * The system Heap size can be set by following place.
 * @code
 * Tab => Project Manager => Code Generator => Linker Settings
 * @endcode
 *
 *
 * \page sec_cm_2 Stack Size
 * @brief In this section, the stack means the interrupt stack.
 *
 * The interrupt stack is used only when the interrupt is accepted. Then, it is basically small.
 *
 * By the way, murasaki uses its assertion often.
 * Once assertion fails, a message is created by snprintf() function and transmitted through FIFO.
 * These operations consume stack.
 * And assertion can be happen also in the ISR context.
 *
 * The debugging in the ISR is not easy without assertion and printf().
 * To make them always possible, it is better to set the interrupt stack size bigger than 256 Bytes.
 * The interrupt stack size can be changed by CubeMX :
 * @code
 * Tab => Project Manager => Code Generator => Linker Settings
 * @endcode
 *
 *
 * @page sec_cm_3 Task stack size of the default task
 * @brief The dealt task has very small stack ( 128 Bytes )
 *
 * This is not enough to use murasaki and its debugger output functionality.
 * It should be increased at smallest 256 Bytes.
 *
 * It can be changed by CubeMX:
 * @code
 * Tab => Pinout & Configuration => Middleware => FreeRTOS => Config Parameters Tab => MINIMAL_STACK_SIZE
 *
 * @endcode
 *
 *
 * @page sec_cm_4 UART peripheral
 * @brief UART/USART peripheral have to be configured as Asynchronous mode.
 *
 * The DMA have to be enabled for both TX and RX. Both DMA must be normal mode.
 *
 * All i3 of the NVIC interrupt have to be enabled.
 *
 * @page sec_cm_5 SPI Master peripheral
 * @brief SPI Master peripheral have to be configured as Full-Duplex Master mode.  The NSS must be disabled.
 *
 * The DMA have to be enabled for both TX and RX. Both DMA must be normal mode.
 *
 * All 3 of the NVIC interrupt have to be enabled.
 *
 * @page sec_cm_6 SPI Slave peripheral
 * @brief SPI Slave peripheral have to be configured as Full-Duplex Slave mode.  The NSS must be input signal.
 *
 * The DMA have to be enabled for both TX and RX. Both DMA must be normal mode.
 *
 * All 3 of the NVIC interrupt have to be enabled.
 *
 * @page sec_cm_7 I2C peripheral
 * @brief I2C have to be configured as "I2" mode.
 *
 * The NVIC interrupt have to be enabled.
 *
 * To configure as I2C device, the primary slave address have to be configured.
 *
 * @page sec_cm_8 EXTI
 * @brief The corresponding interrupt have to be enabled by NVIC.
 *
 *
 */

/**
 * \page murasaki_pg_configuration Configuration
 * \brief Murasaki has configurable parameters.
 * \details
 *
 * These parameters control mainly the task size and task priority.
 *
 * One of the special configurations is \ref MURASAKI_CONFIG_NODEBUG macro.
 * This macro controls whether assertion inside Murasaki source code works or ignored.
 *
 * To customize the configuration, define the configuration macro with the desired value in the
 *  platform_config.hpp file.
 * This definition will override the Murasaki default configuration.
 *
 * For the detail of each macro, see \ref MURASAKI_DEFINITION_GROUP.
 */

/**
 * @page murasaki_pg_task_priority_and_stack Task Priority and Stack Size
 * @brief The FreeRTOS task priority is allowed from 1 to configMAX_PRIORITIES.
 * @details
 *
 * Where configMAX_PRIORITIES is porting dependent. The task with priority == configMAX_PRIORITIES will run with
 * the highest priority among all tasks.
 *
 * At the initial state, the Murasaki has two hidden tasks inside.
 * Both are running for the murasaki::Debugger class, and both task's priority are defined as @ref PLATFORM_CONFIG_DEBUG_TASK_PRIORITY.
 * By default, the value of PLATFORM_CONFIG_DEBUG_TASK_PRIORITY is configMAX_PRIORITIES - 1.
 * That means, debug tasks priority is very high.
 *
 * The debug tasks should have priority as high as possible.
 * Otherwise, another task may block the debugging message.
 *
 * Unlike the task priority, the interrupt priority is easy.
 * Usually, it is not so sensitive because the ISR is very short in the good designed RTOS application design.
 * In this case, all ISR can be a same priority.
 *
 * In the bad designed RTOS application, there are very few things we can do.
 */

/**
 * \page murasaki_pg_heap Heap memory consideration
 * \brief In Murasaki, there is a re-definition of @ref operator new and @ref operator delete inside \ref allocators.cpp.
 * @details
 * This re-definition let the pvPortMalloc() allocate
 * a fragment of memory for the @ref operator new.
 *
 * This changes converges all allocation to the FreeRTOS's heap.
 * There is some merit of the convergence:
 * @li The FreeRTOS heap is thread safe while the system heap in SW4STM32 is not thread-safe
 * @li The FreeRTOS heap is checking the heap size limitation and return an error, while the system heap behavior in SW4STM32  is not clear.
 * @li The heap size calculation is easier if we integrate the memory allocation activity into one heap.
 *
 * On the other hand, FreeRTOS heap is not able to allocate/deallocate in the
 * ISR context. And it is impossible to use the FreeRTOS heap before starting up the FreeRTOS. Then,
 * we have to follow the rules here :
 * @li C++ new / delete operators have to be called after FreeRTOS started.
 * @li C++ new / delete operators have to be called in the task context.
 *
 */

/**
 * \page murasaki_pg_platform_varialbe Platform variable
 * \brief The murasaki::platform and the murasaki::debugger have to be initialized by the InitPlatform() function.
 * @details
 *
 * The programming of this function is a responsibility of the porting programmer.
 *
 * First of all, the porting programmer has to make the peripheral handles as visible from the murasaki_platform.cpp.
 *
 * For example, CubeMx generate the huart2 for Nucleo L152RE for the serial communication over the ST-LINK USB connection.
 * huart2 is defined in main.c as like below:
 * @code
 * UART_HandleTypeDef huart2;
 * DMA_HandleTypeDef hdma_usart2_rx;
 * DMA_HandleTypeDef hdma_usart2_tx;
 * @endcode
 *
 * To use this handle, the porting programmer has to declare the same name as an external variable, in the
 * murasaki_platform.cpp :
 * @code
 * extern UART_HandleTypeDef huart2;
 * @endcode
 *
 * After these preparations, the porting programmer can program the InitPlatform() :
 * @code
 * void InitPlatform()
 * {
 *     // UART device setting for console interface.
 *     // On Nucleo, the port connected to the USB port of ST-Link is
 *     // referred here.
 *     murasaki::platform.uart_console = new murasaki::Uart(&huart2);
 *     // UART is used for logging port.
 *     // At least one logger is needed to run the debugger class.
 *     murasaki::platform.logger = new murasaki::UartLogger(murasaki::platform.uart_console);
 *     // Setting the debugger
 *     murasaki::debugger = new murasaki::Debugger(murasaki::platform.logger);
 *     // Set the debugger as AutoRePrint mode, for the easy operation.
 *     murasaki::debugger->AutoRePrint();  // type any key to show history.
 *
 *     // For demonstration, one GPIO LED port is reserved.
 *     // The port and pin names are fined by CubeMX.
 *     murasaki::platform.led = new murasaki::BitOut(LD2_GPIO_Port, LD2_Pin);
 *
 *
 * }
 * @endcode
 *
 *
 * In this sample, we initialize the uart_console member variable which is AbstractUart class.
 * The applicaiton programmer control the UART2 over this uart_console member variable.
 *
 * In the second step, we pass this uart_cosole to the logger member variable.
 * This member variable is an essential stub for the murasaki::debugger.
 * In this example, we assign the UART2 port as interface for the debugging output.
 *
 * After the logger becomes ready, we initialize the murasaki::debugger.
 * As we already discussed, this debugger receives a logger object as a parameter.
 * The debugger output all messages through this logger.
 *
 * The last step is optional.
 * We invoke the murasaki::Debugger::AutoRePrint() member function.
 * By calling this function, logger re-print the old data in the FIFO again whenever
 * the end-user type any key of the keyboard.
 *
 * This "auto re-print by any key" is convenient in the small system.
 * But for the large system which has its own command line shell, this input-interruption is harmful.
 * For such the system, programmer want to call murasaki::Debugger::RePrint() member function, by certain customer command.
 *
 * Once the debugger is ready to use, we create the led member variable
 * as a general purpose output port of the application .
 *
 * The ExecPlatform() function implements the actual algorithm of application.
 * In the example below, the application is blinking a LED and printing a messages on the console output.
 *
 * @code
 * void ExecPlatform()
 * {
 *     // counter for the demonstration.
 *     static int count = 0;
 *
 *     // Loop forever
 *     while (true) {
 *         // Toggle LED.
 *         murasaki::platform.led->Toggle();
 *
 *         // print a message with counter value to the console.
 *         murasaki::debugger->Printf("Hello %d \n", count);
 *
 *         // update the counter value.
 *         count++;
 *
 *         // wait for a while
 *         murasaki::Sleep(static_cast<murasaki::WaitMilliSeconds>(500));
 *     }
 * }
 * @endcode
 *
 * Finally, above two functions have to be called from StartDefaultTask of the main.c.
 * Also, main.c must include the murasaki_platform.hpp to read the prototype of these functions.
 *
 * Following is the sample of the StartDefaultTask(). The actual code have a comment to work
 * together the code generator of the CubeMX. But this sample remove them because of the
 * documenattion tool ( doxygen ) limitation.
 * @code
 * void StartDefaultTask(void const * argument)
 * {
 *
 *   InitPlatform();
 *   ExecPlatform();
 *
 *   for(;;)
 *   {
 *     osDelay(1);
 *   }
 * }
 * @endcode
 */



/**
 * \page murasaki_pg_interrupt Routing interrupts
 * \brief The murasaki_platform.cpp has skeletons of HAL callback.
 * @details
 *
 * These callbacks are pre-defined inside HAL as receptors of interrupt.
 * These definitions inside HAL are "weak" binding. Thus, these skeletons in murasaki_platform.cpp overrides the definition.
 * The porting programmer have to program these skeltons correctly.
 *
 * In the Murasaki manner, the skeletons have to call the relevant callback member function of platform variables.
 * For example, this is the typical programming of the call back :
 * @code
 * void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart)
 * {
 *     if (murasaki::platform.uart_console->TransmitCompleteCallback(huart))
 *         return;
 *
 * }
 * @endcode
 *
 * In this sample, the TxCpltCallback() calles murasaki::platform.uart_console->TransmitCompleteCallback() member funciton.
 * And then return if that member function returns true. Note that all the callacks in the
 * Murasaki class returns true if the given peripheral handle matches with its internal handle.
 * Thus, this is good way to poll all the UART peripheral inside this callback function.
 *
 * Following is the list of the interrupts which applicaiton have to route to the peripehral class variables.
 *
 * \li void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart);
 * \li void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart);
 * \li void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
 * \li void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
 * \li void HAL_SPI_ErrorCallback(SPI_HandleTypeDef * hspi);
 * \li void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c);
 * \li void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef * hi2c);
 * \li void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef * hi2c);
 * \li void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef * hi2c);
 * \li void HAL_I2C_ErrorCallback(I2C_HandleTypeDef * hi2c);
 * \li void HAL_GPIO_EXTI_Callback(uint16_t GPIO_P);
 *
 */

/**
 * \page murasaki_pg_error Error handling
 * \brief The murasaki_platform.cpp has two error handling functions.
 * @details
 *
 * These functions are pre-programmed from the first.
 * And usually its enough to use the pre-programmed version.
 * In the other hand the porting programmer have to modify the application program to call these
 * error handling functions at appropriate situation.
 * Otherwise, these error handling functions will be never called.
 *
 * The @ref CustomAssertFailed() function should be called from the @ref assert_failed() function.
 * The assert_failed() function is located in the @ref main.c.
 * Modifying the assert_failed() is the responsibility of the porting programmer.
 *
 * @code
 * void assert_failed(uint8_t* file, uint32_t line)
 * {
 *     CustomAssertFailed(file, line);
 * }
 * @endcode
 *
 * To enable the assert_failed(), the porting programmer have to uncomment the @ref USE_FULL_ASSERT macro
 * inside stm32xxxx_hal_conf.h. The file name is depend on the target microprocessor.
 * Thus, the porting programmer have to search the all files inside project.
 *
 * At the time of 2019/May, this definition is in the one for the following files :
 * @li stm32f0xx_hal_conf.h
 * @li stm32f3xx_hal_conf.h
 * @li stm32f7xx_hal_conf.h
 * @li stm32l1xx_hal_conf.h
 *
 *
 * The @ref CustomDefaultHandler() function should be called from the default exception routine.
 * But the system default exception handler ( Default_Handler ) doesn't do anything by default.
 * To maximize the information to the JTAG debugger, this is programmed as very simple eternal loop.
 *
 * The default exception handler can be programmed or left untouched as porting programmer want.
 * It is up to the system policy.
 * If it is re-programmed to call the CustomDefaultHandler(), murasaki::debugger object take the control
 * of the debug message FIFO at the exception handler context.
 *
 * If the exception happened and the CustomDefaultHandler is called, the end user can see the
 * entire messages in the debug FIFO by typing any key of the keyboard.
 * This is useful to see the last message from the assertion.
 * The last message usually represent the cause of the exception.
 * The end user can debug the application program based on this last assertion message.
 *
 * The HAL default exception routine is programmed at startup/startup_stm32xxxxx.s by assembly language.
 *
 * The porting programmer can modify it as below, to call the CustomDefaultHandler();
 * @code
 * Default_Handler:
 * Infinite_Loop:
 *   bl CustomDefaultHandler
 *
 *   b  Infinite_Loop
 *   .size  Default_Handler, .-Default_Handler
 * @endcode
 *
 */

/**
 * \page murasaki_pg_summary Summary of the porting
 * Following is the porting steps :
 * @li Adjust heap size and stack size as described in the @ref murasaki_pg_cubemx
 * @li Generate an application skeleton from CubeMX.
 * @li Checkout Murasaki repository into your project.
 * @li Copy the template files as described in the @ref murasaki_pg_directory .
 * @li Configure Muraaski as described in the @ref murasaki_pg_configuration and the @ref murasaki_pg_task_priority_and_stack
 * @li Call InitPlatform() and ExecPlatform() as described @ref murasaki_pg_platform_varialbe.
 * @li Route the interrupts as described @ref murasaki_pg_interrupt.
 * @li Route the error handling as described @ref murasaki_pg_error
 */


#endif /* MURASAKI_3_PG_HPP_ */

