// wrapper for IDXGIFactory2 in dxgi1_2.h
// generated using wrapper_gen11.rb

#include "dxgi/dxgifactory2.h"

#include "settings.h"

hkIDXGIFactory2::hkIDXGIFactory2(IDXGIFactory2 **ppIDXGIFactory2) {
	pWrapped = *ppIDXGIFactory2;
	*ppIDXGIFactory2 = this;
}

HRESULT APIENTRY hkIDXGIFactory2::QueryInterface(REFIID riid, void **ppvObject) {
	SDLOG(20, "hkIDXGIFactory2::QueryInterface\n");
	return pWrapped->QueryInterface(riid, ppvObject);
}

ULONG APIENTRY hkIDXGIFactory2::AddRef() {
	SDLOG(20, "hkIDXGIFactory2::AddRef\n");
	return pWrapped->AddRef();
}

ULONG APIENTRY hkIDXGIFactory2::Release() {
	SDLOG(20, "hkIDXGIFactory2::Release\n");
	return pWrapped->Release();
}

HRESULT APIENTRY hkIDXGIFactory2::SetPrivateData(REFGUID Name, UINT DataSize, const void *pData) {
	SDLOG(20, "hkIDXGIFactory2::SetPrivateData\n");
	return pWrapped->SetPrivateData(Name, DataSize, pData);
}

HRESULT APIENTRY hkIDXGIFactory2::SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown) {
	SDLOG(20, "hkIDXGIFactory2::SetPrivateDataInterface\n");
	return pWrapped->SetPrivateDataInterface(Name, pUnknown);
}

HRESULT APIENTRY hkIDXGIFactory2::GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData) {
	SDLOG(20, "hkIDXGIFactory2::GetPrivateData\n");
	return pWrapped->GetPrivateData(Name, pDataSize, pData);
}

HRESULT APIENTRY hkIDXGIFactory2::GetParent(REFIID riid, void **ppParent) {
	SDLOG(20, "hkIDXGIFactory2::GetParent\n");
	return pWrapped->GetParent(riid, ppParent);
}

HRESULT APIENTRY hkIDXGIFactory2::EnumAdapters(UINT Adapter, IDXGIAdapter **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory2::EnumAdapters\n");
	return pWrapped->EnumAdapters(Adapter, ppAdapter);
}

HRESULT APIENTRY hkIDXGIFactory2::MakeWindowAssociation(HWND WindowHandle, UINT Flags) {
	SDLOG(20, "hkIDXGIFactory2::MakeWindowAssociation\n");
	return pWrapped->MakeWindowAssociation(WindowHandle, Flags);
}

HRESULT APIENTRY hkIDXGIFactory2::GetWindowAssociation(HWND *pWindowHandle) {
	SDLOG(20, "hkIDXGIFactory2::GetWindowAssociation\n");
	return pWrapped->GetWindowAssociation(pWindowHandle);
}

HRESULT APIENTRY hkIDXGIFactory2::CreateSwapChain(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain) {
	SDLOG(20, "hkIDXGIFactory2::CreateSwapChain\n");
	return pWrapped->CreateSwapChain(pDevice, pDesc, ppSwapChain);
}

HRESULT APIENTRY hkIDXGIFactory2::CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory2::CreateSoftwareAdapter\n");
	return pWrapped->CreateSoftwareAdapter(Module, ppAdapter);
}

HRESULT APIENTRY hkIDXGIFactory2::EnumAdapters1(UINT Adapter, IDXGIAdapter1 **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory2::EnumAdapters1\n");
	return pWrapped->EnumAdapters1(Adapter, ppAdapter);
}

BOOL APIENTRY hkIDXGIFactory2::IsCurrent() {
	SDLOG(20, "hkIDXGIFactory2::IsCurrent\n");
	return pWrapped->IsCurrent();
}

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
