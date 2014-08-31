// wrapper for IDXGIAdapter in dxgi.h
// generated using wrapper_gen11.rb

#pragma once

#include "dxgi.h"

interface hkIDXGIAdapter : public IDXGIAdapter {
	IDXGIAdapter *pWrapped;
	
public:
	hkIDXGIAdapter(IDXGIAdapter **ppIDXGIAdapter);
	
	// original interface

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID Name, UINT DataSize, const void *pData);
	virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown);
	virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData);
	virtual HRESULT STDMETHODCALLTYPE GetParent(REFIID riid, void **ppParent);

	virtual HRESULT STDMETHODCALLTYPE EnumOutputs(UINT Output, IDXGIOutput **ppOutput);
	virtual HRESULT STDMETHODCALLTYPE GetDesc(DXGI_ADAPTER_DESC *pDesc);
	virtual HRESULT STDMETHODCALLTYPE CheckInterfaceSupport(REFGUID InterfaceName, LARGE_INTEGER *pUMDVersion);

};

