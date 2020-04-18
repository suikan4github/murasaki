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
 *  \page murasaki_ug_usage Usage Introduction
 *  \details
 *  In this introduction, we see how to use the Murasaki class library in the STM32 program.
 *
 *  \li \subpage ug_sec_1
 *  \li \subpage ug_sec_2
 *  \li \subpage ug_sec_3
 *  \li \subpage ug_sec_4
 *  \li \subpage ug_sec_5
 *  \li \subpage ug_sec_6
 *
 *  There are some other manuals of murasaki class library :
 *
 *  \li \ref murasaki_ug_preface
 *  \li \ref murasaki_pg
 *  \li \ref MURASAKI_GROUP
 *
 *
 *  For the easy-to-understand description, we assume several things on the application skeleton, which we are going to use Murasaki :
 *  \li The application skeleton is generated by <a href= "https://www.st.com/ja/development-tools/stm32cubeide.html"> CubeIDE </a>
 *  \li The application skeleton is configured to use FreeRTOS
 *  \li UART3 is configured to use DMA.
 *  @li UART3 is configured to use interrupt.
 *
 *  These are requirements from the Murasaki library.
 *
 *  \page ug_sec_1 Message output
 *  \details
 *  The Murasaki library has a Printf() like a message output mechanism.
 *  This mechanism is an easy way to display a message from an embedded microcomputer to the terminal simulator like Kermit on a host computer. Murasaki's Printf() is based on the standard C language formatting library. So, a programmer can output a message like standard printf().
 *
 *  As usual, let's start from "hello, world."
 *
 *  \code
 *  murasaki::debugger->Printf("Hello, world!\n");
 *  \endcode
 *
 *  In Murasaki manner, the Printf() is not a global function. This member function is a method of murasaki::Debugger class.
 *  The murasaki::debugger variable is one of the two Murasaki's global variables. And it provides an easy to use message output.
 *
 *  The end-of-line character depends on the terminal. In the above example, the terminator is '\n'.
 *  The '\n' is for the Linux based Kermit software. Other terminal systems may need other end-of-line characters.
 *  The easiest way to absorb this difference is to change the terminal software behavior.
 *  Almost terminal emulation software can accept any type of end-of-line character.
 *
 *  Because the Printf() works as like standard printf(), you can also use the format string.
 *
 *  \code
 *  murasaki::debugger->Printf("count is %d\n", count);
 *  \endcode
 *
 *  The Printf() is designed as a debugger message output for an embedded real-time system.
 *  This function is :
 *  \li Thread safe
 *  \li Asynchronous
 *  \li Blocking
 *  \li Buffered
 *
 *  In other words, you can use this function in either task or interrupt handler without bothering the real-time process.
 */

/**
 * \page ug_sec_2 Serial communication
 * @details
 * murasaki::Uart is the asynchronous serial communication.
 * The initial baud rate, parity and data size are defined by CubeIDE.
 * So, there is no need to initialize the communication parameter in application program.
 * User can transmit data by passing its buffer address and data length.
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
 * In addition to the transmit function, also Receive() member function exists.
 *
 */

