/**
 * \file murasaki_defs.hpp
 *
 * \date 2017/11/05
 * \author Seiichi "Suikan" Horie
 * \brief common definition of the platfrom.
 */

#ifndef MURASAKI_DEFS_HPP_
#define MURASAKI_DEFS_HPP_

#include "murasaki_include_stub.h"

#include <FreeRTOS.h>
#include <cmsis_os.h>
#include <task.h>

/**
 * \brief Personal Platform parts collection
 * \details
 * This name space encloses personal collections of the software parts to create a "platform" of the software development.
 * This specific collection is based on the STM32Cube HAL and FreeRTOS, both are generated by CubeIDE.
 */
namespace murasaki {

/**
 * @ingroup MURASAKI_DEFINITION_GROUP
 * \{
 */

/**
 * @brief Category to filter the Syslog output
 * @details
 * These are independent facilities to filter the Syslog message output.
 * Each module should specify appropriate facility.
 *
 * Internally, these value will be used as bit position in mask.
 */
enum SyslogFacility {
    kfaKernel = 1 << 0,  //!< kfaKernel is specified when the message is bound with the kernel issue.
    kfaSerial = 1 << 1,  //!< kfaSerial is specified when the message is from the serial module.
    kfaSpiMaster = 1 << 2,  //!< kfaSpi is specified when the message is from the SPI master module
    kfaSpiSlave = 1 << 3,  //!< kfaSpi is specified when the message is from the SPI slave module
    kfaI2cMaster = 1 << 4,  //!< kfaI2c is specified when the message is from the I2C master module.
    kfaI2cSlave = 1 << 5,  //!< kfaI2c is specified when the message is from the I2C slave module.
    kfaAudio = 1 << 6,  //!< kfaI2c is specified when the message is from the Audio module.
    kfaI2s = 1 << 7,  //!< kfaI2s is specified when the message is from the I2S module
    kfaSai = 1 << 8,  //!< kfaSai is specified when the message is from the SAI module.
    kfaLog = 1 << 9,  //!< kfaLog is specified when the message is from the logger and debugger module.
    kfaAudioCodec = 1 << 10,  //!< kfaAudioCodec is specified when the message is from the Audio Codec module
    kfaNone = 1 << 22,  //!< Disable all facility
    kfaAll = 1 << 23,  //!< Enable all facility
    kfaUser0 = 1 << 24,  //!< User defined facility
    kfaUser1 = 1 << 25,  //!< User defined facility
    kfaUser2 = 1 << 26,  //!< User defined facility
    kfaUser3 = 1 << 27,  //!< User defined facility
    kfaUser4 = 1 << 28,  //!< User defined facility
    kfaUser5 = 1 << 29,  //!< User defined facility
    kfaUser6 = 1 << 30,  //!< User defined facility
    kfaUser7 = 1 << 31  //!< User defined facility
};

/**
 * @brief Message severity level
 * @details
 * The lower value is the more serious condition.
 */
enum SyslogSeverity {
    kseEmergency = 0,  //!< kseEmergency means the system is unusable.
    kseAlert,  //!< kseAlert means some acution must be taken immediately.
    kseCritical,  //!< kseCritical means critical condition.
    kseError,  //!< kseError means error conditions.
    kseWarning,  //!< kseWarning means warning condition.
    kseNotice,  //!< kseNotice means normal but significant condition.
    kseInfomational,  //!< kseInfomational means infomational message.
    kseDebug  //!< kseDebug means debug-level message
};

/**
 * \enum WaitMilliSeconds
 * \brief Wait time by milliseconds. For the function which has "wait" or "timeout" parameter.
 * \details
 * An uint32_t derived type for specifying wait duration. The integer value represents the waiting duration by mili-seconds.
 * Usually a value of this type is passed to some functions as parameter. There are two special cases.
 *
 * kwmsPolling means function will return immediately regardless of waited event.In other word, with this parameter,
 * function causes time out immediately.
 * Some function may provides the way to know what was the status of the waited event. But some may not.
 *
 * kwmsIndefinitely means function will will not cause time out.
 */
enum WaitMilliSeconds
: uint32_t
{
    kwmsPolling = 0,  ///< Not waiting. Immediate timeout.
    kwmsIndefinitely = HAL_MAX_DELAY  ///< Wait forever
};

/**
 * \brief Attribute of the UART Hardware Flow Control.
 * \details
 * This is dedicated to the \ref UartStrategy class.
 */
enum UartHardwareFlowControl
{
    kuhfcNone,  ///< No hardware flow control
    kuhfcCts,  ///< Control CTS, but RTS
    kuhfcRts,  ///< Control RTS, but CTS
    kuhfcCtsRts  ///< Control Both CTS and RTS
};
/**
 * \brief SPI clock configuration for Master.
 * \details
 * This enum represents the setting of the SPI POL bit of the master configuration.
 * The POL setting 0/1 is RiseThenFall and Fall thenRise respectively.
 */
enum SpiClockPolarity
{
    kspoRiseThenFall = 0,  //!< kscpRiseThenFall POL = 0
    kspoFallThenRise  //!< kscpFallThenrise POL = 1
};

/**
 * \brief SPI clock configuration for master
 * \details
 * This enum represents the setting of the SPI PHA bit of the master configuration.
 * The PHA setting 0 and 1 is LatchThenShift and ShiftThenLatch respectively.
 */
enum SpiClockPhase
{
    ksphLatchThenShift = 0,  //!< kscpLatchThenShift PHA=0. The first edge is latching. The second edge is shifting.
    ksphShiftThenLatch  //!< kscpShiftThenLatch PHA = 1. The first edge is shifting. The second edge is latching.
};

/**
 * @brief Return status of the I2C classes.
 * @details
 * This enums represents the return status from the I2C class method.
 *
 * In a single master controler system, you need to care only ki2csNak and ki2csTimeOut.
 * Other error may be caused by multiple master system.
 *
 * The ki2csNak is returned when one of two happens :
 * @li The slave device terminated transfer.
 * @li No slave device responded to the address specified by master device.
 *
 * The ki2csTimeOUt is returned when slave device streched transfere too long.
 *
 * The ki2csArbitrationLost is returned when another master won the arbitration. Usulally, the master have to
 * re-try the transfer after certain waiting period.
 *
 * The ki2csBussError is fatal condition. In the master mode, it could be problem of other device.
 * The root cause is not deterministic. Probably it is hardware problem.
 */
enum I2cStatus
{
    ki2csOK = 0,  //!< ki2csOK
    ki2csTimeOut,  //!< Master mode error. No response from device.
    ki2csNak,  //!< Master mode error. Device answeres NAK
    ki2csBussError,  //!< Master&Slave mode error. START/STOP condition at irregular location.
    ki2csArbitrationLost,  //!< Master&Slave mode error. Lost arbitration against other master device.
    ki2csOverrun,  //!< Slave mode error. Overrun or Underrun was detected.
    ki2csDMA,  //!< Some error detected in DMA module.
    ki2csSizeError,
    ki2csUnknown  //!< Unknown error
};

/**
 * @brief Return status of the SPI classes
 * @details
 * This enums represents the return status of from the SPI class method.
 *
 * kspisModeFault is returned when the NSS pins are aserted. Note that the Murasaki library doesn't support
 * the Multi master SPI operation. So, this is fatal condition.
 *
 * kpisOverflow and the kpisDMA are fatal condition. These can be the problem of the lower driver problem.
 *
 */
enum SpiStatus
{
    kspisOK = 0,  //!< ki2csOK
    kspisTimeOut,  //!< Master mode error. No response from device.
    kspisModeFault,  //!< SPI mode fault error. Two master corrision
    kspisModeCRC,  //!< CRC protocol error
    kspisOverflow,  //!< Over run.
    kspisFrameError,  //!< Error on TI frame mode.
    kspisDMA,  //!< DMA error
    kspisErrorFlag,  //!< Other error flag.
    kspisAbort,  //!< Problem in abort process. No way to recover.
    kspisUnknown  //!< Unknown error
};

/**
 * @brief Return status of the UART classes.
 * @details
 * The Parity error and the Frame error may occur when user connects DCT/DTE by different
 * communicaiton setting.
 *
 * The Noise error may cuase by the noise on the line.
 *
 * The overrun may cause when the DMA is too slow or hand shake is not working well.
 *
 * The DMA error may cause some problem inisde HAL.
 *
 */
enum UartStatus
{
    kursOK = 0,  //!< No error
    kursTimeOut,  //!< Time out during transmission / receive.
    kursParity,  //!< Parity error
    kursNoise,  //!< Error by Noise
    kursFrame,  //!< Frame error
    kursOverrun,  //!< Overrun error
    kursDMA,  //!< Error inside DMA module.
    kursUnknown

};

/**
 * @brief This is specific enum for the AbstractUart::Receive() to specify the use of idle line timeout
 * @details
 * The idle line time out is dedicated function of the STM32 peripherals. The interrrupt happens
 * when the receive data is discontinued certain time.
 */
enum UartTimeout
{
    kutNoIdleTimeout = 0,  //!< kutNoIdleTimeout is specified when API should has normal timeout.
    kutIdleTimeout  //!< kutIdleTimeout is specified when API should time out by Idle line
};

/**
 * @brief Task class dedicated priority
 * @details
 * The task class priority have to be speicified by this enum class.
 * This is essential to avoid the imcompatibility with cmsis-os which uses negative
 * priority while FreeRTOS uses positive.
 */
// @formatter:off
enum TaskPriority {
    ktpIdle = 0,                                                 //!< ktpIdle
    ktpLow =            osPriorityLow           - osPriorityIdle,//!< ktpLow
    ktpBelowNormal =    osPriorityBelowNormal   - osPriorityIdle,//!< ktpBelowNormal is for the relatively low priority task.
    ktpNormal =         osPriorityNormal        - osPriorityIdle,//!< ktpNormal is for the default processing.
    ktpAboveNormal =    osPriorityAboveNormal   - osPriorityIdle,//!< ktpAboveNormal is for the relatively high priority task.
    ktpHigh =           osPriorityHigh          - osPriorityIdle,//!< ktpHigh is considered for the debug task.
    ktpRealtime =       osPriorityRealtime      - osPriorityIdle //!< ktpRealtime is dedicated for the realtime signal processing.
};
// @formatter:on
/**
 * \}
 */
/*
 *  @ingroup MURASAKI_DEFINITION_GROUP
 */

/*------------------------------- function ---------------------------------*/

/**
 * @ingroup MURASAKI_FUNCTION_GROUP
 * \{
 */

/**
 * \brief determine task or ISR context
 * \returns true if task context, false if ISR context.
 */
static inline bool IsTaskContext()
{
    // To check the task/interrupt context is done by ISPR.
    // The ISPR has ISR number which is on going.
    // If this field is zero, CPU is in the thread mode which is task context.
    // If this field is non-zero, CPU is in the handler mode which is interrupt context.
    // The field length is depend on the CORE type.
    // For the detail of the ISPR, see the "Cortex-Mx Devices Generic User Guide", where Mx is one of M0, M0+, M1, M3, M4, M7

#if defined( __CORE_CM7_H_GENERIC ) ||defined ( __CORE_CM3_H_GENERIC ) ||defined ( __CORE_CM4_H_GENERIC )
    const unsigned int active_interrupt_mask = 0x1FF; /* bit 8:0 */
#elif defined ( __CORE_CM0_H_GENERIC ) ||defined ( __CORE_CM0PLUS_H_GENERIC ) || defined ( __CORE_CM1_H_GENERIC )
    const unsigned int active_interrupt_mask = 0x03F; /* bit 5:0 */
#else
#error "Unknown core"
#endif

    return !(__get_IPSR() && active_interrupt_mask);

}

/**
 * @brief Clean and Flush the specific region of data cache.
 * @param address Start address of region
 * @param size Size of region
 * @details
 * Keep coherence between the L2 memory and d-cache, between specific region.
 *
 * The region is specified by address and size. If address is not 32byte aligned,
 * it is truncated to the 32byte alignment, and size is adjusted to follow this alignment.
 *
 * Once this function is returned, the specific region is coherent.
 */
static inline void CleanAndInvalidateDataCacheByAddress(void * address, size_t size)
                                                        {
#ifdef __CORE_CM7_H_GENERIC
    unsigned int aligned_address = reinterpret_cast<unsigned int>(address);

    // extract modulo 32. The address have to be aligned to 32byte.
    unsigned int adjustment = aligned_address & 0x1F;
    // Adjust the address and size.
    aligned_address -= adjustment;  // aligne to 32byte boarder
    size += adjustment;  // Because the start address is lower, the size is bigger.

    ::SCB_CleanInvalidateDCache_by_Addr(reinterpret_cast<long unsigned int *>(aligned_address), size);
#elif defined ( __CORE_CM0_H_GENERIC ) ||defined ( __CORE_CM0PLUS_H_GENERIC ) ||defined ( __CORE_CM3_H_GENERIC ) ||defined ( __CORE_CM4_H_GENERIC ) ||defined ( __CORE_CM1_H_GENERIC )
    // Do nothing. These core doesn't have d-cache.
#else
#error "Unknown core"
#endif
}

/**
 * @brief Clean the specific region of data cache.
 * @param address Start address of region
 * @param size Size of region
 * @details
 * Keep coherence between the L2 memory and d-cache, between specific region.
 *
 * The region is specified by address and size. If address is not 32byte aligned,
 * it is truncated to the 32byte alignment, and size is adjusted to follow this alignment.
 *
 * Once this function is returned, the specific region is coherent.
 */
static inline void CleanDataCacheByAddress(void * address, size_t size)
                                           {
#ifdef __CORE_CM7_H_GENERIC
    unsigned int aligned_address = reinterpret_cast<unsigned int>(address);

    // extract modulo 32. The address have to be aligned to 32byte.
    unsigned int adjustment = aligned_address & 0x1F;
    // Adjust the address and size.
    aligned_address -= adjustment;  // aligne to 32byte boarder
    size += adjustment;  // Because the start address is lower, the size is bigger.

    ::SCB_CleanDCache_by_Addr(reinterpret_cast<long unsigned int *>(aligned_address), size);
#elif defined ( __CORE_CM0_H_GENERIC ) ||defined ( __CORE_CM0PLUS_H_GENERIC ) ||defined ( __CORE_CM3_H_GENERIC ) ||defined ( __CORE_CM4_H_GENERIC ) ||defined ( __CORE_CM1_H_GENERIC )
    // Do nothing. These core doesn't have d-cache.
#else
#error "Unknown core"
#endif
}

/**
 * @brief Initialize and start the cycle counter
 * @details
 * This cycle counter ( CYCNT ) is implemented inside CORTEX-Mx core.
 * To implement or not is up to the SoC vender.
 *
 * Regarding CORTEX-M0 and M0+, there is no CYCCNT. THus, we do noting
 * in this function.
 *
 * Programmer can override default function because this funciton is weakly bound.
 */
extern void InitCycleCounter();
/**
 * @brief Obtain the current cycle count of CYCCNT register.
 * @return current core cycle.
 * @details
 * Regarding CORTEX-M0 and M0+, there is no CYCCNT. THus, we do noting
 * in this function.
 *
 * Programmer can override default function because this funciton is weakly bound.
 */
extern unsigned int GetCycleCounter();

/**
 *
 * @brief Keep task sleeping during the specific duration.
 * @param duration Sleeping time by milliseconds.
 * @details
 * Whenever this function is called, that task gets into the sleeping ( or waiting, the name is up to RTOS ) immediately.
 * Then, wake up after specified duration.
 *
 * Note that the duration is interpreted as "at least". The actual sleeping duration could be longer than
 * the specified duration by parameter. The worst error between the actual duration and the specified duration is
 * the period of the tick in system.
 *
 * For example, if the tick period is 10mS, the worst error is 10mS.
 */
static inline void Sleep(unsigned int duration_ms) {
    // if the parameter is Indefinitely, pass it through. If not, convert to milisecond.
    if (duration_ms == murasaki::kwmsIndefinitely)
        ::vTaskDelay(duration_ms);
    else
        ::vTaskDelay(pdMS_TO_TICKS(duration_ms));
}

/**
 * \}
 *
 */
/*
 * end of ingroup MURASAKI_FUNCTION_GROUP Definitions
 */

}  // namespace

#endif /* MURASAKI_DEFS_HPP_ */
