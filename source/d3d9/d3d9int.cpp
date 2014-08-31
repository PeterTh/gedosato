/*	Direct3D9 Interface */

#include "winutil.h"
#include "main.h"
#include "d3d9.h"
#include "d3dutil.h"

#include "settings.h"
#include "renderstate_manager_dx9.h"

HRESULT APIENTRY hkIDirect3D9::QueryInterface(REFIID riid,  void **ppvObj) {
	SDLOG(1, "hkIDirect3D9::QueryInterface\n");
	return m_pD3Dint->QueryInterface(riid,  ppvObj);
}

ULONG APIENTRY hkIDirect3D9::AddRef() {
	SDLOG(1, "hkIDirect3D9::AddRef\n");
	return m_pD3Dint->AddRef();
}

HRESULT APIENTRY hkIDirect3D9::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) {
	SDLOG(0, "CheckDepthStencilMatch adapter %u, device %s, adapterformat: %s, renderformat: %s, depthformat: %s\n",
		Adapter, D3DDevTypeToString(DeviceType), D3DFormatToString(AdapterFormat), D3DFormatToString(RenderTargetFormat), D3DFormatToString(DepthStencilFormat));
	HRESULT res =  m_pD3Dint->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
	SDLOG(0, " -> %s\n", res == D3D_OK ? "OK" : "NOT OK!");
	return res;
}

HRESULT APIENTRY hkIDirect3D9::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) {
	SDLOG(0, "CheckDeviceFormat adapter %u, device %s, adapterformat: %s, usage: %u, resource: %s, checkformat: %s\n", 
		Adapter, D3DDevTypeToString(DeviceType), D3DFormatToString(AdapterFormat), Usage, D3DResourceTypeToString(RType), D3DFormatToString(CheckFormat));
	HRESULT res = m_pD3Dint->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
	SDLOG(0, " -> %s\n", res == D3D_OK ? "OK" : "NOT OK!");
	return res;
}

HRESULT APIENTRY hkIDirect3D9::CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat) {
	SDLOG(0, "CheckDeviceFormatConversion\n");
	return m_pD3Dint->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat);
}

HRESULT APIENTRY hkIDirect3D9::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) {
	SDLOG(0, "CheckDeviceMultiSampleType adapter %u, device %s, surfaceformat: %s, windowed %s, multisampletype: %s\n",
		Adapter, D3DDevTypeToString(DeviceType), D3DFormatToString(SurfaceFormat), Windowed?"true":"false", D3DMultisampleTypeToString(MultiSampleType));
	HRESULT res = m_pD3Dint->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels);
	SDLOG(0, " -> %s / q: %d\n", res == D3D_OK ? "OK" : "NOT OK!", pQualityLevels ? *pQualityLevels : -1);
	return res;
}

HRESULT APIENTRY hkIDirect3D9::CheckDeviceType(UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed) {
	SDLOG(0, "CheckDeviceType\n");
	return m_pD3Dint->CheckDeviceType(Adapter, CheckType, DisplayFormat, BackBufferFormat, Windowed);
}

HRESULT APIENTRY hkIDirect3D9::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9 **ppReturnedDeviceInterface) {
	SDLOG(0, "CreateDevice ------ Adapter %u Thread %u\n", Adapter, GetCurrentThreadId());
	if(!pPresentationParameters) {
		HRESULT hRet = m_pD3Dint->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
		if(SUCCEEDED(hRet)) hkIDirect3DDevice9 *ret = new hkIDirect3DDevice9(ppReturnedDeviceInterface, pPresentationParameters, this);
		return hRet;
	}

	return RSManagerDX9::redirectCreateDevice(m_pD3Dint, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
}

HRESULT APIENTRY hkIDirect3D9::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) {
	SDLOG(2, "EnumAdapterModes ------ Adapter %u | %s : %u\n", Adapter, D3DFormatToString(Format), Mode);
	unsigned baseModes = m_pD3Dint->GetAdapterModeCount(Adapter, Format);
	if(Mode >= baseModes) {
		SDLOG(0, "Injecting downsampling mode!\n");
		const auto& res = Settings::getResSettings().getResolution(Mode - baseModes);
		pMode->RefreshRate = res.hz;
		pMode->Width = res.width;
		pMode->Height = res.height;
		pMode->Format = Format;
		return D3D_OK;
	}
	HRESULT ret = m_pD3Dint->EnumAdapterModes(Adapter, Format, Mode, pMode);
	if(SUCCEEDED(ret) && pMode->Width == Settings::get().getOverrideWidth() && pMode->Height == Settings::get().getOverrideHeight()) {
		pMode->Width = Settings::get().getRenderWidth();
		pMode->Height = Settings::get().getRenderHeight();
		SDLOG(2, "-> Override D3D9\n");
	}
	return ret;
}

UINT APIENTRY hkIDirect3D9::GetAdapterCount() {
	SDLOG(0, "GetAdapterCount ------\n");
	return m_pD3Dint->GetAdapterCount();
}

HRESULT APIENTRY hkIDirect3D9::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE *pMode) {
	SDLOG(0, "GetAdapterDisplayMode ------\n");
	if(Settings::get().getForceBorderlessFullscreen()) {
		pMode->Width = Settings::get().getPresentWidth();
		pMode->Height = Settings::get().getRenderHeight();
		pMode->RefreshRate = Settings::getResSettings().getActiveHz();
		pMode->Format = D3DFMT_A8R8G8B8;
		return D3D_OK;
	}
	return m_pD3Dint->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT APIENTRY hkIDirect3D9::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier) {
	SDLOG(0, "GetAdapterIdentifier ------\n");
	return m_pD3Dint->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT APIENTRY hkIDirect3D9::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) {
	SDLOG(2, "GetAdapterModeCount Adapter %u | %s ------\n", Adapter, D3DFormatToString(Format));
	UINT ret = m_pD3Dint->GetAdapterModeCount(Adapter, Format);
	UINT reported = ret == 0 ? 0 : ret + Settings::getResSettings().getNumResolutions();
	SDLOG(2, " -> %u (reporting %u)\n", ret, reported);
	return reported;
}

HMONITOR APIENTRY hkIDirect3D9::GetAdapterMonitor(UINT Adapter) {
	SDLOG(0, "GetAdapterMonitor adapter %u ------\n", Adapter);
	HMONITOR res = m_pD3Dint->GetAdapterMonitor(Adapter);
	SDLOG(0, "-> %p\n", res);
	return res;
}

HRESULT APIENTRY hkIDirect3D9::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9 *pCaps) {
	SDLOG(0, "GetDeviceCaps %u %x %p ------\n", Adapter, DeviceType, pCaps);
	return m_pD3Dint->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HRESULT APIENTRY hkIDirect3D9::RegisterSoftwareDevice(void *pInitializeFunction) {
	SDLOG(0, "RegisterSoftwareDevice ------\n");
	return m_pD3Dint->RegisterSoftwareDevice(pInitializeFunction);
}

ULONG APIENTRY hkIDirect3D9::Release() {
	SDLOG(20, "Release hkIDirect3D9 ------ %p \n", this);
	return m_pD3Dint->Release();
}