/**
 * \page ug_sec_3 Debugging with Murasaki.
 * \details
 * As we saw again and again, Murasaki has a simple messaging output for real-time debugging.
 * This feature is
 * typically used as UART serial output, but configurable by the programmer.
 *
 * The murasaki::debugger is the useful variable to output the debugging message.
 * There are several good features in this function, as we already saw.
 * \li Versatile printf() style format string.
 * \li Can call from both task and interrupt context
 * \li Asyncronous
 * \li Non-blocking
 *
 * These features help the programmer to display the message in the real-time, multi-task application.
 *
 * In addition to this simple debugging variable, a programmer can use assert_failure() function of the STM32 HAL.
 * The STM32Cube HAL has assert_failure() to check the parameter on the fly.
 * By default, this function is disabled.
 * To use this function, programmer have to make it enable, and add function to receive the
 * debug information.
 *
 * To enable the assert_failuer(), open the Project Management tab of the Device Configuration Tool of the CubeIDE.
 * And then, go to the Code Generation section and check "Enable Full Assert".
 *
 * And then, you must modify assert_failure() in main.c, to call output function
 * (Note, this modification is done by the murasaki/install script ).
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
 * In this User's Guide, logging is done through the UART port.
 *
 * Sometimes, you may not want to connect a serial terminal to your system after you saw something wrong.
 * But this is too late because the assertion message is already transmitted ( and lost ).
 *
 * Murasaki can save this problem.
 * By adding the following code after creating murasaki::Debugger instance, you can use history functionality.
 *
 * @code
 *     murasaki::debugger->AutoHistory();
 * @endcode
 *
 * The murasaki::Debugger::AutoHistory() creates a dedicated task for auto history function.
 * This task watches the input from the logging port.
 * Again, in this User's guide it is UART.
 * Once any character is received from the logging port ( terminal ),
 * previously transmitted message is sent again.
 * Thus you can read the daying message from your application.
 *
 * The auto history is handy, but it blocks all input from the terminal.
 * If you want to have your own console program through the debug port input,
 * do not use the auto history. Alternatively, you can send the previously transmitted message again,
 * by calling murasaki::Debugger::PrintHistory() explicitly.
 *
 * Murasaki also have post-mortem debugging feature which helps to analyze severe error.
 * Murasaki adds a hook into the Default_Handler of the startup_stm32****.s file.
 *
 * @code
 *     .section  .text.Default_Handler,"ax",%progbits
 *     .global CustomDefaultHandler
 * Default_Handler:
 * #if (__ARM_ARCH == 6 )
 *   ldr r0, = CustomDefaultHandler
 *   bx r0
 * #else
 *   b.w CustomDefaultHandler
 * #endif
 * Infinite_Loop:
 *   b  Infinite_Loop
 * @endcode
 *
 * The inserted instructions supersedes the infinite loop at spurious interrupt handler.
 * Alternatively, CustomDefaultHandler() is called. The CustomDefaultHandler() stops entire
 * Debugger process, and get into the polling mode serial operation with auto history.
 *
 * That mean, once spurious interrupt happen, you can read the messages in the debug
 * message FIFO by pressing any key. This feature helps to analyze the assertion message
 * instead of the confusion by unknown trouble.
 *
 */

/**
 * \page ug_sec_4 Tasking
 * @details
 * murasaki::SimpleTask is a wrapper class of the FreeRTOS task.
 * By using murasaki::SimpleTask, a programmer can easily create a task object.
 * This object encapsulates the task of the FreeRTOS.
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
 * There are several parameters to pass for the constructor.
 * The first parameter is the name of the task in FreeRTOS.
 * The second one is the task stack size. The programmer must decide the appropriate size based on the requirement of the task body function.
 * The third one is the priority of the new task.
 * The priority has to be the value of the @ref murasaki::TaskPriority type. The fourth one is the pointer to the task parameter. This parameter is passed to the task function body in the run-time.
 * And then, the last one is the pointer to the task body function.
 *
 * @code
 *     // For demonstration of FreeRTOS task.
 *     murasaki::platform.task1 = new murasaki::SimpleTask(
 *                                                   "Master",
 *                                                   256,
 *                                                   murasaki::ktpNormal,
 *                                                   nullptr,
 *                                                   &TaskBodyFunction
 *                                                   );
 * @endcode
 *
 * Once a task object is created, you must call Start() member function to start the task.
 *
 * @code
 *    murasaki::platform.task1->Start();
 * @endcode
 *
 * Then, a new task starts.
 */

/**
 * @page ug_sec_5 Other peripherals
 * @details
 * This section shows samples of the other peripherals.
 * @li @subpage ug_sect_5_1
 * @li @subpage ug_sect_5_2
 * @li @subpage ug_sect_5_3
 * @li @subpage ug_sect_5_4
 * @li @subpage ug_sect_5_5
 * @li @subpage ug_sect_5_6
 */

/**
 * @page ug_sect_5_1 I2C Master
 * @details
 * \ref murasaki::I2cMaster class provides the serial communication.
 * The I2C master is easy to use. To send a message to the slave device, you need to specify the slave address in 7bits, pointer to data and data size in byte.
 * @code
 *        uint8_t data[5] = { 1, 2, 3, 4, 5 };
 *        murasaki::I2cStatus stat;
 *
 *        stat = murasaki::platform.i2c_master->Transmit(
 *                                                      127,
 *                                                      data,
 *                                                      5);
 * @endcode
 *
 *  Note: By default, there is no member function "i2c_master" in the murasaki::platform variable.
 *  The definition and initialization of this member variable are the responsibility of the programmer.
 *
 *  In addition to the murasaki::I2cMaster::Transmit(), murasaki::I2cMaster class has murasaki::I2cMaster::Receive(), and murasaki::I2cMaster::TransmitThenReceive() member function.
 */

