#pragma once

#include "main.h"

#include <MinHook.h>

static DWORD (WINAPI * TrueSleepEx)(DWORD dwMilliseconds, BOOL bAlertable) = SleepEx;

typedef HRESULT (WINAPI * D3DXCreateTextureFromFileInMemory_FNType)(_In_ LPDIRECT3DDEVICE9 pDevice, _In_ LPCVOID pSrcData, _In_ UINT SrcDataSize, _Out_ LPDIRECT3DTEXTURE9 *ppTexture);
extern D3DXCreateTextureFromFileInMemory_FNType TrueD3DXCreateTextureFromFileInMemory;

typedef HRESULT (WINAPI * D3DXCreateTextureFromFileInMemoryEx_FNType)(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO *pSrcInfo, PALETTEENTRY *pPalette, LPDIRECT3DTEXTURE9 *ppTexture);
extern D3DXCreateTextureFromFileInMemoryEx_FNType TrueD3DXCreateTextureFromFileInMemoryEx;

void startDetour();
void endDetour();

typedef BOOL (WINAPI * SetWindowPos_FNType)(_In_ HWND hWnd, _In_opt_ HWND hWndInsertAfter, _In_ int X, _In_ int Y, _In_ int cx, _In_ int cy, _In_ UINT uFlags);
extern SetWindowPos_FNType TrueSetWindowPos;
typedef BOOL (WINAPI * AdjustWindowRect_FNType)(__inout LPRECT lpRect, __in DWORD dwStyle, __in BOOL bMenu);
extern AdjustWindowRect_FNType TrueAdjustWindowRect;
typedef BOOL (WINAPI * AdjustWindowRectEx_FNType)(_Inout_ LPRECT lpRect, _In_ DWORD dwStyle, _In_ BOOL bMenu, _In_ DWORD dwExStyle);
extern AdjustWindowRectEx_FNType TrueAdjustWindowRectEx;
