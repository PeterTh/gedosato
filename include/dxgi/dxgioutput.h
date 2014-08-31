// wrapper for IDXGIOutput in dxgi.h
// generated using wrapper_gen11.rb

#pragma once

#include "dxgi.h"

interface hkIDXGIOutput : public IDXGIOutput {
	IDXGIOutput *pWrapped;
	
public:
	hkIDXGIOutput(IDXGIOutput **ppIDXGIOutput);
	
	// original interface

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID Name, UINT DataSize, const void *pData);
	virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown);
	virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData);
	virtual HRESULT STDMETHODCALLTYPE GetParent(REFIID riid, void **ppParent);

	virtual HRESULT STDMETHODCALLTYPE GetDesc(DXGI_OUTPUT_DESC *pDesc);
	virtual HRESULT STDMETHODCALLTYPE GetDisplayModeList(DXGI_FORMAT EnumFormat, UINT Flags, UINT *pNumModes, DXGI_MODE_DESC *pDesc);
	virtual HRESULT STDMETHODCALLTYPE FindClosestMatchingMode(const DXGI_MODE_DESC *pModeToMatch, DXGI_MODE_DESC *pClosestMatch, IUnknown *pConcernedDevice);
	virtual HRESULT STDMETHODCALLTYPE WaitForVBlank(void);
	virtual HRESULT STDMETHODCALLTYPE TakeOwnership(IUnknown *pDevice, BOOL Exclusive);
	virtual void STDMETHODCALLTYPE ReleaseOwnership(void);
	virtual HRESULT STDMETHODCALLTYPE GetGammaControlCapabilities(DXGI_GAMMA_CONTROL_CAPABILITIES *pGammaCaps);
	virtual HRESULT STDMETHODCALLTYPE SetGammaControl(const DXGI_GAMMA_CONTROL *pArray);
	virtual HRESULT STDMETHODCALLTYPE GetGammaControl(DXGI_GAMMA_CONTROL *pArray);
	virtual HRESULT STDMETHODCALLTYPE SetDisplaySurface(IDXGISurface *pScanoutSurface);
	virtual HRESULT STDMETHODCALLTYPE GetDisplaySurfaceData(IDXGISurface *pDestination);
	virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics(DXGI_FRAME_STATISTICS *pStats);

};

