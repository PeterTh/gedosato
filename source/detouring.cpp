#include "Detouring.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <MinHook.h>

#include "utils/string_utils.h"
#include "utils/d3d9_utils.h"
#include "utils/win_utils.h"

#include "settings.h"
#include "renderstate_manager_dx9.h"
#include "time_manager.h"

#include "d3d9/d3d9.h"
#include "d3d9/d3d9int_ex.h"

#include "d3d11/d3d11dev.h"
#include "d3d11/d3d11devcontext.h"
#include "dxgi/dxgiswapchain.h"
#include "dxgi/dxgifactory.h"
#include "dxgi/dxgifactory1.h"
//#include "dxgi/dxgifactory2.h"

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

	HMODULE GetCurrentModule() {
		HMODULE hModule = NULL;
		GetModuleHandleEx(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			(LPCTSTR)GetCurrentModule,
			&hModule);
		return hModule;
	}

	template<typename strtype>
	bool preventDllLoading(strtype& fn, bool wide = false) {
		boost::algorithm::to_lower(fn);
		if(Settings::get().getPreventSteamOverlay() 
				&& (wide ? boost::algorithm::ends_with(fn, L"gameoverlayrenderer.dll") : boost::algorithm::ends_with(fn, "gameoverlayrenderer.dll")) ) {
			SDLOG(2, "-> Steam overlay detected, denying access to file\n");
			SetLastError(ERROR_ACCESS_DENIED);
			return true;
		}
		if(boost::algorithm::iequals(getExeFileName(),"FarCry") && boost::algorithm::ends_with(fn, L"dfhengine.dll")) {
			SDLOG(2, "-> DFHEngine (FarCry ad system) detected, denying access to file\n");
			SetLastError(ERROR_ACCESS_DENIED);
			return true;
		}
		return false;
	}
}

GENERATE_INTERCEPT_HEADER(LoadLibraryA, HMODULE, WINAPI, _In_ LPCSTR lpLibFileName) {
	SDLOG(22, "DetouredLoadLibraryA %s\n", lpLibFileName);

	if(preventDllLoading(std::string(lpLibFileName))) return NULL;

	HMODULE mod = TrueLoadLibraryA(lpLibFileName);
	// restart detour in case we missed anything
	if(mod) restartDetour(lpLibFileName);
	return mod;
}
GENERATE_INTERCEPT_HEADER(LoadLibraryW, HMODULE, WINAPI, _In_ LPCWSTR lpLibFileName) {
	SDLOG(22, "DetouredLoadLibraryW %s\n", lpLibFileName);

	if(preventDllLoading(std::wstring(lpLibFileName), true)) return NULL;

	HMODULE mod = TrueLoadLibraryW(lpLibFileName);
	// restart detour in case we missed anything
	if(mod) restartDetour(CW2A(lpLibFileName));
	return mod;
}

GENERATE_INTERCEPT_HEADER(LoadLibraryExA, HMODULE, WINAPI, _In_ LPCSTR lpLibFileName, _Reserved_ HANDLE hFile, _In_ DWORD dwFlags) {
	SDLOG(22, "DetouredLoadLibraryExA %s\n", lpLibFileName);

	string fn(lpLibFileName);
	if(fn.find("GeDoSaTo") != fn.npos) return GetCurrentModule(); // find out why we need this

	if(preventDllLoading(std::string(lpLibFileName))) return NULL;

	HMODULE mod = TrueLoadLibraryExA(lpLibFileName, hFile, dwFlags);
	// restart detour in case we missed anything
	if(mod) restartDetour(lpLibFileName);
	return mod;
}
GENERATE_INTERCEPT_HEADER(LoadLibraryExW, HMODULE, WINAPI, _In_ LPCWSTR lpLibFileName, _Reserved_ HANDLE hFile, _In_ DWORD dwFlags) {
	SDLOG(22, "DetouredLoadLibraryExW %s\n", CW2A(lpLibFileName));

	string fn((CW2A(lpLibFileName)));
	if(fn.find("GeDoSaTo") != fn.npos) return GetCurrentModule(); // find out why we need this

	if(preventDllLoading(std::wstring(lpLibFileName), true)) return NULL;

	HMODULE mod = TrueLoadLibraryExW(lpLibFileName, hFile, dwFlags);
	// restart detour in case we missed anything
	if(mod) restartDetour(CW2A(lpLibFileName));
	return mod;
}

