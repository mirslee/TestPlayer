/*****************************************************************************
 * sigwait.c: POSIX sigwait() replacement
 *****************************************************************************/
#include "stdafx.h"

#ifdef __native_client__
/* NaCl has no working sigwait, but SIGPIPE, for which vlc uses sigwait
 * currently, is never generated in NaCl. So for SIGPIPE it's safe to instantly
 * return, for all others run into an assertion. */

#include <assert.h>
#include <signal.h>

int sigwait(const sigset_t *set, int *sig)
{
    sigset_t s = *set;
    if (sigemptyset(&s))
        return 0;
    assert(sigismember(&s, SIGPIPE));
    sigdelset(&s, SIGPIPE);
    assert(sigemptyset(&s));

    *sig = SIGPIPE;
    return 0;
}
#else
//# error sigwait not implemented on your platform!
#endif
