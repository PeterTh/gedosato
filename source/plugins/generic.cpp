
#include "plugins/generic.h"

#include "d3dutil.h"
#include "renderstate_manager.h"
#include "blacklist.h"

GenericPlugin::~GenericPlugin() {
	SAFEDELETE(fxaa);
	SAFEDELETE(smaa);
	SAFEDELETE(post);
	SAFEDELETE(ssao);
	SAFEDELETE(depthTexture);
	SAFERELEASE(depthStencilTarget);
}

void GenericPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat) {
	drw = rw, drh = rh;
	if(Settings::get().getAAQuality() > 0) {
		if(Settings::get().getAAType() == "smaa") smaa = new SMAA(d3ddev, drw, drh, (SMAA::Preset)(Settings::get().getAAQuality() - 1), false);
		else fxaa = new FXAA(d3ddev, drw, drh, (FXAA::Quality)(Settings::get().getAAQuality() - 1), false);
	}
	if(Settings::get().getEnablePostprocessing()) post = new Post(d3ddev, drw, drh, false);

	if (Settings::get().getSsaoStrength() > 0) {
		ssao = new SSAO(d3ddev, drw, drh, Settings::get().getSsaoStrength() - 1, SSAO::VSSAO2, (Settings::get().getSsaoBlurType() == "sharp") ? SSAO::BLUR_SHARP : SSAO::BLUR_GAUSSIAN, false, true);
		depthTexture = new DepthTexture(RSManager::get().getd3d());
		if (depthTexture->isSupported()) {
			depthTexture->createTexture(d3ddev, drw, drh);
			HRESULT hr = d3ddev->SetDepthStencilSurface(depthTexture->getSurface()); // somes games never call 'SetDepthStencilSurface()'
		}
	}

	tmp = RSManager::getRTMan().createTexture(rw, rh, (bbformat == D3DFMT_UNKNOWN) ? D3DFMT_A8R8G8B8 : bbformat);

	if(!Settings::get().getInjectRenderstate().empty()) {
		auto str = Settings::get().getInjectRenderstate();
		vector<string> strs;
		boost::split(strs, str, boost::is_any_of(" ,"));
		if(strs.size() != 2) {
			SDLOG(-1, "ERROR: %s is not a valid setting for InjectRenderstate", str.c_str());
		}
		else {
			injectRSType = std::stoul(strs[0]);
			injectRSValue = std::stoul(strs[1]);
		}
	}
}

void GenericPlugin::reportStatus() {
	Console::get().add(format("Running on executable %s (%s)", getExeFileName().c_str(), getListedName().c_str()));
	if(doAA && (smaa || fxaa)) {
		if(smaa) Console::get().add(format("SMAA enabled, quality level %d", Settings::get().getAAQuality()));
		else Console::get().add(format("FXAA enabled, quality level %d", Settings::get().getAAQuality()));
	}
	else Console::get().add("AA disabled");
	if(post && doPost) Console::get().add(format("Postprocessing enabled, type %s", Settings::get().getPostProcessingType().c_str()));
	else Console::get().add("Postprocessing disabled");
	if(ssao && doAO) Console::get().add(format("SSAO enabled, strength %d, scale %d, blur %s", Settings::get().getSsaoStrength(), Settings::get().getSsaoScale(), Settings::get().getSsaoBlurType().c_str()));
	else Console::get().add("SSAO disabled");
}

