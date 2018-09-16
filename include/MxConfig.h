#ifndef MXCONFIG_H
#define MXCONFIG_H

#ifdef _WIN32
#include "MxConfig_win.h"
#elif defined(__APPLE__)
#include "MxConfig_mac.h"
#endif

#endif //MXCONFIG_H
