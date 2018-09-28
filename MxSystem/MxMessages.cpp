#include "stdafx.h"
#include "MxMessages.h"

const char mx_module_name[] = {'\0'};

MXSYSTEM_API void mxLog(CMxObject *obj, int prio, const char *module,
                        const char *file, unsigned line, const char *func,
                        const char *format, ...) MX_FORMAT(7, 8)
{
    
}

MXSYSTEM_API void mxVaLog(CMxObject *obj, int prio, const char *module,
                          const char *file, unsigned line, const char *func,
                          const char *format, va_list ap)
{
    
}

MXSYSTEM_API const char *mxStrerror(int)
{
    return "";
}

MXSYSTEM_API const char *mxStrerror_c(int)
{
    return "";
}
