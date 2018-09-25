/*****************************************************************************
 * freeaddrinfo.c: freeaddrinfo() replacement functions
 *****************************************************************************/
#include "stdafx.h"
#include <stdlib.h>

/*
 * This function must be used to free the memory allocated by getaddrinfo().
 */
#ifdef __OS2__

void freeaddrinfo (struct addrinfo *res)
{
    while (res != NULL)
    {
        struct addrinfo *next = res->ai_next;

        free (res->ai_canonname);
        free (res->ai_addr);
        free (res);
        res = next;
    }
}

#endif
