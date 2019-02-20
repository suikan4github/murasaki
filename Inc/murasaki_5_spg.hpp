/**
 * @file murasaki_5_spg.hpp
 * @brief Step by Step Porting guide
 */


/**
 * @page spg Step-by-Step Porting Guide
 * @brief This chapter goes through the actual operation of the CubeMX and SW4STM32 to
 * create an empty application with Murasaki.
 *
 * @details
 * To develop your own application, you should create the platform with Murasaki by yourself.
 * In this chapter, we will see the procedure to create a sample application for the Nucleo F722ZE board.
 *
 * This chapter is written based on the following software and hardware.
 * @li CubeMX ver 5.0.1
 * @li System Workbench for STM32 ver 1.17.0.201812190825
 * @li Ubuntu 16.04 LTS
 * @li Nucleo F722ZE
 *
 *
 * @li @subpage spg_1
 * @li @subpage spg_2
 * @li @subpage spg_3
 * @li @subpage spg_4
 * @li @subpage spg_5
 * @li @subpage spg_6
 *
 */

/**
 * @page spg_1 UART configuration
 * @brief In this section, we configure the UART communication parameter, DMA and interrupts.
 *
 * @details
 * Once you select the Nucleo F722ZE on the CubeMX, let's start to modify it from the UART configuration.
 * Nucleo F722ZE board utilizes the USART3 peripheral as UART3. And this UART3 port is
 * connected with ST-Link board. Thus we can communicate with the application through the
 * USB by terminal software.
 *
 * The Murasaki library support this communication by murasaki::Debugger class.
 * To use the Debugger class, we have to configure the UART port correctly.
 *
 * To configure the UART, select the UART3 peripheral inside the Connectivity category of the
 * Pinout & Configuration tab( Fig. 1).  The default tab is the Parameter and Setting tab. In this tab, we will configure the
 * Basic Parameters like Baud rate, word length, etc...
 *
 * @image html "Screenshot from 2019-02-18 08-42-34.png" "Fig. 1 CubeMX UART panel"
 * @image latex "Screenshot from 2019-02-18 08-42-34.png" "Fig. 1 CubeMX UART panel"
 *
 * And then, we configure the DMA. The murasaki::Uart class uses the DMA transfer for both
 * TX and RX. To enable DMA, click the DMA Settings tab and add DMAs. The default state of the
 * DMA configuration after clicking Add button is undetermined. Then, select the TX and RX DMA channel(Fig. 2).
 *
 * @image html "Screenshot from 2019-02-18 08-43-41.png" "Fig. 2 UART DMA Settings tab"
 * @image latex "Screenshot from 2019-02-18 08-43-41.png" "Fig. 2 UART DMA Settings tab"
 *
 * Finally, we configure the interrupt by NVIC Settings tab. Check all checkboxes.
 *
 * @image html "Screenshot from 2019-02-18 08-44-04.png" "Fig. 3 UART NVIC Settings tab"
 * @image latex "Screenshot from 2019-02-18 08-44-04.png" "Fig. 3 UART NIC Settings tab"
 *
 * By the way, we don't use the USB OTG of the Nucleo F722ZE in this demo. So let's disable it.
 * This is optional. There is no side effect to enable USB except memory usage.
 *
 * @image html "Screenshot from 2019-02-18 08-55-25.png" "Fig. 4 USB_OTG_FS Mode and Configuration"
 * @image latex "Screenshot from 2019-02-18 08-55-25.png" "Fig. 4 USB_OTG_FS Mode and Configuration"
 */

