#include "plugins/ff13.h"

#include "renderstate_manager_dx9.h"
#include "d3d9/d3d9tex.h"
#include "utils/d3d9_utils.h"

#include "utils/win_utils.h"
#define HEAP_LFH 2

FF13Plugin::~FF13Plugin() {
}

void FF13Plugin::reportStatus() {
	Console::get().add(format("Shadow scale: %d", Settings::get().getShadowScale()));
	Console::get().add(format("MSAA: %d samples, coverage sampling %s", Settings::get().getMSAASampleCount(), Settings::get().getEnableCoverageSampling() ? "enabled" : "disabled"));
	Console::get().add("!! NOTE: This FINAL FANTASY XIII Plugin is currently in pre-alpha and HIGHLY EXPERIMENTAL");
}

void FF13Plugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	GenericPlugin::initialize(rw, rh, bbformat, dssformat);

	// Force low fragmentation heap
	SDLOG(2, "Starting heap adjustment (LFH)\n");
	HANDLE heaps[128];
	DWORD numHeaps = GetProcessHeaps(128, heaps);
	for(DWORD i = 0; i < numHeaps; ++i) {
		ULONG HeapInformation = HEAP_LFH;
		BOOL res = HeapSetInformation(heaps[i],	HeapCompatibilityInformation, &HeapInformation, sizeof(HeapInformation));
		if(res != FALSE) {
			SDLOG(1, "Low-fragmentation heap enabled for heap #%d.\n", i);
		} else {
			SDLOG(1, "Failed to enable low-fragmentation for heap #%d; LastError %d.\n", i, GetLastError());
		}
	}
	SDLOG(2, "Completed heap adjustment (LFH)\n");
}

void FF13Plugin::prePresent() {
	//if(IsDebuggerPresent()) DebugBreak();
	GenericPlugin::prePresent();
}

HRESULT FF13Plugin::redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	// shadows
	if(Settings::get().getShadowScale()>1 && Width == 1024 && Height == 1024 &&
		(Format == D3DFMT_D24S8 || Format == D3DFMT_R32F || Format == D3DFMT_D24X8)) {
		Width *= Settings::get().getShadowScale();
		Height *= Settings::get().getShadowScale();
		SDLOG(4, " -> [FF13] scaling shadow buffer to %dx%d\n", Width, Height);
	}
	// other rendertargets and buffers
	else if(Pool != D3DPOOL_MANAGED && ((Width == Settings::get().getPresentWidth() && Height == Settings::get().getPresentHeight()) || (Width == 1280 && Height == 720))) {
		Width = Settings::get().getRenderWidth();
		Height = Settings::get().getRenderHeight();
		SDLOG(4, " -> [FF13] scaling render texture to %dx%d\n", Width, Height);
	}
	else if(Settings::get().getEnableAlternativeTextureDumping() && !(Usage & D3DUSAGE_RENDERTARGET) && Width > 1 && Height > 1) {
		HRESULT hr = GenericPlugin::redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
		if(SUCCEEDED(hr)) {
			new hkIDirect3DTexture9(ppTexture);
		}
		return hr;
	}
	return GenericPlugin::redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT FF13Plugin::redirectCreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	if((Width == Settings::get().getPresentWidth() && Height == Settings::get().getPresentHeight()) || (Width == 1280 && Height == 720)) {
		Width = Settings::get().getRenderWidth();
		Height = Settings::get().getRenderHeight();
		SDLOG(4, " -> [FF13] scaling render target to %dx%d\n", Width, Height);
		if(MultiSample > 0) {
			MultiSample = static_cast<D3DMULTISAMPLE_TYPE>(Settings::get().getMSAASampleCount());
			if(Settings::get().getEnableCoverageSampling()) {
				DWORD quality;
				RSManager::getDX9().getD3D()->CheckDeviceMultiSampleType(0, D3DDEVTYPE_HAL, Format, false, MultiSample, &quality);
				if(quality > 0) MultisampleQuality = quality - 1;
			}
		}
	}
	return GenericPlugin::redirectCreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}


HRESULT FF13Plugin::redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	if(MultiSample > 0) {
		MultiSample = static_cast<D3DMULTISAMPLE_TYPE>(Settings::get().getMSAASampleCount());
		if(Settings::get().getEnableCoverageSampling()) {
			DWORD quality;
			RSManager::getDX9().getD3D()->CheckDeviceMultiSampleType(0, D3DDEVTYPE_HAL, Format, false, MultiSample, &quality);
			if(quality > 0) {
				MultisampleQuality = quality - 1;
				SDLOG(2, "[FF13]: Enabled coverage sampling, quality level %d\n", MultisampleQuality);
			}
		}
	}
	SDLOG(4, " -> [FF13] setting depth stencil surface to %dx%d\n", Settings::get().getRenderWidth(), Settings::get().getRenderHeight());
	return GenericPlugin::redirectCreateDepthStencilSurface(Settings::get().getRenderWidth(), Settings::get().getRenderHeight(), Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}


