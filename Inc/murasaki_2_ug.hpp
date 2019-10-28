/**
 * \file murasaki_2_ug.hpp
 *
 * \date 2018/02/01
 * \author Seiichi "Suikan" Horie
 * \brief Doxygen document file. No need to include.
 */

#ifndef MURASAKI_2_UG_HPP_
#define MURASAKI_2_UG_HPP_

/**
 * \page murasaki_ug_usage Usage Introduction
 * \brief In this introduction, we see how to use  Murasaki class library in the STM32 program.
 * \details
 *
 * In this seciton, we see fowling issues :
 * \li \subpage ug_sec_1
 * \li \subpage ug_sec_2
 * \li \subpage ug_sec_3
 * \li \subpage ug_sec_4
 * \li \subpage ug_sec_5
 * \li \subpage ug_sec_6
 *
 * For the easy-to-understand description, we assumes several things on the application skeleton which we are going to use Murasaki :
 * \li The application skeleton is generated by <a href= "http://www.st.com/ja/development-tools/stm32cubemx.html"> CubeMX </a>
 * \li The application skeleton is configured to use FreeRTOS
 * \li UART3 is configured to work with DMA.
 *
 *
 *
 * \page ug_sec_1 Message output
 * \brief The Murasaki library has a Printf() like message output mechanism.
 *
 * This mechanism is easy way to display a message from an embedded microcomputer to the
 * terminal simulator like kermit on a host computer. Murasaki's Printf() is based on the
 * standard C language formating library. So, programmer can output a message as like standard printf().
 *
 * As usual, let's start from "hello, world".
 *
 * \code
 * murasaki::debugger->Printf("Hello, world!\n");
 * \endcode
 *
 * In Murasaki manner, the Printf() is not a global function. This is a method of murasaki::Debugger class.
 * The murasaki::debugger variable is a one of two Murasaki's golobal variable. And it provide an easy
 * to use message output.
 *
 * The end-of-line charater is depend on the terminal. In the above sample, the terminator is \n.
 * This is for the linux based kermit. Other terminal system may need other end-of-line character.
 *
 * Because the Printf() works as like standard printf(), you can also use the format string.
 *
 * \code
 * murasaki::debugger->Printf("count is %d\n", count);
 * \endcode
 *
 * The Printf() is designed as debugger message output for an embeded realtime system.
 * Thenk this function is :
 * \li Thread safe
 * \li Blocking
 * \li Buffered
 *
 * In the other word, you can use this function in either task or interrupt handler without bothering
 * the real time process.
 */

/**
 * \page ug_sec_2 Serial communication
 * \brief murasaki::Uart is the asynchronous serial communication.
 *
 * The initial baud rate, parity and data size are defined by CubeMX.
 * So, there is no need to initialize the communication parameter in application program.
 * User can transmit data by just passing its address and size.
 *
 *
 * @code
 *        uint8_t data[5] = { 1, 2, 3, 4, 5 };
 *        murasaki::UartStatus stat;
 *
 *        stat = murasaki::platform.uart->Transmit(
 *                                                 data,
 *                                                 5);
 *
 * @endcode
 *
 * Beside of transmit, also Receive() member function exists.
 *
 */

