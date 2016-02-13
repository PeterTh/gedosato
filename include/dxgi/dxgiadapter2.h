// wrapper for IDXGIAdapter2 in dxgi1_3.h
// generated using wrapper_gen11.rb

#pragma once

#include "dxgi1_3.h"

interface hkIDXGIAdapter2 : public IDXGIAdapter2 {
	IDXGIAdapter2 *pWrapped;
	
public:
	hkIDXGIAdapter2(IDXGIAdapter2 **ppIDXGIAdapter2);
	
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

	virtual HRESULT STDMETHODCALLTYPE GetDesc1(DXGI_ADAPTER_DESC1 *pDesc);

	virtual HRESULT STDMETHODCALLTYPE GetDesc2(DXGI_ADAPTER_DESC2 *pDesc);

};

