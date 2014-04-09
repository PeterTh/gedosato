#pragma once

#include <Windows.h>
#include <string>
using std::string;

const char* SystemMetricToString(int metric);
const char* WindowLongOffsetToString(int nIndex);

string RectToString(const RECT* rect);

DWORD RunSilent(const char* command);
