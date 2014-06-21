#pragma once

#include <Windows.h>
#include <string>
using std::string;

const char* SystemMetricToString(int metric);
const char* WindowLongOffsetToString(int nIndex);
const char* GetDeviceCapsParamToString(int index);

string RectToString(const RECT* rect);

DWORD RunSilent(const char* command);
