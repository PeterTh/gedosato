// wrapper for IDXGIAdapter in dxgi.h
// generated using wrapper_gen11.rb

#include "dxgi/dxgiadapter.h"

#include "settings.h"

hkIDXGIAdapter::hkIDXGIAdapter(IDXGIAdapter **ppIDXGIAdapter) {
	pWrapped = *ppIDXGIAdapter;
	*ppIDXGIAdapter = this;
}

HRESULT APIENTRY hkIDXGIAdapter::QueryInterface(REFIID riid, void **ppvObject) {
	SDLOG(20, "hkIDXGIAdapter::QueryInterface\n");
	return pWrapped->QueryInterface(riid, ppvObject);
}

ULONG APIENTRY hkIDXGIAdapter::AddRef() {
	SDLOG(20, "hkIDXGIAdapter::AddRef\n");
	return pWrapped->AddRef();
}

ULONG APIENTRY hkIDXGIAdapter::Release() {
	SDLOG(20, "hkIDXGIAdapter::Release\n");
	return pWrapped->Release();
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
	SDLOG(20, "hkIDXGIAdapter::GetParent\n");
	return pWrapped->GetParent(riid, ppParent);
}

HRESULT APIENTRY hkIDXGIAdapter::EnumOutputs(UINT Output, IDXGIOutput **ppOutput) {
	SDLOG(20, "hkIDXGIAdapter::EnumOutputs\n");
	return pWrapped->EnumOutputs(Output, ppOutput);
}

HRESULT APIENTRY hkIDXGIAdapter::GetDesc(DXGI_ADAPTER_DESC *pDesc) {
	SDLOG(20, "hkIDXGIAdapter::GetDesc\n");
	return pWrapped->GetDesc(pDesc);
}

HRESULT APIENTRY hkIDXGIAdapter::CheckInterfaceSupport(REFGUID InterfaceName, LARGE_INTEGER *pUMDVersion) {
	SDLOG(20, "hkIDXGIAdapter::CheckInterfaceSupport\n");
	return pWrapped->CheckInterfaceSupport(InterfaceName, pUMDVersion);
}
