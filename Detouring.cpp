#include "Detouring.h"

#include <Windows.h>
#include <atlbase.h>
#include <atlconv.h>
#include <mmsystem.h>
#include <Psapi.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <MinHook.h>

#include "string_utils.h"

#include "Settings.h"
#include "RenderstateManager.h"

#include "d3d9.h"
#include "d3d9int_ex.h"
#include "d3dutil.h"
#include "winutil.h"

#define GENERATE_INTERCEPT_HEADER(__name, __rettype, __convention, ...) \
typedef __rettype (__convention * __name##_FNType)(__VA_ARGS__); \
__name##_FNType True##__name, __name##Pointer; \
bool completed##__name##Detour = false; \
__rettype __convention Detoured##__name(__VA_ARGS__)

// LoadLibrary /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
	void restartDetour(LPCSTR fn) {
		static std::set<string> loaded;
		string fns(fn);
		boost::algorithm::to_lower(fns);
		if(loaded.count(fns) == 0) {
			loaded.insert(fns);
			startDetour();
		}
	}
}

GENERATE_INTERCEPT_HEADER(LoadLibraryA, HMODULE, WINAPI, _In_ LPCSTR lpLibFileName) {
	SDLOG(2, "DetouredLoadLibraryA %s\n", lpLibFileName);
	HMODULE mod = TrueLoadLibraryA(lpLibFileName);
	// restart detour in case we missed anything
	if(mod) restartDetour(lpLibFileName);
	return mod;
}
GENERATE_INTERCEPT_HEADER(LoadLibraryW, HMODULE, WINAPI, _In_ LPCWSTR lpLibFileName) {
	SDLOG(2, "DetouredLoadLibraryW %s\n", CW2A(lpLibFileName));
	HMODULE mod = TrueLoadLibraryW(lpLibFileName);
	// restart detour in case we missed anything
	if(mod) restartDetour(CW2A(lpLibFileName));
	return mod;
}

GENERATE_INTERCEPT_HEADER(LoadLibraryExA, HMODULE, WINAPI, _In_ LPCSTR lpLibFileName, _Reserved_ HANDLE hFile, _In_ DWORD dwFlags) {
	SDLOG(2, "DetouredLoadLibraryExA %s\n", lpLibFileName);
	HMODULE mod = TrueLoadLibraryExA(lpLibFileName, hFile, dwFlags);
	// restart detour in case we missed anything
	if(mod) restartDetour(lpLibFileName);
	return mod;
}
GENERATE_INTERCEPT_HEADER(LoadLibraryExW, HMODULE, WINAPI, _In_ LPCWSTR lpLibFileName, _Reserved_ HANDLE hFile, _In_ DWORD dwFlags) {
	SDLOG(2, "DetouredLoadLibraryExW %s\n", CW2A(lpLibFileName));
	HMODULE mod = TrueLoadLibraryExW(lpLibFileName, hFile, dwFlags);
	// restart detour in case we missed anything
	if(mod) restartDetour(CW2A(lpLibFileName));
	return mod;
}

// D3D9 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(Direct3DCreate9, IDirect3D9*, APIENTRY, UINT SDKVersion) {
	SDLOG(0, "=== DetrouredDirect3DCreate9 V:%d\n", SDKVersion);
	IDirect3D9 *d3dint = NULL;
	d3dint = TrueDirect3DCreate9(SDKVersion);

	if(d3dint != NULL) {
		hkIDirect3D9 *ret = new hkIDirect3D9(&d3dint);
		SDLOG(0, "=== Direct3DCreate9 hooking %p\n", ret);
	}
	SDLOG(0, "=== Direct3DCreate9 returning\n");
	return d3dint;
}

GENERATE_INTERCEPT_HEADER(Direct3DCreate9Ex, HRESULT, APIENTRY, UINT SDKVersion, IDirect3D9Ex** pRet) {
	SDLOG(0, "=== DetrouredDirect3DCreate9Ex V:%d\n", SDKVersion);
	HRESULT res = TrueDirect3DCreate9Ex(SDKVersion, pRet);
	
	if(*pRet != NULL) {
		*pRet = new hkIDirect3D9Ex(pRet);
		SDLOG(0, "=== Direct3DCreate9Ex hooking %p\n", *pRet);
	}
	SDLOG(0, "=== Direct3DCreate9Ex returning\n");
	return res;
}

