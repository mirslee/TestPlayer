#ifndef __MXSYSTEM_H__
#define __MXSYSTEM_H__

extern MXSYSTEM_API bool g_bSyncRead;
extern MXSYSTEM_API bool g_bSyncWrite;
extern MXSYSTEM_API bool g_bRealtimeIDX;
extern MXSYSTEM_API int g_rdblocksize;
extern MXSYSTEM_API int g_wrblocksize;
extern MXSYSTEM_API bool g_bUnbuffer;

#ifdef _WIN32
MXSYSTEM_API HANDLE mxCanUnbuffer(const char* lpstr, bool bWrite);
MXSYSTEM_API HANDLE mxCanUnbuffer(const unsigned short* lpstr, bool bWrite);
MXSYSTEM_API DWORD mxGetSectorSizeForFileName(const char* lpFileName);
MXSYSTEM_API DWORD mxGetSectorSizeForFileName(const unsigned short* lpFileName);
#endif

MXSYSTEM_API mxuvoidptr mxRefFile(const char* file);
MXSYSTEM_API void mxUnrefFile(mxuvoidptr fid);


#endif //__MXSYSTEM_H__
