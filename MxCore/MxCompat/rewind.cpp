/*****************************************************************************
 * rewind.c: C rewind replacement
 *****************************************************************************/
#include "stdafx.h"
#include <stdio.h>

#ifndef HAVE_REWIND
void rewind (FILE *stream)
{
    fseek (stream, 0L, SEEK_SET);
    clearerr (stream);
}
#endif