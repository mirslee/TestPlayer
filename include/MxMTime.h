#ifndef MXMTIME_H
#define MXMTIME_H

/*****************************************************************************
 * vlc_mtime.h: high resolution time management functions
 *****************************************************************************
 * This header provides portable high precision time management functions,
 * which should be the only ones used in other segments of the program, since
 * functions like gettimeofday() and ftime() are not always supported.
 * Most functions are declared as inline or as macros since they are only
 * interfaces to system calls and have to be called frequently.
 * 'm' stands for 'micro', since maximum resolution is the microsecond.
 * Functions prototyped are implemented in interface/mtime.c.
 *****************************************************************************/


/*****************************************************************************
 * LAST_MDATE: date which will never happen
 *****************************************************************************
 * This date can be used as a 'never' date, to mark missing events in a function
 * supposed to return a date, such as nothing to display in a function
 * returning the date of the first image to be displayed. It can be used in
 * comparaison with other values: all existing dates will be earlier.
 *****************************************************************************/
#define LAST_MDATE ((mtime_t)((uint64_t)(-1)/2))

/*****************************************************************************
 * MSTRTIME_MAX_SIZE: maximum possible size of mstrtime
 *****************************************************************************
 * This values is the maximal possible size of the string returned by the
 * mstrtime() function, including '-' and the final '\0'. It should be used to
 * allocate the buffer.
 *****************************************************************************/
#define MSTRTIME_MAX_SIZE 22

/*****************************************************************************
 * Prototypes
 *****************************************************************************/
MXSYSTEM_API char * secstotimestr( char *psz_buffer, int32_t secs );

/*****************************************************************************
 * date_t: date incrementation without long-term rounding errors
 *****************************************************************************/
#include "MxCommon.h"
struct date_t
{
    mtime_t  date;
    uint32_t i_divider_num;
    uint32_t i_divider_den;
    uint32_t i_remainder;
};

MXSYSTEM_API void date_Init( date_t *, uint32_t, uint32_t );
MXSYSTEM_API void date_Change( date_t *, uint32_t, uint32_t );
MXSYSTEM_API void date_Set( date_t *, mtime_t );
MXSYSTEM_API mtime_t date_Get( const date_t * );
MXSYSTEM_API void date_Move( date_t *, mtime_t );
MXSYSTEM_API mtime_t date_Increment( date_t *, uint32_t );
MXSYSTEM_API mtime_t date_Decrement( date_t *, uint32_t );
MXSYSTEM_API uint64_t NTPtime64();







#endif /* MXMTIME_H */