HRESULT FF13Plugin::redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
	return GenericPlugin::redirectSetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}
HRESULT FF13Plugin::redirectSetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
	return GenericPlugin::redirectSetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT FF13Plugin::redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
	return GenericPlugin::redirectDrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT FF13Plugin::redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
	return GenericPlugin::redirectSetDepthStencilSurface(pNewZStencil);
}

HRESULT FF13Plugin::redirectSetVertexShader(IDirect3DVertexShader9* pvShader) {
	return GenericPlugin::redirectSetVertexShader(pvShader);
}

HRESULT FF13Plugin::redirectSetPixelShader(IDirect3DPixelShader9* pShader) {
	return GenericPlugin::redirectSetPixelShader(pShader);
}

HRESULT FF13Plugin::redirectSetViewport(CONST D3DVIEWPORT9 * pViewport) {
	// shadows
	if(pViewport->Width == 1024 && pViewport->Height == 1024) {
		D3DVIEWPORT9 vp = *pViewport;
		vp.Width *= Settings::get().getShadowScale();
		vp.Height *= Settings::get().getShadowScale();
		SDLOG(4, " -> [FF13] adjusting shadow VP\n");
		return GenericPlugin::redirectSetViewport(&vp);
	}
	// mein rendering
	if(pViewport != NULL && pViewport->Width == 1280 && pViewport->Height == 720) {
		D3DVIEWPORT9 vp = *pViewport;
		vp.Width = Settings::get().getRenderWidth();
		vp.Height = Settings::get().getRenderHeight();
		SDLOG(4, " -> [FF13] adjusting main VP\n");
		return GenericPlugin::redirectSetViewport(&vp);
	}
	return GenericPlugin::redirectSetViewport(pViewport);
}

HRESULT FF13Plugin::redirectStretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) {
	// StretchRect src->dest, sR->dR : 05A4DAE0 -> 05A4D360, RECT[0 / 0 / 1280 / 720]->RECT[0 / 0 / 5120 / 2880]
	if(pSourceRect != NULL && pSourceRect->right == 1280 && pSourceRect->bottom == 720) {
		SDLOG(4, " -> [FF13] adjusting source rectangle\n");
		RECT source = { 0, 0, Settings::get().getRenderWidth(), Settings::get().getRenderHeight() };
		return GenericPlugin::redirectStretchRect(pSourceSurface, &source, pDestSurface, NULL, Filter);
	}
	return GenericPlugin::redirectStretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT FF13Plugin::redirectScissorRect(CONST RECT* pRect) {
	// adjust
	// [   0/ 165/1279/ 556]
	// [ 501/ 199/1812/ 562]
	if(pRect != NULL) {
		RECT newRect = *pRect;
		newRect.left = newRect.left * Settings::get().getRenderWidth() / 1280;
		newRect.right = newRect.right * Settings::get().getRenderWidth() / 1280;
		newRect.top = newRect.top * Settings::get().getRenderHeight() / 720;
		newRect.bottom = newRect.bottom * Settings::get().getRenderHeight() / 720;
		SDLOG(4, " -> [FF13] set scissor rect to %s\n", RectToString(&newRect));
		return GenericPlugin::redirectScissorRect(&newRect);
	}
	return GenericPlugin::redirectScissorRect(pRect);
}
//
//SetVertexShaderConstantF: start : 0, count : 4
//	- -1.7840218544     0.0000000000     0.0000000000     0.0000000000
//	- 0.0000000000     3.1715941429     0.0000000000     0.0000000000
//	- 0.6531249881     0.6111110449     1.0000151396     0.9999999404
//	- 745.7210693359   697.7506713867  1140.7912597656  1141.7738037109
//SetVertexShaderConstantF: start: 0, count: 4
// -     1.7840219736     0.0000000000     0.0000000000     0.0000000000
// -     0.0000000000    -3.1715941429     0.0000000000     0.0000000000
// -     0.0000000000     0.0000000000     1.0000151396     0.9999998808
// - -1142.6660156250  1143.3597412109  1140.7913818359  1141.7739257813
//SetVertexShaderConstantF: start: 0, count: 4
// -     1.7840219736     0.0000000000     0.0000000000     0.0000000000
// -     0.0000000000    -3.1715941429     0.0000000000     0.0000000000
// -     0.0000000000     0.0000000000     1.0000151396     0.9999998808
// - -1142.6660156250  1143.3597412109  1140.7913818359  1141.7739257813
