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


MXSYSTEM_API int mxOpen(const char *filename, int flags, ...) MX_USED;

MXSYSTEM_API int mxOpenat(int fd, const char *filename, int flags, ...) MX_USED;

MXSYSTEM_API int mxMkstemp( char * );

MXSYSTEM_API int mxDup(int) MX_USED;

MXSYSTEM_API int mxPipe(int [2]) MX_USED;

MXSYSTEM_API int mxMemfd(void) MX_USED;

/*MXSYSTEM_API ssize_t mxWrite(int, const void *, size_t);

MXSYSTEM_API ssize_t mxWritev(int, const struct iovec *, int);*/

MXSYSTEM_API int mxClose(int fd);

MXSYSTEM_API int mxStat(const char *filename, struct stat *) MX_USED;

MXSYSTEM_API int mxLstat(const char *filename, struct stat *) MX_USED;

MXSYSTEM_API int mxUnlink(const char *filename);

MXSYSTEM_API int mxRename(const char *oldpath, const char *newpath);

MXSYSTEM_API FILE * mxFopen( const char *filename, const char *mode ) MX_USED;

MXSYSTEM_API DIR *mxOpendir(const char *dirname) MX_USED;

MXSYSTEM_API const char *mxReaddir(DIR *dir) MX_USED;

MXSYSTEM_API int mxLoaddir( DIR *dir, char ***namelist, int (*select)( const char * ), int (*compar)( const char **, const char ** ) );

MXSYSTEM_API int mxScandir( const char *dirname, char ***namelist, int (*select)( const char * ), int (*compar)( const char **, const char ** ) );

MXSYSTEM_API int mxMkdir(const char *dirname, mode_t mode);

MXSYSTEM_API char *mxGetcwd(void) MX_USED;

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
