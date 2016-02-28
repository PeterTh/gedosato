// wrapper for IDirect3DTexture9 in d3d9.h
// generated using wrapper_gen.rb

#include "d3d9/d3d9tex.h"

#include "settings.h"
#include "utils/win_utils.h"
#include "utils/d3d9_utils.h"
#include "utils/imgproc_utils.h"
#include "renderstate_manager_dx9.h"


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


namespace {
	void convertToARGB8(const void* in, void* out, int width, int height, int inPitch, D3DFORMAT fmt) {
		if(fmt == D3DFORMAT::D3DFMT_A8R8G8B8) {
			for(int y = 0; y < height; ++y) {
				for(int x = 0; x < width; ++x) {
					((uint32_t*)out)[0 + x + y*width] = ((uint32_t*)in)[0 + x + y*inPitch/4];
				}
			}
		}
		//else if(fmt == D3DFORMAT::D3DFMT_A8R8G8B8) {
		//	for(int y = 0; y < height; ++y) {
		//		for(int x = 0; x < width; ++x) {
		//			((BYTE*)out)[0 + x * 4 + y*width * 4] = ((BYTE*)in)[0 + x * 4 + y*inPitch];
		//			((BYTE*)out)[1 + x * 4 + y*width * 4] = ((BYTE*)in)[3 + x * 4 + y*inPitch];
		//			((BYTE*)out)[2 + x * 4 + y*width * 4] = ((BYTE*)in)[2 + x * 4 + y*inPitch];
		//			((BYTE*)out)[3 + x * 4 + y*width * 4] = ((BYTE*)in)[1 + x * 4 + y*inPitch];
		//		}
		//	}
		//}
		else {
			SDLOG(-1, "ERROR: texture scaling conversion - unsupported format: %s", D3DFormatToString(fmt));
		}
	}
	void convertFromARGB8(const void* in, void* out, int width, int height, int outPitch, D3DFORMAT fmt) {
		if(fmt == D3DFORMAT::D3DFMT_A8R8G8B8) {
			for(int y = 0; y < height; ++y) {
				for(int x = 0; x < width; ++x) {
					((uint32_t*)out)[0 + x + y*outPitch/4] = ((uint32_t*)in)[0 + x + y*width];
				}
			}
		}
		//else if(fmt == D3DFORMAT::D3DFMT_A8R8G8B8) {
		//	for(int y = 0; y < height; ++y) {
		//		for(int x = 0; x < width; ++x) {
		//			((BYTE*)out)[0 + x * 4 + y*outPitch] = ((BYTE*)in)[0 + x * 4 + y*width * 4];
		//			((BYTE*)out)[1 + x * 4 + y*outPitch] = ((BYTE*)in)[3 + x * 4 + y*width * 4];
		//			((BYTE*)out)[2 + x * 4 + y*outPitch] = ((BYTE*)in)[2 + x * 4 + y*width * 4];
		//			((BYTE*)out)[3 + x * 4 + y*outPitch] = ((BYTE*)in)[1 + x * 4 + y*width * 4];
		//		}
		//	}
		//}
		else {
			SDLOG(-1, "ERROR: texture scaling inverse conversion - unsupported format: %s", D3DFormatToString(fmt));
		}
	}
}

HRESULT APIENTRY hkIDirect3DTexture9::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) {
	SDLOG(20, "hkIDirect3DTexture9::LockRect\n");

	D3DSURFACE_DESC desc;
	GetLevelDesc(Level, &desc);
	if(pRect != nullptr) {
		rect = *pRect;
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		rect.left *= Settings::get().getTextureScalingFactor();
		rect.right = rect.left + width;
		rect.top *= Settings::get().getTextureScalingFactor();
		rect.bottom = rect.top + height;
	} else {
		rect.left = 0;
		rect.top = 0;
		rect.right = desc.Width / Settings::get().getTextureScalingFactor();
		rect.bottom = desc.Height / Settings::get().getTextureScalingFactor();
	}
	auto ret = m_pWrapped->LockRect(Level, pLockedRect, &rect, Flags);
	if(SUCCEEDED(ret)) {
		dirty = true;
		lockedRect = pLockedRect;
		SDLOG(-1, " -> locked rect %s on texture size %4dx%4d\n", RectToString(&rect), desc.Width, desc.Height);
	}
	return ret;
}

HRESULT APIENTRY hkIDirect3DTexture9::UnlockRect(UINT Level) {
	SDLOG(20, "hkIDirect3DTexture9::UnlockRect\n");
	if(lockedRect != nullptr) {
		SDLOG(-1, " -> unlocking, scaling\n");
		D3DSURFACE_DESC desc;
		GetLevelDesc(Level, &desc);
		SDLOG(-1, " (tex format: %s)\n", D3DSurfaceDescToString(desc));
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		size_t bytesize = width*height*4;
		SDLOG(-1, " -> allocating %d\n", bytesize);
		BYTE* data = new BYTE[bytesize];
		uint32_t* uintData = (uint32_t*)data;
		SDLOG(-1, " -> allocated\n");
		convertToARGB8(lockedRect->pBits, uintData, width, height, lockedRect->Pitch, desc.Format);
		SDLOG(-1, " -> copied\n");
		//stbi_write_png(format(R"(C:\tmp\dump\%p_unscaled.png)",m_pWrapped).c_str(), width, height, 4, data, width*4);
		auto ret = m_pWrapped->UnlockRect(Level);
		SDLOG(-1, " -> scaling\n");
		GlobalTexScaler::get().Scale(uintData, width, height, Settings::get().getTextureScalingFactor());
		D3DLOCKED_RECT relocked;
		rect.right = rect.left + width;
		rect.bottom = rect.top + height;
		if(SUCCEEDED(m_pWrapped->LockRect(Level, &relocked, &rect, 0))) {
			SDLOG(-1, " -> relocked rect %s pitch %d\n", RectToString(&rect), relocked.Pitch);
			convertFromARGB8(uintData, relocked.pBits, width, height, relocked.Pitch, desc.Format);
			SDLOG(-1, " -> copied back\n");
			//stbi_write_png(format(R"(C:\tmp\dump\%p_scaled.png)", m_pWrapped).c_str(), width, height, 4, uintData, width * 4);
			m_pWrapped->UnlockRect(Level);
		}
		SDLOG(-1, " -> cleaning data\n");
		delete [] data;
		lockedRect = nullptr;
		return ret;
	}
	return m_pWrapped->UnlockRect(Level);
}

HRESULT APIENTRY hkIDirect3DTexture9::AddDirtyRect(CONST RECT* pDirtyRect) {
	SDLOG(20, "hkIDirect3DTexture9::AddDirtyRect\n");
	return m_pWrapped->AddDirtyRect(pDirtyRect);
}
