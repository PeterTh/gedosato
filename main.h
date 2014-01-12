// GeDoSaTo Copyright 2013 Peter Thoman (Durante)

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

#define VERSION "0.1"

#define INTERCEPTOR_NAME "GeDoSaTo"

#define SETTINGS_FILE_NAME (INTERCEPTOR_NAME".ini")
#define KEY_FILE_NAME (INTERCEPTOR_NAME"Keys.ini")
#define REG_KEY_PATH ("Software\\Durante\\" INTERCEPTOR_NAME)

//#define RELEASE_VER

#ifndef RELEASE_VER
#define SDLOG(_level, _str, ...) { if(Settings::get().getLogLevel() > _level) { sdlog(_str, __VA_ARGS__); } }
#else
#define SDLOG(_level, _str, ...) {}
#endif
#define SAFERELEASE(_p) { if(_p) { (_p)->Release(); (_p) = NULL; } }
#define SAFEDELETE(_p) { if(_p) { delete (_p); (_p) = NULL; } }
#define SAFEDELETEARR(_p) { if(_p) { delete [] (_p); (_p) = NULL; } }

#define EPSILON (std::numeric_limits<float>::epsilon()*10)
#define FLT_EQ(__a, __b) (std::abs((__a) - (__b)) <= EPSILON * (std::max)(1.0f, (std::max)(std::abs(__a), std::abs(__b))))

#include "d3d9.h"

bool fileExists(const char *filename);
void __cdecl sdlogtime(int level);
void __cdecl sdlog(const char * fmt, ...);
void messageErrorAndExit(string error);
void errorExit(LPTSTR lpszFunction);

string format(const char* formatString, ...);

const string& getInstallDirectory();
const string& getExeFileName();
string getInstalledFileName(string filename);
string getAssetFileName(string filename);
string getTimeString();

LRESULT CALLBACK GeDoSaToHook(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
