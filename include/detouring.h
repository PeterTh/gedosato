#pragma once

#include "main.h"

#include <MinHook.h>

static DWORD (WINAPI * TrueSleepEx)(DWORD dwMilliseconds, BOOL bAlertable) = SleepEx;

typedef HRESULT (WINAPI * D3DXCreateTextureFromFileInMemory_FNType)(_In_ LPDIRECT3DDEVICE9 pDevice, _In_ LPCVOID pSrcData, _In_ UINT SrcDataSize, _Out_ LPDIRECT3DTEXTURE9 *ppTexture);
extern D3DXCreateTextureFromFileInMemory_FNType TrueD3DXCreateTextureFromFileInMemory;

typedef HRESULT (WINAPI * D3DXCreateTextureFromFileInMemoryEx_FNType)(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO *pSrcInfo, PALETTEENTRY *pPalette, LPDIRECT3DTEXTURE9 *ppTexture);
extern D3DXCreateTextureFromFileInMemoryEx_FNType TrueD3DXCreateTextureFromFileInMemoryEx;

string getSystemDllName(const string& name);
void startDetour();
void endDetour();
void setDisplayFaking(bool fake);

#define GENERATE_INTERCEPT_DECLARATION(__name, __rettype, __convention, ...) \
typedef __rettype (__convention * __name##_FNType)(__VA_ARGS__); \
extern __name##_FNType True##__name;

//GENERATE_INTERCEPT_DECLARATION(SetWindowPos, BOOL, WINAPI, _In_ HWND hWnd, _In_opt_ HWND hWndInsertAfter, _In_ int X, _In_ int Y, _In_ int cx, _In_ int cy, _In_ UINT uFlags);

GENERATE_INTERCEPT_DECLARATION(GetClientRect, BOOL, WINAPI, _In_ HWND hWnd, _Out_ LPRECT lpRect);
GENERATE_INTERCEPT_DECLARATION(GetWindowRect, BOOL, WINAPI, _In_ HWND hWnd, _Out_ LPRECT lpRect);

GENERATE_INTERCEPT_DECLARATION(GetWindowLongA, LONG, WINAPI, _In_ HWND hWnd, _In_ int nIndex);
GENERATE_INTERCEPT_DECLARATION(GetWindowLongW, LONG, WINAPI, _In_ HWND hWnd, _In_ int nIndex);
GENERATE_INTERCEPT_DECLARATION(SetWindowLongA, LONG, WINAPI, _In_ HWND hWnd, _In_ int nIndex, _In_ LONG dwNewLong);
GENERATE_INTERCEPT_DECLARATION(SetWindowLongW, LONG, WINAPI, _In_ HWND hWnd, _In_ int nIndex, _In_ LONG dwNewLong);

GENERATE_INTERCEPT_DECLARATION(QueryPerformanceCounter, BOOL, WINAPI, _Out_ LARGE_INTEGER *lpPerformanceCount);