/**
 * @page ug_sect_5_2 I2C Slave
 * @details
 * \ref murasaki::I2cSlave class provides the I2C slave function.
 * The I2C slave is much easier than master, because it doesn't need to specify the slave address.
 * The I2C slave device address is given by CubeIDE port configuration.
 * @code
 *        uint8_t data[5];
 *        murasaki::I2cStatus stat;
 *
 *        stat = murasaki::platform.i2c_slave->Receive(
 *                                                    data,
 *                                                    5);
 * @endcode
 *
 *  Note: By default, there is no member function "i2c_slave" in the murasaki::platform variable.
 *  The definition and initialization of this member variable are the responsibility of the programmer.
 *
 *  In addition to the murasaki::I2cSlave::Transmit(), murasaki::I2cSlave class has murasaki::I2cSlave::Receive() member function.
 *
 */

/**
 * @page ug_sect_5_3 SPI Master
 * @details
 * @ref murasaki::SpiMaster is the SPI master class of Murasaki.
 * This class is more complicated than other peripherals because of its flexibility.
 * The SPI master controller must adapt to the several variations of the SPI communication.
 * \li CPOL configuration
 * \li CPHA configuration
 * \li GPIO port configuration to select a slave
 *
 * To support the above configurations, we need a special mechanism.
 * In Murasaki, this flexibility is the responsibility of the urasaki::SpiSlaveAdapter class.
 * This class holds these configurations and passes them to the I2C master class.
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
 * Then, you can pass the SpiSlaveAdapter class object to the murasaki::SpiMaster::TransmitAndReceive() function.
 * @code
 *         // Transmit and receive data through SPI
 *         uint8_t tx_data[5] = { 1, 2, 3, 4, 5 };
 *         uint8_t rx_data[5];
 *         murasaki::SpiStatus stat;
 *         stat = murasaki::platform.spi_master->TransmitAndReceive(
 *                                                                 slave_spec,
 *                                                                 tx_data,
 *                                                                 rx_data,
 *                                                                 5);
 * @endcode
 *
 * Note: By default, there are no member function "spi_master" and "slave_spec" in the murasaki::platform variable.
 * The definition and initialization of these member variables are the responsibility of the programmer.
 */

/**
 * @page ug_sect_5_4 SPI Slave
 * @details
 * @ref murasaki::SpiSlave class provides the SPI slave functionality.
 * This class encapsulate the SPI slave function.
 * @code
 *         // Transmit and receive data through SPI
 *         uint8_t tx_data[5] = { 1, 2, 3, 4, 5 };
 *         uint8_t rx_data[5];
 *         murasaki::SpiStatus stat;
 *         stat = murasaki::platform.spi_slave->TransmitAndReceive(
 *                                                                tx_data,
 *                                                                rx_data,
 *                                                                5);
 * @endcode
 *
 *  Note: By default, there is no member function "spi_slave" in the murasaki::platform variable.
 *  The definition and initialization of this member variable are the responsibility of the programmer.
 */

/**
 * @page ug_sect_5_5 GPIO
 * @details
 * @ref murasaki::BitOut and murasaki::BitIn provides the GPIO functionality.
 * Following is an example of the murasaki::BitOut class.
 * @code
 *         // Toggle LED.
 *         murasaki::platform.led->Toggle();
 * @endcode
 *
 *  Note: By default, there is no member function "led" in the murasaki::platform variable.
 *  The definition and initialization of this member variable are the responsibility of the programmer.
 *
 *
 * In addition to the murasaki::BitOut::Toggle(), BitOut has murasaki::BitOut::Set() and murasaki::BitOut::Clear() member function.
 */

