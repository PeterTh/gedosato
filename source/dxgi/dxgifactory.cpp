// wrapper for IDXGIFactory in dxgi.h
// generated using wrapper_gen11.rb

#include "dxgi/dxgiswapchain.h"
#include "dxgi/dxgiadapter.h"
#include "dxgi/dxgiadapter1.h"
#include "settings.h"
#include "utils/dxgi_utils.h"
#include "utils/interface_registry.h"
#include "renderstate_manager_dx11.h"

// could be included from dxgifactory1
#ifndef hkIDXGIFactory 

#include "dxgi/dxgifactory.h"

hkIDXGIFactory::hkIDXGIFactory(IDXGIFactory **ppIDXGIFactory) {
	SDLOG(20, "hkIDXGIFactory::hkIDXGIFactory\n");
	pWrapped = *ppIDXGIFactory;
	*ppIDXGIFactory = this;
}

#endif // not def hkIDXGIFactory

HRESULT APIENTRY hkIDXGIFactory::QueryInterface(REFIID riid, void **ppvObject) {
	return InterfaceRegistry::get().QueryInterface("hkIDXGIFactory", pWrapped, riid, ppvObject);
}

ULONG APIENTRY hkIDXGIFactory::AddRef() {
	SDLOG(20, "hkIDXGIFactory::AddRef\n");
	return pWrapped->AddRef();
}

ULONG APIENTRY hkIDXGIFactory::Release() {
	SDLOG(20, "hkIDXGIFactory::Release\n");
	auto ret = pWrapped->Release();
	if(ret == 0) {
		InterfaceRegistry::get().unregisterWrapper(this);
		delete this;
	}
	return ret;
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
	return InterfaceRegistry::get().GetParent("hkIDXGIFactory", pWrapped, riid, ppParent);
}

HRESULT APIENTRY hkIDXGIFactory::EnumAdapters(UINT Adapter, IDXGIAdapter **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory::EnumAdapters %d %p\n", Adapter, ppAdapter);
	HRESULT res = pWrapped->EnumAdapters(Adapter, ppAdapter);
	if(SUCCEEDED(res)) {
		SDLOG(20, "--> Success, wrapping %p\n", *ppAdapter);
		*ppAdapter = InterfaceRegistry::get().getWrappedInterface<IDXGIAdapter>(*ppAdapter);
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
	SDLOG(2, "hkIDXGIFactory::CreateSwapChain with device %p, requested:\n%s\n", pDevice, DxgiSwapChainDescToString(*pDesc));
	HRESULT hr = RSManager::getDX11().redirectCreateSwapChain(pWrapped, pDevice, pDesc, ppSwapChain);
	if(SUCCEEDED(hr) && ppSwapChain != NULL && *ppSwapChain != NULL) {
		SDLOG(2, " -> hkIDXGIFactory::CreateSwapChain hooked, received:\n%s\n", DxgiSwapChainDescToString(*pDesc));
		*ppSwapChain = InterfaceRegistry::get().getWrappedInterface<IDXGISwapChain>(*ppSwapChain);
	}
	return hr;
}

HRESULT APIENTRY hkIDXGIFactory::CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory::CreateSoftwareAdapter\n");
	return pWrapped->CreateSoftwareAdapter(Module, ppAdapter);
}
