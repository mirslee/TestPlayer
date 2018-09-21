
#ifndef MXCOMMON_H
#define MXCOMMON_H

#include "CMxObject.h"
/*****************************************************************************
 * Required system headers
 *****************************************************************************/
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stddef.h>

#ifndef __cplusplus
# include <stdbool.h>
#endif

/*****************************************************************************
 * Compilers definitions
 *****************************************************************************/
/* Helper for GCC version checks */
#ifdef __GNUC__
# define MX_GCC_VERSION(maj,min) \
((__GNUC__ > (maj)) || (__GNUC__ == (maj) && __GNUC_MINOR__ >= (min)))
#else
# define MX_GCC_VERSION(maj,min) (0)
#endif

/* Try to fix format strings for all versions of mingw and mingw64 */
#if defined( _WIN32 ) && defined( __USE_MINGW_ANSI_STDIO )
    #undef PRId64
    #define PRId64 "lld"
    #undef PRIi64
    #define PRIi64 "lli"
    #undef PRIu64
    #define PRIu64 "llu"
    #undef PRIo64
    #define PRIo64 "llo"
    #undef PRIx64
    #define PRIx64 "llx"
    #define snprintf __mingw_snprintf
    #define vsnprintf __mingw_vsnprintf
    #define swprintf _snwprintf
#endif

/* Function attributes for compiler warnings */
#ifdef __GNUC__
    # define MX_DEPRECATED __attribute__((deprecated))
    # if MX_GCC_VERSION(6,0)
        #  define MX_DEPRECATED_ENUM __attribute__((deprecated))
    # else
        #  define MX_DEPRECATED_ENUM
    # endif

    # if defined( _WIN32 )
        #  define MX_FORMAT(x,y) __attribute__ ((format(gnu_printf,x,y)))
    # else
        #  define MX_FORMAT(x,y) __attribute__ ((format(printf,x,y)))
    # endif
    # define MX_FORMAT_ARG(x) __attribute__ ((format_arg(x)))
    # define MX_MALLOC __attribute__ ((malloc))
    # define MX_USED __attribute__ ((warn_unused_result))

#else
    # define MX_DEPRECATED
    # define MX_DEPRECATED_ENUM
    # define MX_FORMAT(x,y)
    # define MX_FORMAT_ARG(x)
    # define MX_MALLOC
    # define MX_USED
#endif

/* Branch prediction */
#ifdef __GNUC__
    # define likely(p)     __builtin_expect(!!(p), 1)
    # define unlikely(p)   __builtin_expect(!!(p), 0)
    # define unreachable() __builtin_unreachable()
#else
    # define likely(p)     (!!(p))
    # define unlikely(p)   (!!(p))
    # define unreachable() ((void)0)
#endif

#define MX_assert_unreachable() (assert(!"unreachable"), unreachable())

/*****************************************************************************
 * Basic types definitions
 *****************************************************************************/
/**
 * High precision date or time interval
 *
 * Store a high precision date or time interval. The maximum precision is the
 * microsecond, and a 64 bits integer is used to avoid overflows (maximum
 * time interval is then 292271 years, which should be long enough for any
 * video). Dates are stored as microseconds since a common date (usually the
 * epoch). Note that date and time intervals can be manipulated using regular
 * arithmetic operators, and that no special functions are required.
 */

#ifdef _WIN32
typedef int     ssize_t;
#endif

#ifdef _MSC_VER
#define mxinline __forceinline
#else
#define mxinline inline
#endif
#define mxinline_s static mxinline

typedef int64_t mtime_t;

typedef uint32_t MxFourcc;
#ifdef WORDS_BIGENDIAN
#define MX_FOURCC( a, b, c, d )
( ((uint32_t)d) | ( ((uint32_t)c) << 8 ) \
| ( ((uint32_t)b) << 16 ) | ( ((uint32_t)a) << 24 ) )
#   define MX_TWOCC( a, b ) \
( (uint16_t)(b) | ( (uint16_t)(a) << 8 ) )

#else
#   define MX_FOURCC( a, b, c, d ) \
( ((uint32_t)a) | ( ((uint32_t)b) << 8 ) \
| ( ((uint32_t)c) << 16 ) | ( ((uint32_t)d) << 24 ) )
#   define MX_TWOCC( a, b ) \
( (uint16_t)(a) | ( (uint16_t)(b) << 8 ) )

#endif

static inline void mxFourccToChar( MxFourcc fcc, char *psz_fourcc )
{
    memcpy( psz_fourcc, &fcc, 4 );
}

#define MX_UNUSED(x) (void)(x)

static inline bool umul_overflow(unsigned a, unsigned b, unsigned *res)
{
#if MX_GCC_VERSION(5,0) || defined(__clang__)
    return __builtin_umul_overflow(a, b, res);
#else
    *res = a * b;
    return b > 0 && a > (UINT_MAX / b);
#endif
}

static inline bool umull_overflow(unsigned long a, unsigned long b,
                                  unsigned long *res)
{
#if MX_GCC_VERSION(5,0) || defined(__clang__)
    return __builtin_umull_overflow(a, b, res);
#else
    *res = a * b;
    return b > 0 && a > (ULONG_MAX / b);
#endif
}

static inline bool umulll_overflow(unsigned long long a, unsigned long long b,
                                   unsigned long long *res)
{
#if MX_GCC_VERSION(5,0) || defined(__clang__)
    return __builtin_umulll_overflow(a, b, res);
#else
    *res = a * b;
    return b > 0 && a > (ULLONG_MAX / b);
#endif
}

