// wrapper for IDXGIOutput in dxgi.h
// generated using wrapper_gen11.rb

#include "dxgi/dxgioutput.h"

#include "settings.h"
#include "utils/dxgi_utils.h"

hkIDXGIOutput::hkIDXGIOutput(IDXGIOutput **ppIDXGIOutput) {
	pWrapped = *ppIDXGIOutput;
	*ppIDXGIOutput = this;
}

HRESULT APIENTRY hkIDXGIOutput::QueryInterface(REFIID riid, void **ppvObject) {
	SDLOG(20, "hkIDXGIOutput::QueryInterface\n");
	return pWrapped->QueryInterface(riid, ppvObject);
}

ULONG APIENTRY hkIDXGIOutput::AddRef() {
	SDLOG(20, "hkIDXGIOutput::AddRef\n");
	return pWrapped->AddRef();
}

ULONG APIENTRY hkIDXGIOutput::Release() {
	SDLOG(20, "hkIDXGIOutput::Release\n");
	return pWrapped->Release();
}

HRESULT APIENTRY hkIDXGIOutput::SetPrivateData(REFGUID Name, UINT DataSize, const void *pData) {
	SDLOG(20, "hkIDXGIOutput::SetPrivateData\n");
	return pWrapped->SetPrivateData(Name, DataSize, pData);
}

HRESULT APIENTRY hkIDXGIOutput::SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown) {
	SDLOG(20, "hkIDXGIOutput::SetPrivateDataInterface\n");
	return pWrapped->SetPrivateDataInterface(Name, pUnknown);
}

HRESULT APIENTRY hkIDXGIOutput::GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData) {
	SDLOG(20, "hkIDXGIOutput::GetPrivateData\n");
	return pWrapped->GetPrivateData(Name, pDataSize, pData);
}

HRESULT APIENTRY hkIDXGIOutput::GetParent(REFIID riid, void **ppParent) {
	SDLOG(20, "hkIDXGIOutput::GetParent\n");
	return pWrapped->GetParent(riid, ppParent);
}

HRESULT APIENTRY hkIDXGIOutput::GetDesc(DXGI_OUTPUT_DESC *pDesc) {
	SDLOG(20, "hkIDXGIOutput::GetDesc\n");
	return pWrapped->GetDesc(pDesc);
}

HRESULT APIENTRY hkIDXGIOutput::GetDisplayModeList(DXGI_FORMAT EnumFormat, UINT Flags, UINT *pNumModes, DXGI_MODE_DESC *pDesc) {
	SDLOG(20, "hkIDXGIOutput::GetDisplayModeList\n");
	SDLOG(20, "   - EnumFormat: %s\n", DxgiFormatToString(EnumFormat));
	SDLOG(20, "   - Flags: %u\n", Flags);
	HRESULT hr = pWrapped->GetDisplayModeList(EnumFormat, Flags, pNumModes, pDesc);
	if(SUCCEEDED(hr) && *pNumModes > 0) {
		UINT totalModes = *pNumModes + static_cast<UINT>(Settings::getResSettings().getNumResolutions());
		if(pDesc != NULL) {
			for(unsigned i = *pNumModes, j = 0; i < totalModes; ++i, ++j) {
				auto res = Settings::get().getResSettings().getResolution(j);
				SDLOG(20, " -> GetDisplayModeList add mode #%u (%ux%u@%u)\n", i, res.width, res.height, res.hz);
				pDesc[i].Format = EnumFormat;
				pDesc[i].Width = res.width;
				pDesc[i].Height = res.height;
				pDesc[i].RefreshRate = { res.hz, 1 };
				pDesc[i].ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
				pDesc[i].Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			}
		}
		SDLOG(20, " -> GetDisplayModeList increase reported number of modes from %u to %u\n", *pNumModes, totalModes);
		*pNumModes = totalModes;
	}
	return hr;
}

HRESULT APIENTRY hkIDXGIOutput::FindClosestMatchingMode(const DXGI_MODE_DESC *pModeToMatch, DXGI_MODE_DESC *pClosestMatch, IUnknown *pConcernedDevice) {
	SDLOG(20, "hkIDXGIOutput::FindClosestMatchingMode\n");
	return pWrapped->FindClosestMatchingMode(pModeToMatch, pClosestMatch, pConcernedDevice);
}

HRESULT APIENTRY hkIDXGIOutput::WaitForVBlank() {
	SDLOG(20, "hkIDXGIOutput::WaitForVBlank\n");
	return pWrapped->WaitForVBlank();
}

HRESULT APIENTRY hkIDXGIOutput::TakeOwnership(IUnknown *pDevice, BOOL Exclusive) {
	SDLOG(20, "hkIDXGIOutput::TakeOwnership\n");
	return pWrapped->TakeOwnership(pDevice, Exclusive);
}

void APIENTRY hkIDXGIOutput::ReleaseOwnership() {
	SDLOG(20, "hkIDXGIOutput::ReleaseOwnership\n");
	return pWrapped->ReleaseOwnership();
}

HRESULT APIENTRY hkIDXGIOutput::GetGammaControlCapabilities(DXGI_GAMMA_CONTROL_CAPABILITIES *pGammaCaps) {
	SDLOG(20, "hkIDXGIOutput::GetGammaControlCapabilities\n");
	return pWrapped->GetGammaControlCapabilities(pGammaCaps);
}

HRESULT APIENTRY hkIDXGIOutput::SetGammaControl(const DXGI_GAMMA_CONTROL *pArray) {
	SDLOG(20, "hkIDXGIOutput::SetGammaControl\n");
	return pWrapped->SetGammaControl(pArray);
}

HRESULT APIENTRY hkIDXGIOutput::GetGammaControl(DXGI_GAMMA_CONTROL *pArray) {
	SDLOG(20, "hkIDXGIOutput::GetGammaControl\n");
	return pWrapped->GetGammaControl(pArray);
}

HRESULT APIENTRY hkIDXGIOutput::SetDisplaySurface(IDXGISurface *pScanoutSurface) {
	SDLOG(20, "hkIDXGIOutput::SetDisplaySurface\n");
	return pWrapped->SetDisplaySurface(pScanoutSurface);
}

HRESULT APIENTRY hkIDXGIOutput::GetDisplaySurfaceData(IDXGISurface *pDestination) {
	SDLOG(20, "hkIDXGIOutput::GetDisplaySurfaceData\n");
	return pWrapped->GetDisplaySurfaceData(pDestination);
}

HRESULT APIENTRY hkIDXGIOutput::GetFrameStatistics(DXGI_FRAME_STATISTICS *pStats) {
	SDLOG(20, "hkIDXGIOutput::GetFrameStatistics\n");
	return pWrapped->GetFrameStatistics(pStats);
}
