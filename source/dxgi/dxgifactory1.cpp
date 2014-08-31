// wrapper for IDXGIFactory1 in dxgi.h
// generated using wrapper_gen11.rb

#include "dxgi/dxgifactory1.h"

#include "dxgi/dxgiadapter.h"
#include "dxgi/dxgiadapter1.h"
#include "settings.h"

hkIDXGIFactory1::hkIDXGIFactory1(IDXGIFactory1 **ppIDXGIFactory1) {
	pWrapped = *ppIDXGIFactory1;
	*ppIDXGIFactory1 = this;
}

HRESULT APIENTRY hkIDXGIFactory1::QueryInterface(REFIID riid, void **ppvObject) {
	SDLOG(20, "hkIDXGIFactory1::QueryInterface\n");
	return pWrapped->QueryInterface(riid, ppvObject);
}

ULONG APIENTRY hkIDXGIFactory1::AddRef() {
	SDLOG(20, "hkIDXGIFactory1::AddRef\n");
	return pWrapped->AddRef();
}

ULONG APIENTRY hkIDXGIFactory1::Release() {
	SDLOG(20, "hkIDXGIFactory1::Release\n");
	return pWrapped->Release();
}

HRESULT APIENTRY hkIDXGIFactory1::SetPrivateData(REFGUID Name, UINT DataSize, const void *pData) {
	SDLOG(20, "hkIDXGIFactory1::SetPrivateData\n");
	return pWrapped->SetPrivateData(Name, DataSize, pData);
}

HRESULT APIENTRY hkIDXGIFactory1::SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown) {
	SDLOG(20, "hkIDXGIFactory1::SetPrivateDataInterface\n");
	return pWrapped->SetPrivateDataInterface(Name, pUnknown);
}

HRESULT APIENTRY hkIDXGIFactory1::GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData) {
	SDLOG(20, "hkIDXGIFactory1::GetPrivateData\n");
	return pWrapped->GetPrivateData(Name, pDataSize, pData);
}

HRESULT APIENTRY hkIDXGIFactory1::GetParent(REFIID riid, void **ppParent) {
	SDLOG(20, "hkIDXGIFactory1::GetParent\n");
	return pWrapped->GetParent(riid, ppParent);
}

HRESULT APIENTRY hkIDXGIFactory1::EnumAdapters(UINT Adapter, IDXGIAdapter **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory1::EnumAdapters %d %p\n", Adapter, ppAdapter);
	HRESULT res = pWrapped->EnumAdapters(Adapter, ppAdapter);
	if(SUCCEEDED(res)) {
		SDLOG(20, "--> Success, wrapping %p\n", *ppAdapter);
		new hkIDXGIAdapter(ppAdapter);
	}
	return res;
}

HRESULT APIENTRY hkIDXGIFactory1::MakeWindowAssociation(HWND WindowHandle, UINT Flags) {
	SDLOG(20, "hkIDXGIFactory1::MakeWindowAssociation\n");
	return pWrapped->MakeWindowAssociation(WindowHandle, Flags);
}

HRESULT APIENTRY hkIDXGIFactory1::GetWindowAssociation(HWND *pWindowHandle) {
	SDLOG(20, "hkIDXGIFactory1::GetWindowAssociation\n");
	return pWrapped->GetWindowAssociation(pWindowHandle);
}

HRESULT APIENTRY hkIDXGIFactory1::CreateSwapChain(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain) {
	SDLOG(20, "hkIDXGIFactory1::CreateSwapChain\n");
	return pWrapped->CreateSwapChain(pDevice, pDesc, ppSwapChain);
}

HRESULT APIENTRY hkIDXGIFactory1::CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory1::CreateSoftwareAdapter\n");
	return pWrapped->CreateSoftwareAdapter(Module, ppAdapter);
}

HRESULT APIENTRY hkIDXGIFactory1::EnumAdapters1(UINT Adapter, IDXGIAdapter1 **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory1::EnumAdapters1\n");
	HRESULT res = pWrapped->EnumAdapters1(Adapter, ppAdapter);
	if(SUCCEEDED(res)) {
		SDLOG(20, "--> Success, wrapping %p\n", *ppAdapter);
		new hkIDXGIAdapter1(ppAdapter);
	}
	return res;
}

BOOL APIENTRY hkIDXGIFactory1::IsCurrent() {
	SDLOG(20, "hkIDXGIFactory1::IsCurrent\n");
	return pWrapped->IsCurrent();
}
