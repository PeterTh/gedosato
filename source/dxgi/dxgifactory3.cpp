// wrapper for IDXGIFactory3 in dxgi1_3.h
// generated using wrapper_gen11.rb

#include "dxgi/dxgifactory3.h"

#include "settings.h"

hkIDXGIFactory3::hkIDXGIFactory3(IDXGIFactory3 **ppIDXGIFactory3) {
	pWrapped = *ppIDXGIFactory3;
	*ppIDXGIFactory3 = this;
}

HRESULT APIENTRY hkIDXGIFactory3::QueryInterface(REFIID riid, void **ppvObject) {
	SDLOG(20, "hkIDXGIFactory3::QueryInterface\n");
	return pWrapped->QueryInterface(riid, ppvObject);
}

ULONG APIENTRY hkIDXGIFactory3::AddRef() {
	SDLOG(20, "hkIDXGIFactory3::AddRef\n");
	return pWrapped->AddRef();
}

ULONG APIENTRY hkIDXGIFactory3::Release() {
	SDLOG(20, "hkIDXGIFactory3::Release\n");
	return pWrapped->Release();
}

HRESULT APIENTRY hkIDXGIFactory3::SetPrivateData(REFGUID Name, UINT DataSize, const void *pData) {
	SDLOG(20, "hkIDXGIFactory3::SetPrivateData\n");
	return pWrapped->SetPrivateData(Name, DataSize, pData);
}

HRESULT APIENTRY hkIDXGIFactory3::SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown) {
	SDLOG(20, "hkIDXGIFactory3::SetPrivateDataInterface\n");
	return pWrapped->SetPrivateDataInterface(Name, pUnknown);
}

HRESULT APIENTRY hkIDXGIFactory3::GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData) {
	SDLOG(20, "hkIDXGIFactory3::GetPrivateData\n");
	return pWrapped->GetPrivateData(Name, pDataSize, pData);
}

HRESULT APIENTRY hkIDXGIFactory3::GetParent(REFIID riid, void **ppParent) {
	SDLOG(20, "hkIDXGIFactory3::GetParent\n");
	return pWrapped->GetParent(riid, ppParent);
}

HRESULT APIENTRY hkIDXGIFactory3::EnumAdapters(UINT Adapter, IDXGIAdapter **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory3::EnumAdapters\n");
	return pWrapped->EnumAdapters(Adapter, ppAdapter);
}

HRESULT APIENTRY hkIDXGIFactory3::MakeWindowAssociation(HWND WindowHandle, UINT Flags) {
	SDLOG(20, "hkIDXGIFactory3::MakeWindowAssociation\n");
	return pWrapped->MakeWindowAssociation(WindowHandle, Flags);
}

HRESULT APIENTRY hkIDXGIFactory3::GetWindowAssociation(HWND *pWindowHandle) {
	SDLOG(20, "hkIDXGIFactory3::GetWindowAssociation\n");
	return pWrapped->GetWindowAssociation(pWindowHandle);
}

HRESULT APIENTRY hkIDXGIFactory3::CreateSwapChain(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain) {
	SDLOG(20, "hkIDXGIFactory3::CreateSwapChain\n");
	return pWrapped->CreateSwapChain(pDevice, pDesc, ppSwapChain);
}

HRESULT APIENTRY hkIDXGIFactory3::CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory3::CreateSoftwareAdapter\n");
	return pWrapped->CreateSoftwareAdapter(Module, ppAdapter);
}

HRESULT APIENTRY hkIDXGIFactory3::EnumAdapters1(UINT Adapter, IDXGIAdapter1 **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory3::EnumAdapters1\n");
	return pWrapped->EnumAdapters1(Adapter, ppAdapter);
}

BOOL APIENTRY hkIDXGIFactory3::IsCurrent() {
	SDLOG(20, "hkIDXGIFactory3::IsCurrent\n");
	return pWrapped->IsCurrent();
}