/**
 * \page ug_sec_3 Debugging with Murasaki.
 * \brief As we saw, Murasaki has a simple messaging output for real-time debugging.
 *
 * This feature is
 * typically used as UART serial output, but configurable by the programmer.
 *
 * The murasaki::debugger is the useful variable to output the debugging message.
 * murasaki::debugger->prrntf() has several good feature.
 * \li Versatile printf() style format string.
 * \li Can call from both task and interrupt context
 * \li Non-blocking
 *
 * These features help the programmer to display the message in the real-time, multi-task application.
 *
 * In addition to this simple debugging variable, a programmer can use assert_failure() function of the STM32 HA.
 * The STM32Cube HAL has assert_failure() to check the parameter on the fly.
 * By default, this function is disabled.
 * To use this function, programmer have to make it enable, and add function to receive the
 * debug information.
 *
 * To enable the assert_failuer(), edit the stm32fxx_hal_conf.h in the Inc directory. This
 * file is generated by CubeMX. You can find USE_FULL_ASERT macro as comment out.
 * By declaring this macro, assert_failure is enabled.
 *
 * \code
 * #define USE_FULL_ASSERT    1
 * \endcode
 *
 * And then, you should modify assert_failure() in main.c, to call output function
 * (Note, this modification is altered by the install script. See @ref spg_6 of the @ref spg.
 * Still USE_FULL_ASSERT macro is a responsibility of the porting programmer ).
 *
 * \code
 * void assert_failed(uint8_t* file, uint32_t line)
 * {
 *     CustomAssertFailed(file, line);  // debugging stub.
 * }
 * \endcode
 *
 * This hook calls CustomAssertFailed() function.
 *
 * \code
 * // Hook for the assert_failure() in main.c
 * void CustomAssertFailed(uint8_t* file, uint32_t line)
 * {
 *     murasaki::debugger->Printf("Wrong parameters value: file %s on line %d\n", file, line);
 * }
 *
 * \endcode
 * Once above programming is done, you can watch the integrity of the HAL parameter by
 * reading the console output.
 *
 * Above debugging mechanism redirects all HAL assertion, Murasaki assertion and
 * application debug message to the specified logging port.
 * That logging port is able to customize.
 * In the case of the User's Guide, logging is done through the UART port.
 *
 * Time by time, you may not want to connect a serial terminal to the board unless you have a problem.
 * That means when you find a problem and connect your serial terminal, the assertion
 * message is already transmitted ( and lost ).
 *
 * Murasaki can save this problem.
 * By adding the following code after creating murasaki::Debugger instance, you can use history functionality.
 *
 * @code
 *     murasaki::debugger->AutoHistory();
 * @endcode
 *
 * The murasaki::Debugger::AutoHistory() creates a dedicated task for auto history function.
 * This task watch the input from the logging port.
 * Again, in this User's guide it is UART.
 * Once any character is received from the logging port ( terminal ),
 * previously transmitted message is sent again.
 * So you can read the last tens of messages.
 *
 * The auto history is handy, but it blocks all input from the terminal.
 * If you want to have your own console program through the debug port input,
 * do not you the auto history. Alternatively, you can send the previously transmitted message again,
 * by calling murasaki::Debugger::PrintHistory() explicitly.
 *
 * Murasaki also have post-mortem debugging feature which helps to analyze severe error.
 * Murasaki adds a hook into the Default_Handler of the startup_stm32****.s file.
 *
 * @code
 *     .section  .text.Default_Handler,"ax",%progbits
 *     .global CustomDefaultHandler
 * Default_Handler:
 *   bl CustomDefaultHandler
 * Infinite_Loop:
 *   b  Infinite_Loop
 * @endcode
 *
 * The inserted bl instruction supersedes the infinite loop at spurious interrupt handler.
 * Alternatively, CustomDefaultHandler() is called. The CustomDefaultHandler() stops entire
 * Debugger process, and get into the polling mode serial operation with auto history.
 *
 * That mean, once spurious interrupt happen, you can read the messages in the debug
 * message FIFO by pressing any key. This feature helps to analyze the assertion message
 * just before the trouble.
 *
 */

