/*****************************************************************************
 * atof.c: C atof() replacement
 *****************************************************************************/
#include "stdafx.h"
#include <stdlib.h>

#ifndef HAVE_ATOF
#ifndef __ANDROID__
double atof (const char *str)
{
    return strtod (str, NULL);
}
#endif
#endif