// D3D9 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(Direct3DCreate9, IDirect3D9*, APIENTRY, UINT SDKVersion) {
	SDLOG(0, "=== DetouredDirect3DCreate9 V:%d\n", SDKVersion);
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

namespace {
	// get size if the game sucks
	D3DSURFACE_DESC getTexInfo(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData) {
		IDirect3DTexture9* tex;
		D3DSURFACE_DESC ret;
		if(SUCCEEDED(TrueD3DXCreateTextureFromFileInMemory(pDevice, pSrcData, 4294967295, &tex))) {
			tex->GetLevelDesc(0, &ret);
			tex->Release();
		}
		return ret;
	}
}

GENERATE_INTERCEPT_HEADER(D3DXCreateTexture, HRESULT, WINAPI, _In_ LPDIRECT3DDEVICE9 pDevice, _In_ UINT Width, _In_ UINT Height, 
						  _In_ UINT MipLevels, _In_ DWORD Usage, _In_ D3DFORMAT Format, _In_ D3DPOOL Pool, _Out_ LPDIRECT3DTEXTURE9 *ppTexture) {
	SDLOG(4, "DetouredD3DXCreateTexture\n");
	HRESULT res = TrueD3DXCreateTexture(pDevice, Width, Height, MipLevels, Usage, Format, Pool, ppTexture);
	return res;
}
GENERATE_INTERCEPT_HEADER(D3DXCreateTextureFromFileInMemory, HRESULT, WINAPI, _In_ LPDIRECT3DDEVICE9 pDevice, _In_ LPCVOID pSrcData, _In_ UINT SrcDataSize, _Out_ LPDIRECT3DTEXTURE9 *ppTexture) {
	SDLOG(4, "DetouredD3DXCreateTextureFromFileInMemory\n");
	HRESULT res = TrueD3DXCreateTextureFromFileInMemory(pDevice, pSrcData, SrcDataSize, ppTexture);
	RSManager::getDX9().registerD3DXCreateTextureFromFileInMemory(pSrcData, (SrcDataSize == 2147483647u || SrcDataSize == 4294967295u) ? 256 : SrcDataSize, *ppTexture);
	return res;
}
GENERATE_INTERCEPT_HEADER(D3DXCreateTextureFromFileInMemoryEx, HRESULT, WINAPI, LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, 
						  UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO *pSrcInfo, PALETTEENTRY *pPalette, LPDIRECT3DTEXTURE9 *ppTexture) {
	SDLOG(4, "DetouredD3DXCreateTextureFromFileInMemoryEx: \n -- pDevice %p, pSrcData %p, SrcDataSize %u, Width %u, Height %u, MipLevels %u, Usage %d, Format %s\n",
		pDevice, pSrcData, SrcDataSize, Width, Height, MipLevels, Usage, D3DFormatToString(Format));
	if(SrcDataSize == 4294967295u && Width == 4294967295u && Height == 4294967295u) {
		D3DSURFACE_DESC d = getTexInfo(pDevice, pSrcData);
		Width = d.Width;
		Height = d.Height;
	}
	HRESULT res = RSManager::getDX9().redirectD3DXCreateTextureFromFileInMemoryEx(pDevice, pSrcData, SrcDataSize, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
	if(SrcDataSize == 2147483647u || SrcDataSize == 4294967295u) SrcDataSize = Width*Height / 2;
	RSManager::getDX9().registerD3DXCreateTextureFromFileInMemory(pSrcData, SrcDataSize, *ppTexture); 
	return res;
}

GENERATE_INTERCEPT_HEADER(D3DXCompileShader, HRESULT, WINAPI, _In_ LPCSTR pSrcData, _In_ UINT srcDataLen, _In_ const D3DXMACRO *pDefines, _In_ LPD3DXINCLUDE pInclude, _In_ LPCSTR pFunctionName, 
						  _In_ LPCSTR pProfile, _In_ DWORD Flags, _Out_ LPD3DXBUFFER *ppShader, _Out_ LPD3DXBUFFER *ppErrorMsgs, _Out_ LPD3DXCONSTANTTABLE *ppConstantTable) {
	HRESULT res = TrueD3DXCompileShader(pSrcData, srcDataLen, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable);
	RSManager::getDX9().registerD3DXCompileShader(pSrcData, srcDataLen, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs, ppConstantTable);
	return res;
}

// Display Enum /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool g_FakeChangedDisplaySettings = false;

void setDisplayFaking(bool fake) { g_FakeChangedDisplaySettings = fake; }

namespace {
	template<typename DevModeType, typename StringType, typename TrueFnType>
	BOOL DetouredEnumDisplaySettingsTemplate(StringType lpszDeviceName, DWORD iModeNum, DevModeType *lpDevMode, DWORD dwFlags, TrueFnType trueFunction) {
		BOOL ret = trueFunction(lpszDeviceName, iModeNum, lpDevMode, dwFlags);
		if(ret) {
			SDLOG(2, "-> %ux%u %ubpp @ %u Hz\n", lpDevMode->dmPelsWidth, lpDevMode->dmPelsHeight, lpDevMode->dmBitsPerPel, lpDevMode->dmDisplayFrequency);
		}
		else {
			SDLOG(2, "-> NULL\n");
		}
		// find initial count of modes when Windows caches the list (at id 0 call)
		static unsigned emptyMode = 0;
		if(iModeNum == 0) {
			emptyMode = 1;
			DevModeType mode;
			mode.dmSize = sizeof(DevModeType);
			mode.dmDriverExtra = 0;
			while(trueFunction(lpszDeviceName, emptyMode, &mode, dwFlags) != NULL) ++emptyMode;
			SDLOG(2, "*** Found Emptymode: %u\n", emptyMode);
		}
		// enter our modes, either if its number is requested
		if((emptyMode != 0 && iModeNum >= emptyMode && iModeNum < emptyMode + Settings::getResSettings().getNumResolutions() && ret == NULL)
			// or if we are downsampling and need to fake it as current
			|| (iModeNum == ENUM_CURRENT_SETTINGS && RSManager::currentlyDownsampling())
			|| (iModeNum == ENUM_CURRENT_SETTINGS && g_FakeChangedDisplaySettings)) {
			const auto& res = Settings::getResSettings().getResolution(iModeNum - emptyMode);
			lpDevMode->dmBitsPerPel = 32;
			lpDevMode->dmPelsWidth = res.width;
			lpDevMode->dmPelsHeight = res.height;
			lpDevMode->dmDisplayFlags = 0;
			lpDevMode->dmDisplayFrequency = res.hz;
			SDLOG(2, "-> Fake %ux%u\n", res.width, res.height);
			return TRUE;
		}
		if(ret && lpDevMode->dmPelsWidth == Settings::get().getOverrideWidth() && lpDevMode->dmPelsHeight == Settings::get().getOverrideHeight()) {
			lpDevMode->dmPelsWidth = Settings::get().getRenderWidth();
			lpDevMode->dmPelsHeight = Settings::get().getRenderHeight();
			SDLOG(2, "-> Override\n");
		}
		return ret;
	}
}

GENERATE_INTERCEPT_HEADER(EnumDisplaySettingsExA, BOOL, WINAPI, _In_ LPCTSTR lpszDeviceName, _In_ DWORD iModeNum, _Out_ DEVMODE *lpDevMode, _In_ DWORD dwFlags) {
	SDLOG(2, "EnumDisplaySettingsExA %s -- %u\n", lpszDeviceName ? lpszDeviceName : "NULL", iModeNum);
	return DetouredEnumDisplaySettingsTemplate(lpszDeviceName, iModeNum, lpDevMode, dwFlags, TrueEnumDisplaySettingsExA);
}
GENERATE_INTERCEPT_HEADER(EnumDisplaySettingsExW, BOOL, WINAPI, _In_ LPCWSTR lpszDeviceName, _In_ DWORD iModeNum, _Out_ DEVMODEW *lpDevMode, _In_ DWORD dwFlags) {
	SDLOG(2, "EnumDisplaySettingsExW %s -- %u\n", lpszDeviceName ? CW2A(lpszDeviceName) : "NULL", iModeNum);
	return DetouredEnumDisplaySettingsTemplate(lpszDeviceName, iModeNum, lpDevMode, dwFlags, TrueEnumDisplaySettingsExW);
}

// Display Changing /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
	template<typename DEVM>
	void fixDevMode(DEVM* lpDevMode) {
		if(lpDevMode->dmPelsWidth == Settings::get().getRenderWidth() && lpDevMode->dmPelsHeight == Settings::get().getRenderHeight()) {
			SDLOG(2, " -> Overriding\n");
			lpDevMode->dmPelsWidth = Settings::get().getPresentWidth();
			lpDevMode->dmPelsHeight = Settings::get().getPresentHeight();
			lpDevMode->dmDisplayFrequency = Settings::get().getPresentHz();
			g_FakeChangedDisplaySettings = true;
		}
	}
}