/**
 * \page ug_sec_4 Tasking
 * @brief murasaki::SimpleTask is a type of the task of the FreeRTOS.
 *
 * By using murasaki::SimpleTask, a programmer can easily create a task object.
 * This object encapsulate the task of the FreeRTOS.
 *
 * First of all, you must define a task body function.
 * Any function name is acceptable, Only the return type and parameter type is specified.
 *
 *
 * @code
 * // Task body of the murasaki::platform.task1
 * void TaskBodyFunction(const void* ptr)
 *                       {
 *
 *     while (true)    // dummy loop
 *     {
 *         murasaki::platform.led2->Toggle();  // toggling LED
 *         murasaki::Sleep(700);
 *     }
 * }
 * @endcode
 *
 * Then, create a Task object.
 *
 * There are several parameter to pass for the constructor.
 * The first parameter is the name of the task in FreeRTOS.
 * The second one is the task stack size. This size is depend on the
 * task body function. The third one is the priority of the new task.
 * This bigger value is the higher priority. The fourth one is the pointer
 * to the task parameter. This parameter is passed to the task function body.
 * And then, the last one is the pointer to the task body function.
 *
 * @code
 *     // For demonstration of FreeRTOS task.
 *     murasaki::platform.task1 = new murasaki::SimpleTask(
 *                                                   "Master",
 *                                                   256,
 *                                                   (( configMAX_PRIORITIES > 1) ? 1 : 0),
 *                                                   nullptr,
 *                                                   &TaskBodyFunction
 *                                                   );
 * @endcode
 *
 * Once task object is created, you must call Start() member function to start the task.
 *
 * @code
 *    murasaki::platform.task1->Start();
 * @endcode
 *
 * Then, task you can call Start() member function to run.
 */

/**
 * @page ug_sec_5 Other peripheral
 * @brief This section shows samples of the other peripherals.
 * @li @subpage ug_sect_5_1
 * @li @subpage ug_sect_5_2
 * @li @subpage ug_sect_5_3
 * @li @subpage ug_sect_5_4
 * @li @subpage ug_sect_5_5
 */

/**
 * @page ug_sect_5_1 I2C Master
 * @brief \ref murasaki::I2cMaster class provides the serial communication
 *
 * The I2C master is easy to use. To send a message to the slave device, you need to specify the slave address in 7bits, pointer to data and data size in byte.
 * @code
 *        uint8_t data[5] = { 1, 2, 3, 4, 5 };
 *        murasaki::I2cStatus stat;
 *
 *        stat = murasaki::platform.i2cMaster->Transmit(
 *                                                      127,
 *                                                      data,
 *                                                      5);
 * @endcode
 *
 * In addition to the Transmit(), murasaki::I2cMaster class has Receive(), and TransmitThenReceive() member function.
 */

/**
 * @page ug_sect_5_2 I2C Slave
 * @brief \ref murasaki::I2cSlave class provides the I2C slave function.
 *
 * The I2C slave is much easier than master, because it doesn't need to specify the slave address. The I2C slave device address is given by CubeMX.
 * @code
 *        uint8_t data[5];
 *        murasaki::I2cStatus stat;
 *
 *        stat = murasaki::platform.i2cSlave->Receive(
 *                                                    data,
 *                                                    5);
 * @endcode
 * In addition to the Transmit(), murasaki::I2cSlave class has Receive() member function.
 *
 */

/**
 * @page ug_sect_5_3 SPI Master
 * @brief @ref murasaki::SpiMaster is the SPI master class of Murasaki.
 *
 * This class is more complicated than other peripherals, because of flexibility.
 * The SPI master controller must adapt to the several variation of the SPI communication.
 * \li CPOL configuration
 * \li CPHA configuration
 * \li GPIO port configuration to select a slave
 *
 * The flexibility to above configurations need special mechanism. In Murasaki, this flexibility is responsibility of the murasaki::SpiSlaveAdapter class.
 * This class holds these configuration. Then, passed to the master class.
 *
 * So, you must create a murasaki::SpiSlaveAdapter class object, at first.
 * @code
 *         // Create a slave adapter. This object specify the protocol and slave select pin
 *         murasaki::SpiSlaveAdapterStrategy * slave_spec;
 *         slave_spec = new murasaki::SpiSlaveAdapter(
 *                                                    murasaki::kspoFallThenRise,
 *                                                    murasaki::ksphLatchThenShift,
 *                                                    SPI_SLAVE_SEL_GPIO_Port,
 *                                                    SPI_SLAVE_SEL_Pin
 *                                                    );
 * @endcode
 *
 * Then, you can pass the SpiSlaveAdapter class object to the murasaki::SpiMaster::TransmitAndRecieve() function.
 * @code
 *         // Transmit and receive data through SPI
 *         uint8_t tx_data[5] = { 1, 2, 3, 4, 5 };
 *         uint8_t rx_data[5];
 *         murasaki::SpiStatus stat;
 *         stat = murasaki::platform.spiMaster->TransmitAndReceive(
 *                                                                 slave_spec,
 *                                                                 tx_data,
 *                                                                 rx_data,
 *                                                                 5);
 * @endcode
 */

