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

#define MX_OBJECT( x ) ((CMxObject *)&(x)->obj)

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


// libvlc
void mx_objres_clear(CMxObject *obj);

/**
 * Assign a name to an object for vlc_object_find_name().
 */
extern int mx_object_set_name(CMxObject *, const char *);
#define mx_object_set_name(o, n) mx_object_set_name(MX_OBJECT(o), n)

/* Types */
typedef void(*mx_destructor_t) (struct CMxObject *);
void mx_object_set_destructor(CMxObject *, mx_destructor_t);
#define mx_object_set_destructor(a,b) \
        mx_object_set_destructor (MX_OBJECT(a), b)

/**
 * VLC value structure
 */
struct mx_list_t;
typedef union
{
	int64_t         i_int;
	bool            b_bool;
	float           f_float;
	char *          psz_string;
	void *          p_address;
	mx_list_t *    p_list;
	struct { int32_t x; int32_t y; } coords;

} mx_value_t;

/**
 * VLC list structure
 */
#include <vector>
struct mx_list_t
{
	int          i_type;
	/*int          i_count;
	mx_value_t *p_values;*/
	std::vector<mx_value_t> p_values;
};

/*****************************************************************************
 * Prototypes
 *****************************************************************************/
MXCORE_API void *mx_object_create(CMxObject *, size_t);
MXCORE_API CMxObject *mx_object_find_name(CMxObject *, const char *);
MXCORE_API void * mx_object_hold(CMxObject *);
MXCORE_API void mx_object_release(CMxObject *);
MXCORE_API mx_list_t *mx_list_children(CMxObject *);
MXCORE_API void mx_list_release(mx_list_t *);
MXCORE_API char *mx_object_get_name(const CMxObject *);
#define mx_object_get_name(o) mx_object_get_name(MX_OBJECT(o))

#define mx_object_create(a,b) mx_object_create( MX_OBJECT(a), b )

#define mx_object_find_name(a,b) \
    mx_object_find_name( MX_OBJECT(a),b)

#define mx_object_hold(a) \
    mx_object_hold( MX_OBJECT(a) )

#define mx_object_release(a) \
    mx_object_release( MX_OBJECT(a) )

#define mx_list_children(a) \
    mx_list_children( MX_OBJECT(a) )

MXCORE_API void *mx_obj_malloc(CMxObject *, size_t);
MXCORE_API void *mx_obj_calloc(CMxObject *, size_t, size_t);
MXCORE_API void mx_obj_free(CMxObject *, void *);


/*****************************************************************************
 * Variable callbacks: called when the value is modified
 *****************************************************************************/
typedef int(*mx_callback_t) (CMxObject *,      /* variable's object */
	char const *,            /* variable name */
	mx_value_t,                 /* old value */
	mx_value_t,                 /* new value */
	void *);                /* callback data */

/*****************************************************************************
 * List callbacks: called when elements are added/removed from the list
 *****************************************************************************/
typedef int(*mx_list_callback_t) (CMxObject *,      /* variable's object */
	char const *,            /* variable name */
	int,                  /* VLC_VAR_* action */
	mx_value_t *,      /* new/deleted value  */
	void *);                 /* callback data */

/* Integer overflow */
static inline bool uadd_overflow(unsigned a, unsigned b, unsigned *res)
{
#if MX_GCC_VERSION(5,0) || defined(__clang__)
	return __builtin_uadd_overflow(a, b, res);
#else
	*res = a + b;
	return (a + b) < a;
#endif
}

static inline bool uaddl_overflow(unsigned long a, unsigned long b,
	unsigned long *res)
{
#if MX_GCC_VERSION(5,0) || defined(__clang__)
	return __builtin_uaddl_overflow(a, b, res);
#else
	*res = a + b;
	return (a + b) < a;
#endif
}

static inline bool uaddll_overflow(unsigned long long a, unsigned long long b,
	unsigned long long *res)
{
#if MX_GCC_VERSION(5,0) || defined(__clang__)
	return __builtin_uaddll_overflow(a, b, res);
#else
	*res = a + b;
	return (a + b) < a;
#endif
}

