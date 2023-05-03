/**
 * @file murasaki_platform.cpp
 *
 * @date 2018/05/20
 * @author Seiichi "Suikan" Horie
 * @brief A glue file between the user application and HAL/RTOS.
 */

// Include the definition created by CubeIDE.
#include <murasaki_platform.hpp>
#include "main.h"

// Include the murasaki class library.
#include "murasaki.hpp"

// Include the prototype  of functions of this file.

/* -------------------- PLATFORM Macros -------------------------- */

/* -------------------- PLATFORM Type and classes -------------------------- */

/* -------------------- PLATFORM Variables-------------------------- */

// Essential definition.
// Do not delete
murasaki::Platform murasaki::platform;
murasaki::Debugger *murasaki::debugger;

/* ------------------------ STM32 Peripherals ----------------------------- */

/*
 * Platform dependent peripheral declaration.
 *
 * The variables here are defined at the top of the main.c.
 * Only the variable needed by the InitPlatform() are declared here
 * as external symbols.
 *
 * The declaration here is user project dependent.
 */
// Following block is just sample.
// Original declaration is in the top of main.c.
#if 0
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi4;
extern UART_HandleTypeDef huart2;
#endif

// Definition for Nucleo.
// Note that the given processor name MACRO doesn't have the suffix. Thus, certain processor
// Which has multiple Nucleo ( ex: G431 32/48 pin, F446 48/144 pin ) may cause problem.
#if defined(STM32F091xC)
// For Nucleo F091RC (32pin)
#define UART_PORT huart2
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32F446xx)
// For Nucleo G431RB (48pin)
#define UART_PORT huart2
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32F722xx)
// For Nucleo F722ZE (144pin)
#define UART_PORT huart3
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32F746xx)
// For Nucleo F746ZG (144pin)
#define UART_PORT huart3
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32G070xx)
// For Nucleo G070RB (48pin)
#define UART_PORT huart2
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32G431xx)
// For Nucleo G431RB (48pin)
#define UART_PORT hlpuart1
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32H743xx)
// For Nucleo H743ZI (144pin)
#define UART_PORT huart3
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32L152xE)
// For Nucleo L152RE (48pin)
#define UART_PORT hlpuart2
#define LED_PORT LD2_GPIO_Port
#define LED_PIN LD2_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32L412xx)
// For Nucleo L412RB (48pin)
#define UART_PORT huart2
#define LED_PORT LD4_GPIO_Port
#define LED_PIN LD4_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32G0B1xx)
// For Nucleo G01B1RE (48pin)
#define UART_PORT huart2
#define LED_PORT LED_GREEN_GPIO_Port
#define LED_PIN LED_GREEN_Pin
extern UART_HandleTypeDef UART_PORT;

#elif defined(STM32H503xx)
// For Nucleo G01B1RE (48pin)
#define UART_PORT huart3
#define LED_PORT USER_LED_GPIO_Port
#define LED_PIN USER_LED_Pin
extern UART_HandleTypeDef UART_PORT;

#else
#error "Unknown nucleo. Please define the UART_PORT, LED_PORT, LED_PIN macro."
#endif

/* -------------------- PLATFORM Prototypes ------------------------- */

void TaskBodyFunction(const void *ptr);

/* -------------------- PLATFORM Implementation ------------------------- */

// Initialization of the system.
void InitPlatform()
{
#if ! MURASAKI_CONFIG_NOCYCCNT
    // Start the cycle counter to measure the cycle in MURASAKI_SYSLOG.
    murasaki::InitCycleCounter();
#endif
    // UART device setting for console interface.
    // On Nucleo, the port connected to the USB port of ST-Link is
    // referred here.
    murasaki::platform.uart_console = new murasaki::DebuggerUart(&UART_PORT);
    while (nullptr == murasaki::platform.uart_console)
        ;  // stop here on the memory allocation failure.

    // UART is used for logging port.
    // At least one logger is needed to run the debugger class.
    murasaki::platform.logger = new murasaki::UartLogger(murasaki::platform.uart_console);
    while (nullptr == murasaki::platform.logger)
        ;  // stop here on the memory allocation failure.

    // Setting the debugger
    murasaki::debugger = new murasaki::Debugger(murasaki::platform.logger);
    while (nullptr == murasaki::debugger)
        ;  // stop here on the memory allocation failure.

    // Set the debugger as AutoRePrint mode, for the easy operation.
    murasaki::debugger->AutoRePrint();  // type any key to show history.

    // For demonstration, one GPIO LED port is reserved.
    // The port and pin names are fined by CubeIDE.
    murasaki::platform.led = new murasaki::BitOut(LED_PORT, LED_PIN);
    MURASAKI_ASSERT(nullptr != murasaki::platform.led)

    // For demonstration of FreeRTOS task.
    murasaki::platform.task1 = new murasaki::SimpleTask(
                                                        "task1",
                                                        256,
                                                        murasaki::ktpNormal,
                                                        nullptr,
                                                        &TaskBodyFunction
                                                        );
    MURASAKI_ASSERT(nullptr != murasaki::platform.task1)

    // Following block is just for sample.
#if 0
    // For demonstration of the serial communication.
    murasaki::platform.uart = new murasaki::Uart(&huart2);
    // For demonstration of master and slave I2C
    murasaki::platform.i2c_master = new murasaki::I2cMaster(&hi2c1);
    murasaki::platform.i2c_slave = new murasaki::I2cSlave(&hi2c2);
    // For demonstration of master and slave SPI
    murasaki::platform.spi_master = new murasaki::SpiMaster(&hspi1);
    murasaki::platform.spi_slave = new murasaki::SpiSlave(&hspi4);
#endif

}

// main routine of the system.
void ExecPlatform()
{
    // counter for the demonstration.
    int count = 0;

    murasaki::platform.task1->Start();

    // Loop forever
    while (true) {

        // print a message with counter value to the console.
        murasaki::debugger->Printf("Hello %d \n", count);

        // update the counter value.
        count++;

        // wait for a while
        murasaki::Sleep(500);
    }
}

/* ------------------ User Functions -------------------------- */
/**
 * @brief Demonstration task.
 * @param ptr Pointer to the parameter block
 * @details
 * Task body function as demonstration of the @ref murasaki::SimpleTask.
 *
 * You can delete this function if you don't use.
 */
void TaskBodyFunction(const void *ptr)
                      {

    while (true)  // dummy loop
    {
        murasaki::platform.led->Toggle();  // toggling LED
        murasaki::Sleep(700);
    }
}

