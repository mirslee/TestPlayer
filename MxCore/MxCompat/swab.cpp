/*****************************************************************************
 * swab.c: swab() replacement
 *****************************************************************************/
#include "stdafx.h"
#include <stdlib.h>
#include <stdint.h>

typedef int ssize_t;
void swab( const void *p_src_, void *p_dst_, ssize_t n )
{
    const uint8_t *p_src = (const uint8_t *)p_src_;
    uint8_t *p_dst = (uint8_t *)p_dst_;

    if( n < 0 )
        return;

    for( ssize_t i = 0; i < n-1; i += 2 )
    {
        uint8_t i_tmp = p_src[i+0];
        p_dst[i+0] = p_src[i+1];
        p_dst[i+1] = i_tmp;
    }
}

