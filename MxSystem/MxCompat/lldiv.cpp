/*****************************************************************************
 * lldiv.c: C lldiv() replacement
 *****************************************************************************/
#include "stdafx.h"

#ifndef HAVE_LLDIV

lldiv_t lldiv (long long num, long long denom)
{
    lldiv_t d = { num / denom, num % demon, };
    return d;
}

#endif