/**
 * @page ug_sect_5_4 SPI Slave
 * @brief @ref murasaki::SpiSlave class provides the SPI slave functionality.
 *
 * This class encapsulate the SPI slave function.
 * @code
 *         // Transmit and receive data through SPI
 *         uint8_t tx_data[5] = { 1, 2, 3, 4, 5 };
 *         uint8_t rx_data[5];
 *         murasaki::SpiStatus stat;
 *         stat = murasaki::platform.spiSlave->TransmitAndReceive(
 *                                                                tx_data,
 *                                                                rx_data,
 *                                                                5);
 * @endcode
 */

/**
 * @page ug_sect_5_5 GPIO
 * @brief @ref murasaki::BitOut and murasaki::BitIn provides the GPIO functionality

 * Following is the example of the murasaki::BitOut class.
 * @code
 *         // Toggle LED.
 *         murasaki::platform.led->Toggle();
 * @endcode
 * In addition to the Toggle(), BitIn has Set() and Clear() member function.
 */

/**
 * @page ug_sec_6 Program flow
 * @brief In this section, we see the program flow of a Murasaki application.
 *
 * Murasaki has 3 program flows. The start point of these flows are always inside
 * CubeMX generated code. 2 out of 3 flows are for debugging. Only 1 flow have to be
 * understood well by an application programmer.
 *
 * @li @subpage ug_sect_6_1
 * @li @subpage ug_sect_6_2
 * @li @subpage ug_sect_6_3
 * @li @subpage ug_sect_6_31
 * @li @subpage ug_sect_6_4
 * @li @subpage ug_sect_6_5
 */

