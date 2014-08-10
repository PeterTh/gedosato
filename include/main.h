// GeDoSaTo Copyright 2013-2014 Peter Thoman (Durante)

//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>

#pragma once

#include <cmath>
#include <limits>
#include <string>
using std::string;

#define EDITION "GENERIC"

#define INTERCEPTOR_NAME "GeDoSaTo"

#define SETTINGS_FILE_NAME (INTERCEPTOR_NAME".ini")
#define KEY_FILE_NAME (INTERCEPTOR_NAME"Keys.ini")
#define REG_KEY_PATH ("Software\\Durante\\" INTERCEPTOR_NAME)

//#define RELEASE_VER

#ifndef RELEASE_VER
#define SDLOG(_level, _str, ...) { if(Settings::get().getLogLevel() > _level) { sdlog(_str, __VA_ARGS__); } }
#define LOG_CHECK(_level, _code) { if(Settings::get().getLogLevel() > _level) { _code; } }
#else
#define SDLOG(_level, _str, ...) {}
#define LOG_CHECK(_level, _code) {}
#endif
#define SAFERELEASE(_p) { if(_p) { \
	auto __retval = (_p)->Release(); \
	SDLOG(10, "Saferelease: %p (%u), loc: %s : %d\n", _p, __retval, __FILE__, __LINE__); \
	(_p) = NULL; } }
#define SAFEDELETE(_p) { if(_p) { delete (_p); (_p) = NULL; } }
#define SAFEDELETEARR(_p) { if(_p) { delete [] (_p); (_p) = NULL; } }

#define EPSILON (std::numeric_limits<float>::epsilon()*10)
#define FLT_EQ(__a, __b) (std::abs((__a) - (__b)) <= EPSILON * (std::max)(1.0f, (std::max)(std::abs(__a), std::abs(__b))))
#define FLT_EPS(__a, __b, __eps) (std::abs((__a) - (__b)) <= __eps)

#include "d3d9/d3d9.h"

bool fileExists(const char *filename);
void __cdecl sdlogtime(int level);
void __cdecl sdlog(const char * fmt, ...);
void messageErrorAndExit(string error);

const string& getInstallDirectory();
const bool getUseBlacklist();
const string& getExeFileName();
string getInstalledFileName(string filename);
string getAssetFileName(string filename);
string getConfigFileName(string filename);
string getTimeString(bool forDisplay = false);

LRESULT CALLBACK GeDoSaToHook(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
const char* GeDoSaToVersion();