GENERATE_INTERCEPT_HEADER(ChangeDisplaySettingsExA, LONG, WINAPI, _In_opt_ LPCSTR lpszDeviceName, _In_opt_ DEVMODEA* lpDevMode, _Reserved_ HWND hwnd, _In_ DWORD dwflags, _In_opt_ LPVOID lParam) {
	SDLOG(2, "ChangeDisplaySettingsExA\n");
	g_FakeChangedDisplaySettings = false;
	if(lpDevMode == NULL) return TrueChangeDisplaySettingsExA(lpszDeviceName, NULL, hwnd, dwflags, lParam);
	DEVMODEA copy = *lpDevMode;
	fixDevMode(&copy);
	return TrueChangeDisplaySettingsExA(lpszDeviceName, &copy, hwnd, dwflags, lParam);
}
GENERATE_INTERCEPT_HEADER(ChangeDisplaySettingsExW, LONG, WINAPI, _In_opt_ LPCWSTR lpszDeviceName, _In_opt_ DEVMODEW* lpDevMode, _Reserved_ HWND hwnd, _In_ DWORD dwflags, _In_opt_ LPVOID lParam) {
	SDLOG(2, "ChangeDisplaySettingsExW\n");
	g_FakeChangedDisplaySettings = false;
	if(lpDevMode == NULL) return TrueChangeDisplaySettingsExW(lpszDeviceName, NULL, hwnd, dwflags, lParam);
	DEVMODEW copy = *lpDevMode;
	fixDevMode(&copy);
	return TrueChangeDisplaySettingsExW(lpszDeviceName, &copy, hwnd, dwflags, lParam);
}

