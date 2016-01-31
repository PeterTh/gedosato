#pragma once

#pragma warning(push)
#pragma warning (disable : 4005)
#define _USING_V110_SDK71_
#include <windows.h>
#include <atlbase.h>
#include <atlconv.h>
#include <mmsystem.h>
#include <Psapi.h>
// deprecated in x64 (this isn't a fix, it's just a terrible patch to get it to compile)
#ifndef GWL_WNDPROC 
#define GWL_WNDPROC (-4)
#endif 
#ifndef GWL_HINSTANCE
#define GWL_HINSTANCE (-6)
#endif
#ifndef GWL_HWNDPARENT
#define GWL_HWNDPARENT (-8)
#endif
#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif
#pragma warning(pop)

#define HEAP_LFH 2

#include <string>
using std::string;

const char* SystemMetricToString(int metric);
const char* WindowLongOffsetToString(int nIndex);
const char* GetDeviceCapsParamToString(int index);

string RectToString(const RECT* rect);

DWORD RunSilent(const char* command);

void forceLFHs();