// D3DX Textures & Shaders /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(D3DXCreateTexture, HRESULT, WINAPI, _In_ LPDIRECT3DDEVICE9 pDevice, _In_ UINT Width, _In_ UINT Height, 
						  _In_ UINT MipLevels, _In_ DWORD Usage, _In_ D3DFORMAT Format, _In_ D3DPOOL Pool, _Out_ LPDIRECT3DTEXTURE9 *ppTexture) {
	SDLOG(4, "DetouredD3DXCreateTexture\n");
	HRESULT res = TrueD3DXCreateTexture(pDevice, Width, Height, MipLevels, Usage, Format, Pool, ppTexture);
	return res;
}
GENERATE_INTERCEPT_HEADER(D3DXCreateTextureFromFileInMemory, HRESULT, WINAPI, _In_ LPDIRECT3DDEVICE9 pDevice, _In_ LPCVOID pSrcData, _In_ UINT SrcDataSize, _Out_ LPDIRECT3DTEXTURE9 *ppTexture) {
	SDLOG(4, "DetouredD3DXCreateTextureFromFileInMemory\n");
	HRESULT res = TrueD3DXCreateTextureFromFileInMemory(pDevice, pSrcData, SrcDataSize, ppTexture);
	RSManager::get().registerD3DXCreateTextureFromFileInMemory(pSrcData, (SrcDataSize == 2147483647u) ? 256 : SrcDataSize, *ppTexture);
	return res;
}
GENERATE_INTERCEPT_HEADER(D3DXCreateTextureFromFileInMemoryEx, HRESULT, WINAPI, LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, 
						  UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO *pSrcInfo, PALETTEENTRY *pPalette, LPDIRECT3DTEXTURE9 *ppTexture) {
	SDLOG(4, "DetouredD3DXCreateTextureFromFileInMemoryEx: \n -- pDevice %p, pSrcData %p, SrcDataSize %u, Width %u, Height %u, MipLevels %u, Usage %d, Format %s\n",
		pDevice, pSrcData, SrcDataSize, Width, Height, MipLevels, Usage, D3DFormatToString(Format));
	HRESULT res = RSManager::get().redirectD3DXCreateTextureFromFileInMemoryEx(pDevice, pSrcData, SrcDataSize, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
	if(SrcDataSize == 2147483647u) SrcDataSize = Width*Height/2;
	RSManager::get().registerD3DXCreateTextureFromFileInMemory(pSrcData, SrcDataSize, *ppTexture); 
	return res;
}

GENERATE_INTERCEPT_HEADER(D3DXCompileShader, HRESULT, WINAPI, _In_ LPCSTR pSrcData, _In_ UINT srcDataLen, _In_ const D3DXMACRO *pDefines, _In_ LPD3DXINCLUDE pInclude, _In_ LPCSTR pFunctionName, 
						  _In_ LPCSTR pProfile, _In_ DWORD Flags, _Out_ LPD3DXBUFFER *ppShader, _Out_ LPD3DXBUFFER *ppErrorMsgs, _Out_ LPD3DXCONSTANTTABLE *ppConstantTable) {
	HRESULT res = TrueD3DXCompileShader(pSrcData, srcDataLen, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable);
	RSManager::get().registerD3DXCompileShader(pSrcData, srcDataLen, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable);
	return res;
}

// Display Enum /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: Refactor these 2
GENERATE_INTERCEPT_HEADER(EnumDisplaySettingsExA, BOOL, WINAPI, _In_ LPCTSTR lpszDeviceName, _In_ DWORD iModeNum, _Out_ DEVMODE *lpDevMode, _In_ DWORD dwFlags) {
	SDLOG(2, "EnumDisplaySettingsExA %s -- %u\n", lpszDeviceName?lpszDeviceName:"NULL", iModeNum);
	BOOL ret = TrueEnumDisplaySettingsExA(lpszDeviceName, iModeNum, lpDevMode, dwFlags);
	if(ret) {
		SDLOG(2, "-> %ux%u %ubpp @ %u Hz\n", lpDevMode->dmPelsWidth, lpDevMode->dmPelsHeight, lpDevMode->dmBitsPerPel, lpDevMode->dmDisplayFrequency);
	} else {
		SDLOG(2, "-> NULL\n");
	}
	// find initial count of modes when Windows caches the list (at id 0 call)
	static unsigned emptyMode = 0;
	if(iModeNum == 0) {
		emptyMode = 1;
		DEVMODEA mode;
		mode.dmSize = sizeof(DEVMODEA);
		mode.dmDriverExtra = 0;
		while(TrueEnumDisplaySettingsExA(lpszDeviceName, emptyMode, &mode, dwFlags) != NULL) ++emptyMode;
		SDLOG(2, "*** Found Emptymode: %u\n", emptyMode);
	}
	// enter our mode, either if its number is requested
	if((emptyMode != 0 && iModeNum == emptyMode && ret == NULL)
	// or if we are downsampling and need to fake it as current
	 /*|| (RSManager::currentlyDownsampling() && iModeNum == ENUM_CURRENT_SETTINGS)*/ ) {
		lpDevMode->dmBitsPerPel = 32;
		lpDevMode->dmPelsWidth = Settings::get().getRenderWidth();
		lpDevMode->dmPelsHeight = Settings::get().getRenderHeight();
		lpDevMode->dmDisplayFlags = 0;
		lpDevMode->dmDisplayFrequency = Settings::get().getReportedHz();
		SDLOG(2, "-> Fake\n");
		return TRUE;
	}
	if(ret && lpDevMode->dmPelsWidth == Settings::get().getOverrideWidth() && lpDevMode->dmPelsHeight == Settings::get().getOverrideHeight()) {
		lpDevMode->dmPelsWidth = Settings::get().getRenderWidth();
		lpDevMode->dmPelsHeight = Settings::get().getRenderHeight();
		SDLOG(2, "-> Overrride\n");
	}
	return ret;
}
GENERATE_INTERCEPT_HEADER(EnumDisplaySettingsExW, BOOL, WINAPI, _In_ LPCWSTR lpszDeviceName, _In_ DWORD iModeNum, _Out_ DEVMODEW *lpDevMode, _In_ DWORD dwFlags) {
	SDLOG(2, "EnumDisplaySettingsExW %s -- %u\n", lpszDeviceName?CW2A(lpszDeviceName):"NULL", iModeNum);
	BOOL ret = TrueEnumDisplaySettingsExW(lpszDeviceName, iModeNum, lpDevMode, dwFlags);
	if(ret) {
		SDLOG(2, "-> %ux%u %ubpp @ %u Hz\n", lpDevMode->dmPelsWidth, lpDevMode->dmPelsHeight, lpDevMode->dmBitsPerPel, lpDevMode->dmDisplayFrequency);
	} else {
		SDLOG(2, "-> NULL\n");
	}
	// find initial count of modes when Windows caches the list (at id 0 call)
	static unsigned emptyMode = 0;
	if(iModeNum == 0) {
		emptyMode = 1;
		DEVMODEW mode;
		mode.dmSize = sizeof(DEVMODEW);
		mode.dmDriverExtra = 0;
		while(TrueEnumDisplaySettingsExW(lpszDeviceName, emptyMode, &mode, dwFlags) != NULL) ++emptyMode;
		SDLOG(2, "*** Found Emptymode: %u\n", emptyMode);
	}
	// enter our mode, either if its number is requested
	if((emptyMode != 0 && iModeNum == emptyMode && ret == NULL)
	// or if we are downsampling and need to fake it as current
	 /*|| (RSManager::currentlyDownsampling() && iModeNum == ENUM_CURRENT_SETTINGS)*/ ) {
		lpDevMode->dmBitsPerPel = 32;
		lpDevMode->dmPelsWidth = Settings::get().getRenderWidth();
		lpDevMode->dmPelsHeight = Settings::get().getRenderHeight();
		lpDevMode->dmDisplayFlags = 0;
		lpDevMode->dmDisplayFrequency = Settings::get().getReportedHz();
		SDLOG(2, "-> Fake\n");
		return TRUE;
	}
	if(ret && lpDevMode->dmPelsWidth == Settings::get().getOverrideWidth() && lpDevMode->dmPelsHeight == Settings::get().getOverrideHeight()) {
		lpDevMode->dmPelsWidth = Settings::get().getRenderWidth();
		lpDevMode->dmPelsHeight = Settings::get().getRenderHeight();
		SDLOG(2, "-> Overrride\n");
	}
	return ret;
}

// Display Changing /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename DEVM>
void fixDevMode(DEVM* lpDevMode) {
	if(lpDevMode->dmPelsWidth == Settings::get().getRenderWidth() && lpDevMode->dmPelsHeight == Settings::get().getRenderHeight()) {
		SDLOG(2, " -> Overriding\n");
		lpDevMode->dmPelsWidth = Settings::get().getPresentWidth();
		lpDevMode->dmPelsHeight = Settings::get().getPresentHeight();
		lpDevMode->dmDisplayFrequency = Settings::get().getPresentHz();
	}
}

GENERATE_INTERCEPT_HEADER(ChangeDisplaySettingsExA, LONG, WINAPI, _In_opt_ LPCSTR lpszDeviceName, _In_opt_ DEVMODEA* lpDevMode, _Reserved_ HWND hwnd, _In_ DWORD dwflags, _In_opt_ LPVOID lParam) {
	SDLOG(2, "ChangeDisplaySettingsExA\n");
	if(lpDevMode == NULL) return TrueChangeDisplaySettingsExA(lpszDeviceName, NULL, hwnd, dwflags, lParam);
	DEVMODEA copy = *lpDevMode;
	fixDevMode(&copy);
	return TrueChangeDisplaySettingsExA(lpszDeviceName, &copy, hwnd, dwflags, lParam);
}
GENERATE_INTERCEPT_HEADER(ChangeDisplaySettingsExW, LONG, WINAPI, _In_opt_ LPCWSTR lpszDeviceName, _In_opt_ DEVMODEW* lpDevMode, _Reserved_ HWND hwnd, _In_ DWORD dwflags, _In_opt_ LPVOID lParam) {
	SDLOG(2, "ChangeDisplaySettingsExW\n");
	if(lpDevMode == NULL) return TrueChangeDisplaySettingsExW(lpszDeviceName, NULL, hwnd, dwflags, lParam);
	DEVMODEW copy = *lpDevMode;
	fixDevMode(&copy);
	return TrueChangeDisplaySettingsExW(lpszDeviceName, &copy, hwnd, dwflags, lParam);
}

// Rect Queries /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(GetClientRect, BOOL, WINAPI, _In_ HWND hWnd, _Out_ LPRECT lpRect) {
	SDLOG(2, "DetouredGetClientRect\n");
	HRESULT ret = TrueGetClientRect(hWnd, lpRect);
	if(RSManager::currentlyDownsampling()) {
		SDLOG(2, "- override from %s\n", RectToString(lpRect).c_str());
		lpRect->right = Settings::get().getRenderWidth();
		lpRect->bottom = Settings::get().getRenderHeight();
	}
	SDLOG(2, "-> %s\n", RectToString(lpRect).c_str());
	return ret;
}
GENERATE_INTERCEPT_HEADER(GetWindowRect, BOOL, WINAPI, _In_ HWND hWnd, _Out_ LPRECT lpRect) {
	SDLOG(2, "DetouredGetWindowRect\n");
	HRESULT ret = TrueGetWindowRect(hWnd, lpRect);
	if(RSManager::currentlyDownsampling()) {
		SDLOG(2, "- override from %s\n", RectToString(lpRect).c_str());
		lpRect->right = Settings::get().getRenderWidth();
		lpRect->bottom = Settings::get().getRenderHeight();
	}
	SDLOG(2, "-> %s\n", RectToString(lpRect).c_str());
	return ret;
}

