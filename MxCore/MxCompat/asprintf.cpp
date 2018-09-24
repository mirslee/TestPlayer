/*****************************************************************************
 * asprintf.c: GNU asprintf() replacement
 *****************************************************************************/
#include "stdafx.h"
#include "MxFixups.h"
#include <stdio.h>
#include <stdarg.h>

int asprintf (char **strp, const char *fmt, ...)
{
    va_list ap;
    int ret;

    va_start (ap, fmt);
    ret = vasprintf (strp, fmt, ap);
    va_end (ap);
    return ret;
}
