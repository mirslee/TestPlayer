#include "stdafx.h"
#include "MxConfig.h"
#include "MxCommon.h"
#include "MxMemStream.h"

#ifdef HAVE_OPEN_MEMSTREAM
int CMxMemStream::open()
{
    data.stream = open_memstream(&data.ptr, &data.length);
    return likely(data.stream != NULL) ? 0 : EOF;
}

int CMxMemStream::flush()
{
    if (unlikely(data.stream == NULL))
        return EOF;
    
    if (ferror(data.stream))
        return EOF;
    
    return fflush(data.stream);
}

int CMxMemStream::close()
{
    FILE *stream = data.stream;
    int ret;
    
    if (unlikely(stream == NULL))
        return EOF;
    
    data.stream = NULL;
    ret = ferror(stream);
    
    if (fclose(stream))
        return EOF;
    
    if (unlikely(ret))
    {
        free(data.ptr);
        return EOF;
    }
    return 0;
}

size_t CMxMemStream::write(const void *ptr, size_t len)
{
    if (unlikely(data.stream == NULL))
        return 0;
    
    return fwrite(ptr, 1, len, data.stream);
}

int CMxMemStream::putc(int c)
{
    if (unlikely(data.stream == NULL))
        return EOF;
    
    return fputc(c, data.stream);
}

int CMxMemStream::puts(const char *str)
{
    if (unlikely(data.stream == NULL))
        return EOF;
    
    return fputs(str, data.stream);
}

int CMxMemStream::vprintf(const char *fmt,
                          va_list args)
{
    if (unlikely(data.stream == NULL))
        return EOF;
    
    return vfprintf(data.stream, fmt, args);
}

#else
#include <stdlib.h>

int CMxMemStream::open()
{
    data.error = 0;
    data.ptr = (char*)calloc(1, 1);
    if (unlikely(data.ptr == NULL))
        data.error = EOF;
    data.length = 0;
    return data.error;
}

int CMxMemStream::flush()
{
    return data.error;
}

int CMxMemStream::close()
{
    if (data.error)
        free(data.ptr);
    return data.error;
}

size_t CMxMemStream::write(const void *ptr, size_t len)
{
    char *base = (char*)realloc(data.ptr, data.length + len + 1u);
    if (unlikely(base == NULL))
        goto error;
    
    memcpy(base + data.length, ptr, len);
    data.ptr = base;
    data.length += len;
    base[data.length] = '\0';
    return len;
    
error:
    data.error = EOF;
    return 0;
}

int CMxMemStream::putc(int c)
{
    return (this->write((unsigned char*)c, 1u) == 1) ? c : EOF;
}

int CMxMemStream::puts(const char *str)
{
    size_t len = strlen(str);
    return (this->write(str, len) == len) ? 0 : EOF;
}

int CMxMemStream::vprintf(const char *fmt, va_list args)
{
    va_list ap;
    char *ptr;
    int len;
    
    va_copy(ap, args);
    len = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    
    if (len < 0)
        goto error;
    
    ptr = (char*)realloc(data.ptr, data.length + len + 1);
    if (ptr == NULL)
        goto error;
    
    vsnprintf(ptr + data.length, len + 1, fmt, args);
    data.ptr = ptr;
    data.length += len;
    return len;
    
error:
    data.error = EOF;
    return EOF;
}
#endif

int CMxMemStream::printf(const char *fmt, ...)
{
    va_list ap;
    int ret;
    
    va_start(ap, fmt);
    ret = this->vprintf(fmt, ap);
    va_end(ap);
    return ret;
}
