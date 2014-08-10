#include "d3d9/d3d9int_ex.h"

// This is pretty terrible, but less terrible than lots of duplicate code
#define hkIDirect3D9 hkIDirect3D9Ex
#include "d3d9int.cpp"
#undef hkIDirect3D9

/////////

UINT APIENTRY hkIDirect3D9Ex::GetAdapterModeCountEx(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter) {
	SDLOG(2, "GetAdapterModeCountEx Adapter %u | %s ------\n", Adapter, D3DFormatToString(pFilter->Format));
	UINT ret = m_pD3Dint->GetAdapterModeCountEx(Adapter, pFilter);
	UINT reported = ret == 0 ? 0 : ret + Settings::getResSettings().getNumResolutions();
	SDLOG(2, " -> %u (reporting %u)\n", ret, reported);
	return reported;
}

HRESULT APIENTRY hkIDirect3D9Ex::EnumAdapterModesEx(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode) {
	SDLOG(2, "EnumAdapterModesEx ------ Adapter %u | %s : %u\n", Adapter, D3DFormatToString(pFilter->Format), Mode);
	unsigned baseModes = m_pD3Dint->GetAdapterModeCountEx(Adapter, pFilter);
	if(baseModes >= Mode) {
		SDLOG(0, "Injecting downsampling mode!\n");
		const auto& res = Settings::getResSettings().getResolution(Mode - baseModes);
		pMode->RefreshRate = res.hz;
		pMode->Width = res.width;
		pMode->Height = res.height;
		pMode->ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
		pMode->Format = pFilter->Format;
		return D3D_OK;
	}
	HRESULT ret = m_pD3Dint->EnumAdapterModesEx(Adapter, pFilter, Mode, pMode);
	if(SUCCEEDED(ret) && pMode->Width == Settings::get().getOverrideWidth() && pMode->Height == Settings::get().getOverrideHeight()) {
		pMode->Width = Settings::get().getRenderWidth();
		pMode->Height = Settings::get().getRenderHeight();
		SDLOG(2, "-> Override D3D9Ex\n");
	}
	return ret;
}

HRESULT APIENTRY hkIDirect3D9Ex::GetAdapterDisplayModeEx(UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) {
	SDLOG(2, "GetAdapterDisplayModeEx ------\n");
	return m_pD3Dint->GetAdapterDisplayModeEx(Adapter, pMode, pRotation);
}

HRESULT APIENTRY hkIDirect3D9Ex::CreateDeviceEx(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, 
												D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface) {
	SDLOG(0, "CreateDeviceEx ------ Adapter %u Thread %u\n", Adapter, GetCurrentThreadId());
	return RSManager::redirectCreateDeviceEx(m_pD3Dint, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);
	//return m_pD3Dint->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);
}

HRESULT APIENTRY hkIDirect3D9Ex::GetAdapterLUID(UINT Adapter, LUID* pLUID) {
	SDLOG(2, "GetAdapterLUID ------\n");
	return m_pD3Dint->GetAdapterLUID(Adapter, pLUID);
}
