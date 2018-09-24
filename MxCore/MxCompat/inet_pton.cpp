/*****************************************************************************
 * inet_pton.c: POSIX/IETF inet_pton() and inet_ntop() replacements
 *****************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#ifndef _WIN32
# include <sys/socket.h>
#else
# include <winsock2.h>
# include <ws2tcpip.h>
# undef EAFNOSUPPORT
# define EAFNOSUPPORT WSAEAFNOSUPPORT
#endif

#ifndef HAVE_INET_PTON
int inet_pton (int af, const char *src, void *dst)
{
    unsigned char *b = (unsigned char *)dst;

    switch (af)
    {
        case AF_INET:
            return sscanf (src, "%hhu.%hhu.%hhu.%hhu",
                           b + 0, b + 1, b + 2, b + 3) == 4;
    }
    errno = EAFNOSUPPORT;
    return -1;
}

const char *inet_ntop (int af, const void *src, char *dst, socklen_t len)
{
    const unsigned char *b = (const unsigned char *)src;

    switch (af)
    {
        case AF_INET:
            if (snprintf (dst, len, "%hhu.%hhu.%hhu.%hhu",
                          b[0], b[1], b[2], b[3]) >= len)
            {
                errno = ENOSPC;
                return NULL;
            }
            return dst;
    }
    errno = EAFNOSUPPORT;
    return NULL;
}
#endif