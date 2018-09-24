/*****************************************************************************
 * timespec_get.c: C11 timespec_get() replacement
 *****************************************************************************/
#include "stdafx.h"

#ifndef HAVE_TIMESPEC_GET

#include <time.h>
#include <unistd.h> /* _POSIX_TIMERS */
#ifndef _POSIX_TIMERS
#define _POSIX_TIMERS (-1)
#endif
#if (_POSIX_TIMERS <= 0)
# include <sys/time.h> /* gettimeofday() */
#endif

int timespec_get(struct timespec *ts, int base)
{
    switch (base)
    {
        case TIME_UTC:
#if (_POSIX_TIMERS >= 0)
            if (clock_gettime(CLOCK_REALTIME, ts) == 0)
                break;
#endif
#if (_POSIX_TIMERS <= 0)
        {
            struct timeval tv;

            if (gettimeofday(&tv, NULL) == 0)
            {
                ts->tv_sec = tv.tv_sec;
                ts->tv_nsec = tv.tv_usec * 1000;
                break;
            }
        }
#endif
            /* fall through */
        default:
            return 0;
    }
    return base;
}

#endif  //HAVE_TIMESPEC_GET