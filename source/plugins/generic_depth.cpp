#include "plugins/generic_depth.h"

#include "utils/d3d9_utils.h"
#include "renderstate_manager_dx9.h"

GenericDepthPlugin::~GenericDepthPlugin() {
	SAFEDELETE(ssao);
	SAFERELEASE(gameDepthStencil);
}

void GenericDepthPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	GenericPlugin::initialize(rw, rh, bbformat, dssformat);
	drw = rw, drh = rh;
	dssw = 0; dssh = 0;
	countClear = 0;

	if(Settings::get().getSsaoStrength() > 0) {
		ssao = new SSAO(d3ddev, drw, drh, Settings::get().getSsaoStrength() - 1, 
			(Settings::get().getSsaoType() == "VSSAO2") ? SSAO::VSSAO2 : ((Settings::get().getSsaoType() == "SAO") ? SSAO::SAO: SSAO::MSSAO), 
			(Settings::get().getSsaoBlurType() == "sharp") ? SSAO::BLUR_SHARP : SSAO::BLUR_GAUSSIAN, false, true);
		depthTexture.reset(new DepthTexture(manager.getD3D()));
		if(depthTexture->isSupported()) {
			SDLOG(-1, "[GenericDepthPlugin] depthTexture supported\n");
			depthTexture->createTexture(d3ddev, drw, drh);
			// Backup
			IDirect3DSurface9 *dss = NULL;
			if(SUCCEEDED(d3ddev->GetDepthStencilSurface(&gameDepthStencil))) // This is not needed when downsampling as it was created previously
				SDLOG(-1, "[GenericDepthPlugin] saved original DSS\n")
			else {
				SDLOG(-1, "[GenericDepthPlugin] couldn't save original DSS. Creating a new one\n");
				if(FAILED(d3ddev->CreateDepthStencilSurface(drw, drh, dssformat, D3DMULTISAMPLE_NONE, 0, false, &gameDepthStencil, NULL)))
					SDLOG(-1, "[GenericDepthPlugin] Failed to create DSS - format: %s\n", D3DFormatToString(dssformat))
				else
					SDLOG(-1, "[GenericDepthPlugin] Generated DSS - format: %s\n", D3DFormatToString(dssformat));
			}
			// Now that we saved the game original DSS let's inject our own
			// (some games never call SetDepthStencilSurface later so let's do it here)
			d3ddev->SetDepthStencilSurface(depthTexture->getSurface());
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
		SDLOG(8, "[GenericDepthPlugin] processing start\n");
		if((doAA && (fxaa || smaa)) || (doPost && post) || (doAO && ssao)) {
			manager.setNeutralRenderState();
			d3ddev->StretchRect(backBuffer, NULL, tmp->getSurf(), NULL, D3DTEXF_NONE);
			bool didAO = false;
			if(doAO && ssao) {
				if(depthTexture->isSupported()) {
					didAO = true;
					if(manager.usingMultisampling()) depthTexture->resolveDepth(d3ddev);
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
		SDLOG(8, "[GenericDepthPlugin] processing end\n");
	}
}

HRESULT GenericDepthPlugin::redirectClear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
	if((doAO && ssao) && Flags == Settings::get().getZBufCompatibilityFlag()) {
		countClear++;
		SDLOG(8, "[GenericDepthPlugin] removing the D3DCLEAR_ZBUFFER flag from Flag %d\n - countClear %d", Flags, countClear);
		if(countClear > Settings::get().getZBufClearIndex()) {
			if (Flags == 2) return D3D_OK; else Flags = Flags & (~D3DCLEAR_ZBUFFER);
		}
	}
	return GenericPlugin::redirectClear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT GenericDepthPlugin::redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	if(doAO && ssao) {
		// We have to find and store the most suitable DS surface to replace (in redirectSetDepthStencilSurface)
		// Can be equal to backbuffer dimensions or higher (ie. 2048x2048 in DMC4).
		// Generally the higher the better
		if(Width >= dssw || Height >= dssh) {
			SDLOG(8, "[GenericDepthPlugin] found surface to replace (%4dx%4d), format: %s\n", Width, Height, D3DFormatToString(Format));
			dssw = Width; dssh = Height;
		}
	}
	return GenericPlugin::redirectCreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT GenericDepthPlugin::redirectBeginScene() {
	countClear = 0; // Ok this is bad I know but what else could be done
	return GenericPlugin::redirectBeginScene();
}

HRESULT GenericDepthPlugin::redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
	HRESULT hr = NULL;
	if(doAO && ssao && pNewZStencil) {
		isOrigDssRestored = false;
		D3DSURFACE_DESC descStencilOrigin;
		pNewZStencil->GetDesc(&descStencilOrigin);

		// Ideally we substitute the DS matching the dimensions of our 'hooked' DS
		// TODO: handle cases where the games throws a 1664x936 (16:9) at us (Binary Domain doesnt work right now for that reason)
		if(descStencilOrigin.Width == drw && (descStencilOrigin.Height == drh || descStencilOrigin.Height == drw * 9 / 16)) { // for us 16/10 users
			SDLOG(8, "[GenericDepthPlugin] substituting original DS surface (%4dx%4d) with our own (%4dx%4d).  pointer: %p\n", descStencilOrigin.Width, descStencilOrigin.Height, drw, drh, depthTexture);
			hr = GenericPlugin::redirectSetDepthStencilSurface(depthTexture->getSurface());
			isOrigDssReplaced = true;
		}
		else if(!isOrigDssReplaced && descStencilOrigin.Width == dssw && descStencilOrigin.Height == dssh) {
			SDLOG(8, "[GenericDepthPlugin] substituting original DS surface (found in CreateDepthStencilSurface) with our own.  depthTexture pointer: %p\n", depthTexture);
			hr = GenericPlugin::redirectSetDepthStencilSurface(depthTexture->getSurface());
			isOrigDssReplaced = true;
		}
		else {
			SDLOG(8, "[GenericDepthPlugin] original DS surface (%4dx%4d) doesn't match. descStencilHook (%4dx%4d) depthTexture pointer: %p\n", descStencilOrigin.Width, descStencilOrigin.Height, drw, drh, depthTexture);
			hr = GenericPlugin::redirectSetDepthStencilSurface(pNewZStencil);
		}
	}
	else if(!isOrigDssRestored) {   // Restore the original DSS (needed for GTAIV)
		SDLOG(8, "[GenericDepthPlugin] restoring the game original DSS surface");
		isOrigDssRestored = true;
		isOrigDssReplaced = false;
		hr = GenericPlugin::redirectSetDepthStencilSurface(gameDepthStencil);
	}
	else { // We ain't gonna do nothing
		hr = GenericPlugin::redirectSetDepthStencilSurface(pNewZStencil);
	}

	return hr;
}

void GenericDepthPlugin::reloadShaders() {
	GenericPlugin::reloadShaders();
	if(ssao) ssao->reloadShader();
}
