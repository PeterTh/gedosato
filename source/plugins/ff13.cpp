#include "plugins/ff13.h"

#include "renderstate_manager_dx9.h"
#include "utils/d3d9_utils.h"

FF13Plugin::~FF13Plugin() {
	SAFEDELETE(scaler);
}

void FF13Plugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	GenericPlugin::initialize(rw, rh, bbformat, dssformat);
	scaler = new Scaler(d3ddev, Settings::get().getRenderWidth(), Settings::get().getRenderHeight(), Settings::get().getPresentWidth(), Settings::get().getPresentHeight());
	Console::get().add("!! NOTE: This FINAL FANTASY XIII Plugin is currently in pre-alpha and HIGHLY EXPERIMENTAL");
}

void FF13Plugin::prePresent() {
	GenericPlugin::prePresent();
}

HRESULT FF13Plugin::redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	if (D3DFMT_DXT1 && Pool == D3DPOOL_MANAGED) {
		return GenericPlugin::redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
	}
	else if(D3DUSAGE_RENDERTARGET) {
		if((Width == Settings::get().getPresentWidth() && Height == Settings::get().getPresentHeight()) || (Width == 1280 && Height == 720)) {
			Width = Settings::get().getRenderWidth();
			Height = Settings::get().getRenderHeight();
		}
	}
	return GenericPlugin::redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT FF13Plugin::redirectCreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	if((Width == Settings::get().getPresentWidth() && Height == Settings::get().getPresentHeight()) || (Width == 1280 && Height == 720)) {
		Width = Settings::get().getRenderWidth();
		Height = Settings::get().getRenderHeight();
	}
	return GenericPlugin::redirectCreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}


HRESULT FF13Plugin::redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
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
	if(pViewport != NULL && pViewport->Width == 1280 && pViewport->Height == 720) {
		D3DVIEWPORT9 vp = *pViewport;
		vp.Width = Settings::get().getRenderWidth();
		vp.Height = Settings::get().getRenderHeight();
		return GenericPlugin::redirectSetViewport(&vp);
	}
	return GenericPlugin::redirectSetViewport(pViewport);
}

HRESULT FF13Plugin::redirectStretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) {
	if(pSourceRect != NULL && pDestRect != NULL && pSourceRect->right == 1280 && pSourceRect->bottom == 720) {
		//RECT source = { 0, 0, Settings::get().getRenderWidth(), Settings::get().getRenderHeight() };
		manager.storeRenderState();
		scaler->go(D3DGetSurfTexture(pSourceSurface), pDestSurface);
		if(manager.getTakeScreenshot() == RSManager::SCREENSHOT_FULL || manager.getTakeScreenshot() == RSManager::SCREENSHOT_HUDLESS) {
			manager.captureRTScreen("full");
		}
		manager.tookScreenshot();
		manager.restoreRenderState();
		return D3D_OK;
	}
	return GenericPlugin::redirectStretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT FF13Plugin::redirectScissorRect(CONST RECT* pRect) {
	// adjust
	// [   0/ 165/1279/ 556]
	// [ 501/ 199/1812/ 562]
	if(pRect != NULL /*&& pRect->bottom == 556 && pRect->right == 1279*/) {
		RECT newRect = *pRect;
		newRect.left = newRect.left * Settings::get().getRenderWidth() / 1280;
		newRect.right = newRect.right * Settings::get().getRenderWidth() / 1280;
		newRect.top = newRect.top * Settings::get().getRenderHeight() / 720;
		newRect.bottom = newRect.bottom * Settings::get().getRenderHeight() / 720;
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
