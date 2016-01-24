#include "plugins/generic_depth.h"

#include "utils/d3d9_utils.h"
#include "renderstate_manager_dx9.h"

GenericDepthPlugin::~GenericDepthPlugin() {
	SAFEDELETE(ssao);
	SAFEDELETE(dof);
	depthTextureAlt.reset(NULL);
}

void GenericDepthPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	GenericPlugin::initialize(rw, rh, bbformat, dssformat);
	drw = rw, drh = rh;
	dssw = 0; dssh = 0;
	texw = 0; texh = 0;
	countClear = 0;
	aspectHeight = drw * 9 / 16;

	// Depth texture created early but we'll refine the dimensions later on if the game happens to use SetDepthStencilSurface
	// which is the case most of the time. Older games however like Max Payne, Mafia, or some dx9 emus do *not* so it is necessary
	// to set it up at least once here.
	depthTexture.reset(new DepthTexture(manager.getD3D()));
	if(depthTexture->isSupported()) {
		SDLOG(-1, "[GenericDepthPlugin] depthTexture supported\n");
		depthTexture->createTexture(d3ddev, drw, drh);

		// Some games never call SetDepthStencilSurface later so let's do it here
		GenericPlugin::redirectSetDepthStencilSurface(depthTexture->getSurface());

		if(Settings::get().getEnableDoF()) {
			SDLOG(6, "[GenericDepthPlugin] DOF dimensions -> Backbuffer width: %d, Backbuffer height: %d, Depthwidth: %d, Depthheight: %d\n", drw, drh, drw, drh);
			dof = new DOF(d3ddev, drw, drh, (Settings::get().getDOFType() == "bokeh") ? DOF::BOKEH : DOF::BASIC, Settings::get().getDOFBaseRadius(), true);
		}

		if(Settings::get().getSsaoStrength() > 0) {
			SDLOG(6, "[GenericDepthPlugin] SSAO dimensions -> Backbuffer width: %d, Backbuffer height: %d, Depthwidth: %d, Depthheight: %d\n", drw, drh, drw, drh);
			ssao = new SSAO(d3ddev, drw, drh, drw, drh, Settings::get().getSsaoStrength() - 1, 
							Settings::get().getSsaoType() == "VSSAO2" ? SSAO::VSSAO2 : 
							Settings::get().getSsaoType() == "SAO" ? SSAO::SAO: Settings::get().getSsaoType() == "HBAO" ? SSAO::HBAO : SSAO::MSSAO,
							Settings::get().getSsaoBlurType() == "sharp" ? SSAO::BLUR_SHARP : SSAO::BLUR_GAUSSIAN, false, true);
		}
	}
}

void GenericDepthPlugin::reportStatus() {
	GenericPlugin::reportStatus();
	if(ssao && doAO) Console::get().add(format("SSAO enabled, strength %d, scale %d, blur %s", Settings::get().getSsaoStrength(), Settings::get().getSsaoScale(), Settings::get().getSsaoBlurType().c_str()));
	else Console::get().add("SSAO disabled");
	if(dof && doDof) Console::get().add(format("DoF enabled, type %s, base blur size %f", Settings::get().getDOFType().c_str(), Settings::get().getDOFBaseRadius()));
	else Console::get().add("DoF disabled");
}

void GenericDepthPlugin::processInternal(IDirect3DSurface9* backBuffer, bool aoOnly) {
	if(!postDone) {
		postDone = true;
		processedBB = backBuffer;
		SDLOG(6, "[GenericDepthPlugin] processing start\n");
		if((doAA && (fxaa || smaa)) || (doAO && ssao) || (doPost && post) || (doDof && dof)) {
			manager.setNeutralRenderState();
			d3ddev->StretchRect(backBuffer, NULL, tmp->getSurf(), NULL, D3DTEXF_NONE);
			bool didAO = false;
			if(!skipAO && !aoDone && doAO && ssao) {
				if(texw == 0) {
					if(depthTexture->isSupported()) {
						didAO = true;
						aoDone = true;
						if(manager.usingMultisampling()) depthTexture->resolveDepth(d3ddev);
						ssao->goHDR(tmp->getTex(), depthTexture->getTexture(), backBuffer);
					}
				}
				else if(depthTextureAlt->isSupported()) {
					didAO = true;
					aoDone = true;
					if(manager.usingMultisampling()) depthTextureAlt->resolveDepth(d3ddev);
					ssao->goHDR(tmp->getTex(), depthTextureAlt->getTexture(), backBuffer);
				}
			}
			if(!aoOnly) {
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
				bool didPost = false;
				if(doPost && post) {
					didPost = true;
					if(didAO || didAA) d3ddev->StretchRect(backBuffer, NULL, tmp->getSurf(), NULL, D3DTEXF_NONE);
					post->go(tmp->getTex(), backBuffer);
				}
				if(doDof && dof) {
					if(didAO || didAA || didPost) d3ddev->StretchRect(backBuffer, NULL, tmp->getSurf(), NULL, D3DTEXF_NONE);
					if(texw == 0) {
						if(depthTexture->isSupported()) {
							if(manager.usingMultisampling()) depthTexture->resolveDepth(d3ddev);
							dof->go(tmp->getTex(), depthTexture->getTexture(), backBuffer);
						}
					}
					else if(depthTextureAlt->isSupported()) {
						if(manager.usingMultisampling()) depthTextureAlt->resolveDepth(d3ddev);
						dof->go(tmp->getTex(), depthTextureAlt->getTexture(), backBuffer);
					}
				}
			}
		}
		SDLOG(6, "[GenericDepthPlugin] processing end\n");
	}
}