#ifndef __cplusplus
#define mul_overflow(a,b,r) \
_Generic(*(r), \
unsigned: umul_overflow(a, b, (unsigned *)(r)), \
unsigned long: umull_overflow(a, b, (unsigned long *)(r)), \
unsigned long long: umulll_overflow(a, b, (unsigned long long *)(r)))
#else
static inline bool mul_overflow(unsigned a, unsigned b, unsigned *res)
{
    return umul_overflow(a, b, res);
}

static inline bool mul_overflow(unsigned long a, unsigned long b,
                                unsigned long *res)
{
    return umull_overflow(a, b, res);
}

static inline bool mul_overflow(unsigned long long a, unsigned long long b,
                                unsigned long long *res)
{
    return umulll_overflow(a, b, res);
}
#endif

MX_USED MX_MALLOC
static inline void *vlc_alloc(size_t count, size_t size)
{
    return mul_overflow(count, size, &size) ? NULL : malloc(size);
}

struct libvlc_int_t
{
    //VLC_COMMON_MEMBERS
	struct libvlc_int_t* obj;
};

/*****************************************************************************
* Loosy memory allocation functions. Do not use in new code.
*****************************************************************************/
static inline void *xmalloc(size_t len)
{
	void *ptr = malloc(len);
	if (unlikely(ptr == NULL && len > 0))
		abort();
	return ptr;
}

static inline void *xrealloc(void *ptr, size_t len)
{
	void *nptr = realloc(ptr, len);
	if (unlikely(nptr == NULL && len > 0))
		abort();
	return nptr;
}

static inline void *xcalloc(size_t n, size_t size)
{
	void *ptr = calloc(n, size);
	if (unlikely(ptr == NULL && (n > 0 || size > 0)))
		abort();
	return ptr;
}

static inline char *xstrdup(const char *str)
{
	char *ptr = strdup(str);
	if (unlikely(ptr == NULL))
		abort();
	return ptr;
}

#if !defined(__cplusplus)
# define VLC_OBJECT(x) \
    _Generic((x)->obj, \
        struct vlc_common_members: (vlc_object_t *)(&(x)->obj), \
        const struct vlc_common_members: (const vlc_object_t *)(&(x)->obj) \
    )
#else
# define VLC_OBJECT( x ) ((vlc_object_t *)&(x)->obj)
#endif

MX_USED /*最大公约数*/
static inline int64_t GCD ( int64_t a, int64_t b )
{
    while( b )
    {
        int64_t c = a % b;
        a = b;
        b = c;
    }
    return a;
}

/* 判断n的二进制中1的个数的奇偶性 */
static inline unsigned parity(unsigned x)
{
#ifdef __GNUC__
    return __builtin_parity (x);
#else
    for (unsigned i = 4 * sizeof (x); i > 0; i /= 2)
        x ^= x >> i;
        return x & 1;
#endif
}

/** Count leading zeroes */
MX_USED
static inline unsigned (clz)(unsigned x)
{
#ifdef __GNUC__
    return __builtin_clz (x);
#else
    unsigned i = sizeof (x) * 8;
    
    while (x)
    {
        x >>= 1;
        i--;
    }
    return i;
#endif
}

#define clz8( x ) (clz(x) - ((sizeof(unsigned) - sizeof (uint8_t)) * 8))
#define clz16( x ) (clz(x) - ((sizeof(unsigned) - sizeof (uint16_t)) * 8))
/* XXX: this assumes that int is 32-bits or more */
#define clz32( x ) (clz(x) - ((sizeof(unsigned) - sizeof (uint32_t)) * 8))


//libc.cpp
/*typedef void * MxIconv;
MXCORE_API MxIconv mxIconvOpen( const char *tocode, const char *fromcode );
MXCORE_API size_t mxIconv( MxIconv cd, const char **inbuf, size_t *inbytesleft,
               char **outbuf, size_t *outbytesleft );
MXCORE_API int mxIconvClose( MxIconv cd );*/
MXCORE_API bool mxUreduce( unsigned *pi_dst_nom, unsigned *pi_dst_den,
               uint64_t i_nom, uint64_t i_den, uint64_t i_max );

#if defined( _WIN32 ) || defined( __OS2__ )
#   define DIR_SEP_CHAR '\\'
#   define DIR_SEP "\\"
#   define PATH_SEP_CHAR ';'
#   define PATH_SEP ";"
#else
#   define DIR_SEP_CHAR '/'
#   define DIR_SEP "/"
#   define PATH_SEP_CHAR ':'
#   define PATH_SEP ":"
#endif

#define EMPTY_STR(str) (!str || !*str)

/*****************************************************************************
 * I18n stuff
 *****************************************************************************/
MXCORE_API char *mxGettext( const char *msgid ) MX_FORMAT_ARG(1);
MXCORE_API char *mxNgettext( const char *s, const char *p, unsigned long n ) MX_FORMAT_ARG(1) MX_FORMAT_ARG(2);

#define mx_pgettext( ctx, id ) \
mx_pgettext( ctx "\004" id, id )

MX_FORMAT_ARG(2)
static inline const char *mx_pgettext_aux( const char *ctx, const char *id )
{
    const char *tr = mxGettext( ctx );
    return (tr == ctx) ? id : tr;
}


#endif //MXCOMMON_H
