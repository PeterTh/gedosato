
// This is a "shim" dll for loading GeDoSaTo into whitelisted programs
//
// It is designed to be minimally invasive, as it will be loaded into every process which loads user32.dll
// Therefore, its only dependencies are on kernel32.dll, no CRT or other Windows libs.
//
// General workflow:
// 1) On DLL_PROCESS_ATTACH:
// 1.1) DisableThreadLibraryCalls 
// 1.2) Create a Hooking Thread
// 1.3) Return
// 2) In the hooking thread:
// 2.1) Determine dll path and exe name
// 2.2) load the whitelist files and check executable name against their patterns
// 2.3) if there's a match, load the actual gedosato.dll
// 2.4) FreeLibraryAndExitThread

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if 1
#define DBG(__str) OutputDebugString(__str)
#define DBGA(__str) OutputDebugStringA(__str)
#else
#define DBG(__str) 
#define DBGA(__str) 
#endif

#define PATH_SIZE 256
#define SHIM_DLL_NAME L"gedoshim.dll"
#define MAIN_GEDO_DLL_NAME L"GeDoSaTo.dll"
#define BAIL_ON(__condition, __msg) if(__condition) { DBG(__msg); return false; }

typedef void(__cdecl *gedosatoInitFuncType)();

// Yay C string processing without any libraries! Not annoying at all.
#pragma region String processing

bool matchWildcard(char *subject, char *pattern) {
	for(; *pattern; pattern++) {
		switch(*pattern) {
		case '*':
			if(*(pattern + 1) == '\0') {
				// This wildcard appears at the end of the pattern.
				// If we made it this far already, the match succeeds regardless of what the rest of the subject looks like.
				return true;
			}
			for(char *s = subject; *s; s++) {
				if(matchWildcard(s, pattern + 1)) return true;
			}
			return false;
		case '?':
			if(*(subject++) == '\0') return false;
			break;
		default:
			if(*subject == '\0' || *pattern != *(subject++)) {
				return false;
			}
		}
	}
	// End of pattern reached.  If this also the end of the subject, then match succeeds.
	return *subject == '\0';
}

bool checkWhitelistEntries(char* whitelistBuffer, LPCTSTR exeFileName) {
	// convert exe file name
	char exeFn[PATH_SIZE];
	char defChr = '?';
	auto convRet = WideCharToMultiByte(CP_ACP, 0, exeFileName, -1, exeFn, PATH_SIZE, &defChr, NULL);
	BAIL_ON(convRet == 0, L"gedosato_shim: could not convert exe name to ANSI\r\n");
	DBG(L"gedosato_shim: converted exe file name:\r\n"); DBGA(exeFn);

	DBG(L"gedosato_shim: checking whitelist:\r\n"); DBGA(whitelistBuffer);

	char *wListCur = whitelistBuffer, *curStart = nullptr, *lastNonWs = nullptr;
	while(*wListCur != '\0') {
		switch(*wListCur) {
		case '#': // comment until EOL
			DBG(L"gedosato_shim: -- skipping comment.\r\n");
			while(*wListCur != '\0' && *wListCur != '\n') wListCur++;
			break;
		case '\r': // skip empty lines
		case '\n':
			DBG(L"gedosato_shim: -- skipping empty line.\r\n");
			wListCur++;
			break;
		default: // extract executable pattern (start until last non-whitespace before | or newline)
			DBG(L"gedosato_shim: -- handling new entry.\r\n");
			curStart = wListCur;
			lastNonWs = nullptr;
			while(*wListCur != '\0' && *wListCur != '|' && *wListCur != '\n' && *wListCur != '\r') {
				if(*wListCur != '\t' && *wListCur != ' ') lastNonWs = wListCur;
				wListCur++;
			}
			if(*wListCur != '\0') wListCur++;
			if(lastNonWs) {
				lastNonWs[1] = '\0';
				DBGA(curStart);
				// check this pattern against exeName
				if(matchWildcard(exeFn, curStart)) return true;
				// consume until EOL
				while(*wListCur != '\0' && *wListCur != '\n') wListCur++;
			}
			else {
				DBG(L"gedosato_shim: -- no lastNonWS.\r\n");
			}
		}
	}
	return false;
}

#pragma endregion