// find a better way to do this than repeating code
void GenericDepthPlugin::process(IDirect3DSurface9* backBuffer) {
	processInternal(backBuffer, false);
}

HRESULT GenericDepthPlugin::redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	if(Width >= dssw || Height >= dssh) {
		SDLOG(6, "[GenericDepthPlugin] found surface to replace -in CreateDepthStencilSurface- (%4dx%4d), format: %s\n", Width, Height, D3DFormatToString(Format));
		dssw = Width; dssh = Height;
	}
	return GenericPlugin::redirectCreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT GenericDepthPlugin::redirectBeginScene() {
	countClear = 0;
	aoDone = false;
	return GenericPlugin::redirectBeginScene();
}

HRESULT GenericDepthPlugin::redirectClear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
	if((doAO && ssao) && (Flags == Settings::get().getZBufCompatibilityFlag() / 10 || Flags == Settings::get().getZBufCompatibilityFlag() % 10)) { // 2 flags possible
		countClear++;
		if(countClear > Settings::get().getZBufClearIndex()) {
			SDLOG(6, "[GenericDepthPlugin] removing the D3DCLEAR_ZBUFFER flag from Flag %d - countClear %d\n", Flags, countClear);
			if(Flags == 2) return D3D_OK; 
			else if(Flags == 7) { // may not be pretty but makes RE4 works
				Z = 0;
				D3DRECT newRect = { 0, 0, 2, 2};
				return GenericPlugin::redirectClear(1, &newRect, Flags, Color, Z, Stencil);
			}
			else Flags = Flags & (~D3DCLEAR_ZBUFFER);
		}
	}
	return GenericPlugin::redirectClear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT GenericDepthPlugin::redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
	HRESULT hr = NULL;
	if(doAO && pNewZStencil) {
		D3DSURFACE_DESC dsDesc;
		pNewZStencil->GetDesc(&dsDesc);
		SDLOG(6, "[GenericDepthPlugin] pNewZStencil dimensions -> width: %d, height: %d\n", dsDesc.Width, dsDesc.Height);

		// Depth texture (re)created at a later stage to perfectly match the dimensions of the original game depthstencil which can be 16/9 even though the game is supposed
		// to output to a 16/10 backbuffer. Hence the convoluted code below with aspectHeight and the likes (sorry)...
		// If you're asking : yes the DSS detection algorithm would make more sense in redirectCreateTexture but CreateTexture is often too early in the process
		// for us to decide which DSS is the closest matching one
		if(texw == 0) {
			// We consider DSS surfaces "valid" when they approach our backbuffer size by a 5% tolerance margin (probably the below code could be leaner)
			double widthTolerance = 0.05;
			double augmentedWidth = drw * widthTolerance;
			double augmentedHeight = drh * widthTolerance;
			if( ((dsDesc.Width <= drw && dsDesc.Width > drw - augmentedWidth) && (dsDesc.Height <= drh && dsDesc.Height > drh - augmentedHeight))                                  ||
				((dsDesc.Width >= drw && dsDesc.Width < drw + augmentedWidth) && (dsDesc.Height >= drh && dsDesc.Height < drh + augmentedHeight))                                  ||  
				((dsDesc.Width <= drw && dsDesc.Width > drw - augmentedWidth) && (dsDesc.Height <= aspectHeight && dsDesc.Height > aspectHeight - aspectHeight * widthTolerance))  || // for Binary Domain (1664x936) and some UE3 games
				((dsDesc.Width >= drw && dsDesc.Width < drw + augmentedWidth) && (dsDesc.Height >= aspectHeight && dsDesc.Height < aspectHeight + aspectHeight * widthTolerance)) ) { // that add +6 pixel to th height (ie. 1680x1056)

				SDLOG(6, "[GenericDepthPlugin] Found a good candidate for substitution -> width: %d, height: %d\n", dsDesc.Width, dsDesc.Height);
				texw = dsDesc.Width;
				texh = dsDesc.Height;

				// I have to use another variable here as reusing (reset + createTexture()) the original 'depthTexture' one causes *some* games to crash 
				// with the message ("Run-Time Check Failure #0"...) even if I make sure to detach the depthTexture from the device doing :
				// device->SetDepthStencilSurface(NULL). That had me stumped for many hours
				// TODO: find a way to not use a second variable
				depthTextureAlt.reset(new DepthTexture(manager.getD3D()));
				if(depthTextureAlt->isSupported()) {
					depthTextureAlt->createTexture(d3ddev, texw, texh);
					//depthTextureAlt->createTexture(d3ddev, 6720, 3780);

					if(Settings::get().getEnableDoF()) {
						SAFEDELETE(dof);
						SDLOG(6, "[GenericDepthPlugin] DOF dimensions -> Backbuffer width: %d, Backbuffer height: %d, Depthwidth: %d, Depthheight: %d\n", drw, drh, drw, drh);
						dof = new DOF(d3ddev, texw, texh, (Settings::get().getDOFType() == "bokeh") ? DOF::BOKEH : DOF::BASIC, Settings::get().getDOFBaseRadius(), true);
					}

					if(Settings::get().getSsaoStrength() > 0) {
						SAFEDELETE(ssao);
						SDLOG(6, "[GenericDepthPlugin] SSAO dimensions -> Backbuffer width: %d, Backbuffer height: %d, Depthwidth: %d, Depthheight: %d\n", drw, drh, texw, texh);
						ssao = new SSAO(d3ddev, drw, drh, texw, texh, Settings::get().getSsaoStrength() - 1, 
										Settings::get().getSsaoType() == "VSSAO2" ? SSAO::VSSAO2 : 
										Settings::get().getSsaoType() == "SAO" ? SSAO::SAO: Settings::get().getSsaoType() == "HBAO" ? SSAO::HBAO : SSAO::MSSAO,
										Settings::get().getSsaoBlurType() == "sharp" ? SSAO::BLUR_SHARP : SSAO::BLUR_GAUSSIAN, false, true);
					}
				}
			}
		}

		// Replacing the target DSS with ours. Priority is given to texw/texh as they should faithfully respect the original DSS dimensions. 
		// Some games however *only* create and attach a 2048x2048 DSS (not matching at all the backbuffer size) so we need to accomodate that
		// DSS dimensions found in CreateDepthStencilSurface have the lowest priority as a criterion for substitution (as they can cause issues with rendering)
		// We therefore first check if 'texw' is empty before using 'dssw' as a last resort
		if(texw > 0 ? dsDesc.Width == texw && dsDesc.Height == texh : dsDesc.Width == dssw && dsDesc.Height == dssh) {
			SDLOG(6, "[GenericDepthPlugin] substituting original DS surface (%4dx%4d) with our own (%4dx%4d)\n", dsDesc.Width, dsDesc.Height, texw > 0 ? texw : dssw, texh > 0 ? texh : dssh);
			hr = (texw > 0) ? GenericPlugin::redirectSetDepthStencilSurface(depthTextureAlt->getSurface()) : GenericPlugin::redirectSetDepthStencilSurface(depthTexture->getSurface());
		}
		else {
			SDLOG(6, "[GenericDepthPlugin] original DS surface (%4dx%4d) doesn't match our own texture (%4dx%4d)\n", dsDesc.Width, dsDesc.Height, texw, texh);
			hr = GenericPlugin::redirectSetDepthStencilSurface(pNewZStencil);
		}
	}
	else hr = GenericPlugin::redirectSetDepthStencilSurface(pNewZStencil); // pNewZStencil == NULL

	return hr;
}

HRESULT GenericDepthPlugin::redirectSetPixelShader(IDirect3DPixelShader9* pShader) {
	if(RSManager::getDX9().getShaderManager().isAOInjectionShader(pShader)) {
		SDLOG(8, "Generic depth plugin processing AO only\n");
		IDirect3DSurface9* bb = nullptr;
		d3ddev->GetRenderTarget(0, &bb);
		if(bb) {
			manager.storeRenderState();
			processInternal(bb, true);
			manager.restoreRenderState();
		}
		SAFERELEASE(bb);
	}
	return GenericPlugin::redirectSetPixelShader(pShader);
}

void GenericDepthPlugin::reloadShaders() {
	GenericPlugin::reloadShaders();
	if(ssao) ssao->reloadShader();
	if(dof) dof->reloadShader();
}
