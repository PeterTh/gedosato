// wrapper for IDirect3DTexture9 in d3d9.h
// generated using wrapper_gen.rb

#include "d3d9/d3d9tex.h"

#include "settings.h"


IDirect3DTexture9* hkIDirect3DTexture9::getTextureToUse() {
	if(dirty) {
		dirty = false;
		if(Settings::get().getEnableAlternativeTextureDumping()) {
			// TODO
		}
	}
	return m_pWrapped;
}


const GUID IID_GedosatoTexture = { 0x31337, 0x3de5, 0x4f00, 0x9b, 0x3a, 0xf1, 0x1a, 0xc3, 0x8c, 0x18, 0xb5 };

hkIDirect3DTexture9::hkIDirect3DTexture9(IDirect3DTexture9 **ppIDirect3DTexture9) {
	m_pWrapped = *ppIDirect3DTexture9;
	*ppIDirect3DTexture9 = this;
}

HRESULT APIENTRY hkIDirect3DTexture9::QueryInterface(REFIID riid, void** ppvObj) {
	if(IsEqualIID(riid, IID_GedosatoTexture)) {
		return S_OK;
	}
	SDLOG(20, "hkIDirect3DTexture9::QueryInterface\n");
	return m_pWrapped->QueryInterface(riid, ppvObj);
}

ULONG APIENTRY hkIDirect3DTexture9::AddRef() {
	SDLOG(20, "hkIDirect3DTexture9::AddRef\n");
	return m_pWrapped->AddRef();
}

ULONG APIENTRY hkIDirect3DTexture9::Release() {
	SDLOG(20, "hkIDirect3DTexture9::Release\n");
	return m_pWrapped->Release();
}

HRESULT APIENTRY hkIDirect3DTexture9::GetDevice(IDirect3DDevice9** ppDevice) {
	SDLOG(20, "hkIDirect3DTexture9::GetDevice\n");
	return m_pWrapped->GetDevice(ppDevice);
}

HRESULT APIENTRY hkIDirect3DTexture9::SetPrivateData(REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags) {
	SDLOG(20, "hkIDirect3DTexture9::SetPrivateData\n");
	return m_pWrapped->SetPrivateData(refguid, pData, SizeOfData, Flags);
}

HRESULT APIENTRY hkIDirect3DTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) {
	SDLOG(20, "hkIDirect3DTexture9::GetPrivateData\n");
	return m_pWrapped->GetPrivateData(refguid, pData, pSizeOfData);
}

HRESULT APIENTRY hkIDirect3DTexture9::FreePrivateData(REFGUID refguid) {
	SDLOG(20, "hkIDirect3DTexture9::FreePrivateData\n");
	return m_pWrapped->FreePrivateData(refguid);
}

DWORD APIENTRY hkIDirect3DTexture9::SetPriority(DWORD PriorityNew) {
	SDLOG(20, "hkIDirect3DTexture9::SetPriority\n");
	return m_pWrapped->SetPriority(PriorityNew);
}

DWORD APIENTRY hkIDirect3DTexture9::GetPriority() {
	SDLOG(20, "hkIDirect3DTexture9::GetPriority\n");
	return m_pWrapped->GetPriority();
}

void APIENTRY hkIDirect3DTexture9::PreLoad() {
	SDLOG(20, "hkIDirect3DTexture9::PreLoad\n");
	return m_pWrapped->PreLoad();
}

D3DRESOURCETYPE APIENTRY hkIDirect3DTexture9::GetType() {
	SDLOG(20, "hkIDirect3DTexture9::GetType\n");
	return m_pWrapped->GetType();
}

DWORD APIENTRY hkIDirect3DTexture9::SetLOD(DWORD LODNew) {
	SDLOG(20, "hkIDirect3DTexture9::SetLOD\n");
	return m_pWrapped->SetLOD(LODNew);
}

DWORD APIENTRY hkIDirect3DTexture9::GetLOD() {
	SDLOG(20, "hkIDirect3DTexture9::GetLOD\n");
	return m_pWrapped->GetLOD();
}

DWORD APIENTRY hkIDirect3DTexture9::GetLevelCount() {
	SDLOG(20, "hkIDirect3DTexture9::GetLevelCount\n");
	return m_pWrapped->GetLevelCount();
}

HRESULT APIENTRY hkIDirect3DTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType) {
	SDLOG(20, "hkIDirect3DTexture9::SetAutoGenFilterType\n");
	return m_pWrapped->SetAutoGenFilterType(FilterType);
}

D3DTEXTUREFILTERTYPE APIENTRY hkIDirect3DTexture9::GetAutoGenFilterType() {
	SDLOG(20, "hkIDirect3DTexture9::GetAutoGenFilterType\n");
	return m_pWrapped->GetAutoGenFilterType();
}

void APIENTRY hkIDirect3DTexture9::GenerateMipSubLevels() {
	SDLOG(20, "hkIDirect3DTexture9::GenerateMipSubLevels\n");
	return m_pWrapped->GenerateMipSubLevels();
}

HRESULT APIENTRY hkIDirect3DTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc) {
	SDLOG(20, "hkIDirect3DTexture9::GetLevelDesc\n");
	return m_pWrapped->GetLevelDesc(Level, pDesc);
}

HRESULT APIENTRY hkIDirect3DTexture9::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel) {
	SDLOG(20, "hkIDirect3DTexture9::GetSurfaceLevel\n");
	dirty = true;
	return m_pWrapped->GetSurfaceLevel(Level, ppSurfaceLevel);
}

HRESULT APIENTRY hkIDirect3DTexture9::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) {
	SDLOG(20, "hkIDirect3DTexture9::LockRect\n");
	dirty = true;
	return m_pWrapped->LockRect(Level, pLockedRect, pRect, Flags);
}

HRESULT APIENTRY hkIDirect3DTexture9::UnlockRect(UINT Level) {
	SDLOG(20, "hkIDirect3DTexture9::UnlockRect\n");
	return m_pWrapped->UnlockRect(Level);
}

HRESULT APIENTRY hkIDirect3DTexture9::AddDirtyRect(CONST RECT* pDirtyRect) {
	SDLOG(20, "hkIDirect3DTexture9::AddDirtyRect\n");
	return m_pWrapped->AddDirtyRect(pDirtyRect);
}
