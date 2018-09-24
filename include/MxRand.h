/*****************************************************************************
 * vlc_rand.h: RNG
 *****************************************************************************/

#ifndef MX_RAND_H
#define MX_RAND_H

/**
 * \file
 * This file defined random number generator function
 */

MXCORE_API void mxRandBytes(void *buf, size_t len);

/* Interlocked (but not reproducible) functions for the POSIX PRNG */
MXCORE_API double mxDrand48(void) MX_USED;
MXCORE_API long mxLrand48(void) MX_USED;
MXCORE_API long mxMrand48(void) MX_USED;

#endif
