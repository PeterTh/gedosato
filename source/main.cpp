#include <fstream>
#include <ostream>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

#include <boost/algorithm/string.hpp>

#include "winutil.h"
#include "main.h"
#include "d3d9.h"
#include "version.h"
#include "d3dutil.h"
#include "settings.h"
#include "key_actions.h"
#include "detouring.h"
#include "registry.h"
#include "blacklist.h"
#include "string_utils.h"


FILE* g_oFile = NULL;
bool g_active = false;

LRESULT CALLBACK GeDoSaToHook(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	SDLOG(18, "GeDoSaToHook called\n");
	return CallNextHookEx(NULL, nCode, wParam, lParam); 
}

const char* GeDoSaToVersion() {
	static string verString;
	if(verString.empty()) verString = format("%s \"%s\" (%s/%s)", VER_STRING, VER_NAME, EDITION, MODE_STRING);
	return verString.c_str();
}

const char* GeDoSaToSettings() {
	return
		#define SETTING(_type, _var, _inistring, _defaultval) \
		_inistring ","
		#include "Settings.def"
		#undef SETTING
	;
}

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, PVOID pvReserved) {	
	if(dwReason == DLL_PROCESS_ATTACH) {
		OutputDebugString("GeDoSaTo: startup");

		// read install location from registry
		getInstallDirectory();
		OutputDebugString("GeDoSaTo: Got install dir");

		// don't attach to processes on the blacklist / not on the whitelist
		if(getUseBlacklist() ? onList(getExeFileName(), "blacklist.txt") : !onList(getExeFileName(), "whitelist.txt")) {
			OutputDebugString("GeDoSaTo: blacklisted / not whitelisted");
			if(getExeFileName() == "GeDoSaToTool") return true;
			return false;
		}
		g_active = true;
		OutputDebugString("GeDoSaTo: Active");
		
		// initialize log
		string logFn = format("logs\\%s_%s.log", getExeFileName().c_str(), getTimeString().c_str());
		std::replace(logFn.begin(), logFn.end(), ':', '-');
		std::replace(logFn.begin(), logFn.end(), ' ', '_');
		logFn = getInstalledFileName(logFn);
		fopen_s(&g_oFile, logFn.c_str(), "w");
		if(!g_oFile) OutputDebugString(format("GeDoSaTo: Error opening log fn %s", logFn.c_str()).c_str());
		else OutputDebugString(format("GeDoSaTo: Opening log fn %s, handle: %p", logFn.c_str(), g_oFile).c_str());
		OutputDebugString("GeDoSaTo: Log file initialized, let that take over");

		// startup
		sdlogtime(-1);
		SDLOG(-1, "===== start "INTERCEPTOR_NAME" %s = fn: %s\n", GeDoSaToVersion(), getExeFileName().c_str());
		SDLOG(-1, "===== installation directory: %s\n", getInstallDirectory().c_str());

		// load settings
		Settings::get().load();
		Settings::get().report();
		 
		KeyActions::get().load();
		KeyActions::get().report();

		if(!Settings::get().getPreventSteamOverlay()) {
			SDLOG(2, "Attempting to pre-load Steam overlay dll.\n");
			LoadLibrary("gameoverlayrenderer.dll");
		}

		if(Settings::get().getLoadD3DEarly()) {
			SDLOG(2, "Early d3d loading.\n");
			auto dllname = getSystemDllName("d3d9.dll");
			LoadLibrary(dllname.c_str());
		}

		// detour
		startDetour();
		return true;
	}
	if(dwReason == DLL_PROCESS_DETACH && g_active) {
		Settings::get().shutdown();
		endDetour();
		SDLOG(-1, "===== end =\n");
		fclose(g_oFile);
	}
    return false;
}

const std::string& getInstallDirectory() {
	static string installDir;
	if(installDir.empty()) {
		installDir = getRegString(REG_KEY_PATH, "InstallPath");
		if(installDir.empty()) {
			messageErrorAndExit("Could not read the install location from the registry.\nMake sure to extract the downloaded files to a suitable location and run GeDoSaToTool.exe");
		}
	}
	return installDir;
}
const bool getUseBlacklist() {
	return getRegString(REG_KEY_PATH, "UseBlacklist") == "True";
}


const string& getExeFileName() {
	static string exeFn;
	if(exeFn.empty()) {
		char fileName[MAX_PATH];
		GetModuleFileNameA(NULL, fileName, MAX_PATH);
		exeFn = fileName;
		size_t pos = exeFn.rfind("\\");
		if(pos != string::npos) {
			exeFn = exeFn.substr(pos+1);
			pos = exeFn.rfind(".exe");
			if(pos == string::npos) pos = exeFn.rfind(".EXE");
			if(pos != string::npos) {
				exeFn = exeFn.substr(0, pos);
			}
		}
	}
	return exeFn;
}
string getInstalledFileName(string filename) {
	return getInstallDirectory() + filename;
}
string getAssetFileName(string filename) {
	return getInstallDirectory() + "assets\\" + filename;
}
string getConfigFileName(string filename) {
	return getInstallDirectory() + "config\\" + filename;
}

string getTimeString() {
	char timebuf[26];
    time_t ltime;
    struct tm gmt;
	time(&ltime);
    _localtime64_s(&gmt, &ltime);
    asctime_s(timebuf, 26, &gmt);
	timebuf[24] = '\0'; // remove newline
	return string(timebuf);
}

void __cdecl sdlogtime(int level) {
	SDLOG(level, "===== %s =====\n", getTimeString().c_str());
}

void __cdecl sdlog(const char *fmt, ...) {
	if(!fmt) { return; }

	va_list va_alist;

	va_start (va_alist, fmt);
	vfprintf(g_oFile, fmt, va_alist);
	va_end (va_alist);

	fflush(g_oFile);
}

bool fileExists(const char *filename) {
  std::ifstream ifile(filename);
  return !!ifile;
}

void messageErrorAndExit(string error) {
	std::ofstream ofile("ERROR.txt", std::ios::out);
	ofile << error;
	ofile.close();
	exit(-1);
}


