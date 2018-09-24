/*****************************************************************************
 * ffsll.c: GNU ffsll() replacement
 *****************************************************************************/
#include "stdafx.h"
#include <limits.h>

int ffsll(long long x)
{
    for (unsigned i = 0; i < sizeof (x) * CHAR_BIT; i++)
        if ((x >> i) & 1)
            return i + 1;
    return 0;
}