/**
 * @page ug_sect_6_1 Application flow
 * @brief  The application program flow is the main flow of a Murasaki application.
 *
 * @details
 * This program flow starts from the StartDefaultTask() in the Src/main.c.
 * The StartDefaultTas() is a default and first task created by CubeMX.
 * In the other words, this task is automatically created without configuration.
 *
 * From this function, two Murasaki function is called. One is InitPlatoform().
 * The other is ExecPlatform(). Note that both function calls are inserted by
 * installer. See @ref spg_6 of the @ref spg for details.
 *
 * @code
 * void StartDefaultTask(void const * argument)
 * {
 *
 *   // USER CODE BEGIN 5
 *     InitPlatform();
 *     ExecPlatform();
 *   // Infinite loop
 *   for(;;)
 *   {
 *     osDelay(1);
 *   }
 *   // USER CODE END 5
 * }
 * @endcode
 *
 * The InitPlatform() function is defined in the Src/murasaki_platform.cpp.
 * Because the file extention is .cpp, the murasaki_platfrom.cpp is compiled
 * by C++ compiler while the main.c is compiled by C compiler.
 * This allows programmer uses C++ language. Thus, the InitPlatform() is the
 * good place to initialize the class based variables.
 *
 * As the name suggests, InitPlatform() is where programmer initialize the
 * platform variables murasaki::platform and murasaki::debugger.
 * @code
 * void InitPlatform()
 * {
 *    // UART device setting for console interface.
 *    // On Nucleo, the port connected to the USB port of ST-Link is
 *    // referred here.
 *    murasaki::platform.uart_console = new murasaki::DebuggerUart(&huart3);
 *    // UART is used for logging port.
 *    // At least one logger is needed to run the debugger class.
 *    murasaki::platform.logger = new murasaki::UartLogger(murasaki::platform.uart_console);
 *    // Setting the debugger
 *    murasaki::debugger = new murasaki::Debugger(murasaki::platform.logger);
 *    // Set the debugger as AutoRePrint mode, for the easy operation.
 *    murasaki::debugger->AutoRePrint();  // type any key to show history.


 *    // For demonstration, one GPIO LED port is reserved.
 *    // The port and pin names are fined by CubeMX.
 *    murasaki::platform.led = new murasaki::BitOut(LD2_GPIO_Port, LD2_Pin);

 *    // For demonstration of master and slave I2C
 *    murasaki::platform.i2c_master = new murasaki::I2cMaster(&hi2c1);

 *    murasaki::platform.sync_with_button = new murasaki::Synchronizer();

 *    // For demonstration of FreeRTOS task.
 *    murasaki::platform.task1 = new murasaki::SimpleTask(
 *                                                  "Master",
 *                                                  256,
 *                                                  1,
 *                                                  nullptr,
 *                                                  &TaskBodyFunction
 *                                                  );

 *    // the Following block is just for sample.

 * }
 * @endcode
 *
 * In this sample, the first half of the InitPlatform() is building a murasaki::debugger
 * variable. Because this variable is utilized for the debugging of the entire application,
 * there is a value to make it at first.
 *
 * Probably the most critical statement in this part is the creation of the DebuggerUart class object.
 *
 * @code
 *    murasaki::platform.uart_console = new murasaki::DebuggerUart(&huart3);
 * @endcode
 *
 * In this statement, the DebgguerUart receives the pointer to the huart3 as a parameter.
 * The hauart3 is a handle variable of the UART3 generated by CubeMx. Let's remind the
 * UART3 is utilized as communication path through the USB. So, in this sample code,
 * we are making debugging console through the USB-serial line of the Nucleo F722ZE board.
 *
 * Because the huart3 is generated into the main.c directory, we have to declare this
 * variable as an external variable. You can find the declaration around the top of the
 * Src/murasaki_platform.cpp.
 *
 * @code
 * extern UART_HandleTypeDef huart3;
 * @endcode
 *
 * Note that the UART port number varies among the different Nucleo board. So, the
 * porting programmer have a responsibility to refer the right UART.
 *
 * The second half of the InitPlatform() is the creation part of the other peripheral
 * object. This part fully depends on the application. A programmer can define any
 * object, by modifying the murasaki::Platform struct in the Inc/platform_defs.hpp.
 *
 * The second function called from the @ref StartDefaultTask() is the @ref ExecPlatform().
 * This function is also defined in the @ref Src/murasaki_platform.cpp.
 *
 *
 * @code
 * void ExecPlatform()
 * {

 *     murasaki::platform.task1->Start();


 *     // print a message with counter value to the console.
 *     murasaki::debugger->Printf("Push user button to display the I2C slave device \n");


 *     // Loop forever
 *     while (true) {
 *         murasaki::platform.sync_with_button->Wait();
 *         I2cSearch(murasaki::platform.i2c_master);

 *     }
 * }
 *  @endcode
 *
 * This function is the body of application. So, you can read GPIO, ADC other peripherals.
 * And output to the DAC, GPIO, and other peripherals from here.
 *
 */

/**
 * @page ug_sect_6_2 HAL Assertion flow
 * @brief HAL Assertion is a STM32Cube HAL's programming help mechanism.
 *
 * STM32Cube HAL provies a run-time parameter check. This parameter check is
 * enabled by un-comment the USE_FULL_ASSERT macro inside stm32xxxx_hal_conf.h file.
 * See "Run-time checking" of the HAL manual for detail.
 *
 * Assertion is defined in Src/main.c. As assert_failed() function. This function
 * is empty at first. The murasaki install script fills by CustomerAssertFailed() calling
 * statement.
 *
 * @code
 * void assert_failed(uint8_t *file, uint32_t line)
 * {
 *   // USER CODE BEGIN 6
 *     CustomAssertFailed(file, line);
 *   // USER CODE END 6
 * }
 * @endcode
 *
 * If a HAL API received wrong parameter, the assert_failed() function is
 * called with its filename and line number. Then. assert_failed() call CustomAssertFailed()
 * function in the @ref Src/murasaki_platform.cpp file.
 *
 * The CustomAssertFailed() print the filename and line number with message.
 *
 * @code
 * void CustomAssertFailed(uint8_t* file, uint32_t line) {
 *     murasaki::debugger->Printf(
 *                              "Wrong parameters value: file %s on line %d\n",
 *                              file,
 *                              line);
 * }
 * @endcode
 */

