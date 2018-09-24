/*****************************************************************************
 * vlc_rand.h: RNG
 *****************************************************************************/

#ifndef MX_RAND_H
#define MX_RAND_H

/**
 * \file
 * This file defined random number generator function
 */

MXSYSTEM_API void mxRandBytes(void *buf, size_t len);

/* Interlocked (but not reproducible) functions for the POSIX PRNG */
MXSYSTEM_API double mxDrand48(void);
MXSYSTEM_API long mxLrand48(void);
MXSYSTEM_API long mxMrand48(void);

#endif
