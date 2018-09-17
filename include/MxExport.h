#ifndef MXEXPORT_H
#define MXEXPORT_H

#ifdef __cplusplus
# define MX_EXTERN extern "C"
#else
# define MX_EXTERN
#endif

#if defined (_WIN32)
# define MX_EXPORT __declspec(dllexport)
# define MX_IMPORT __declspec(dllimport)
#elif defined (__GNUC__)
# define MX_EXPORT __attribute__((visibility("default")))
# define MX_IMPORT
#else
# define MX_EXPORT
# define MX_IMPORT
#endif

#define MX_EXPORT_C MX_EXTERN MX_EXPORT

#ifdef MXCORE_EXPORT_DLL
#define MXCORE_API MX_EXPORT
#else
#define MXCORE_API MX_IMPORT
#endif


#endif //MXEXPORT_H