/**
 * @page ug_sect_5_6 Duplex Audio
 * @details
 * @ref murasaki::DuplexAudio class provides a real-time audio IO for both  TX and RX together.
 * This class needs a @ref murasaki::AudioPortAdapterStrategy object as an interface with hardware.
 *
 * This class doesn't care about the CODEC IC control.
 * The CODEC initialization and control have to be done by external software.
 *
 * See the following sample code :
 *
 * @code
 *     // audio CODEC
 *     murasaki::platform.codec = new murasaki::Adau1361(
 *                                                       48000,             // Fs
 *                                                       12000000,          // ADAU1361 master clock freq
 *                                                       murasaki::platform.i2c_master, // CODEC port
 *                                                       CODEC_ADDRESS);    // I2C device address
 *
 *    murasaki::platform.sai = new murasaki::SaiPortAdaptor(
 *                                                          &hsai_BlockB1, // TX SAI block
 *                                                          &hsai_BlockA1); // RX SAI block
 *    murasaki::platform.audio = new murasaki::DuplexAudio(
 *                                                          murasaki::platform.sai,
 *                                                          CHANNEL_LEN);
 *
 * @endcode
 *
 * The processing of the audio is in the real-time domain.
 * It is recommended to run in the audio task as @ref murasaki::ktpRealtime priority.
 * The murasaki::DuplexAudio::TransmitAndReceive method is synchronous and blocking.
 * Thus, the processing loop is pretty simple.
 *
 * @code
 * void TaskBodyFunction(const void* ptr) {
 *
 *    // audio buffer
 *    float * l_tx = new float[CHANNEL_LEN];
 *    float * r_tx = new float[CHANNEL_LEN];
 *    float * l_rx = new float[CHANNEL_LEN];
 *    float * r_rx = new float[CHANNEL_LEN];
 *
 *    murasaki::platform.codec->Start();  // Start the audio codec
 *
 *
 *    // Loop forever
 *    while (true) {
 *
 *
 *        // Talk through
 *        for (int i = 0; i < CHANNEL_LEN; i++) {
 *            l_tx[i] = l_rx[i];
 *            r_tx[i] = r_rx[i];
 *        }
 *        murasaki::platform.audio->TransmitAndReceive(l_tx, r_tx, l_rx, r_rx);
 *
 *    }
 *
 * }
 * @endcode
 */

