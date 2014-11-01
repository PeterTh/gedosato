
#include "plugins/generic.h"

#include "utils/d3d9_utils.h"
#include "renderstate_manager_dx9.h"
#include "blacklist.h"

GenericPlugin::~GenericPlugin() {
	SAFEDELETE(fxaa);
	SAFEDELETE(smaa);
	SAFEDELETE(post);
}

void GenericPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	unsigned drw = rw, drh = rh;
	if(Settings::get().getpostProcessAfterScaling()) {
		drw = Settings::get().getPresentWidth();
		drh = Settings::get().getPresentHeight();
	}
	if(Settings::get().getAAQuality() > 0) {
		if(Settings::get().getAAType() == "smaa") smaa = new SMAA(d3ddev, drw, drh, (SMAA::Preset)(Settings::get().getAAQuality() - 1), false);
		else fxaa = new FXAA(d3ddev, drw, drh, (FXAA::Quality)(Settings::get().getAAQuality() - 1), false);
	}
	if(Settings::get().getEnablePostprocessing()) post = new Post(d3ddev, drw, drh, false);

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
}

void GenericPlugin::process(IDirect3DSurface9* backBuffer) {
	if(!postDone) {
		postDone = true;
		processedBB = backBuffer;
		SDLOG(8, "Generic plugin processing start\n");
		if(doAA || doPost) {
			d3ddev->StretchRect(backBuffer, NULL, tmp->getSurf(), NULL, D3DTEXF_NONE);
			bool didAA = false;
			if(doAA && (fxaa || smaa)) {
				didAA = true;
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
		manager.storeRenderState();
		process(bb);
		if(manager.getTakeScreenshot() == RSManager::SCREENSHOT_HUDLESS) {
			manager.captureRTScreen("hudless", bb);
			manager.tookScreenshot();
		}
		manager.restoreRenderState();
	}
	SAFERELEASE(bb);
}


void GenericPlugin::preDownsample(IDirect3DSurface9* backBuffer) {
	// move rendered content to BB if HUD hidden and it was rendered to different RT
	if(postDone && !hudEnabled && processedBB != NULL && backBuffer != processedBB) {
		d3ddev->StretchRect(processedBB, NULL, backBuffer, NULL, D3DTEXF_NONE);
	}
	if(!Settings::get().getpostProcessAfterScaling()) {
		process(backBuffer);
	}
	processedBB = NULL;
}

void GenericPlugin::postDownsample(IDirect3DSurface9* backBuffer) {
	if(Settings::get().getpostProcessAfterScaling()) {
		process(backBuffer);
	}
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
HRESULT GenericPlugin::redirectSetVertexShader(IDirect3DVertexShader9* pShader) {
	if(!postDone && manager.getShaderManager().getName(pShader) == Settings::get().getInjectVSHash()) {
		performInjection();
	}
	return GamePlugin::redirectSetVertexShader(pShader);
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
	if(injectRSType == 0 && Settings::get().getInjectPSHash().empty() && Settings::get().getInjectVSHash().empty()) {
		Console::get().add("ERROR: HUD toggling requires some form of injection targeting.");
	}
	else {
		hudEnabled = !hudEnabled;
		Console::get().add(string("HUD rendering ") + (hudEnabled ? "enabled" : "disabled"));
	}
}

void GenericPlugin::reloadShaders() {
	if(post) post->reloadShader();
}
