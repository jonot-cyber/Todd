#ifndef CMOS_H
#define CMOS_H

#include "common.h"

// Get number of seconds from the CMOS battery. Currently broken
u8 cmos_seconds();

// Get number of minutes from the CMOS battery. Currently broken
u8 cmos_minutes();

// Get number of hours from the CMOS battery. Currently broken
u8 cmos_hours();

// Get number of days from the CMOS battery. Currently broken
u8 cmos_days();

// Get number of months from the CMOS battery. Currently broken
u8 cmos_months();

// Get number of years from the CMOS battery. Currently broken
u16 cmos_years();

#endif
