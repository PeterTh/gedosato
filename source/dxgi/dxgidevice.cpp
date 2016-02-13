// wrapper for IDXGIDevice in dxgi.h
// generated using wrapper_gen11.rb

#include "settings.h"
#include "utils/interface_registry.h"

// could be included from dxgidevice1
#ifndef hkIDXGIDevice 

#include "dxgi/dxgidevice.h"

hkIDXGIDevice::hkIDXGIDevice(IDXGIDevice **ppIDXGIDevice) {
	SDLOG(5, "hkIDXGIDevice::hkIDXGIDevice(%p)", *ppIDXGIDevice);
	pWrapped = *ppIDXGIDevice;
	*ppIDXGIDevice = this;
}

#endif // not def hkIDXGIDevice  

HRESULT APIENTRY hkIDXGIDevice::QueryInterface(REFIID riid, void **ppvObject) {
	SDLOG(20, "hkIDXGIDevice::QueryInterface\n");
	return pWrapped->QueryInterface(riid, ppvObject);
}

ULONG APIENTRY hkIDXGIDevice::AddRef() {
	SDLOG(20, "hkIDXGIDevice::AddRef\n");
	return pWrapped->AddRef();
}

ULONG APIENTRY hkIDXGIDevice::Release() {
	SDLOG(20, "hkIDXGIDevice::Release\n");
	return pWrapped->Release();
}

HRESULT APIENTRY hkIDXGIDevice::SetPrivateData(REFGUID Name, UINT DataSize, const void *pData) {
	SDLOG(20, "hkIDXGIDevice::SetPrivateData\n");
	return pWrapped->SetPrivateData(Name, DataSize, pData);
}

HRESULT APIENTRY hkIDXGIDevice::SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown) {
	SDLOG(20, "hkIDXGIDevice::SetPrivateDataInterface\n");
	return pWrapped->SetPrivateDataInterface(Name, pUnknown);
}

HRESULT APIENTRY hkIDXGIDevice::GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData) {
	SDLOG(20, "hkIDXGIDevice::GetPrivateData\n");
	return pWrapped->GetPrivateData(Name, pDataSize, pData);
}

HRESULT APIENTRY hkIDXGIDevice::GetParent(REFIID riid, void **ppParent) {
	return InterfaceRegistry::get().GetParent("hkIDXGIDevice", pWrapped, riid, ppParent);
}

HRESULT APIENTRY hkIDXGIDevice::GetAdapter(IDXGIAdapter **pAdapter) {
	SDLOG(20, "hkIDXGIDevice::GetAdapter\n");
	return pWrapped->GetAdapter(pAdapter);
}

HRESULT APIENTRY hkIDXGIDevice::CreateSurface(const DXGI_SURFACE_DESC *pDesc, UINT NumSurfaces, DXGI_USAGE Usage, const DXGI_SHARED_RESOURCE *pSharedResource, IDXGISurface **ppSurface) {
	SDLOG(20, "hkIDXGIDevice::CreateSurface\n");
	return pWrapped->CreateSurface(pDesc, NumSurfaces, Usage, pSharedResource, ppSurface);
}

HRESULT APIENTRY hkIDXGIDevice::QueryResourceResidency(IUnknown *const *ppResources, DXGI_RESIDENCY *pResidencyStatus, UINT NumResources) {
	SDLOG(20, "hkIDXGIDevice::QueryResourceResidency\n");
	return pWrapped->QueryResourceResidency(ppResources, pResidencyStatus, NumResources);
}

HRESULT APIENTRY hkIDXGIDevice::SetGPUThreadPriority(INT Priority) {
	SDLOG(20, "hkIDXGIDevice::SetGPUThreadPriority\n");
	return pWrapped->SetGPUThreadPriority(Priority);
}

HRESULT APIENTRY hkIDXGIDevice::GetGPUThreadPriority(INT *pPriority) {
	SDLOG(20, "hkIDXGIDevice::GetGPUThreadPriority\n");
	return pWrapped->GetGPUThreadPriority(pPriority);
}
