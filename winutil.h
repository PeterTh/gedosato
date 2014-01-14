#pragma once

#include <Windows.h>
#include <string>
using std::string;

const char* SystemMetricToString(int metric);

string RectToString(const RECT* rect);
