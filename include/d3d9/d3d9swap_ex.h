// wrapper for IDirect3DSwapChain9Ex in d3d9.h
// generated using wrapper_gen.rb

#include "d3d9.h"

interface hkIDirect3DSwapChain9Ex : public IDirect3DSwapChain9Ex {
	IDirect3DSwapChain9Ex *m_pWrapped;
	
public:
	hkIDirect3DSwapChain9Ex(IDirect3DSwapChain9Ex **ppIDirect3DSwapChain9Ex);
	
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
	STDMETHOD(GetLastPresentCount)(UINT* pLastPresentCount);
	STDMETHOD(GetPresentStats)(D3DPRESENTSTATS* pPresentationStatistics);
	STDMETHOD(GetDisplayModeEx)(D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation);
};