// System Metrics ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(GetSystemMetrics, int, WINAPI, _In_ int nIndex) {
	SDLOG(1, "DetouredGetSystemMetrics %d - %s\n", nIndex, SystemMetricToString(nIndex));
	int ret = TrueGetSystemMetrics(nIndex);
	//if(RSManager::currentlyDownsampling()) {
		switch(nIndex) {
		case SM_CXSCREEN:
			ret = Settings::get().getRenderWidth();
			break;
		case SM_CYSCREEN:
			ret = Settings::get().getRenderHeight();
			break;
		case SM_CXVIRTUALSCREEN:
			ret = Settings::get().getRenderWidth();
			break;
		case SM_CYVIRTUALSCREEN:
			ret = Settings::get().getRenderHeight();
			break;
		} 
	//}
	SDLOG(1, " -> %d\n", ret);
	return ret;
}

// Mouse stuff //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(GetCursorPos, BOOL, WINAPI, __out LPPOINT lpPoint) {
	SDLOG(2, "DetouredGetCursorPos\n");
	BOOL ret = TrueGetCursorPos(lpPoint);
	SDLOG(2, " -> %d/%d\n", lpPoint->x, lpPoint->y);
	if(RSManager::currentlyDownsampling() && Settings::get().getModifyGetCursorPos()) {
		lpPoint->x = lpPoint->x * Settings::get().getRenderWidth() / Settings::get().getPresentWidth();
		lpPoint->y = lpPoint->y * Settings::get().getRenderHeight() / Settings::get().getPresentHeight();
	}
	return ret;
}
GENERATE_INTERCEPT_HEADER(SetCursorPos, BOOL, WINAPI, _In_ int X, _In_ int Y) {
	SDLOG(2, "DetouredSetCursorPos %d/%d\n", X, Y);
	if(false && RSManager::currentlyDownsampling()) {
		X = X * Settings::get().getPresentWidth() / Settings::get().getRenderWidth();
		Y = Y * Settings::get().getPresentHeight() / Settings::get().getRenderHeight();
		return TrueSetCursorPos(X, Y);
	} else {
		return TrueSetCursorPos(X, Y);
	}
}
GENERATE_INTERCEPT_HEADER(SetCursor, HCURSOR, WINAPI, _In_opt_ HCURSOR hCursor) {
	SDLOG(2, "DetouredSetCursor %p\n", hCursor);
	return TrueSetCursor(hCursor);
}

