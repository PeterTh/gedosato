// wrapper for IDirect3DSwapChain9Ex in d3d9.h
// generated using wrapper_gen.rb

#include "d3d9/d3d9swap_ex.h"

hkIDirect3DSwapChain9Ex::hkIDirect3DSwapChain9Ex(IDirect3DSwapChain9Ex **ppIDirect3DSwapChain9Ex) {
	m_pWrapped = *ppIDirect3DSwapChain9Ex;
	*ppIDirect3DSwapChain9Ex = this;
}

HRESULT APIENTRY hkIDirect3DSwapChain9Ex::QueryInterface(REFIID riid, void** ppvObj) {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::QueryInterface\n");
	return m_pWrapped->QueryInterface(riid, ppvObj);
}

ULONG APIENTRY hkIDirect3DSwapChain9Ex::AddRef() {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::AddRef\n");
	return m_pWrapped->AddRef();
}

ULONG APIENTRY hkIDirect3DSwapChain9Ex::Release() {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::Release\n");
	return m_pWrapped->Release();
}

HRESULT APIENTRY hkIDirect3DSwapChain9Ex::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags) {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::Present\n");
	return m_pWrapped->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

HRESULT APIENTRY hkIDirect3DSwapChain9Ex::GetFrontBufferData(IDirect3DSurface9* pDestSurface) {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::GetFrontBufferData\n");
	return m_pWrapped->GetFrontBufferData(pDestSurface);
}

HRESULT APIENTRY hkIDirect3DSwapChain9Ex::GetBackBuffer(UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::GetBackBuffer\n");
	return m_pWrapped->GetBackBuffer(iBackBuffer, Type, ppBackBuffer);
}

HRESULT APIENTRY hkIDirect3DSwapChain9Ex::GetRasterStatus(D3DRASTER_STATUS* pRasterStatus) {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::GetRasterStatus\n");
	return m_pWrapped->GetRasterStatus(pRasterStatus);
}

HRESULT APIENTRY hkIDirect3DSwapChain9Ex::GetDisplayMode(D3DDISPLAYMODE* pMode) {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::GetDisplayMode\n");
	return m_pWrapped->GetDisplayMode(pMode);
}

HRESULT APIENTRY hkIDirect3DSwapChain9Ex::GetDevice(IDirect3DDevice9** ppDevice) {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::GetDevice\n");
	return m_pWrapped->GetDevice(ppDevice);
}

HRESULT APIENTRY hkIDirect3DSwapChain9Ex::GetPresentParameters(D3DPRESENT_PARAMETERS* pPresentationParameters) {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::GetPresentParameters\n");
	return m_pWrapped->GetPresentParameters(pPresentationParameters);
}

HRESULT APIENTRY hkIDirect3DSwapChain9Ex::GetLastPresentCount(UINT* pLastPresentCount) {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::GetLastPresentCount\n");
	return m_pWrapped->GetLastPresentCount(pLastPresentCount);
}

HRESULT APIENTRY hkIDirect3DSwapChain9Ex::GetPresentStats(D3DPRESENTSTATS* pPresentationStatistics) {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::GetPresentStats\n");
	return m_pWrapped->GetPresentStats(pPresentationStatistics);
}

HRESULT APIENTRY hkIDirect3DSwapChain9Ex::GetDisplayModeEx(D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) {
	SDLOG(20, "hkIDirect3DSwapChain9Ex::GetDisplayModeEx\n");
	return m_pWrapped->GetDisplayModeEx(pMode, pRotation);
}
