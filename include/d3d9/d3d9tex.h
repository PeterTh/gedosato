// wrapper for IDirect3DTexture9 in d3d9.h
// generated using wrapper_gen.rb

#pragma once

#include "d3d9.h"

const extern GUID IID_GedosatoTexture;

interface hkIDirect3DTexture9 : public IDirect3DTexture9 {
	IDirect3DTexture9 *m_pWrapped;
	bool dirty = false;
	D3DLOCKED_RECT* lockedRect = nullptr;
	RECT rect;
		
public:
	hkIDirect3DTexture9(IDirect3DTexture9 **ppIDirect3DTexture9);
	
	// original interface
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObj);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice);
	STDMETHOD(SetPrivateData)(REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags);
	STDMETHOD(GetPrivateData)(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	STDMETHOD(FreePrivateData)(REFGUID refguid);
	STDMETHOD_(DWORD, SetPriority)(DWORD PriorityNew);
	STDMETHOD_(DWORD, GetPriority)();
	STDMETHOD_(void, PreLoad)();
	STDMETHOD_(D3DRESOURCETYPE, GetType)();
	STDMETHOD_(DWORD, SetLOD)(DWORD LODNew);
	STDMETHOD_(DWORD, GetLOD)();
	STDMETHOD_(DWORD, GetLevelCount)();
	STDMETHOD(SetAutoGenFilterType)(D3DTEXTUREFILTERTYPE FilterType);
	STDMETHOD_(D3DTEXTUREFILTERTYPE, GetAutoGenFilterType)();
	STDMETHOD_(void, GenerateMipSubLevels)();
	STDMETHOD(GetLevelDesc)(UINT Level, D3DSURFACE_DESC *pDesc);
	STDMETHOD(GetSurfaceLevel)(UINT Level, IDirect3DSurface9** ppSurfaceLevel);
	STDMETHOD(LockRect)(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags);
	STDMETHOD(UnlockRect)(UINT Level);
	STDMETHOD(AddDirtyRect)(CONST RECT* pDirtyRect);

	// additions
	IDirect3DTexture9* getTextureToUse();
};