// Messages /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(PeekMessageA, BOOL, WINAPI, _Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax, _In_ UINT wRemoveMsg) {
	SDLOG(2, "DetouredPeekMessageA\n");
	BOOL ret = TruePeekMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
	if(ret && RSManager::currentlyDownsampling() && Settings::get().getAdjustMessagePt()) {
		SDLOG(2, "-> Adjusting position\n");
		lpMsg->pt.x = lpMsg->pt.x * Settings::get().getRenderWidth() / Settings::get().getPresentWidth();
		lpMsg->pt.x = lpMsg->pt.x * Settings::get().getRenderHeight() / Settings::get().getPresentHeight();
	}
	return ret;
}
GENERATE_INTERCEPT_HEADER(GetMessageA, BOOL, WINAPI, _Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax) {
	SDLOG(2, "DetouredGetMessageA\n");
	BOOL ret = TrueGetMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
	if(ret && RSManager::currentlyDownsampling() && Settings::get().getAdjustMessagePt()) {
		SDLOG(2, "-> Adjusting position\n");
		lpMsg->pt.x = lpMsg->pt.x * Settings::get().getRenderWidth() / Settings::get().getPresentWidth();
		lpMsg->pt.x = lpMsg->pt.x * Settings::get().getRenderHeight() / Settings::get().getPresentHeight();
	}
	return ret;
}