/**
 * @page ug_sect_6_3 Spurious Interrupt flow
 * @brief Murasaki provides a mechanism to catch a spurious interrupt.
 *
 * Default_handler is the entry point of the spurious interrupt handler.
 * This is defined in  startup/startup_stm32******.s.
 *
 * The install script modify this handler to call the pref CustomDefaultHanlder()
 * in the @ref Src/murasaki_platform.cpp.
 * @code
 *     .section  .text.Default_Handler,"ax",%progbits
 *     .global CustomDefaultHandler
 * Default_Handler:
 *   bl CustomDefaultHandler
 * Infinite_Loop:
 *   b  Infinite_Loop
 * @endcode
 *
 * CustomDefaultHandler() put the debugger to the post mortem state which
 * can work without the debug helper tasks. This function keep watching UART and
 * if any input is found, it flushes the entire data of debug FIFO.
 *
 * Thus, programmer can see the last messages before triggering spurious interrupt.
 *
 * @code
 * void CustomDefaultHandler() {
 *     // Call debugger's post mortem processing. Never return again.
 *     murasaki::debugger->DoPostMortem();
 * }
 * @endcode
 */

/**
 * @page ug_sect_6_31 Assertion flow
 * @brief The assertion flow is similar to the Spurious Interrupt flow.
 *
 * Once assertion is raised, assertion macro raised Hard Fault execption.
 * The Hard Fault exception handler in the  Src/st32****_it.c calles
 * CustomDefaultHandler.
 *
 * @code
 * void HardFault_Handler(void)
 * {
 *   CustomDefaultHandler();
 *   while (1)
 *   {
 *   }
 * }
 * @endcode
 */

/**
 * @page ug_sect_6_4 General Interrupt flow
 * @brief As described in the HAL manual, STM32Cube HAL handles all peripheral related interrupt
 * , and then, call corresponding callback function.
 *
 * @details
 * These call backs are optional from the view point of the peripheral hardware, but
 * essential hook to sync with software.
 *
 * Murasaki is using these callback to notify the end of processing, to the peripheral class objects.
 * For example, following is the sample of callback.
 *
 * @code
 * void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart)
 * {
 *     // Poll all uart rx related interrupt receivers.
 *     // If hit, return. If not hit,check next.
 *     if (murasaki::platform.uart_console->ReceiveCompleteCallback(huart))
 *         return;

 * }
 * @endcode
 *
 * This callback is called from HAL, after the end of peripheral interrupt processing.
 * And calling the ReceiveCompleteCallback() of the UART object in the platform. Note
 * that Murasaki object returns true, if the callback member function parameter matches
 * with its own hardware handle. Then, the function can return if the return value
 * is true.
 *
 * Note that forwarding this call back to all the relevant peripheral is a
 * Responsibility of the porting programmer. To forward the callback to the multiple objects,
 * you can call like this.
 *
 * @code
 *     if (murasaki::platform.uart_console->ReceiveCompleteCallback(huart))
 *         return;
 *     if (murasaki::platform.uart_1->ReceiveCompleteCallback(huart))
 *         return;
 *     if (murasaki::platform.uart_2->ReceiveCompleteCallback(huart))
 *         return;
 * @endcode
 *
 */

/**
 * @page ug_sect_6_5 EXTI flow
 * @brief EXTI flow is very similar to the @ref ug_sect_6_4 except its timing.
 *
 * @details
 * While other peripheral raises interrupt after the peripheral instance are created,
 * EXTI peripheral may raise the interrupt before the platform peripherals are ready.
 *
 * Then, EXTI call back has guard to avoid the null pointer access.
 *
 * @code
 * void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 * {

 *     if ( USER_Btn_Pin == GPIO_Pin) {
 *         // release the waiting task
 *         if (murasaki::platform.sync_with_button != nullptr)
 *             murasaki::platform.sync_with_button->Release();
 *     }

 * }
 * @endcode
 *
 */

#endif /* MURASAKI_2_UG_HPP_ */
