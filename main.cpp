
#include <windows.h>
#include <fstream>
#include <ostream>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <strsafe.h>
#include <future>

#include "main.h"
#include "d3d9.h"
#include "d3dutil.h"
#include "Settings.h"
#include "KeyActions.h"
#include "Detouring.h"
#include "Registry.h"

// globals
std::ofstream g_ofile;

#define EXPORTED(__rettype, __convention, __name, ...) \
__name##_FNType True##__name;
#include "Exports.def"

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, PVOID pvReserved) {	
	if(dwReason == DLL_PROCESS_ATTACH) {
		// read install location from registry
		getInstallDirectory();
		
		// startup
		char dlldir[512];
		DisableThreadLibraryCalls(hDll);
		GetModuleFileName(hDll, dlldir, 512);
		for(int i = strlen(dlldir); i > 0; i--) { if(dlldir[i] == '\\') { dlldir[i+1] = 0; break; } }
		g_ofile.open(getInstalledFileName(LOG_FILE_NAME), std::ios::out);
		sdlogtime(-1);
		SDLOG(-1, "===== start "INTERCEPTOR_NAME" %s = fn: %s\n", VERSION, getExeFileName().c_str());
		SDLOG(-1, "===== in directory: %s\n", dlldir);
		SDLOG(-1, "===== installation directory: %s\n", getInstallDirectory().c_str());

		// load settings
		Settings::get().load();
		Settings::get().report();
		 
		KeyActions::get().load();
		KeyActions::get().report();

		// detour
		startDetour();

		// load original d3d9.dll
		HMODULE hMod;
		if(Settings::get().getDinput8dllWrapper().empty() || (Settings::get().getDinput8dllWrapper().find("none") == 0)) {
			char syspath[320];
			GetSystemDirectory(syspath, 320);
			strcat_s(syspath, "\\d3d9.dll");
			hMod = LoadLibrary(syspath);
		} else {
			sdlog(0, "Loading dinput wrapper %s\n", Settings::get().getDinput8dllWrapper().c_str());
			hMod = LoadLibrary(Settings::get().getDinput8dllWrapper().c_str());
		}
		if(!hMod) {
			sdlog("Could not load original dinput8.dll\nABORTING.\n");
			errorExit("Loading of specified dinput wrapper");
		}

		// get original symbols for exports
		#define EXPORTED(__rettype, __convention, __name, ...) \
		True##__name = (__name##_FNType) GetProcAddress(hMod, #__name);
		#include "Exports.def"

		return true;
	} else if(dwReason == DLL_PROCESS_DETACH) {
		Settings::get().shutdown();
		endDetour();
		SDLOG(-1, "===== end =\n");
		if(g_ofile) { g_ofile.close(); }
	}

    return false;
}

const std::string& getInstallDirectory() {
	static string installDir;
	if(installDir.empty()) {
		installDir = getRegString(REG_KEY_PATH, "InstallPath");
		if(installDir.empty()) {
			messageErrorAndExit("Could not read the install location from the registry.\nMake sure to extract the downloaded files to a suitable location and run install_here.bat.");
		}
	}
	return installDir;
}
const string& getExeFileName() {
	static string exeFn;
	if(exeFn.empty()) {
		char fileName[2048];
		GetModuleFileName(NULL, fileName, 2048);
		exeFn = string(fileName);
		size_t pos = exeFn.rfind("\\");
		if(pos != string::npos) {
			exeFn = exeFn.substr(pos+1);
			pos = exeFn.rfind(".exe");
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

void __cdecl sdlogtime(int level) {
	char timebuf[26];
    time_t ltime;
    struct tm gmt;
	time(&ltime);
    _gmtime64_s(&gmt, &ltime);
    asctime_s(timebuf, 26, &gmt);
	timebuf[24] = '\0'; // remove newline
	SDLOG(level, "===== %s =====\n", timebuf);
}

void __cdecl sdlog(const char *fmt, ...) {
	if(g_ofile != NULL) {
		if(!fmt) { return; }

		va_list va_alist;
		char logbuf[8192] = {0};

		va_start (va_alist, fmt);
		_vsnprintf_s(logbuf+strlen(logbuf), sizeof(logbuf) - strlen(logbuf), _TRUNCATE, fmt, va_alist);
		va_end (va_alist);

		g_ofile << logbuf;
		g_ofile.flush();
	}
}

void errorExit(LPTSTR lpszFunction) { 
    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );

    // Display the error message and exit the process
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"), lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}

bool fileExists(const char *filename) {
  std::ifstream ifile(filename);
  return NULL != ifile;
}

void messageErrorAndExit(string error) {
	std::ofstream ofile("ERROR.txt", std::ios::out);
	ofile << error;
	ofile.close();
	exit(-1);
}

#include <stdarg.h>

string format(const char* formatString, ...) {
	va_list arglist;
	va_start(arglist, formatString);
	const unsigned BUFFER_SIZE = 2048*8;
	char buffer[BUFFER_SIZE];
	vsnprintf_s(buffer, BUFFER_SIZE, formatString, arglist);
	va_end(arglist);
	return string(buffer);
}
