// wrapper for IDirect3DSwapChain9 in d3d9.h
// generated using wrapper_gen.rb

#include "d3d9/d3d9swap.h"
#include "d3d9/d3d9dev.h"

#include "settings.h"
#include "renderstate_manager_dx9.h"

hkIDirect3DSwapChain9::hkIDirect3DSwapChain9(IDirect3DSwapChain9 **ppIDirect3DSwapChain9, hkIDirect3DDevice9* hdev) 
		: hookedDev(hdev) {
	m_pWrapped = *ppIDirect3DSwapChain9;
	*ppIDirect3DSwapChain9 = this;
}
hkIDirect3DSwapChain9::hkIDirect3DSwapChain9(IDirect3DSwapChain9 **ppIDirect3DSwapChain9, hkIDirect3DDevice9Ex* hdev)
		: hookedDev((hkIDirect3DDevice9*)hdev) {
	m_pWrapped = *ppIDirect3DSwapChain9;
	*ppIDirect3DSwapChain9 = this;
}

HRESULT APIENTRY hkIDirect3DSwapChain9::QueryInterface(REFIID riid, void** ppvObj) {
	SDLOG(20, "hkIDirect3DSwapChain9::QueryInterface\n");
	return m_pWrapped->QueryInterface(riid, ppvObj);
}

ULONG APIENTRY hkIDirect3DSwapChain9::AddRef() {
	SDLOG(20, "hkIDirect3DSwapChain9::AddRef\n");
	return m_pWrapped->AddRef();
}

ULONG APIENTRY hkIDirect3DSwapChain9::Release() {
	SDLOG(20, "hkIDirect3DSwapChain9::Release\n");
	return m_pWrapped->Release();
}

HRESULT APIENTRY hkIDirect3DSwapChain9::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags) {
	SDLOG(3, "hkIDirect3DSwapChain9::Present\n");
	return RSManager::getDX9().redirectPresent(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion); // TODO: dwflags?
}

HRESULT APIENTRY hkIDirect3DSwapChain9::GetFrontBufferData(IDirect3DSurface9* pDestSurface) {
	SDLOG(-1, "hkIDirect3DSwapChain9::GetFrontBufferData\n");
	return m_pWrapped->GetFrontBufferData(pDestSurface);
}

HRESULT APIENTRY hkIDirect3DSwapChain9::GetBackBuffer(UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) {
	SDLOG(7, "hkIDirect3DSwapChain9::GetBackBuffer\n");
	return RSManager::getDX9().redirectGetBackBuffer(0, iBackBuffer, Type, ppBackBuffer);
}

HRESULT APIENTRY hkIDirect3DSwapChain9::GetRasterStatus(D3DRASTER_STATUS* pRasterStatus) {
	SDLOG(-1, "hkIDirect3DSwapChain9::GetRasterStatus\n");
	return m_pWrapped->GetRasterStatus(pRasterStatus);
}

HRESULT APIENTRY hkIDirect3DSwapChain9::GetDisplayMode(D3DDISPLAYMODE* pMode) {
	SDLOG(1, "hkIDirect3DSwapChain9::GetDisplayMode\n");
	return RSManager::getDX9().redirectGetDisplayMode(0, pMode);
}

HRESULT APIENTRY hkIDirect3DSwapChain9::GetDevice(IDirect3DDevice9** ppDevice) {
	SDLOG(5, "hkIDirect3DSwapChain9::GetDevice\n");
	*ppDevice = hookedDev;
	return D3D_OK;
}

HRESULT APIENTRY hkIDirect3DSwapChain9::GetPresentParameters(D3DPRESENT_PARAMETERS* pPresentationParameters) {
	SDLOG(-1, "hkIDirect3DSwapChain9::GetPresentParameters\n");
	return m_pWrapped->GetPresentParameters(pPresentationParameters);
}