GENERATE_INTERCEPT_HEADER(GetMessagePos, DWORD, WINAPI) {
	SDLOG(2, "DetouredGetMessagePos\n");
	DWORD ret = TrueGetMessagePos();
	return ret;
}


// DirectInput 8 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(DirectInput8Create, HRESULT, WINAPI, HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter) {
	SDLOG(0, "DetouredDirectInput8Create!\n");
	return TrueDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}
GENERATE_INTERCEPT_HEADER(DirectInputCreateEx, HRESULT, WINAPI, HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter) {
	SDLOG(0, "DetouredDirectInputCreateEx!\n");
	return TrueDirectInputCreateEx(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}
GENERATE_INTERCEPT_HEADER(DirectInputCreateA, HRESULT, WINAPI, HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA *ppDI, LPUNKNOWN punkOuter) {
	SDLOG(0, "DetouredDirectInputCreateA!\n");
	return TrueDirectInputCreateA(hinst, dwVersion, ppDI, punkOuter);
}
GENERATE_INTERCEPT_HEADER(DirectInputCreateW, HRESULT, WINAPI, HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW *ppDI, LPUNKNOWN punkOuter) {
	SDLOG(0, "DetouredDirectInputCreateW!\n");
	return TrueDirectInputCreateW(hinst, dwVersion, ppDI, punkOuter);
}

// Window stuff /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(AdjustWindowRect, BOOL, WINAPI, __inout LPRECT lpRect, __in DWORD dwStyle, __in BOOL bMenu) {
	return true;
}
GENERATE_INTERCEPT_HEADER(AdjustWindowRectEx, BOOL, WINAPI, _Inout_ LPRECT lpRect, _In_ DWORD dwStyle, _In_ BOOL bMenu, _In_ DWORD dwExStyle) {
	return true;
}
GENERATE_INTERCEPT_HEADER(SetWindowPos, BOOL, WINAPI, _In_ HWND hWnd, _In_opt_ HWND hWndInsertAfter, _In_ int X, _In_ int Y, _In_ int cx, _In_ int cy, _In_ UINT uFlags) {
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Actual detouring /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HMODULE findDll(const string& name) {
	if(name.find("*") != name.npos) {
		SDLOG(16, "Dll pattern search...\n");
		HMODULE modules[1024];
		DWORD needed;
		if(EnumProcessModules(GetCurrentProcess(), modules, 1024*sizeof(HMODULE), &needed)) {
			for(unsigned i=0; i < (needed / sizeof(HMODULE)); i++) {
				TCHAR modname[MAX_PATH];
				if(GetModuleFileNameEx(GetCurrentProcess(), modules[i], modname, sizeof(modname)/sizeof(TCHAR))) {
					SDLOG(16, " - checking against module %s\n", modname);
					if(matchWildcard(modname, name)) {
						SDLOG(1, "Found dll matching pattern %s in %s\n", name.c_str(), modname);
						return modules[i];
					}
				}
			}
		}
	}
	return GetModuleHandle(name.c_str());
}

void hookFunction(const char* name, const char* dllname, void **ppTarget, void* const pDetour, void** ppOriginal, bool& flag) {
	if(!flag) {
		SDLOG(6, "Trying to hook %s in %s\n", name, dllname);
		HMODULE dllhandle = findDll(dllname);
		if(dllhandle) {
			*ppTarget = GetProcAddress(dllhandle, name);
			MH_STATUS ret = MH_CreateHook(*ppTarget, pDetour, ppOriginal); \
			if(ret == MH_OK) { SDLOG(1, " -> Created hook for %s\n", name); } \
			else { SDLOG(0, " -> ERROR creating hook for %s\n", name); } \
			ret = MH_EnableHook(*ppTarget); \
			if(ret == MH_OK) { 
				SDLOG(1, " -> Enabled hook for %s\n", name); 
				flag = true;
			} \
			else { SDLOG(0, " -> ERROR enabling hook for %s\n", name); } \
		} else SDLOG(6, " -> DLL not found!\n")
	}
}

// perform hooking using MinHook
void startDetour() {		
	MH_Initialize();

	#define HOOK(__name, __dllname) \
	hookFunction(#__name, __dllname, (void**)&__name##Pointer, Detoured##__name, (void**)&True##__name, completed##__name##Detour);
	#include "Hooks.def"	
}

void endDetour() {
	MH_DisableHook(MH_ALL_HOOKS);
	MH_STATUS ret;
	
	#define HOOK(__name, __dllname) \
	if(completed##__name##Detour) { \
		ret = MH_RemoveHook(__name##Pointer); \
		if(ret == MH_OK) { SDLOG(1, "Removed hook for " #__name "\n"); } \
		else { SDLOG(0, "ERROR removing hook for" #__name "\n"); } \
	} else { \
		SDLOG(1, "Not removing hook for " #__name", not set\n"); \
	}
	#include "Hooks.def"

	MH_Uninitialize();
}