// Rect Queries /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(GetClientRect, BOOL, WINAPI, _In_ HWND hWnd, _Out_ LPRECT lpRect) {
	SDLOG(2, "DetouredGetClientRect\n");
	HRESULT ret = TrueGetClientRect(hWnd, lpRect);
	if(RSManager::currentlyDownsampling() && Settings::get().getModifyGetClientRect()) {
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
	if(RSManager::currentlyDownsampling() && Settings::get().getModifyGetWindowRect()) {
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
	if(RSManager::currentlyDownsampling()) {
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
	}
	SDLOG(1, " -> %d\n", ret);
	return ret;
}

GENERATE_INTERCEPT_HEADER(GetDeviceCaps, int, WINAPI, _In_opt_ HDC hdc, _In_ int index) {
	SDLOG(1, "DetouredGetDeviceCaps %p %s\n", hdc, GetDeviceCapsParamToString(index));
	int ret = TrueGetDeviceCaps(hdc, index);
	return ret;
}

namespace {
	void adjustMonitorInfo(LPMONITORINFO lpmi) {
		if(RSManager::currentlyDownsampling()) {
			lpmi->rcMonitor.right = lpmi->rcMonitor.left + Settings::get().getRenderWidth();
			lpmi->rcMonitor.bottom = lpmi->rcMonitor.top + Settings::get().getRenderHeight();
		}
	}
}

GENERATE_INTERCEPT_HEADER(GetMonitorInfoA, BOOL, WINAPI, _In_ HMONITOR hMonitor, _Inout_ LPMONITORINFO lpmi) {
	SDLOG(1, "DetouredGetMonitorInfoA %p\n", hMonitor);
	BOOL ret = TrueGetMonitorInfoA(hMonitor, lpmi);
	adjustMonitorInfo(lpmi);
	return ret;	
}
GENERATE_INTERCEPT_HEADER(GetMonitorInfoW, BOOL, WINAPI, _In_ HMONITOR hMonitor, _Inout_ LPMONITORINFO lpmi) {
	SDLOG(1, "DetouredGetMonitorInfoW %p\n", hMonitor);
	BOOL ret = TrueGetMonitorInfoW(hMonitor, lpmi);
	adjustMonitorInfo(lpmi);
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
	if(RSManager::currentlyDownsampling() && Settings::get().getModifySetCursorPos()) {
		if(X == Settings::get().getRenderHeight()/2 && Y == Settings::get().getRenderWidth()/2) {
			X = Settings::get().getPresentWidth()/2;
			Y = Settings::get().getPresentHeight()/2;
		} else {
			X = X * Settings::get().getPresentWidth() / Settings::get().getRenderWidth();
			Y = Y * Settings::get().getPresentHeight() / Settings::get().getRenderHeight();
		}
		return TrueSetCursorPos(X, Y);
	}
	return TrueSetCursorPos(X, Y);
}
GENERATE_INTERCEPT_HEADER(SetCursor, HCURSOR, WINAPI, _In_opt_ HCURSOR hCursor) {
	SDLOG(2, "DetouredSetCursor %p\n", hCursor);
	return TrueSetCursor(hCursor);
}

GENERATE_INTERCEPT_HEADER(ShowCursor, int, WINAPI, _In_ BOOL bShow) {
	SDLOG(2, "DetouredShowCursor %s\n", bShow ? "true" : "false");
	return TrueShowCursor(Settings::get().getHideMouseCursor() ? false : bShow);
}

GENERATE_INTERCEPT_HEADER(ClipCursor, BOOL, WINAPI, __in_opt CONST RECT *lpRect) {
	SDLOG(2, "ClipCursor %s\n", RectToString(lpRect));
	return TrueClipCursor(lpRect);
}

GENERATE_INTERCEPT_HEADER(WindowFromPoint, HWND, WINAPI, _In_ POINT Point) {
	return GetActiveWindow();
	//return TrueWindowFromPoint(Point);
}

// Messages /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(PeekMessageA, BOOL, WINAPI, _Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax, _In_ UINT wRemoveMsg) {
	SDLOG(12, "DetouredPeekMessageA\n");
	BOOL ret = TruePeekMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
	if(ret && RSManager::currentlyDownsampling() && Settings::get().getAdjustMessagePt()) {
		SDLOG(2, "-> Adjusting position\n");
		lpMsg->pt.x = lpMsg->pt.x * Settings::get().getRenderWidth() / Settings::get().getPresentWidth();
		lpMsg->pt.x = lpMsg->pt.x * Settings::get().getRenderHeight() / Settings::get().getPresentHeight();
	}
	return ret;
}
GENERATE_INTERCEPT_HEADER(GetMessageA, BOOL, WINAPI, _Out_ LPMSG lpMsg, _In_opt_ HWND hWnd, _In_ UINT wMsgFilterMin, _In_ UINT wMsgFilterMax) {
	SDLOG(12, "DetouredGetMessageA\n");
	BOOL ret = TrueGetMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
	if(ret && RSManager::currentlyDownsampling() && Settings::get().getAdjustMessagePt()) {
		SDLOG(2, "-> Adjusting position\n");
		lpMsg->pt.x = lpMsg->pt.x * Settings::get().getRenderWidth() / Settings::get().getPresentWidth();
		lpMsg->pt.x = lpMsg->pt.x * Settings::get().getRenderHeight() / Settings::get().getPresentHeight();
	}
	return ret;
}

