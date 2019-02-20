/**
 * @file murasaki_5_spg.hpp
 * @brief Step by Step Porting guide
 */


/**
 * @page spg Step-by-Step Porting Guide
 * @brief This chapter goes though the actual operation of the CubeMX and SW4STM32 to
 * create an empty application with Murasaki.
 *
 * @details
 * To develop your own application, you should create the platform with Murasaki by yourself.
 * In this chapter, we will see the procedure to create an sample application for the Nucleo F722ZE board.
 *
 * The related software versions are :
 * @li CubeMX ver 5.0.1
 * @li Sytem Workbench for STM32 ver 1.17.0.201812190825
 *
 * First of all, you have to create a new project by CubeMX for Nucleo F722ZE.
 * This skips how to make that project.
 *
 * @li @subpage spg_1
 * @li @subpage spg_2
 * @li @subpage spg_3
 * @li @subpage spg_4
 * @li @subpage spg_5
 *
 */

/**
 * @page spg_1 UART configuration
 * @brief UART is grouped as connectivity device.
 *
 * @details
 * Once Nucleo F722ZE is created, let's start to modify it from the UART configuration.
 * Nucleo F722ZE board utilizes the USART3 peripheral as UART3. And this UART3 port is
 * connected with ST-Link board. So we can communicate with the application through the
 * USB by terminal software.
 *
 * The Murasaki library support this communication by murasaki::Debugger class.
 * To use the Debugger class, we have to configure the UART port correctly.
 *
 * To configure the UART, select the UART3 peripheral inside the Connectivity category of the
 * Pinout & Configuration tab( Fig. 1).  The default tab is the Parameter and Setting tab. In this tab, we will configure the
 * Basic Parameters like Baud rate, word length, etc...
 *
 * @image html "Screenshot from 2019-02-18 08-42-34.png" "Fig.1 CubeMX UART panel"
 * @image latex "Screenshot from 2019-02-18 08-42-34.png" "Fig.1 CubeMX UART panel"
 *
 * And then, we configure the DMA. The murasaki::uart class uses the DMA transfer for both
 * TX and RX. To enable DMA, click the DMA Settings tab and add DMAs. The default state of the
 * DMA configuration after clicking Add button is undetermine. Then, select TX and RX DMA channel(Fig. 2).
 *
 * @image html "Screenshot from 2019-02-18 08-43-41.png" "Fig.2 UART DMA Settings tab"
 * @image latex "Screenshot from 2019-02-18 08-43-41.png" "Fig.2 UART DMA Settings tab"
 *
 * @image html "Screenshot from 2019-02-18 08-44-04.png" "Fig.3 UART NVIC Settings tab"
 * @image latex "Screenshot from 2019-02-18 08-44-04.png" "Fig.3 UART NIC Settings tab"
 */

/**
 * @page spg_2 CPU, EXTI, and System tick configuration
 * @brief System group contains CPU, EXTI and System tick timer.
 */

/**
 * @page spg_3 FreeRTOS configuration
 * @brief To run a FreeRTOS application, the heap memory and the default memory
 * have to be configured.
 */

/**
 * @page spg_4 Clock configuration
 * @brief This is required becaue of the Nucleo F722 bug of the CubeMX.
 */

/**
 * @page spg_5 Project configuration and code generation
 * @brief At last, we configure the SW4STM32 project and generate a code
 */

/**
 * @page spg_5 Clone the Murasaki repogitory and install
 * @brief At last, we configure the SW4STM32 project and generate a code
 */

