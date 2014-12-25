#include "plugins/dw8.h"

#include "renderstate_manager_dx9.h"
#include "d3d9/d3d9tex.h"
#include "utils/d3d9_utils.h"

#include "utils/win_utils.h"

void DW8Plugin::reportStatus() {
	Console::get().add("!! NOTE: This Dynasty Warriors 8 Plugin is currently in pre-alpha and HIGHLY EXPERIMENTAL");
}

void DW8Plugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	GenericPlugin::initialize(rw, rh, bbformat, dssformat);
}

void DW8Plugin::prePresent() {
	GenericPlugin::prePresent();
}

HRESULT DW8Plugin::redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	// other rendertargets and buffers
	if(Pool != D3DPOOL_MANAGED && (Width == 1920 && Height == 1080)) {
		Width = Settings::get().getRenderWidth();
		Height = Settings::get().getRenderHeight();
		SDLOG(4, " -> [DW8] scaling render texture to %dx%d\n", Width, Height);
	}
	return GenericPlugin::redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT DW8Plugin::redirectCreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	if((Width == Settings::get().getPresentWidth() && Height == Settings::get().getPresentHeight()) || (Width == 1920 && Height == 1080)) {
		Width = Settings::get().getRenderWidth();
		Height = Settings::get().getRenderHeight();
		SDLOG(4, " -> [DW8] scaling render target to %dx%d\n", Width, Height);
	}
	return GenericPlugin::redirectCreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}


HRESULT DW8Plugin::redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	if((Width == Settings::get().getPresentWidth() && Height == Settings::get().getPresentHeight()) || (Width == 1920 && Height == 1080)) {
		Width = Settings::get().getRenderWidth();
		Height = Settings::get().getRenderHeight();
		SDLOG(4, " -> [DW8] scaling dss to %dx%d\n", Width, Height);
	}
	return GenericPlugin::redirectCreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT DW8Plugin::redirectSetViewport(CONST D3DVIEWPORT9 * pViewport) {
	// mein rendering
	if(pViewport != NULL && pViewport->Width == 1920 && pViewport->Height == 1080) {
		D3DVIEWPORT9 vp = *pViewport;
		vp.Width = Settings::get().getRenderWidth();
		vp.Height = Settings::get().getRenderHeight();
		SDLOG(4, " -> [DW8] adjusting main VP\n");
		return GenericPlugin::redirectSetViewport(&vp);
	}
	return GenericPlugin::redirectSetViewport(pViewport);
}

HRESULT DW8Plugin::redirectScissorRect(CONST RECT* pRect) {
	// adjust
	// [   0/ 165/1279/ 556]
	// [ 501/ 199/1812/ 562]
	if(pRect != NULL && pRect->right == 1920 && pRect->bottom == 1080) {
		RECT newRect = *pRect;
		newRect.left = 0;
		newRect.right = Settings::get().getRenderWidth();
		newRect.top = 0;
		newRect.bottom = Settings::get().getRenderHeight();
		SDLOG(4, " -> [DW8] set scissor rect to %s\n", RectToString(&newRect));
		return GenericPlugin::redirectScissorRect(&newRect);
	}
	return GenericPlugin::redirectScissorRect(pRect);
}
