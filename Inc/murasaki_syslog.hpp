/**
 * @file murasaki_syslog.hpp
 * @brief Syslog definition
 * @date  2018/09/01
 * @author takemasa
 */

#ifndef MURASAKI_SYSLOG_HPP_
#define MURASAKI_SYSLOG_HPP_

#include <debugger.hpp>
#include "murasaki_config.hpp"
#include "murasaki_defs.hpp"
#include "string.h"


namespace murasaki {
/**
 * @brief Set the syslog severity threshold
 * @param severity
 * @details
 * Set the severity threshold. The message below this levels are ignored.
 */
void SetSyslogSererityThreshold(murasaki::SyslogSeverity severity);

/**
 * @brief Set the syslog facility mask
 * @param mask Facility bit mask. "1" allows output of the corresponding facility
 * @details
 * The parameter is not the facility. A bit mask. By default, the bit mask is 0xFFFFFFFF which allows
 * all facility.
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
 * @param severity Message seveirty
 * @return True if the message is allowed to out. False if not allowed.
 * @details
 * By comapring internal seveiry threshold and facility mask, decide
 * whether the message can be out or not.
 *
 * If seveirty is higher than or equal to kseError, message is allowed to out.
 *
 * If the severity is lower than kseError, the message is allowered to out only whhen :
 * @li The seveiry is higher than or equal to the internal threshold
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
 * \param FACILITY Specify which facility makes this log. Choose from @ref murasaki::SyslogFacility
 * \param SEVERITY Specify how message is severe. Choose from @ref murasaki::SyslogSeverity
 * \param FORMAT Message format as printf style.
 * \brief output The debug message
 * \details
 * Output the debugg message to debug console output.
 *
 * The output message is filtered by the internal thereshold set by murasaki::SetSyslogSererityThreshold,
 * murasaki::SetSyslogFacilityMask and murasaki::AddSyslogFacilityToMask. See these function's document
 * to understand how filter works.
 *
 * There is recommendation in the SEVERITY parameter :
 * @li murasaki::kseDebug for Development/Debug message for tracing normal operation.
 * @li murasaki::kseWarning for relatively severe condition which need abnormal action, or cannot handle.
 * @li murasaki::kseError for falty condtion from HAL or hardware.
 * @li murasaki::kseEmergency for software logic error like assert fail
 *
 * \ingroup MURASAKI_GROUP
 */
#if MURASAKI_CONFIG_NOSYSLOG
#define MURASAKI_SYSLOG( FACILITY, SEVERITY, FORMAT, ... )
#else
#define MURASAKI_SYSLOG( FACILITY, SEVERITY, FORMAT, ... )\
    if ( murasaki::AllowedSyslogOut(FACILITY, SEVERITY) )\
    {\
        murasaki::debugger->Printf("%s, %s: %s, line %4d, %s(): ", #FACILITY, #SEVERITY, __MURASAKI__FILE__, __LINE__, __func__);\
        murasaki::debugger->Printf( FORMAT __VA_ARGS__ );\
        murasaki::debugger->Printf("\r\n");\
    }
#endif


#endif /* MURASAKI_SYSLOG_HPP_ */
