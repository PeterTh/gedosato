#include "plugins/generic_depth.h"

#include "d3dutil.h"
#include "renderstate_manager.h"


GenericDepthPlugin::~GenericDepthPlugin() {
	SAFEDELETE(ssao);
	SAFERELEASE(depthStencilTarget);
}

void GenericDepthPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat) {
	GenericPlugin::initialize(rw, rh, bbformat);
	drw = rw, drh = rh;

	if(Settings::get().getSsaoStrength() > 0) {
		ssao = new SSAO(d3ddev, drw, drh, Settings::get().getSsaoStrength() - 1, SSAO::VSSAO2, (Settings::get().getSsaoBlurType() == "sharp") ? SSAO::BLUR_SHARP : SSAO::BLUR_GAUSSIAN, false, true);
		depthTexture.reset(new DepthTexture(RSManager::get().getD3D()));
		if(depthTexture->isSupported()) {
			SDLOG(-1, "[GenericDepthPlugin] depthTexture supported\n");
			depthTexture->createTexture(d3ddev, drw, drh);
			d3ddev->SetDepthStencilSurface(depthTexture->getSurface()); // somes games never call 'SetDepthStencilSurface()'
		}
	}
}

void GenericDepthPlugin::reportStatus() {
	GenericPlugin::reportStatus();
	if(ssao && doAO) Console::get().add(format("SSAO enabled, strength %d, scale %d, blur %s", Settings::get().getSsaoStrength(), Settings::get().getSsaoScale(), Settings::get().getSsaoBlurType().c_str()));
	else Console::get().add("SSAO disabled");
}

// find a better way to do this than repeating code
void GenericDepthPlugin::process(IDirect3DSurface9* backBuffer) {
	if(!postDone) {
		postDone = true;
		processedBB = backBuffer;
		SDLOG(8, "Generic plugin processing start\n");
		if((doAA && (fxaa || smaa)) || (doPost && post) || (doAO && ssao)) {
			RSManager::get().setNeutralRenderState();
			d3ddev->StretchRect(backBuffer, NULL, tmp->getSurf(), NULL, D3DTEXF_NONE);
			bool didAO = false;
			if(doAO && ssao) {
				if(depthTexture->isSupported()) {
					didAO = true;
					if(RSManager::get().usingMultisampling()) depthTexture->resolveDepth(d3ddev); // only necessary when multisampling is enabled (doh)
					ssao->goHDR(tmp->getTex(), depthTexture->getTexture(), backBuffer);
				}
			}
			bool didAA = false;
			if(doAA && (fxaa || smaa)) {
				didAA = true;
				if(didAO) d3ddev->StretchRect(backBuffer, NULL, tmp->getSurf(), NULL, D3DTEXF_NONE);
				if(fxaa) {
					fxaa->go(tmp->getTex(), backBuffer);
				}
				else if(smaa) {
					smaa->go(tmp->getTex(), tmp->getTex(), backBuffer, SMAA::INPUT_COLOR);
				}
			}
			if(doPost && post) {
				if(didAA) d3ddev->StretchRect(backBuffer, NULL, tmp->getSurf(), NULL, D3DTEXF_NONE);
				post->go(tmp->getTex(), backBuffer);
			}
		}
		SDLOG(8, "Generic plugin processing end\n");
	}
}

HRESULT GenericDepthPlugin::redirectClear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
	if((doAO && ssao) && Flags == Settings::get().getZBufCompatibilityFlag()) {
		SDLOG(8, "Generic plugin: [depth access] removing the D3DCLEAR_ZBUFFER flag from Flag %d\n", Flags);
		Flags = Flags & (~D3DCLEAR_ZBUFFER);
	}
	return GenericPlugin::redirectClear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT GenericDepthPlugin::redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	HRESULT hr = NULL;
	hr = GenericPlugin::redirectCreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);

	if(doAO && ssao) {
		// We have to find and store the most suitable DS surface to replace (in redirectSetDepthStencilSurface)
		// Can be equal to backbuffer dimensions or higher (ie. 2048x2048 in DMC4)
		if(Width >= drw || Height >= drh) {
			depthStencilTarget = *ppSurface;
			SDLOG(8, "Generic plugin: [depth access] found surface to replace (%4dx%4d), format: %s\n", Width, Height, D3DFormatToString(Format));
		}
	}
	return hr;
}

HRESULT GenericDepthPlugin::redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
	HRESULT hr = NULL;
	if(doAO && ssao) {
		if(pNewZStencil) {
			if(pNewZStencil == depthTexture->getSurface() || pNewZStencil == depthStencilTarget) {
				// Either the game already sets the hooked DS for us OR the current DS is exactly the one we determined as 'replaceable' (see 'redirectCreateDepthStencilSurface')
				SDLOG(8, "Generic plugin: [depth access] substituting original DS surface (found in CreateDepthStencilSurface) with our own.  depthTexture pointer: %p\n", depthTexture);
				hr = GenericPlugin::redirectSetDepthStencilSurface(depthTexture->getSurface());
			}
			else {
				D3DSURFACE_DESC descStencilOrigin;
				pNewZStencil->GetDesc(&descStencilOrigin);

				D3DSURFACE_DESC descStencilHook;
				depthTexture->getSurface()->GetDesc(&descStencilHook);

				// Ideally we substitute the DS matching the dimensions of our 'hooked' DS
				// TODO: handle cases where the games throws a 1664x936 (16:9) at us (Binary Domain doesnt work right now for that reason)
				if(descStencilOrigin.Width == descStencilHook.Width || descStencilOrigin.Height == descStencilHook.Height) {
					SDLOG(8, "Generic plugin: [depth access] substituting original DS surface (%4dx%4d) with our own.  pointer: %p\n", descStencilOrigin.Width, descStencilOrigin.Height, depthTexture);
					hr = GenericPlugin::redirectSetDepthStencilSurface(depthTexture->getSurface());
				}
				else {
					SDLOG(8, "Generic plugin: [depth access] Do nothing to the original DS surface (%4dx%4d) : not the same size. descStencilHook (%4dx%4d) depthTexture pointer: %p\n", descStencilOrigin.Width, descStencilOrigin.Height, descStencilHook.Width, descStencilHook.Height, depthTexture);
					hr = GenericPlugin::redirectSetDepthStencilSurface(pNewZStencil);
				}
			}
			return hr;
		}
	}

	return GenericPlugin::redirectSetDepthStencilSurface(pNewZStencil);
}

void GenericDepthPlugin::reloadShaders() {
	GenericPlugin::reloadShaders();
	if(ssao) ssao->reloadShader();
}
