/*****************************************************************************
 * strdup.c: POSIX strdup() replacement
 *****************************************************************************/
#include "stdafx.h"
#include <string.h>
#include <stdlib.h>

#ifndef HAVE_STRDUP
char *strdup (const char *str)
{
    size_t len = strlen (str) + 1;
    char *res = (char *)malloc (len);
    if (res)
        memcpy (res, str, len);
    return res;
}
#endif