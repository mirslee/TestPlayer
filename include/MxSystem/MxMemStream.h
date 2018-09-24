#ifndef MXMEMSTREAM_H
#define MXMEMSTREAM_H

# include <stdarg.h>
# include <stdio.h>

class MXSYSTEM_API CMxMemStream {
    
public:
    struct
    {
        union
        {
            FILE *stream;
            int error;
        };
        char *ptr;
        size_t length;
    }data;
    
public:
    int open();
    int flush();
    int close();
    size_t write(const void *ptr, size_t len);
    int putc(int c);
    int puts(const char *str);
    int vprintf(const char *fmt, va_list args);
    int printf(const char *fmt, ...);
};

struct vlc_memstream
{
    union
    {
        FILE *stream;
        int error;
    };
    char *ptr;
    size_t length;
};

# ifdef __GNUC__
static inline int mx_memstream_puts_len(CMxMemStream *ms, const char *str, size_t len)
{
    return (ms->write(str, len) == len) ? (int)len : EOF;
}
#  define mx_memstream_puts(ms,s) \
(__builtin_constant_p(__builtin_strlen(s)) ? \
mx_memstream_puts_len(ms,s,__builtin_strlen(s)) : \
mx_memstream_puts(ms,s))
# endif


#endif /* MXMEMSTREAM_H */
