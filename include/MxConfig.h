#ifndef MXCONFIG_H
#define MXCONFIG_H

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