BOOL APIENTRY hkIDXGIFactory3::IsWindowedStereoEnabled() {
	SDLOG(20, "hkIDXGIFactory3::IsWindowedStereoEnabled\n");
	return pWrapped->IsWindowedStereoEnabled();
}

HRESULT APIENTRY hkIDXGIFactory3::CreateSwapChainForHwnd(IUnknown *pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1 *pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc, IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain) {
	SDLOG(20, "hkIDXGIFactory3::CreateSwapChainForHwnd\n");
	return pWrapped->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
}

HRESULT APIENTRY hkIDXGIFactory3::CreateSwapChainForCoreWindow(IUnknown *pDevice, IUnknown *pWindow, const DXGI_SWAP_CHAIN_DESC1 *pDesc, IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain) {
	SDLOG(20, "hkIDXGIFactory3::CreateSwapChainForCoreWindow\n");
	return pWrapped->CreateSwapChainForCoreWindow(pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
}

HRESULT APIENTRY hkIDXGIFactory3::GetSharedResourceAdapterLuid(HANDLE hResource, LUID *pLuid) {
	SDLOG(20, "hkIDXGIFactory3::GetSharedResourceAdapterLuid\n");
	return pWrapped->GetSharedResourceAdapterLuid(hResource, pLuid);
}

HRESULT APIENTRY hkIDXGIFactory3::RegisterStereoStatusWindow(HWND WindowHandle, UINT wMsg, DWORD *pdwCookie) {
	SDLOG(20, "hkIDXGIFactory3::RegisterStereoStatusWindow\n");
	return pWrapped->RegisterStereoStatusWindow(WindowHandle, wMsg, pdwCookie);
}

HRESULT APIENTRY hkIDXGIFactory3::RegisterStereoStatusEvent(HANDLE hEvent, DWORD *pdwCookie) {
	SDLOG(20, "hkIDXGIFactory3::RegisterStereoStatusEvent\n");
	return pWrapped->RegisterStereoStatusEvent(hEvent, pdwCookie);
}

void APIENTRY hkIDXGIFactory3::UnregisterStereoStatus(DWORD dwCookie) {
	SDLOG(20, "hkIDXGIFactory3::UnregisterStereoStatus\n");
	return pWrapped->UnregisterStereoStatus(dwCookie);
}

HRESULT APIENTRY hkIDXGIFactory3::RegisterOcclusionStatusWindow(HWND WindowHandle, UINT wMsg, DWORD *pdwCookie) {
	SDLOG(20, "hkIDXGIFactory3::RegisterOcclusionStatusWindow\n");
	return pWrapped->RegisterOcclusionStatusWindow(WindowHandle, wMsg, pdwCookie);
}

HRESULT APIENTRY hkIDXGIFactory3::RegisterOcclusionStatusEvent(HANDLE hEvent, DWORD *pdwCookie) {
	SDLOG(20, "hkIDXGIFactory3::RegisterOcclusionStatusEvent\n");
	return pWrapped->RegisterOcclusionStatusEvent(hEvent, pdwCookie);
}

void APIENTRY hkIDXGIFactory3::UnregisterOcclusionStatus(DWORD dwCookie) {
	SDLOG(20, "hkIDXGIFactory3::UnregisterOcclusionStatus\n");
	return pWrapped->UnregisterOcclusionStatus(dwCookie);
}

HRESULT APIENTRY hkIDXGIFactory3::CreateSwapChainForComposition(IUnknown *pDevice, const DXGI_SWAP_CHAIN_DESC1 *pDesc, IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain) {
	SDLOG(20, "hkIDXGIFactory3::CreateSwapChainForComposition\n");
	return pWrapped->CreateSwapChainForComposition(pDevice, pDesc, pRestrictToOutput, ppSwapChain);
}

UINT APIENTRY hkIDXGIFactory3::GetCreationFlags() {
	SDLOG(20, "hkIDXGIFactory3::GetCreationFlags\n");
	return pWrapped->GetCreationFlags();
}