GENERATE_INTERCEPT_HEADER(GetMessagePos, DWORD, WINAPI) {
	SDLOG(12, "DetouredGetMessagePos\n");
	DWORD ret = TrueGetMessagePos();
	return ret;
}

// WindowProc ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region WindowProc

namespace {
	std::map<HWND, WNDPROC> prevWndProcs;
	LRESULT CALLBACK InterceptWindowProc(_In_  HWND hwnd, _In_  UINT uMsg, _In_  WPARAM wParam, _In_  LPARAM lParam) {
		SDLOG(2, "InterceptWindowProc hwnd: %p\n", hwnd);
		if(RSManager::currentlyDownsampling()) {
			if(uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) {
				POINTS p = MAKEPOINTS(lParam);
				p.x = p.x * Settings::get().getRenderWidth() / Settings::get().getPresentWidth();
				p.y = p.y * Settings::get().getRenderWidth() / Settings::get().getPresentWidth();
				return CallWindowProc(prevWndProcs[hwnd], hwnd, uMsg, wParam, MAKELPARAM(p.x, p.y));
			}
			//if(uMsg >= WM_DISPLAYCHANGE) {
			//	return CallWindowProc(prevWndProcs[hwnd], hwnd, uMsg, wParam, MAKELPARAM(Settings::get().getRenderWidth(), Settings::get().getRenderHeight()));
			//}
		}
		SDLOG(2, " -> calling original: %d\n", prevWndProcs[hwnd]);
		LRESULT res = CallWindowProc(prevWndProcs[hwnd], hwnd, uMsg, wParam, lParam);
		return res;
	}
}

