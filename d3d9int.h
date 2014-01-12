#pragma once

#include "d3d9.h"

interface hkIDirect3D9 : public IDirect3D9 {
public:
	hkIDirect3D9(IDirect3D9 **ppIDirect3D9) {
		m_pD3Dint = *ppIDirect3D9;
		*ppIDirect3D9 = this;
	}
	
	// callback interface
	IDirect3D9 *m_pD3Dint;

	// original interface
    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObj);
    STDMETHOD_(ULONG,AddRef)();
    STDMETHOD_(ULONG,Release)();
	STDMETHOD(RegisterSoftwareDevice)(void* pInitializeFunction);
    STDMETHOD_(UINT, GetAdapterCount)();
    STDMETHOD(GetAdapterIdentifier)(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier);
    STDMETHOD_(UINT, GetAdapterModeCount)(UINT Adapter,D3DFORMAT Format);
    STDMETHOD(EnumAdapterModes)(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode);
    STDMETHOD(GetAdapterDisplayMode)(UINT Adapter,D3DDISPLAYMODE* pMode);
    STDMETHOD(CheckDeviceType)(UINT Adapter,D3DDEVTYPE DevType,D3DFORMAT AdapterFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed);
    STDMETHOD(CheckDeviceFormat)(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat);
    STDMETHOD(CheckDeviceMultiSampleType)(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels);
    STDMETHOD(CheckDepthStencilMatch)(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat);
    STDMETHOD(CheckDeviceFormatConversion)(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat);
    STDMETHOD(GetDeviceCaps)(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps);
    STDMETHOD_(HMONITOR, GetAdapterMonitor)(UINT Adapter);
    STDMETHOD(CreateDevice)(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface);
};