bool checkWhitelist(LPCTSTR directory, LPCTSTR fileName, LPCTSTR exeFileName) {
	// build path whitelist
	TCHAR listPath[PATH_SIZE*2];
	lstrcpy(listPath, directory);
	lstrcat(listPath, fileName);
	DBG(L"gedosato_shim: loading whitelist:\r\n"); DBG(listPath);

	// open whitelist file for reading and determine size
	HANDLE file = CreateFile(listPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	BAIL_ON(file == INVALID_HANDLE_VALUE, L"gedosato_shim: could not open whitelist file for reading\r\n");
	LARGE_INTEGER whitelistSize;
	BOOL gotFileSize = GetFileSizeEx(file, &whitelistSize);
	BAIL_ON(gotFileSize == FALSE, L"gedosato_shim: could not determine whitelist size\r\n");
	DBG(L"gedosato_shim: opened whitelist.\r\n");

	// read file and close
	LPVOID buffer = HeapAlloc(GetProcessHeap(), 0, (SIZE_T)whitelistSize.QuadPart+1);
	BAIL_ON(buffer == NULL, L"gedosato_shim: could not allocate buffer to load whitelist\r\n");
	DWORD bytesRead;
	BOOL readFile = ReadFile(file, buffer, (SIZE_T)whitelistSize.QuadPart, &bytesRead, NULL);
	BAIL_ON(readFile == FALSE, L"gedosato_shim: failed to read whitelist file\r\n");
	CloseHandle(file);
	DBG(L"gedosato_shim: read whitelist.\r\n");

	// process buffer
	char* whitelistBuffer = (char*)buffer;
	whitelistBuffer[whitelistSize.QuadPart] = '\0';
	bool ret = checkWhitelistEntries(whitelistBuffer, exeFileName);
	DBG(L"gedosato_shim: processed whitelist.\r\n");

	// free buffer and return
	HeapFree(GetProcessHeap(), 0, buffer);
	return ret;
}

bool hookIfRequired(HMODULE hModule) {
	// retrieve the fully qualified dll path
	TCHAR dllPath[PATH_SIZE];
	auto dllPathSize = GetModuleFileName(hModule, dllPath, PATH_SIZE);
	BAIL_ON(dllPathSize == 0 || dllPathSize == PATH_SIZE, L"gedosato_shim: could not retreive dll path\r\n");
	DBG(L"gedosato_shim: dll path:\r\n"); DBG(dllPath);

	// get dll directory from path
	auto dllPathIndex = FindStringOrdinal(FIND_FROMSTART, dllPath, -1, SHIM_DLL_NAME, -1, TRUE);
	BAIL_ON(dllPathIndex <= 0, L"gedosato_shim: dll path unexpected end\r\n");
	dllPath[dllPathIndex] = '\0';
	DBG(L"gedosato_shim: dll directory:\r\n"); DBG(dllPath);

	// retrieve the executable name of the process
	TCHAR exePath[PATH_SIZE];
	auto exePathSize = GetModuleFileName(NULL, exePath, PATH_SIZE);
	BAIL_ON(exePathSize == 0 || exePathSize == PATH_SIZE, L"gedosato_shim: could not retreive exe path\r\n");
	DBG(L"gedosato_shim: exe path:\r\n"); DBG(exePath);
	auto exeIndex = FindStringOrdinal(FIND_FROMEND, exePath, -1, L"\\", 1, TRUE);
	BAIL_ON(exeIndex <= 0, L"gedosato_shim: exe path directory end not found\r\n");
	TCHAR* exeFileName = exePath + exeIndex + 1;
	exeIndex = FindStringOrdinal(FIND_FROMEND, exeFileName, -1, L".exe", 1, TRUE);
	BAIL_ON(exeIndex <= 0, L"gedosato_shim: exe file ending not found\r\n");
	exeFileName[exeIndex] = '\0';
	DBG(L"gedosato_shim: exe file name:\r\n"); DBG(exeFileName);

	// check whitelists
	bool whitelisted = checkWhitelist(dllPath, L"config\\whitelist.txt", exeFileName) || checkWhitelist(dllPath, L"config\\whitelist_user.txt", exeFileName);
	if(whitelisted) {
		DBG(L"gedosato_shim: whitelisted!\r\n");
		// build dll path
		TCHAR mainDllPath[PATH_SIZE * 2];
		lstrcpy(mainDllPath, dllPath);
		lstrcat(mainDllPath, MAIN_GEDO_DLL_NAME);
		DBG(L"gedosato_shim: loading main dll:\r\n"); DBG(mainDllPath);
		// perform actual hooking
		HMODULE mainDllModule = LoadLibrary(mainDllPath);
		BAIL_ON(mainDllModule <= 0, L"gedosato_shim: could not load main GeDoSaTo dll\r\n");
		gedosatoInitFuncType initFunc = (gedosatoInitFuncType)GetProcAddress(mainDllModule, "GeDoSaToInit");
		BAIL_ON(mainDllModule <= 0, L"gedosato_shim: could not resolve initialization procedure address\r\n");
		initFunc();
		DBG(L"gedosato_shim: done.\r\n");
		return true;
	}
	else {
		DBG(L"gedosato_shim: not whitelisted!\r\n");
	}
	return false;
}

DWORD hookThread(LPVOID hModule) {
	hookIfRequired((HMODULE)hModule);
	FreeLibraryAndExitThread((HMODULE)hModule, 0);
	return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {
	if(fdwReason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
		DBG(L"gedosato_shim: DllMain create thread\r\n");
		HANDLE hookThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&hookThread, (LPVOID)hModule, 0, NULL);
		SetThreadPriority(hookThreadHandle, THREAD_PRIORITY_TIME_CRITICAL);
	}
	return TRUE;
}
