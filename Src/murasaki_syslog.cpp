/*
 * murasaki_syslog.cpp
 *
 *  Created on: 2018/09/02
 *      Author: takemasa
 */

#include "murasaki_syslog.hpp"

namespace murasaki {
// mask for the Syslog facility check
static uint32_t facility_mask = 0xFFFFFFFF;
// threshold for the Syslog severity check
static murasaki::SyslogSeverity severity_threashold = murasaki::kseError;

void SetSyslogSererityThreshold(murasaki::SyslogSeverity severity) {
    severity_threashold = severity;
}

void SetSyslogFacilityMask(uint32_t mask) {
    facility_mask = mask;
}

void AddSyslogFacilityToMask(murasaki::SyslogFacility facility) {
    // set one the corresponding bit position
    facility_mask |= facility;
}

void RemoveSyslogFacilityFromMask(murasaki::SyslogFacility facility) {
    // set zero the corresponding bit position
    facility_mask &= ~facility;
}

bool AllowedSyslogOut(murasaki::SyslogFacility facility,
                      murasaki::SyslogSeverity severity) {
    // if severe than Error, always output
    // note : lower the enum order is the higher severity
    if (severity <= murasaki::kseError)
        return true;

    // or if severe than the threshold and allowed by facility mask, output
    // note : lower the enum order is the higher severity
    else if ((severity <= severity_threashold) &&
            (facility & facility_mask))
        return true;

    // otherwise, not allowed
    else
        return false;
}

}


