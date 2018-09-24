/*****************************************************************************
 * sincos.c: GNU sincos() & sincosf() replacements
 *****************************************************************************/
#include "stdafx.h"
#include <math.h>

void sincos(double r, double * sr, double * cr)
{
    *sr = sin(r);
    *cr = cos(r);
}

void sincosf(float r, float * sr, float * cr)
{
    *sr = sinf(r);
    *cr = cosf(r);
}
