// wrapper for IDXGIAdapter in dxgi.h
// generated using wrapper_gen11.rb

#include "dxgi/dxgioutput.h"
#include "settings.h"
#include "utils/interface_registry.h"

// ifdef because we could be included from hkIDXGIAdapter1
#ifndef hkIDXGIAdapter
#include "dxgi/dxgiadapter.h"

hkIDXGIAdapter::hkIDXGIAdapter(IDXGIAdapter **ppIDXGIAdapter) {
	SDLOG(5, "hkIDXGIAdapter::hkIDXGIAdapter(%p)\n", *ppIDXGIAdapter);
	pWrapped = *ppIDXGIAdapter;
	*ppIDXGIAdapter = this;
}
#endif

HRESULT APIENTRY hkIDXGIAdapter::QueryInterface(REFIID riid, void **ppvObject) {
	return InterfaceRegistry::get().QueryInterface("hkIDXGIAdapter", pWrapped, riid, ppvObject);
}

ULONG APIENTRY hkIDXGIAdapter::AddRef() {
	SDLOG(20, "hkIDXGIAdapter::AddRef\n");
	return pWrapped->AddRef();
}

ULONG APIENTRY hkIDXGIAdapter::Release() {
	SDLOG(20, "hkIDXGIAdapter::Release\n");
	auto ret = pWrapped->Release();
	if(ret == 0) {
		InterfaceRegistry::get().unregisterWrapper(this);
		delete this;
	}
	return ret;
}

HRESULT APIENTRY hkIDXGIAdapter::SetPrivateData(REFGUID Name, UINT DataSize, const void *pData) {
	SDLOG(20, "hkIDXGIAdapter::SetPrivateData\n");
	return pWrapped->SetPrivateData(Name, DataSize, pData);
}

HRESULT APIENTRY hkIDXGIAdapter::SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown) {
	SDLOG(20, "hkIDXGIAdapter::SetPrivateDataInterface\n");
	return pWrapped->SetPrivateDataInterface(Name, pUnknown);
}

HRESULT APIENTRY hkIDXGIAdapter::GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData) {
	SDLOG(20, "hkIDXGIAdapter::GetPrivateData\n");
	return pWrapped->GetPrivateData(Name, pDataSize, pData);
}

HRESULT APIENTRY hkIDXGIAdapter::GetParent(REFIID riid, void **ppParent) {
	return InterfaceRegistry::get().GetParent("hkIDXGIAdapter", pWrapped, riid, ppParent);
}

HRESULT APIENTRY hkIDXGIAdapter::EnumOutputs(UINT Output, IDXGIOutput **ppOutput) {
	SDLOG(20, "hkIDXGIAdapter::EnumOutputs\n");
	HRESULT res = pWrapped->EnumOutputs(Output, ppOutput);
	if(SUCCEEDED(res)) {
		*ppOutput = InterfaceRegistry::get().getWrappedInterface<IDXGIOutput>(*ppOutput);
	}
	return res;
}

HRESULT APIENTRY hkIDXGIAdapter::GetDesc(DXGI_ADAPTER_DESC *pDesc) {
	SDLOG(20, "hkIDXGIAdapter::GetDesc\n");
	return pWrapped->GetDesc(pDesc);
}

HRESULT APIENTRY hkIDXGIAdapter::CheckInterfaceSupport(REFGUID InterfaceName, LARGE_INTEGER *pUMDVersion) {
	SDLOG(20, "hkIDXGIAdapter::CheckInterfaceSupport\n");
	return pWrapped->CheckInterfaceSupport(InterfaceName, pUMDVersion);
}