/**
 * @page spg_2 CPU, EXTI, and System tick configuration
 * @brief In this section, we configure the CPU, EXTI, and System tick timer.
 *
 * By default, CubeMX doesn't configure the CPU core. As a result, all caches and flash accelerator are disabled.
 * Enabling these features accelerates the code execution speed.
 *
 * Select CORETEX_M7 tab of the System Core category. Then, enable these items.
 *
 * @image html "Screenshot from 2019-02-18 08-47-58.png" "Fig. 5 CORTEX_M7 Mode and Configuration"
 * @image latex "Screenshot from 2019-02-18 08-47-58.png" "Fig. 5 CORTEX_M7 Mode and Configuration"
 *
 * Nucleo F722ZE board uses PC13 pin as user button (Bule button on the board). In this demo
 * we will use this button as an interrupt source. Then, we have to configure this PC13 pin as
 * interruptible.
 *
 * Select GPIO in the System Core category and set the PC13 GPIO mode to External Interrupt.
 *
 * @image html "Screenshot from 2019-02-18 08-46-54.png" "Fig. 6 GPIO Mode and Configuration"
 * @image latex "Screenshot from 2019-02-18 08-46-54.png" "Fig. 6 GPIO Mode and Configuration"
 *
 * Select NVIC tab to enable the EXTI line input.
 *
 * @image html "Screenshot from 2019-02-18 08-47-07.png" "Fig. 7 GPIO NVIC(EXTI)"
 * @image latex "Screenshot from 2019-02-18 08-47-07.png" "Fig. 7 GPIO NVIC(EXTI)"
 *
 * Then set the Timebase source. This is timer selection for the system tick.
 * FreeRTOS recommend using the GP timer as system tick source. So, the select
 * one of the unused timer. In the Fig.8, we chose TIM14.
 *
 * @image html "Screenshot from 2019-02-18 08-48-58.png" "Fig. 8 Sys Mode and Configuration"
 * @image latex "Screenshot from 2019-02-18 08-48-58.png" "Fig. 8 Sys Mode and Configuration"
 */

/**
 * @page spg_3 FreeRTOS configuration
 * @brief To run a FreeRTOS application, the heap memory size, and the default stack size
 * have to be configured.
 *
 * FreeRTOS is the important part of the Murasaki platform. To run the FreeRTOS,
 * we have to configure at least two parameters.
 *
 * At first, we have to increase  MINIMAL_STACK_SIZE. This is the stack size of the
 * first task created by CubeMX. See the @ref sec_cm_3 for detail.
 * The default value is 128 Byte. It should be at least 256 Byte.
 *
 * @image html "Screenshot from 2019-02-18 08-49-55.png" "Fig. 9 FREERTOS Mode and Configuration: MINIMAL_STACK_SIZE"
 * @image latex "Screenshot from 2019-02-18 08-49-55.png" "Fig. 9 FREERTOS Mode and Configuration: MINIMAL_STACK_SIZE"
 *
 * Another important parameter is TOTAL_HEAP_SIZE. This is the size of the heap
 * under the FreeRTOS management. See the @ref sec_cm_1 for detail.
 *
 * 16kB is a little bit smaller. 32kB and greater is preferable.
 *
 * @image html "Screenshot from 2019-02-18 08-50-35.png" "Fig. 10 FREERTOS Mode and Configuration: TOTAL_HEAP_SIZE"
 * @image latex "Screenshot from 2019-02-18 08-50-35.png" "Fig. 10 FREERTOS Mode and Configuration: TOTAL_HEAP_SIZE"
 *
 *
 *
 */

/**
 * @page spg_4 Clock configuration
 * @brief At the Feb/2019, CubeMX has a bug on the Nucleo F722ZE clock setting.
 *
 * The Nucleo F722ZE board has 8MHz as input clock (HSE). But CubeMX default setting
 * is 25MHz. So, we have to fix this bug by hand.
 *
 * Select the Clock Configuration tab. The HSE input frequency is at the left end of the
 * clock chain (Fig 11). Change this frequency from 25 to 8.
 *
 * Once you change, CubeMX adjusts the entire clock but that is still not enough.
 * CPU clock (HCLK) is too low. Then, we should modify by hand again.
 *
 * The HCLK is located at the right end of the Fig. 11.  Change it to 216 MHz which is the
 * Maximum operation frequency.
 *
 * @image html "Screenshot from 2019-02-18 08-52-28.png" "Fig. 11 Clock Configuration"
 * @image latex "Screenshot from 2019-02-18 08-52-28.png" "Fig. 11 Clock Configuration"
 */