void GenericPlugin::process(IDirect3DSurface9* backBuffer) {
	if(!postDone) {
		postDone = true;
		processedBB = backBuffer;
		SDLOG(8, "Generic plugin processing start\n");
		if(doAA || doPost || doAO) {
			d3ddev->StretchRect(backBuffer, NULL, tmp->getSurf(), NULL, D3DTEXF_NONE);
			bool didAO = false;
			if(doAO && ssao) {
				if(depthTexture->isSupported()) {
					didAO = true;
					//depthTexture->resolveDepth(d3ddev); // only necessary when multisampling is enabled (doh)
					ssao->go(tmp->getTex(), depthTexture->getTexture(), backBuffer);
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

void GenericPlugin::processCurrentBB() {
	SDLOG(8, "Generic plugin processCurrentBB\n");
	IDirect3DSurface9* bb = NULL;
	d3ddev->GetRenderTarget(0, &bb);
	if(bb) {
		RSManager::get().storeRenderState();
		process(bb);
		RSManager::get().restoreRenderState();
		if(manager.getTakeScreenshot() == RSManager::SCREENSHOT_HUDLESS) {
			manager.captureRTScreen("hudless");
			manager.tookScreenshot();
		}
	}
	SAFERELEASE(bb);
}


void GenericPlugin::preDownsample(IDirect3DSurface9* backBuffer) {
	// move rendered content to BB if HUD hidden and it was rendered to different RT
	if(postDone && !hudEnabled && processedBB != NULL && backBuffer != processedBB) {
		d3ddev->StretchRect(processedBB, NULL, backBuffer, NULL, D3DTEXF_NONE);
	}
	processedBB = NULL;
	process(backBuffer);
}

void GenericPlugin::prePresent() {
	postDone = false;
	postReady = false;
}

void GenericPlugin::performInjection() {
	if(Settings::get().getInjectDelayAfterDraw()) {
		SDLOG(8, "Generic plugin: found injection point, readying postprocessing\n");
		postReady = true;
	}
	else {
		SDLOG(8, "Generic plugin: found injection point, performing postprocessing\n");
		processCurrentBB();
	}
}

HRESULT GenericPlugin::redirectSetPixelShader(IDirect3DPixelShader9* pShader) {
	if(!postDone && manager.getShaderManager().getName(pShader) == Settings::get().getInjectPSHash()) {
		performInjection();
	}
	return GamePlugin::redirectSetPixelShader(pShader);
}

HRESULT GenericPlugin::redirectClear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
	if((doAO && ssao)
    && Flags == Settings::get().getZbufCompatibilityFlag()) {
		SDLOG(8, "Generic plugin: [depth access] removing the D3DCLEAR_ZBUFFER flag from Flag %d\n", Flags);
		Flags = Flags & (~D3DCLEAR_ZBUFFER);
	}
	return GamePlugin::redirectClear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT GenericPlugin::redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	HRESULT hr = NULL;
	hr = GamePlugin::redirectCreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);

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

HRESULT GenericPlugin::redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
	HRESULT hr = NULL;
	if(doAO && ssao) {
		if(pNewZStencil) {
			if(pNewZStencil == depthTexture->getSurface() || pNewZStencil == depthStencilTarget) {
				// Either the game already sets the hooked DS for us OR the current DS is exactly the one we determined as 'replaceable' (see 'redirectCreateDepthStencilSurface')
				SDLOG(8, "Generic plugin: [depth access] substituting original DS surface (found in CreateDepthStencilSurface) with our own.  depthTexture pointer: %p\n", depthTexture);
				hr = GamePlugin::redirectSetDepthStencilSurface(depthTexture->getSurface());
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
					hr = GamePlugin::redirectSetDepthStencilSurface(depthTexture->getSurface());
				}
				else {
					SDLOG(8, "Generic plugin: [depth access] Do nothing to the original DS surface (%4dx%4d) : not the same size. descStencilHook (%4dx%4d) depthTexture pointer: %p\n", descStencilOrigin.Width, descStencilOrigin.Height, descStencilHook.Width, descStencilHook.Height, depthTexture);
					hr = GamePlugin::redirectSetDepthStencilSurface(pNewZStencil);
				}
			}
			return hr;
		}
	}

	return GamePlugin::redirectSetDepthStencilSurface(pNewZStencil);
}

HRESULT GenericPlugin::drawingStep(std::function<HRESULT(void)> drawCall) {
	if(!hudEnabled && postDone) return D3D_OK;
	HRESULT ret = drawCall();
	if(!postDone && postReady) {
		SDLOG(8, "Generic plugin: found shader, waited until draw, performing postprocessing\n");
		processCurrentBB();
	}
	return ret;
}

HRESULT GenericPlugin::redirectDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
	return drawingStep([&](){ return GamePlugin::redirectDrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount); });
}

HRESULT GenericPlugin::redirectDrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
	return drawingStep([&](){ return GamePlugin::redirectDrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount); });
}

HRESULT GenericPlugin::redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
	return drawingStep([&](){ return GamePlugin::redirectDrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride); });
}

HRESULT GenericPlugin::redirectDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, CONST void * pIndexData, D3DFORMAT IndexDataFormat, CONST void * pVertexStreamZeroData, UINT VertexStreamZeroStride) {
	return drawingStep([&](){ return GamePlugin::redirectDrawIndexedPrimitiveUP(PrimitiveType, MinIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride); });
}


HRESULT GenericPlugin::redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
	if(!postDone && State == injectRSType && Value == injectRSValue) {
		performInjection();
	}
	return GamePlugin::redirectSetRenderState(State, Value);
}

void GenericPlugin::toggleHUD() {
	if(injectRSType == 0 && Settings::get().getInjectPSHash().empty()) {
		Console::get().add("ERROR: HUD toggling requires some form of injection targeting.");
	}
	else {
		hudEnabled = !hudEnabled;
		Console::get().add(string("HUD rendering ") + (hudEnabled ? "enabled" : "disabled"));
	}
}
