#include "plugins/lrff13.h"

#include "renderstate_manager_dx9.h"
#include "d3d9/d3d9tex.h"
#include "utils/d3d9_utils.h"

#include "utils/win_utils.h"

void LightningReturnsPlugin::reportStatus() {
	PluginBase::reportStatus();
	Console::get().add("!! Lightning Returns Plugin: fixing DoF aliasing");
}

void LightningReturnsPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	PluginBase::initialize(rw, rh, bbformat, dssformat);
	//dofAA = std::make_unique<FXAA>(d3ddev, 1280, 720, FXAA::QualityUltra, false);
	//temp = RSManager::getRTMan().createTexture(3840, 2160);
	//scaler = std::make_unique<Scaler>(d3ddev, 3840, 2160, 2560, 1440, true);
	//scaler->setScaling(Scaler::BICUBIC);
}

void LightningReturnsPlugin::prePresent() {
	PluginBase::prePresent();
	//isDofGeneration = false;
	//finishedDofGeneration = false;
	//SDLOG(-1, "LRFF13: prePresent\n");
}

HRESULT LightningReturnsPlugin::redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	if((Usage & D3DUSAGE_RENDERTARGET || Format == D3DFMT_D24S8) && Width == 1280 && Height == 720) {
		SDLOG(-1, "FF13LR: CreateTexture override\n");
		Width *= 2;
		Height *= 2;
	}
	return PluginBase::redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT LightningReturnsPlugin::redirectSetViewport(CONST D3DVIEWPORT9 * pViewport) {
	if(pViewport->Width == 1280 && pViewport->Height == 720) {
		SDLOG(-1, "FF13LR: SetViewport override");
		D3DVIEWPORT9 newView = *pViewport;
		newView.Width *= 2;
		newView.Height *= 2;
		return GenericPlugin::redirectSetViewport(&newView);
	}
	return PluginBase::redirectSetViewport(pViewport);
}

HRESULT LightningReturnsPlugin::redirectSetPixelShader(IDirect3DPixelShader9* pShader) {
	//if(manager.getShaderManager().isLRFF13DOFShader(pShader)) {
	//	SDLOG(2, "LRFF13: shader hit!\n");
	//	// anti-alias current texture, which should be RT and 1280x720
	//	IDirect3DBaseTexture9 *dof = nullptr;
	//	d3ddev->GetTexture(0, &dof); // texture 0 is the dof texture
	//	IDirect3DSurface9 *rt = nullptr;
	//	d3ddev->GetRenderTarget(0, &rt);
	//	if(dof) {
	//		IDirect3DTexture9 *doftex = (IDirect3DTexture9*)dof;
	//		LOG_CHECK(3, {
	//			D3DSURFACE_DESC desc;
	//			doftex->GetLevelDesc(0, &desc);
	//			SDLOG(2, "LRFF13: dof surface: %s\n", D3DSurfaceDescToString(desc));
	//		});
	//		// get rt surface
	//		IDirect3DSurface9 *dofsurf = nullptr;
	//		doftex->GetSurfaceLevel(0, &dofsurf);

	//		dofAA->go(doftex, temp->getSurf());
	//		d3ddev->StretchRect(temp->getSurf(), nullptr, rt, nullptr, D3DTEXF_NONE);	
	//		//dofAA->go(doftex, rt);

	//		//bloom->blurPass(doftex, rt, 1280, 720, false);
	//		//d3ddev->StretchRect(rt, nullptr, dofsurf, nullptr, D3DTEXF_NONE);
	//		//bloom->blurPass(doftex, rt, 1280, 720, true);
	//		//d3ddev->StretchRect(rt, nullptr, dofsurf, nullptr, D3DTEXF_NONE);

	//		SAFERELEASE(dofsurf);
	//	}
	//	SAFERELEASE(dof);
	//	isBloom = true;
	//} else {
	//	isBloom = false;
	//}
	//if(!finishedDofGeneration && manager.getShaderManager().isLRFF13DownsampleShader(pShader)) {
	//	isDofGeneration = true;
	//}
	return PluginBase::redirectSetPixelShader(pShader);
}

HRESULT LightningReturnsPlugin::redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
	//if(isBloom) return D3D_OK;
	//if(isDofGeneration && !finishedDofGeneration) {
	//	// anti-alias current texture, which should be RT and 3840x2160
	//	IDirect3DBaseTexture9 *currentFrame = nullptr;
	//	d3ddev->GetTexture(0, &currentFrame); // texture 0 is the current Frame texture
	//	IDirect3DSurface9 *rt = nullptr;
	//	d3ddev->GetRenderTarget(0, &rt);
	//	if(currentFrame) {
	//		IDirect3DTexture9 *currentFrameTex = (IDirect3DTexture9*)currentFrame;
	//		LOG_CHECK(-1, {
	//			D3DSURFACE_DESC desc;
	//			currentFrameTex->GetLevelDesc(0, &desc);
	//			SDLOG(-1, "LRFF13: currentFrame surface: %s\n", D3DSurfaceDescToString(desc));
	//		});
	//		LOG_CHECK(-1, {
	//			D3DSURFACE_DESC desc;
	//			rt->GetDesc(&desc);
	//			SDLOG(-1, "LRFF13: rt surface: %s\n", D3DSurfaceDescToString(desc));
	//		});

	//		dofAA->go(currentFrameTex, temp->getSurf());
	//		//d3ddev->StretchRect(temp->getSurf(), nullptr, rt, nullptr, D3DTEXF_NONE);
	//		scaler->go(temp->getTex(), rt);

	//		//bloom->blurPass(doftex, rt, 1280, 720, false);
	//		//d3ddev->StretchRect(rt, nullptr, dofsurf, nullptr, D3DTEXF_NONE);
	//		//bloom->blurPass(doftex, rt, 1280, 720, true);
	//		//d3ddev->StretchRect(rt, nullptr, dofsurf, nullptr, D3DTEXF_NONE);

	//	}
	//	SAFERELEASE(currentFrame);
	//	SAFERELEASE(rt);

	//	finishedDofGeneration = true;
	//	return D3D_OK;
	//}
	return PluginBase::redirectDrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT LightningReturnsPlugin::redirectStretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) {
	RECT overrideRect = {0,0,1280,720};
	if(pSourceRect && *pSourceRect == overrideRect) {
		overrideRect.right *= 2;
		overrideRect.bottom *= 2;
		return PluginBase::redirectStretchRect(pSourceSurface, &overrideRect, pDestSurface, pDestRect, Filter);
	}
	return PluginBase::redirectStretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

void LightningReturnsPlugin::preDownsample(IDirect3DSurface9* backBuffer) {
	skipAO = true;
	PluginBase::preDownsample(backBuffer);
	skipAO = false;
}
