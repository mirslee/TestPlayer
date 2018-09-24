/*****************************************************************************
 * atoll.c: C atoll() replacement
 *****************************************************************************/
#include "stdafx.h"
#include <stdlib.h>

#ifndef HAVE_ATOLL
long long atoll (const char *str)
{
    return strtoll (str, NULL, 10);
}
#endif