GENERATE_INTERCEPT_HEADER(GetWindowLongA, LONG, WINAPI, _In_ HWND hWnd, _In_ int nIndex) {
	SDLOG(2, "DetouredGetWindowLongA hwnd: %p -- index: %s\n", hWnd, WindowLongOffsetToString(nIndex));
	if(nIndex == GWL_WNDPROC && Settings::get().getInterceptWindowProc()) {
		SDLOG(2, " -> return from table\n");
		if(prevWndProcs.count(hWnd)>0) return (LONG)prevWndProcs[hWnd];
	}
	return TrueGetWindowLongA(hWnd, nIndex);
}
GENERATE_INTERCEPT_HEADER(GetWindowLongW, LONG, WINAPI, _In_ HWND hWnd, _In_ int nIndex) {
	SDLOG(2, "DetouredGetWindowLongW hwnd: %p -- index: %s\n", hWnd, WindowLongOffsetToString(nIndex));
	if(nIndex == GWL_WNDPROC && Settings::get().getInterceptWindowProc()) {
		SDLOG(2, " -> return from table\n");
		if(prevWndProcs.count(hWnd)>0) return (LONG)prevWndProcs[hWnd];
	}
	return TrueGetWindowLongW(hWnd, nIndex);
}

GENERATE_INTERCEPT_HEADER(SetWindowLongA, LONG, WINAPI, _In_ HWND hWnd, _In_ int nIndex, _In_ LONG dwNewLong) {
	SDLOG(2, "DetouredSetWindowLongA hwnd: %p -- index: %s -- value: %d\n", hWnd, WindowLongOffsetToString(nIndex), dwNewLong);
	LONG ret = TrueSetWindowLongA(hWnd, nIndex, dwNewLong);
	if(nIndex == GWL_WNDPROC && Settings::get().getInterceptWindowProc()) {
		prevWndProcs[hWnd] = (WNDPROC)dwNewLong;
		TrueSetWindowLongA(hWnd, GWL_WNDPROC, (LONG)&InterceptWindowProc);
	}
	return ret;
}
GENERATE_INTERCEPT_HEADER(SetWindowLongW, LONG, WINAPI, _In_ HWND hWnd, _In_ int nIndex, _In_ LONG dwNewLong) {
	SDLOG(2, "DetouredSetWindowLongW hwnd: %p -- index: %s -- value: %d\n", hWnd, WindowLongOffsetToString(nIndex), dwNewLong);
	LONG ret = TrueSetWindowLongW(hWnd, nIndex, dwNewLong);
	if(nIndex == GWL_WNDPROC && Settings::get().getInterceptWindowProc()) {
		prevWndProcs[hWnd] = (WNDPROC)dwNewLong;
		TrueSetWindowLongW(hWnd, GWL_WNDPROC, (LONG)&InterceptWindowProc);
	}
	return ret;
}

#pragma endregion

// DirectInput 8 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region DirectInput 8

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

#pragma endregion

