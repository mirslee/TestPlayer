/*****************************************************************************
 * strnlen.c: POSIX strnlen() replacement
 *****************************************************************************/
#include "stdafx.h"
#include <string.h>

#ifndef HAVE_STRNLEN
size_t strnlen (const char *str, size_t max)
{
    const char *end = (const char *)memchr (str, 0, max);
    return end ? (size_t)(end - str) : max;
}
#endif