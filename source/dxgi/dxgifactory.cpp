// wrapper for IDXGIFactory in dxgi.h
// generated using wrapper_gen11.rb

// could be included from dxgifactory1
#ifndef hkIDXGIFactory 

#include "dxgi/dxgifactory.h"

#include "dxgi/dxgiswapchain.h"
#include "dxgi/dxgiadapter.h"
#include "dxgi/dxgiadapter1.h"
#include "settings.h"

hkIDXGIFactory::hkIDXGIFactory(IDXGIFactory **ppIDXGIFactory) {
	pWrapped = *ppIDXGIFactory;
	*ppIDXGIFactory = this;
}
#endif // not def hkIDXGIFactory

HRESULT APIENTRY hkIDXGIFactory::QueryInterface(REFIID riid, void **ppvObject) {
	SDLOG(20, "hkIDXGIFactory::QueryInterface\n");
	return pWrapped->QueryInterface(riid, ppvObject);
}

ULONG APIENTRY hkIDXGIFactory::AddRef() {
	SDLOG(20, "hkIDXGIFactory::AddRef\n");
	return pWrapped->AddRef();
}

ULONG APIENTRY hkIDXGIFactory::Release() {
	SDLOG(20, "hkIDXGIFactory::Release\n");
	return pWrapped->Release();
}

HRESULT APIENTRY hkIDXGIFactory::SetPrivateData(REFGUID Name, UINT DataSize, const void *pData) {
	SDLOG(20, "hkIDXGIFactory::SetPrivateData\n");
	return pWrapped->SetPrivateData(Name, DataSize, pData);
}

HRESULT APIENTRY hkIDXGIFactory::SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown) {
	SDLOG(20, "hkIDXGIFactory::SetPrivateDataInterface\n");
	return pWrapped->SetPrivateDataInterface(Name, pUnknown);
}

HRESULT APIENTRY hkIDXGIFactory::GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData) {
	SDLOG(20, "hkIDXGIFactory::GetPrivateData\n");
	return pWrapped->GetPrivateData(Name, pDataSize, pData);
}

HRESULT APIENTRY hkIDXGIFactory::GetParent(REFIID riid, void **ppParent) {
	SDLOG(20, "hkIDXGIFactory::GetParent\n");
	return pWrapped->GetParent(riid, ppParent);
}

HRESULT APIENTRY hkIDXGIFactory::EnumAdapters(UINT Adapter, IDXGIAdapter **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory::EnumAdapters %d %p\n", Adapter, ppAdapter);
	HRESULT res = pWrapped->EnumAdapters(Adapter, ppAdapter);
	if(SUCCEEDED(res)) {
		SDLOG(20, "--> Success, wrapping %p\n", *ppAdapter);
		new hkIDXGIAdapter(ppAdapter);
	}
	return res;
}

HRESULT APIENTRY hkIDXGIFactory::MakeWindowAssociation(HWND WindowHandle, UINT Flags) {
	SDLOG(20, "hkIDXGIFactory::MakeWindowAssociation\n");
	return pWrapped->MakeWindowAssociation(WindowHandle, Flags);
}

HRESULT APIENTRY hkIDXGIFactory::GetWindowAssociation(HWND *pWindowHandle) {
	SDLOG(20, "hkIDXGIFactory::GetWindowAssociation\n");
	return pWrapped->GetWindowAssociation(pWindowHandle);
}

HRESULT APIENTRY hkIDXGIFactory::CreateSwapChain(IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain) {
	SDLOG(20, "hkIDXGIFactory::CreateSwapChain\n");
	HRESULT hr = pWrapped->CreateSwapChain(pDevice, pDesc, ppSwapChain);
	if(SUCCEEDED(hr) && ppSwapChain != NULL && *ppSwapChain != NULL) {
		SDLOG(2, " -> hkIDXGIFactory1::CreateSwapChain hooked\n");
		new hkIDXGISwapChain(ppSwapChain);
	}
	return hr;
}

HRESULT APIENTRY hkIDXGIFactory::CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory::CreateSoftwareAdapter\n");
	return pWrapped->CreateSoftwareAdapter(Module, ppAdapter);
}