// Window stuff /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(AdjustWindowRect, BOOL, WINAPI, __inout LPRECT lpRect, __in DWORD dwStyle, __in BOOL bMenu) {
	return TrueAdjustWindowRect(lpRect, dwStyle, bMenu);
}
GENERATE_INTERCEPT_HEADER(AdjustWindowRectEx, BOOL, WINAPI, _Inout_ LPRECT lpRect, _In_ DWORD dwStyle, _In_ BOOL bMenu, _In_ DWORD dwExStyle) {
	return TrueAdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
}
GENERATE_INTERCEPT_HEADER(SetWindowPos, BOOL, WINAPI, _In_ HWND hWnd, _In_opt_ HWND hWndInsertAfter, _In_ int X, _In_ int Y, _In_ int cx, _In_ int cy, _In_ UINT uFlags) {
	if(cx > static_cast<int>(Settings::get().getPresentWidth())) cx = Settings::get().getPresentWidth();
	if(cy > static_cast<int>(Settings::get().getPresentHeight())) cy = Settings::get().getPresentHeight();
	return TrueSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

GENERATE_INTERCEPT_HEADER(CreateWindowExA, HWND, WINAPI,
		_In_ DWORD dwExStyle, _In_opt_ LPCTSTR lpClassName, _In_opt_ LPCTSTR lpWindowName, _In_ DWORD dwStyle,
		_In_ int x, _In_ int y, _In_ int nWidth, _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam) {
	return TrueCreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}
GENERATE_INTERCEPT_HEADER(CreateWindowExW, HWND, WINAPI,
	_In_ DWORD dwExStyle, _In_opt_ LPCWSTR lpClassName, _In_opt_ LPCWSTR lpWindowName, _In_ DWORD dwStyle,
	_In_ int x, _In_ int y, _In_ int nWidth, _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam) {
	return TrueCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

// DXGI /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(CreateDXGIFactory, HRESULT, WINAPI, _In_ REFIID riid, _Out_ void **ppFactory) {
	SDLOG(0, "DetouredCreateDXGIFactory\n");
	HRESULT ret = TrueCreateDXGIFactory(riid, ppFactory);
	if(SUCCEEDED(ret)) {
		if(ppFactory != NULL) new hkIDXGIFactory(reinterpret_cast<IDXGIFactory**>(ppFactory));
	}
	return ret;
}
GENERATE_INTERCEPT_HEADER(CreateDXGIFactory1, HRESULT, WINAPI, _In_ REFIID riid, _Out_ void **ppFactory) {
	SDLOG(0, "DetouredCreateDXGIFactory1\n");
	HRESULT ret = TrueCreateDXGIFactory1(riid, ppFactory);
	if(SUCCEEDED(ret)) {
		if(ppFactory != NULL) new hkIDXGIFactory1(reinterpret_cast<IDXGIFactory1**>(ppFactory));
	}
	return ret;
}
GENERATE_INTERCEPT_HEADER(CreateDXGIFactory2, HRESULT, WINAPI, _In_ UINT flags, _In_ const IID &riid, _Out_ void **ppFactory) {
	SDLOG(0, "DetouredCreateDXGIFactory2\n");
	HRESULT ret = TrueCreateDXGIFactory2(flags, riid, ppFactory);
	//if(SUCCEEDED(ret)) {
	//	if(ppFactory != NULL) new hkIDXGIFactory2(reinterpret_cast<IDXGIFactory2**>(ppFactory));
	//}
	return ret;
}

// D3D11 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "d3d11/d3d11.h"

namespace {
	// used to check for delegation within the D3D11 dll
	bool g_NeedD3D11Hooking = false;
}

GENERATE_INTERCEPT_HEADER(D3D11CreateDevice, HRESULT, WINAPI,
		_In_opt_ IDXGIAdapter* pAdapter,
		D3D_DRIVER_TYPE DriverType,
		HMODULE Software,
		UINT Flags,
		_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
		UINT FeatureLevels,
		UINT SDKVersion,
		_Out_opt_ ID3D11Device** ppDevice,
		_Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
		_Out_opt_ ID3D11DeviceContext** ppImmediateContext) {
	SDLOG(0, "DetouredD3D11CreateDevice ppDevice: %p (%p) ppImmediateContext: %p (%p)\n",
		ppDevice, ppDevice ? *ppDevice : NULL, ppImmediateContext, ppImmediateContext ? *ppImmediateContext : NULL);
	g_NeedD3D11Hooking = true;
	HRESULT ret = TrueD3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);
	SDLOG(0, "-> ppDevice: %p (%p) ppImmediateContext: %p (%p)\n",
		ppDevice, ppDevice ? *ppDevice : NULL, ppImmediateContext, ppImmediateContext ? *ppImmediateContext : NULL);
	// Check for success and non-null ppDevice and context (can be set to null to check feature level)
	// also check if call was delegated to D3D11CreateDeviceAndSwapChain internally
	if(SUCCEEDED(ret) && g_NeedD3D11Hooking) {
		if(ppDevice != NULL) new hkID3D11Device(ppDevice);
		if(ppImmediateContext != NULL) new hkID3D11DeviceContext(ppImmediateContext);
		g_NeedD3D11Hooking = false;
	}
	return ret;
}

GENERATE_INTERCEPT_HEADER(D3D11CreateDeviceAndSwapChain, HRESULT, WINAPI,
		_In_opt_ IDXGIAdapter* pAdapter,
		D3D_DRIVER_TYPE DriverType,
		HMODULE Software,
		UINT Flags,
		_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
		UINT FeatureLevels,
		UINT SDKVersion,
		_In_opt_ CONST DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
		_Out_opt_ IDXGISwapChain** ppSwapChain,
		_Out_opt_ ID3D11Device** ppDevice,
		_Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
		_Out_opt_ ID3D11DeviceContext** ppImmediateContext) {
	SDLOG(0, "DetouredD3D11CreateDeviceAndSwapChain ppDevice: %p (%p) ppImmediateContext: %p (%p) ppSwapChain: %p (%p)\n",
		ppDevice, ppDevice ? *ppDevice : NULL, ppImmediateContext, ppImmediateContext ? *ppImmediateContext : NULL, ppSwapChain, ppSwapChain ? *ppSwapChain : NULL);
	HRESULT ret = TrueD3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
	SDLOG(0, "-> ppDevice: %p(%p) ppImmediateContext : %p(%p) ppSwapChain : %p(%p)\n",
		ppDevice, ppDevice ? *ppDevice : NULL, ppImmediateContext, ppImmediateContext ? *ppImmediateContext : NULL, ppSwapChain, ppSwapChain ? *ppSwapChain : NULL);
	// Check for success and non-null ppDevice and context (can be set to null to check feature level)
	if(SUCCEEDED(ret)) {
		if(ppDevice != NULL) new hkID3D11Device(ppDevice);
		if(ppImmediateContext != NULL) new hkID3D11DeviceContext(ppImmediateContext);
		if(ppSwapChain != NULL) new hkIDXGISwapChain(ppSwapChain, &RSManager::getDX11());
		g_NeedD3D11Hooking = false;
	}
	return ret;
}