/**
 * @page ug_sec_6 Program flow
 * @details In this section, we see the program flow of a Murasaki application.
 * Murasaki has 6 program flows. The start point of these flows are always inside  CubeIDE generated code.
 * 3 out of 6 flows are for debugging.
 * Only 1 flow has to be understood well by an application programmer.
 * Rest of 2 are the responsibility of the porting programmer.
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
 * @details
 *  The application program flow is the main flow of a Murasaki application.
 *  This program flow starts from the StartDefaultTask() in the Src/main.c.
 *  The StartDefaultTas() is default and first task created by CubeIDE.
 *  In other words, this task is automatically created without configuration.
 *
 *  From this function, two Murasaki function is called. One is InitPlatoform().
 *  The other is ExecPlatform(). Note that both function calls are inserted by
 *  murasaki/install script.
 *
 *  @code
 *  void StartDefaultTask(void const * argument)
 *  {
 *
 *    // USER CODE BEGIN 5
 *      InitPlatform();
 *      ExecPlatform();
 *    // Infinite loop
 *    for(;;)
 *    {
 *      osDelay(1);
 *    }
 *    // USER CODE END 5
 *  }
 *  @endcode
 *
 *  The InitPlatform() function is defined in the Src/murasaki_platform.cpp.
 *  Because the file extension is .cpp, the murasaki_platfrom.cpp is compiled
 *  by C++ compiler while the main.c is compiled by C compiler.
 *  This allows the programmer to uses the C++ language.
 *
 *  As the name suggests, InitPlatform() is where programmer initialize the
 *  platform variables murasaki::platform and murasaki::debugger.
 *  @code
 *  void InitPlatform()
 *  {
 *  #if ! MURASAKI_CONFIG_NOCYCCNT
 *      // Start the cycle counter to measure the cycle in MURASAKI_SYSLOG.
 *      murasaki::InitCycleCounter();
 *  #endif
 *      // UART device setting for console interface.
 *      // On Nucleo, the port connected to the USB port of ST-Link is
 *      // referred here.
 *      murasaki::platform.uart_console = new murasaki::DebuggerUart(&huart3);
 *      while (nullptr == murasaki::platform.uart_console)
 *          ;  // stop here on the memory allocation failure.
 *
 *      // UART is used for logging port.
 *      // At least one logger is needed to run the debugger class.
 *      murasaki::platform.logger = new murasaki::UartLogger(murasaki::platform.uart_console);
 *      while (nullptr == murasaki::platform.logger)
 *          ;  // stop here on the memory allocation failure.
 *
 *      // Setting the debugger
 *      murasaki::debugger = new murasaki::Debugger(murasaki::platform.logger);
 *      while (nullptr == murasaki::debugger)
 *          ;  // stop here on the memory allocation failure.
 *
 *      // Set the debugger as AutoRePrint mode, for the easy operation.
 *      murasaki::debugger->AutoRePrint();  // type any key to show history.
 *
 *      // For demonstration, one GPIO LED port is reserved.
 *      // The port and pin names are fined by CubeIDE.
 *      murasaki::platform.led = new murasaki::BitOut(LD2_GPIO_Port, LD2_Pin);
 *      MURASAKI_ASSERT(nullptr != murasaki::platform.led)
 *
 *      // For demonstration of FreeRTOS task.
 *      murasaki::platform.task1 = new murasaki::SimpleTask(
 *                                                          "task1",
 *                                                          256,
 *                                                          murasaki::ktpNormal,
 *                                                          nullptr,
 *                                                          &TaskBodyFunction
 *                                                          );
 *      MURASAKI_ASSERT(nullptr != murasaki::platform.task1)
 *
 *      // Following block is just for sample.
 *
 *  }
 *  @endcode
 *
 *  In this example, the first half of the InitPlatform() is building a murasaki::debugger variable.
 *
 *  Probably the most critical statement in this part is the creation of the DebuggerUart class object.
 *
 *  @code
 *     murasaki::platform.uart_console = new murasaki::DebuggerUart(&huart3);
 *  @endcode
 *
 *  In this statement, the DebgguerUart receives the pointer to the huart3 as a parameter.
 *  The hauart3 is a handle variable of the UART3 generated by CubeIDE. Let's remind the  UART3 is utilized as a communication path through the USB in the Nucleo 144 board.
 *  So, in this sample code,  we are making debugging console through the USB-serial line of the Nucleo F722ZE board.
 *
 *  Because the huart3 is generated into the main.c directory, we have to declare this  variable as an external variable. You can find the declaration around the top of the
 *  Src/murasaki_platform.cpp.
 *
 *  @code
 *  extern UART_HandleTypeDef huart3;
 *  @endcode
 *
 *  Note that the UART port number depends on the  Nucleo board. So, the
 *  porting programmer have a responsibility to refer the right UART.
 *
 *  The second half of the InitPlatform() is the creative part of the other peripheral object. This part is fully depends on the application. A programmer can define any
 *  member variable in the platform variable, by modifying the murasaki::Platform struct in the Inc/platform_defs.hpp.
 *
 *  The second function which is called from the @ref StartDefaultTask() is the @ref ExecPlatform().
 *  This function is also defined in the @ref Src/murasaki_platform.cpp.
 *
 *
 *  @code
 *  void ExecPlatform()
 *  {
 *
 *      murasaki::platform.task1->Start();
 *
 *
 *      // print a message with counter value to the console.
 *      murasaki::debugger->Printf("Push user button to display the I2C slave device \n");
 *
 *
 *      // Loop forever
 *      while (true) {
 *          murasaki::platform.sync_with_button->Wait();
 *          I2cSearch(murasaki::platform.i2c_master);
 *
 *      }
 *  }
 *   @endcode
 *
 *  This function is the body of the application. So, you can read GPIO, ADC other peripherals.
 *  And output to the DAC, GPIO, and other peripherals from here.
 *
 */

