#pragma once

#pragma warning(push)
#pragma warning (disable : 4005)
#define _USING_V110_SDK71_
#include <windows.h>
#include <atlbase.h>
#include <atlconv.h>
#include <mmsystem.h>
#include <Psapi.h>
#pragma warning(pop)

#include <string>
using std::string;

const char* SystemMetricToString(int metric);
const char* WindowLongOffsetToString(int nIndex);
const char* GetDeviceCapsParamToString(int index);

string RectToString(const RECT* rect);

DWORD RunSilent(const char* command);
