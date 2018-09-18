#ifndef MXCONFIG_H
#define MXCONFIG_H

/* All timestamp below or equal to this define are invalid/unset
 * XXX the numerical value is 0 because of historical reason and will change.*/
#define MX_TS_INVALID INT64_C(0)
#define MX_TS_0 INT64_C(1)

#ifdef _WIN32
#include "MxConfig_win.h"
#elif defined(__APPLE__)
#include "MxConfig_mac.h"
#endif

#ifdef _WIN32
#include "MxConfig_win.h"
#elif defined(__APPLE__)
#define HAVE_USELOCALE
#endif

#endif //MXCONFIG_H