/**
 * @page ug_sect_6_2 HAL Assertion flow
 * @details
 *  HAL Assertion is a STM32Cube HAL's programming help mechanism.
 *
 *  STM32Cube HAL provides a run-time parameter check. This parameter check is enabled by un-comment the USE_FULL_ASSERT macro inside stm32xxxx_hal_conf.h file.
 *  See "Run-time checking" of the HAL manual for detail.
 *
 *  Assertion is defined in Src/main.c. As assert_failed() function. This function
 *  is empty at first. The murasaki install script fills by CustomerAssertFailed() calling
 *  statement.
 *
 *  @code
 *  void assert_failed(uint8_t *file, uint32_t line)
 *  {
 *    // USER CODE BEGIN 6
 *      CustomAssertFailed(file, line);
 *    // USER CODE END 6
 *  }
 *  @endcode
 *
 *  If a HAL API received wrong parameter, the assert_failed() function is
 *  called with its filename and line number. Then. assert_failed() call CustomAssertFailed()
 *  function in the @ref Src/murasaki_platform.cpp file.
 *
 *  The CustomAssertFailed() prints the filename and line number with the message.
 *
 *  @code
 *  void CustomAssertFailed(uint8_t* file, uint32_t line) {
 *      murasaki::debugger->Printf(
 *                               "Wrong parameters value: file %s on line %d\n",
 *                               file,
 *                               line);
 *  }
 *  @endcode
 */

/**
 * @page ug_sect_6_3 Spurious Interrupt flow
 * @details
 * Murasaki provides a mechanism to catch a spurious interrupt.
 * Default_handler is the entry point of the spurious interrupt handler.
 * This is defined in  startup/startup_stm32******.s.
 *
 * The install script modify this handler to call the pref CustomDefaultHanlder()
 * in the @ref Src/murasaki_platform.cpp.
 * @code
 *     .section  .text.Default_Handler,"ax",%progbits
 *     .global CustomDefaultHandler
 * Default_Handler:
 *
 * #if (__ARM_ARCH == 6 )
 *     ldr r0, = CustomDefaultHandler
 *     bx r0
 * #else
 *     b.w CustomDefaultHandler
 * #endif
 *
 * Infinite_Loop:
 *   b  Infinite_Loop
 * @endcode
 *
 * CustomDefaultHandler() is an assembly program. Which pushes
 * register on the stack to allow the PrintFaultResult function
 * to print out the register and exception environment.
 * After printing, the system gets into the post-motem mode which
 * responses any key from the console and then flush out the contents of printf message FIFO.
 *
 * Note that the CustomDefaultHandler() works correctly only when  both conditions are met :
 * \li Core is ARM v7m ( The CORTEX-Mx except for M0, M0+ )
 * \li Murasaki is the release build.
 * \li The exception is Hard Fault.
 *
 * The Debug build makes an unexpected stack frame in the entry code of the HardFaultHandler.
 * Thus, the printed resources by the debug build are not precise.
 *
 */

/**
 * @page ug_sect_6_31 Assertion flow
 * @details
 * The assertion flow is similar to the Spurious Interrupt flow.
 * Once the assertion is raised, the assertion macro raises Hard Fault exception.
 * The Hard Fault exception handler in the  Src/st32****_it.c calls  CustomDefaultHandler.
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
 * @details
 *  As described in the HAL manual, STM32Cube HAL handles all interrupts relevant to the peripherals, and then, call the corresponding callback function.
 *  These callbacks are optional from the viewpoint of the peripheral hardware, but an essential hook to sync with the software.
 *
 *  Murasaki is using these callbacks to notify the end of processing, to the peripheral class objects.
 *  These callbacks are managed automatically. For example, the following is the UART RX completion call back inside @ref murasaki_callback.cpp.
 *
 *  @code
 * void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 *                              {
 *     // Obtain the responding object.
 *     murasaki::PeripheralStrategy *peripheral = murasaki::CallbackRepositorySingleton::GetInstance()->GetPeripheralObject(huart);
 *     // Convert it to the appropriate type.
 *     murasaki::UartStrategy *uart = reinterpret_cast<murasaki::UartStrategy*>(peripheral);
 *     // Handle the callback by the object.
 *     uart->ReceiveCompleteCallback(huart);
 * }
 *  @endcode
 *
 *  This callback is called from HAL, after the end of peripheral interrupt processing. Then, the @ref CallbackRepositorySingleton
 *  class search for a class object which has responsibility to the given peripheral handle.
 *
 *  Then, its appropriate member function is called.
 *
 */

/**
 * @page ug_sect_6_5 EXTI flow
 * @details
 * The EXTI flow is very similar to the @ref ug_sect_6_4 except its timing.
 *
 *
 */

#endif /* MURASAKI_2_UG_HPP_ */
