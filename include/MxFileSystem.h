#ifndef MXFILESYSTEM_H
#define MXFILESYSTEM_H

#include <sys/types.h>
#include <dirent.h>
#include "MxCommon.h"
#include "MxTypes.h"

struct stat;
struct iovec;

#ifdef _WIN32
	typedef int     ssize_t;
    # include <sys/stat.h>
    # ifndef stat
        #  define stat _stati64
    # endif
    # ifndef fstat
        #  define fstat _fstati64
    # endif
    # ifndef _MSC_VER
        #  undef lseek
        #  define lseek _lseeki64
    # endif
#endif

#ifdef __ANDROID__
# define lseek lseek64
#endif


MXCORE_API int mxOpen(const char *filename, int flags, ...) MX_USED;

MXCORE_API int mxOpenat(int fd, const char *filename, int flags, ...) MX_USED;

MXCORE_API int mxMkstemp( char * );

MXCORE_API int mxDup(int) MX_USED;

MXCORE_API int mxPipe(int [2]) MX_USED;

MXCORE_API int mxMemfd(void) MX_USED;

/*MXCORE_API ssize_t mxWrite(int, const void *, size_t);

MXCORE_API ssize_t mxWritev(int, const struct iovec *, int);*/

MXCORE_API int mxClose(int fd);

MXCORE_API int mxStat(const char *filename, struct stat *) MX_USED;

MXCORE_API int mxLstat(const char *filename, struct stat *) MX_USED;

MXCORE_API int mxUnlink(const char *filename);

MXCORE_API int mxRename(const char *oldpath, const char *newpath);

MXCORE_API FILE * mxFopen( const char *filename, const char *mode ) MX_USED;

MXCORE_API DIR *mxOpendir(const char *dirname) MX_USED;

MXCORE_API const char *mxReaddir(DIR *dir) MX_USED;

MXCORE_API int mxLoaddir( DIR *dir, char ***namelist, int (*select)( const char * ), int (*compar)( const char **, const char ** ) );

MXCORE_API int mxScandir( const char *dirname, char ***namelist, int (*select)( const char * ), int (*compar)( const char **, const char ** ) );

MXCORE_API int mxMkdir(const char *dirname, mode_t mode);

MXCORE_API char *mxGetcwd(void) MX_USED;

#if defined( _WIN32 )
typedef struct MX_DIR
{
    _WDIR *wdir;
    char *entry;
    union
    {
        DWORD drives;
        bool insert_dot_dot;
    } u;
} MX_DIR;

static inline int mxClosedir( DIR *dir )
{
    MX_DIR *vdir = (MX_DIR *)dir;
    _WDIR *wdir = vdir->wdir;
    
    free( vdir->entry );
    free( vdir );
    return (wdir != NULL) ? _wclosedir( wdir ) : 0;
}
# undef closedir
# define closedir mxClosedir

static inline void mxRewinddir( DIR *dir )
{
    _WDIR *wdir = *(_WDIR **)dir;
    
    _wrewinddir( wdir );
}
# undef rewinddir
# define rewinddir mxRewinddir
#endif

#ifdef __ANDROID__
# define lseek lseek64
#endif

#endif /* MXFILESYSTEM_H */
