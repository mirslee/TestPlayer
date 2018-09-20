#ifndef stdafx_h
#define stdafx_h

#ifdef _WIN32
#define MXCORE_EXPORT_DLL
#endif

#include "MxExport.h"
#include "MxConfig.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include "stdint.h"
#endif

//std
#include <string>
#include <vector>
#include <queue>
#include <iostream>
using namespace std;





#endif /* stdafx_h */
