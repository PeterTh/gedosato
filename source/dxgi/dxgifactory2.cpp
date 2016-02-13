// wrapper for IDXGIFactory2 in dxgi1_2.h
// generated using wrapper_gen11.rb

#include "settings.h"

// could be included from dxgifactory3
#ifndef hkIDXGIFactory2

#include "dxgi/dxgifactory2.h"

hkIDXGIFactory2::hkIDXGIFactory2(IDXGIFactory2 **ppIDXGIFactory2) {
	SDLOG(20, "hkIDXGIFactory2::hkIDXGIFactory2\n");
	pWrapped = *ppIDXGIFactory2;
	*ppIDXGIFactory2 = this;
}

#endif // not def hkIDXGIFactory1

#define hkIDXGIFactory1 hkIDXGIFactory2
#include "dxgifactory1.cpp"
#undef hkIDXGIFactory

BOOL APIENTRY hkIDXGIFactory2::IsWindowedStereoEnabled() {
	SDLOG(20, "hkIDXGIFactory2::IsWindowedStereoEnabled\n");
	return pWrapped->IsWindowedStereoEnabled();
}

HRESULT APIENTRY hkIDXGIFactory2::CreateSwapChainForHwnd(IUnknown *pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1 *pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc, IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain) {
	SDLOG(20, "hkIDXGIFactory2::CreateSwapChainForHwnd\n");
	return pWrapped->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
}

HRESULT APIENTRY hkIDXGIFactory2::CreateSwapChainForCoreWindow(IUnknown *pDevice, IUnknown *pWindow, const DXGI_SWAP_CHAIN_DESC1 *pDesc, IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain) {
	SDLOG(20, "hkIDXGIFactory2::CreateSwapChainForCoreWindow\n");
	return pWrapped->CreateSwapChainForCoreWindow(pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
}

HRESULT APIENTRY hkIDXGIFactory2::GetSharedResourceAdapterLuid(HANDLE hResource, LUID *pLuid) {
	SDLOG(20, "hkIDXGIFactory2::GetSharedResourceAdapterLuid\n");
	return pWrapped->GetSharedResourceAdapterLuid(hResource, pLuid);
}

HRESULT APIENTRY hkIDXGIFactory2::RegisterStereoStatusWindow(HWND WindowHandle, UINT wMsg, DWORD *pdwCookie) {
	SDLOG(20, "hkIDXGIFactory2::RegisterStereoStatusWindow\n");
	return pWrapped->RegisterStereoStatusWindow(WindowHandle, wMsg, pdwCookie);
}

HRESULT APIENTRY hkIDXGIFactory2::RegisterStereoStatusEvent(HANDLE hEvent, DWORD *pdwCookie) {
	SDLOG(20, "hkIDXGIFactory2::RegisterStereoStatusEvent\n");
	return pWrapped->RegisterStereoStatusEvent(hEvent, pdwCookie);
}

void APIENTRY hkIDXGIFactory2::UnregisterStereoStatus(DWORD dwCookie) {
	SDLOG(20, "hkIDXGIFactory2::UnregisterStereoStatus\n");
	return pWrapped->UnregisterStereoStatus(dwCookie);
}

HRESULT APIENTRY hkIDXGIFactory2::RegisterOcclusionStatusWindow(HWND WindowHandle, UINT wMsg, DWORD *pdwCookie) {
	SDLOG(20, "hkIDXGIFactory2::RegisterOcclusionStatusWindow\n");
	return pWrapped->RegisterOcclusionStatusWindow(WindowHandle, wMsg, pdwCookie);
}

HRESULT APIENTRY hkIDXGIFactory2::RegisterOcclusionStatusEvent(HANDLE hEvent, DWORD *pdwCookie) {
	SDLOG(20, "hkIDXGIFactory2::RegisterOcclusionStatusEvent\n");
	return pWrapped->RegisterOcclusionStatusEvent(hEvent, pdwCookie);
}

void APIENTRY hkIDXGIFactory2::UnregisterOcclusionStatus(DWORD dwCookie) {
	SDLOG(20, "hkIDXGIFactory2::UnregisterOcclusionStatus\n");
	return pWrapped->UnregisterOcclusionStatus(dwCookie);
}

HRESULT APIENTRY hkIDXGIFactory2::CreateSwapChainForComposition(IUnknown *pDevice, const DXGI_SWAP_CHAIN_DESC1 *pDesc, IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain) {
	SDLOG(20, "hkIDXGIFactory2::CreateSwapChainForComposition\n");
	return pWrapped->CreateSwapChainForComposition(pDevice, pDesc, pRestrictToOutput, ppSwapChain);
}
