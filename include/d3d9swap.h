// wrapper for IDirect3DSwapChain9 in d3d9.h
// generated using wrapper_gen.rb

#pragma once

#include "d3d9.h"

struct hkIDirect3DDevice9;
struct hkIDirect3DDevice9Ex;

interface hkIDirect3DSwapChain9 : public IDirect3DSwapChain9 {
	IDirect3DSwapChain9 *m_pWrapped;
	hkIDirect3DDevice9 *hookedDev;
	
public:
	hkIDirect3DSwapChain9(IDirect3DSwapChain9 **ppIDirect3DSwapChain9, hkIDirect3DDevice9* hdev);
	hkIDirect3DSwapChain9(IDirect3DSwapChain9 **ppIDirect3DSwapChain9, hkIDirect3DDevice9Ex* hdev);
	
	// original interface
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObj);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(Present)(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
	STDMETHOD(GetFrontBufferData)(IDirect3DSurface9* pDestSurface);
	STDMETHOD(GetBackBuffer)(UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer);
	STDMETHOD(GetRasterStatus)(D3DRASTER_STATUS* pRasterStatus);
	STDMETHOD(GetDisplayMode)(D3DDISPLAYMODE* pMode);
	STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice);
	STDMETHOD(GetPresentParameters)(D3DPRESENT_PARAMETERS* pPresentationParameters);
};