/**
 * @page spg_5 Project configuration and code generation
 * @brief At last, we configure the SW4STM32 project and generate a skeleton code.
 *
 * CubeMX's board setting is done. Now, we have to configure the project itself.
 * In this chapter, we will define the project type and location. Note that
 * to start the following procedure, we must create a workspace of the SW4STM32
 * and keep it open by SW4STM32 during the following configuration.
 *
 * At first, select the  Project Manager tab (Fig. 12).
 *
 * Because Murasaki Class library target is the SW4STM32, we must set the tool chaine to
 * SW4STM32. And then, brouse the SW4STM32 workspace which is open by SW4STM32.
 * Then type a prefered project name. In this guide, we choose ;
 * @li murasaki_demo as project name.
 * @li workspace_murasaki_sample as workspace directory name.
 *
 * @image html "Screenshot from 2019-02-18 08-57-07.png" "Fig. 12 Project Manager"
 * @image latex "Screenshot from 2019-02-18 08-57-07.png" "Fig. 12 Project Manager"
 *
 * Now, we are ready to generate the code. Click the "GENERATE CODE" button near the
 * right upper corner.
 *
 * Onece code negenration is finished, CubeMX shows the Code Generation dialog(Fig 13).
 * Click "Open Project".
 *
 * @image html "Screenshot from 2019-02-18 08-58-05.png" "Fig. 13 Code Generation"
 * @image latex "Screenshot from 2019-02-18 08-58-05.png" "Fig. 13 Code Generation"
 *
 * Then, CubeMX let the SW4STM32 import the generated project into the workspace
 * (This is tricky part. Generating the code into workspace is not enough. We have
 * to import that project to the workspace. "Open Project" button let the SW4STM32
 * import it).
 *
 * SW4STM32 import the project and show a dialog ( Fig 14 ).
 *
 * @image html "Screenshot from 2019-02-18 08-58-57.png" "Fig. 14 Successfully imported"
 * @image latex "Screenshot from 2019-02-18 08-58-57.png" "Fig. 14 Successfully imported"
 *
 * Now, the project is ready to build. But to go to next step, we must convert the
 * Project to the C++ project. The generated code is C project. But we use the
 * class library inside application. Thus, this conversion is essential.
 *
 * @image html "Screenshot from 2019-02-18 09-01-35.png" "Fig. 15 Convert to C++ Project"
 * @image latex "Screenshot from 2019-02-18 09-01-35.png" "Fig. 15 Convert to C++ Project"
 *
 *
 *
 */

/**
 * @page spg_6 Clone the Murasaki repository and install
 * @brief At last, we configure the SW4STM32 project and generate a code
 *
 * The project is still as is after CubeMX code generation. Remember,
 * our workspace and project was like this :
 *
 * @li murasaki_demo as project name.
 * @li workspace_murasaki_sample as workspace directory name.
 *
 * So, the directory is workspace_murasaki_sample/murasaki_demo.
 * Let's open a shell window, and execute following command :
 *
 * @code
 * cd workspace_murasaki_sample/murasaki_demo
 * git clone git@github.com:suikan4github/murasaki.git
 * cd murasaki
 * ./install
 * @endcode
 *
 * That's it. The Murasaki source tree is integrated into your project, and
 * the installer script embed the essential codes into several files generated by
 * CubeMX.
 *
 * Let's go back to SW4STM and refresh the project ( Type F5 ).
 * Without refreshing, you cannot see the Murasaki directory inside your project.
 *
 * Now, we are at the final stage. Open the project property, expand the C/C++ General, choose the
 * Paths and Symbols, select the include tab, and click the GNU C++. This is the include path
 * lists. Click Add button and type "murasaki/Inc". Then check the Add to all configurations ( Fig. 16 )..
 *
 * Click ok if the directory is correctly typed.
 *
 * @image html "Screenshot from 2019-02-20 18-20-48.png" "Fig. 16 Add Murasaki include path"
 * @image latex "Screenshot from 2019-02-20 18-20-48.png" "Fig. 16 Add Murasaki include path"
 *
 * Next, click the Source Location tab, and add "murasaki/Src" (Fig. 17).
 *
 * @image html "Screenshot from 2019-02-18 09-12-04.png" "Fig. 17 Add Murasaki source path"
 * @image latex "Screenshot from 2019-02-18 09-12-04.png" "Fig. 17 Add Murasaki source path"
 *
 */

