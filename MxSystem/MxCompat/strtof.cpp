/*****************************************************************************
 * strtof.c: C strtof() replacement
 *****************************************************************************/
#include "stdafx.h"
#include <stdlib.h>

#ifndef HAVE_STRTOF
#ifndef __ANDROID__
float strtof (const char *str, char **end)
{
    return strtod (str, end);
}
#endif
#endif
