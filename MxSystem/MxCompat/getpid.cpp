/*****************************************************************************
 * getpid.c: POSIX getpid() replacement
 *****************************************************************************/
#include "stdafx.h"

#include <sys/types.h>
#ifdef _WIN32
# include <windows.h>
#endif

#ifndef HAVE_GETPID

pid_t getpid (void)
{
#if defined (_WIN32)
    return (pid_t) GetCurrentProcessId ();
#elif defined (__native_client__)
    return 1;
#else
# error Unimplemented!
#endif
}

#endif