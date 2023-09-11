/**
 * @file murasaki_syslog.hpp
 * @brief Syslog definition
 * @date  2018/09/01
 * @author Seiichi "Suikan" Horie
 */

#ifndef MURASAKI_SYSLOG_HPP_
#define MURASAKI_SYSLOG_HPP_

// Include murasaki related headers if not google test.
#ifndef __MURASAKI_UTEST__
#include <debugger.hpp>
#include "murasaki_config.hpp"
#include "murasaki_defs.hpp"
#endif // __MURASAKI_UTEST__


#include "string.h"

namespace murasaki {
/**
 * @brief Set the syslog severity threshold
 * @param severity
 * @details
 * Set the severity threshold. The message below this levels are ignored.
 */
void SetSyslogSeverityThreshold(murasaki::SyslogSeverity severity);

/**
 * @brief Set the syslog facility mask
 * @param mask Facility bitmask. "1" allows the output of the corresponding facility
 * @details
 * The parameter is not the facility. A bitmask. By default, the bitmask is 0xFFFFFFFF which allows
 * all facilities.
 *
 * See @ref AllowedSyslogOut to understand when the message is out.
 */
void SetSyslogFacilityMask(uint32_t mask);

/**
 * @brief Add Syslog facility to the filter mask.
 * @param facility Allow this facility to output
 * @details
 * See @ref AllowedSyslogOut to understand when the message is out.
 */
void AddSyslogFacilityToMask(murasaki::SyslogFacility facility);

/**
 * @brief Remove Syslog facility to the filter mask.
 * @param facility Deny this facility to output
 * @details
 * See @ref AllowedSyslogOut to understand when the message is out.
 */
void RemoveSyslogFacilityFromMask(murasaki::SyslogFacility facility);

/**
 * @brief Check if given facility and severity message is allowed to output
 * @param facility Message facility
 * @param severity Message severity
 * @return True if the message is allowed to out. False if not allowed.
 * @details
 * By comparing internal severity threshold and facility mask, decide
 * whether the message can be out or not.
 *
 * If the severity is higher than or equal to kseError, the message is allowed to out.
 *
 * If the severity is lower than kseError, the message is allowed to out only when :
 * @li The severity is higher than or equal to the internal threshold
 * @li The facility is "1" in the corresponding bit of the internal facility mask.
 */
bool AllowedSyslogOut(murasaki::SyslogFacility facility,
                      murasaki::SyslogSeverity severity);

}

// Remove directory path from __FILE__
#ifndef __MURASAKI__FILE__
#define __MURASAKI__FILE__  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
/**
 * \def MURASAKI_SYSLOG
 * @param OBJPTR the pointer to the object. Usually, path the "this" pointer here.
 * \param FACILITY Specify which facility makes this log. Choose from @ref murasaki::SyslogFacility
 * \param SEVERITY Specify how message is severe. Choose from @ref murasaki::SyslogSeverity
 * \param FORMAT Message format as printf style.
 * \brief output The debug message
 * \details
 * Output the debugg message to debug console output.
 *
 * The output message is filtered by the internal threshold set by murasaki::SetSyslogSererityThreshold,
 * murasaki::SetSyslogFacilityMask and murasaki::AddSyslogFacilityToMask. See these function's document
 * to understand how filter works.
 *
 * There is recommendation in the SEVERITY parameter :
 * @li murasaki::kseDebug for Development/Debug message for tracing normal operation.
 * @li murasaki::kseWarning for relatively severe condition which need abnormal action, or cannot handle.
 * @li murasaki::kseError for falty condtion from HAL or hardware.
 * @li murasaki::kseEmergency for software logic error like assert fail
 *
 * The output format is as following :
 * @li Clock cycles by @ref GetCycleCounter()
 * @li Object address
 * @li Facility
 * @li Severity
 * @li File name of source code
 * @li Line number of source code
 * @li Function name
 * @li Other programmer specified information
 *
 * \ingroup MURASAKI_GROUP
 */

// Output only when SYSLOG is not true and GTEST_EXCEPT_TREE is not defined.
#if MURASAKI_CONFIG_NOSYSLOG || defined(__MURASAKI_UTEST__)
#define MURASAKI_SYSLOG( OBJPTR, FACILITY, SEVERITY, FORMAT, ... )
#else
#define MURASAKI_SYSLOG( OBJPTR, FACILITY, SEVERITY, FORMAT, ... )\
    if ( murasaki::AllowedSyslogOut(FACILITY, SEVERITY) )\
    {\
        murasaki::debugger->Printf("%10u, %p, %s, %s: %s, line %4d, %s(): " FORMAT "\n", murasaki::GetCycleCounter(), OBJPTR, #FACILITY, #SEVERITY, __MURASAKI__FILE__, __LINE__, __func__, ##__VA_ARGS__);\
    }
#endif

#endif /* MURASAKI_SYSLOG_HPP_ */
