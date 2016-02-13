// wrapper for IDXGIDevice3 in dxgi1_3.h
// generated using wrapper_gen11.rb

#pragma once

#include "dxgi1_3.h"

interface hkIDXGIDevice3 : public IDXGIDevice3 {
	IDXGIDevice3 *pWrapped;
	
public:
	hkIDXGIDevice3(IDXGIDevice3 **ppIDXGIDevice3);
	
	// original interface

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID Name, UINT DataSize, const void *pData);
	virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown);
	virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData);
	virtual HRESULT STDMETHODCALLTYPE GetParent(REFIID riid, void **ppParent);

	virtual HRESULT STDMETHODCALLTYPE GetAdapter(IDXGIAdapter **pAdapter);
	virtual HRESULT STDMETHODCALLTYPE CreateSurface(const DXGI_SURFACE_DESC *pDesc, UINT NumSurfaces, DXGI_USAGE Usage, const DXGI_SHARED_RESOURCE *pSharedResource, IDXGISurface **ppSurface);
	virtual HRESULT STDMETHODCALLTYPE QueryResourceResidency(IUnknown *const *ppResources, DXGI_RESIDENCY *pResidencyStatus, UINT NumResources);
	virtual HRESULT STDMETHODCALLTYPE SetGPUThreadPriority(INT Priority);
	virtual HRESULT STDMETHODCALLTYPE GetGPUThreadPriority(INT *pPriority);

	virtual HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency(UINT MaxLatency);
	virtual HRESULT STDMETHODCALLTYPE GetMaximumFrameLatency(UINT *pMaxLatency);

	virtual HRESULT STDMETHODCALLTYPE OfferResources(UINT NumResources, IDXGIResource *const *ppResources, DXGI_OFFER_RESOURCE_PRIORITY Priority);
	virtual HRESULT STDMETHODCALLTYPE ReclaimResources(UINT NumResources, IDXGIResource *const *ppResources, BOOL *pDiscarded);
	virtual HRESULT STDMETHODCALLTYPE EnqueueSetEvent(HANDLE hEvent);

	virtual void STDMETHODCALLTYPE Trim(void);

};

