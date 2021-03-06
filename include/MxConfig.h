#ifndef MXCONFIG_H
#define MXCONFIG_H

/* All timestamp below or equal to this define are invalid/unset
 * XXX the numerical value is 0 because of historical reason and will change.*/
#define MX_TS_INVALID INT64_C(0)
#define MX_TS_0 INT64_C(1)

/*****************************************************************************
 * Messages and console interfaces configuration
 *****************************************************************************/

/* Maximal depth of the object tree output by vlc_dumpstructure */
#define MAX_DUMPSTRUCTURE_DEPTH         100


#ifdef _WIN32
#include "MxConfig_win.h"
#define HAVE_SWAB 1
#define HAVE_STRUCT_POLLFD 1
#define HAVE_REWIND 1
#define HAVE_SEARCH_H 1
#define HAVE_STRDUP 1
#define HAVE_STRNLEN 1
#define HAVE_ATOF 1
#define HAVE_ATOLL 1
#define HAVE_LLDIV 1
#define HAVE_STRTOF 1
#define HAVE_STRTOLL 1
#define HAVE_INET_PTON 1
#define HAVE_TIMESPEC_GET 1
#define HAVE_GETPID 1
#define HAVE_FDOPENDIR 1
//#define HAVE_TIMEGM 1
#define HAVE_TIMESPEC_GET 1
#define HAVE_STRNICMP 1
#elif defined(__APPLE__)
#include "MxConfig_mac.h"
#endif

#ifdef _WIN32
#include "MxConfig_win.h"
#elif defined(__APPLE__)
#define HAVE_USELOCALE 1
#define HAVE_LLDIV 1
#define HAVE_VASPRINTF 1
#define HAVE_GETPID 1
#define HAVE_POSIX_MEMALIGN 1
#define HAVE_PATHCONF 1
#define HAVE_FSYNC 1
#define HAVE_NANF 1
#endif

#endif //MXCONFIG_H
