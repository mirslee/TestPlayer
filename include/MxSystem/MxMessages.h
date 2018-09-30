/*****************************************************************************
 * vlc_messages.h: messages interface
 * This library provides basic functions for threads to interact with user
 * interface, such as message output.
 *****************************************************************************/

#ifndef MXMESSAGES_H
#define MXMESSAGES_H

#include <stdarg.h>
#include "MxCommon.h"

/**
 * \defgroup messages Logging
 * \brief Message logs
 *
 * Functions for modules to emit log messages.
 *
 * @{
 * \file
 * Logging functions
 */

/** Message types */
enum MxLogType
{
    MxLogType_INFO=0, /**< Important information */
    MxLogType_ERR,    /**< Error */
    MxLogType_WARN,   /**< Warning */
    MxLogType_DBG,    /**< Debug */
};

/**
 * Log message
 */
typedef struct MxLog
{
    uintptr_t   i_object_id; /**< Emitter (temporarily) unique object ID or 0 */
    const char *psz_object_type; /**< Emitter object type name */
    const char *psz_module; /**< Emitter module (source code) */
    const char *psz_header; /**< Additional header (used by VLM media) */
    const char *file; /**< Source code file name or NULL */
    int line; /**< Source code file line number or -1 */
    const char *func; /**< Source code calling function name or NULL */
    unsigned long tid; /**< Emitter thread ID */
} MxLog;

MXSYSTEM_API void mxLog(CMxObject *obj, int prio, const char *module,
                     const char *file, unsigned line, const char *func,
                     const char *format, ...) MX_FORMAT(7, 8);
MXSYSTEM_API void mxVaLog(CMxObject *obj, int prio, const char *module,
                       const char *file, unsigned line, const char *func,
                       const char *format, va_list ap);
#define MX_msgGenericVa(o, p, fmt, ap) \
    mxVaLog(MX_OBJECT(o), p, mx_module_name, __FILE__, __LINE__, \
              __func__, fmt, ap)

#define MX_msgGeneric(o, p, ...) \
    mxLog(MX_OBJECT(o), p, mx_module_name, __FILE__, __LINE__, \
            __func__, __VA_ARGS__)
#define MX_msgInfo(p_this, ...) \
    MX_msgGeneric(p_this, MxLogType_INFO, __VA_ARGS__)
#define MX_msgErr(p_this, ...) \
    MX_msgGeneric(p_this, MxLogType_ERR, __VA_ARGS__)
#define MX_msgWarn(p_this, ...) \
    MX_msgGeneric(p_this, MxLogType_WARN, __VA_ARGS__)
#define MX_msgDbg(p_this, ...) \
    MX_msgGeneric(p_this, MxLogType_DBG, __VA_ARGS__)

extern const char mx_module_name[];

MXSYSTEM_API const char *mxStrerror(int);
MXSYSTEM_API const char *mxStrerror_c(int);

/**
 * Message logging callback signature.
 * \param data data pointer as provided to vlc_msg_SetCallback().
 * \param type message type (VLC_MSG_* values from enum vlc_log_type)
 * \param item meta information
 * \param fmt format string
 * \param args format string arguments
 */
typedef void (*MxLogCallback) (void *data, int type, const MxLog *item, const char *fmt, va_list args);

/**
 * @}
 */
#endif /*MXMESSAGES_H*/