// Time ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GENERATE_INTERCEPT_HEADER(QueryPerformanceCounter, BOOL, WINAPI, _Out_ LARGE_INTEGER *lpPerformanceCount) {
	SDLOG(55, "DetouredQueryPerformanceCounter\n");
	BOOL ret = TimeManager::get().redirectQueryPerformanceCounter(lpPerformanceCount);
	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Actual detouring /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string getSystemDllName(const string& name) {
	char sysdir[MAX_PATH];
	GetSystemDirectory(sysdir, MAX_PATH);
	string fullPath = string(sysdir) + "\\" + name;
	SDLOG(28, "Full dll path: %s\n", fullPath.c_str());
	return fullPath;
}

HMODULE findDll(const string& name) {
	if(name.find("*") != name.npos) {
		SDLOG(46, "Dll pattern search...\n");
		HMODULE modules[1024];
		DWORD needed;
		if(EnumProcessModules(GetCurrentProcess(), modules, 1024*sizeof(HMODULE), &needed)) {
			for(unsigned i=0; i < (needed / sizeof(HMODULE)); i++) {
				TCHAR modname[MAX_PATH];
				if(GetModuleFileNameEx(GetCurrentProcess(), modules[i], modname, sizeof(modname)/sizeof(TCHAR))) {
					SDLOG(46, " - checking against module %s\n", modname);
					if(matchWildcard(modname, name)) {
						SDLOG(1, "Found dll matching pattern %s in %s\n", name.c_str(), modname);
						return modules[i];
					}
				}
			}
		}
	}
	// first try full path, may increase compatibility with other injectors
	string fullPath = getSystemDllName(name);
	HMODULE ret = GetModuleHandle(fullPath.c_str());
	if(ret == NULL && !Settings::get().getInterceptOnlySystemDlls()) ret = GetModuleHandle(name.c_str());
	return ret;
}

void hookFunction(const char* name, const char* dllname, void **ppTarget, void* const pDetour, void** ppOriginal, bool& flag) {
	if(!Settings::get().getEnableDX11() && (boost::algorithm::icontains(name, "DX11") || boost::algorithm::icontains(name, "DXGI"))) return;
	if(!flag) {
		SDLOG(36, "Trying to hook %s in %s\n", name, dllname);
		HMODULE dllhandle = findDll(dllname);
		if(dllhandle) {
			*ppTarget = GetProcAddress(dllhandle, name);
			MH_STATUS ret = MH_CreateHook(*ppTarget, pDetour, ppOriginal); 
			if(ret == MH_OK) { SDLOG(1, " -> Created hook for %s, target: %p, dll: %p\n", name, ppTarget, dllhandle); } 
			else { SDLOG(0, " -> ERROR %d creating hook for %s, target: %p, dll: %p\n", ret, name, ppTarget, dllhandle); } 
			ret = MH_EnableHook(*ppTarget); 
			if(ret == MH_OK) { 
				SDLOG(1, " -> Enabled hook for %s\n", name); 
				flag = true;
			} 
			else { SDLOG(0, " -> ERROR %d enabling hook for %s\n", ret, name); } 
		} else SDLOG(36, " -> DLL not found!\n")
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
		if(ret == MH_OK) { \
			SDLOG(1, "Removed hook for " #__name "\n"); \
			completed##__name##Detour = false; \
		} \
		else { SDLOG(0, "ERROR removing hook for" #__name "\n"); } \
	} else { \
		SDLOG(1, "Not removing hook for " #__name", not set\n"); \
	}
	#include "Hooks.def"

	MH_Uninitialize();
}
