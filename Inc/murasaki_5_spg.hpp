/**
 * @file murasaki_5_spg.hpp
 * @brief Step by Step Porting guide
 */


/**
 * @page spg Step-by-Step Porting Guide
 * @brief This chapter go though the actual operation of the CubeMX and SW4STM32 to
 * create an empty application with Murasaki.
 *
 * @details
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
 *
 * @image html "Screenshot from 2019-02-18 08-42-34.png" "CubeMX"
 * @image latex "Screenshot from 2019-02-18 08-42-34.png" "CubeMX"
 *
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