#ifndef __cplusplus
# define add_overflow(a,b,r) \
    _Generic(*(r), \
        unsigned: uadd_overflow(a, b, (unsigned *)(r)), \
        unsigned long: uaddl_overflow(a, b, (unsigned long *)(r)), \
        unsigned long long: uaddll_overflow(a, b, (unsigned long long *)(r)))
#else
static inline bool add_overflow(unsigned a, unsigned b, unsigned *res)
{
	return uadd_overflow(a, b, res);
}

static inline bool add_overflow(unsigned long a, unsigned long b,
	unsigned long *res)
{
	return uaddl_overflow(a, b, res);
}

static inline bool add_overflow(unsigned long long a, unsigned long long b,
	unsigned long long *res)
{
	return uaddll_overflow(a, b, res);
}
#endif

#define container_of(ptr, type, member) \
    ((type *)(((char *)(ptr)) - offsetof(type, member)))


#if !defined(__NetBSD__)
/** Byte swap (16 bits) */
static inline uint16_t (bswap16)(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

/** Byte swap (32 bits) */
static inline uint32_t (bswap32)(uint32_t x)
{
#if defined (__GNUC__) || defined(__clang__)
    return __builtin_bswap32 (x);
#else
    return ((x & 0x000000FF) << 24)
    | ((x & 0x0000FF00) <<  8)
    | ((x & 0x00FF0000) >>  8)
    | ((x & 0xFF000000) >> 24);
#endif
}

/** Byte swap (64 bits) */
static inline uint64_t (bswap64)(uint64_t x)
{
#if defined (__GNUC__) || defined(__clang__)
    return __builtin_bswap64 (x);
#elif !defined (__cplusplus)
    return ((x & 0x00000000000000FF) << 56)
    | ((x & 0x000000000000FF00) << 40)
    | ((x & 0x0000000000FF0000) << 24)
    | ((x & 0x00000000FF000000) <<  8)
    | ((x & 0x000000FF00000000) >>  8)
    | ((x & 0x0000FF0000000000) >> 24)
    | ((x & 0x00FF000000000000) >> 40)
    | ((x & 0xFF00000000000000) >> 56);
#else
    return ((x & 0x00000000000000FFULL) << 56)
    | ((x & 0x000000000000FF00ULL) << 40)
    | ((x & 0x0000000000FF0000ULL) << 24)
    | ((x & 0x00000000FF000000ULL) <<  8)
    | ((x & 0x000000FF00000000ULL) >>  8)
    | ((x & 0x0000FF0000000000ULL) >> 24)
    | ((x & 0x00FF000000000000ULL) >> 40)
    | ((x & 0xFF00000000000000ULL) >> 56);
#endif
}
#endif

#ifdef WORDS_BIGENDIAN
# define hton16(i) ((uint16_t)(i))
# define hton32(i) ((uint32_t)(i))
# define hton64(i) ((uint64_t)(i))
#else
# define hton16(i) bswap16(i)
# define hton32(i) bswap32(i)
# define hton64(i) bswap64(i)
#endif
#define ntoh16(i) hton16(i)
#define ntoh32(i) hton32(i)
#define ntoh64(i) hton64(i)

/** Reads 16 bits in network byte order */
static inline uint16_t U16_AT (const void *p)
{
    uint16_t x;
    
    memcpy (&x, p, sizeof (x));
    return ntoh16 (x);
}

/** Reads 32 bits in network byte order */
static inline uint32_t U32_AT (const void *p)
{
    uint32_t x;
    
    memcpy (&x, p, sizeof (x));
    return ntoh32 (x);
}

/** Reads 64 bits in network byte order */
static inline uint64_t U64_AT (const void *p)
{
    uint64_t x;
    
    memcpy (&x, p, sizeof (x));
    return ntoh64 (x);
}

static inline void *realloc_or_free( void *p, size_t sz )
{
    void *n = realloc(p,sz);
    if( !n )
        free(p);
    return n;
}

#if (defined (LIBVLC_USE_PTHREAD) || defined(__ANDROID__)) && !defined (NDEBUG)
void vlc_assert_locked (vlc_mutex_t *);
#else
# define vlc_assert_locked( m ) (void)m
#endif

#endif //MXCOMMON_H
