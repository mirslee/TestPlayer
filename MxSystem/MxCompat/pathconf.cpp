/*****************************************************************************
 * pathconf.c: POSIX pathconf() replacement
 *****************************************************************************/
#include "stdafx.h"
#include "MxCommon.h"

#ifdef __native_client__
long pathconf (const char *path, int name)
{
    MX_UNUSED(path);
    MX_UNUSED(name);
    return -1;
}
#elif defined(_WIN32)
/* Windows does not have pathconf, but that is OK */
#else
//# error pathconf not implemented on your platform!
#endif
