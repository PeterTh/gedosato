
#include "plugins/dark_souls_2.h"

#include "renderstate_manager_dx9.h"
#include "utils/d3d9_utils.h"

DS2Plugin::~DS2Plugin() {
	SAFERELEASE(defaultState);
	SAFERELEASE(zBufferSurf);
	SAFERELEASE(normalSurf);
	SAFERELEASE(hdrRT);
	SAFEDELETE(dof);
	SAFEDELETE(fxaa);
	SAFEDELETE(smaa);
	SAFEDELETE(ssao);
	SAFEDELETE(post);
	SAFEDELETE(bloom);
}

void DS2Plugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	unsigned drw = rw, drh = rh;
	if(rh > 1 && rw > 1 && rh * 16 > rw * 9) { // 16:10 or 4:3
		drh = rw * 9 / 16;
		SDLOG(2, "Resource init: adjusting for non-16:9 resolution, screen: %ux%u, render: %ux%u\n", rw, rh, drw, drh);
	}
	RSManager::getRTMan().setOverrideFormat(RenderTarget::FMT_ARGB_FP16);
	if(Settings::get().getEnableDoF()) dof = new DOF(d3ddev, drw, drh, (Settings::get().getDOFType() == "bokeh") ? DOF::BOKEH : DOF::BASIC, Settings::get().getDOFBaseRadius(), false);
	if(Settings::get().getAAQuality() > 0) {
		if(Settings::get().getAAType() == "smaa") smaa = new SMAA(d3ddev, drw, drh, (SMAA::Preset)(Settings::get().getAAQuality() - 1));
		else fxaa = new FXAA(d3ddev, drw, drh, (FXAA::Quality)(Settings::get().getAAQuality() - 1));
	}
	auto ssaoBlurType = (Settings::get().getSsaoBlurType() == "sharp") ? SSAO::BLUR_SHARP : SSAO::BLUR_GAUSSIAN;
	if(Settings::get().getSsaoStrength() > 0) ssao = new SSAO(d3ddev, drw, drh, drw, drh, Settings::get().getSsaoStrength() - 1, SSAO::VSSAO2, ssaoBlurType, true, false);
	if(Settings::get().getEnablePostprocessing()) post = new Post(d3ddev, drw, drh);
	if(Settings::get().getEnableBloom()) bloom = new Bloom(d3ddev, drw, drh, 0.9f, 1.0f, 0.5f);
}

void DS2Plugin::reloadShaders() {
	if(ssao) ssao->reloadShader();
	if(post) post->reloadShader();
	if(bloom) bloom->reloadShader();
}


void DS2Plugin::reportStatus() {
	if(doAA && (smaa || fxaa)) {
		if(smaa) Console::get().add(format("SMAA enabled, quality level %d", Settings::get().getAAQuality()));
		else Console::get().add(format("FXAA enabled, quality level %d", Settings::get().getAAQuality()));
	}
	else Console::get().add("AA disabled");
	if(post && doPost) Console::get().add(format("Postprocessing enabled, type %s", Settings::get().getPostProcessingType().c_str()));
	else Console::get().add("Postprocessing disabled");
	if(dof && doDof) Console::get().add(format("DoF enabled, type %s, base blur size %f", Settings::get().getDOFType().c_str(), Settings::get().getDOFBaseRadius()));
	else Console::get().add("DoF disabled");
	if(ssao && doAO) Console::get().add(format("SSAO enabled, strength %d, scale %d, blur %s", Settings::get().getSsaoStrength(), Settings::get().getSsaoScale(), Settings::get().getSsaoBlurType().c_str()));
	else Console::get().add("SSAO disabled");
	if(bloom && doBloom) Console::get().add("Bloom enabled");
	else Console::get().add("Bloom disabled");
}

void DS2Plugin::prePresent() {
	SAFERELEASE(zBufferSurf);
	SAFERELEASE(normalSurf);
	SAFERELEASE(hdrRT);
	aaStepStarted = false;
	aoDone = false;
}

HRESULT DS2Plugin::redirectSetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) {
	// At this point, we can grab the z (RT1) and normal (RT0) RTs
	if(RenderTargetIndex == 1 && pRenderTarget == NULL && zBufferSurf == NULL) {
		SAFERELEASE(zBufferSurf);
		SAFERELEASE(normalSurf);
		d3ddev->GetRenderTarget(1, &zBufferSurf);
		d3ddev->GetRenderTarget(0, &normalSurf);
	}
	// If previous RT was D3DFMT_A16B16G16R16F, store RT
	else if(((doAO && ssao) || (doBloom && bloom)) && !aoDone && !hdrRT && RenderTargetIndex == 0 && zBufferSurf != NULL && defaultState != NULL) {
		IDirect3DSurface9* prevRT = NULL;
		d3ddev->GetRenderTarget(0, &prevRT);
		if(prevRT) {
			D3DSURFACE_DESC desc;
			prevRT->GetDesc(&desc);
			if(desc.Format == D3DFMT_A16B16G16R16F && (desc.Width == manager.getRenderWidth() || desc.Height == manager.getRenderHeight()) && !aoDone) {
				// store RT for later use
				hdrRT = prevRT;
			}
			else {
				SAFERELEASE(prevRT);
			}
		}
	}
	return GamePlugin::redirectSetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT DS2Plugin::redirectSetPixelShader(IDirect3DPixelShader9* pShader) {
	if(manager.getShaderManager().isDS2AAShader(pShader)) {
		aaStepStarted = true;
		SDLOG(8, "DS2: set aaStepStarted")
	}
	return GamePlugin::redirectSetPixelShader(pShader);
}

HRESULT DS2Plugin::redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
	// perform postprocessing and AA instead of the original DS2 FXAA shader pass
	if(aaStepStarted && (((smaa || fxaa) && doAA) || (post && doPost) || (dof && doDof) || (bloom && doBloom))) {
		manager.storeRenderState();
		// Perform post-processing
		SDLOG(2, "Starting DS2 post-processing.")
		IDirect3DSurface9 *rt = NULL, *framesurf = NULL;
		IDirect3DBaseTexture9 *frame = NULL;
		IDirect3DTexture9 *depth = NULL, *frametex = NULL;
		d3ddev->GetTexture(0, &frame); // texture 0 is the frame texture
		frametex = (IDirect3DTexture9*)frame;
		if(frame) frametex->GetSurfaceLevel(0, &framesurf);
		depth = D3DGetSurfTexture(zBufferSurf); // the depth buffer, stored previously
		d3ddev->GetRenderTarget(0, &rt); // rt 0 is the FXAA target of the game
		if(frame && depth && rt) {
			if(doAA && (smaa || fxaa)) {
				if(smaa) smaa->go(frametex, frametex, rt, SMAA::INPUT_COLOR);
				else fxaa->go(frametex, rt);
				d3ddev->StretchRect(rt, NULL, framesurf, NULL, D3DTEXF_NONE);
			}
			if(post && doPost) {
				post->go(frametex, rt);
				d3ddev->StretchRect(rt, NULL, framesurf, NULL, D3DTEXF_NONE);
			}
			if(dof && doDof) {
				dof->go(frametex, depth, rt);
				d3ddev->StretchRect(rt, NULL, framesurf, NULL, D3DTEXF_NONE);
			}
			if(bloom && doBloom && hdrRT) {
				bloom->go(D3DGetSurfTexture(hdrRT), frametex, rt);
			}
			if(defaultState == NULL) d3ddev->CreateStateBlock(D3DSBT_ALL, &defaultState);
		}
		else {
			SDLOG(0, "ERROR performing frame processing: could not find required surfaces/textures");
		}
		SAFERELEASE(rt);
		SAFERELEASE(framesurf);
		SAFERELEASE(frame);

		if(manager.takingScreenshot(RSManager::SCREENSHOT_HUDLESS)) {
			manager.captureRTScreen("hudless");
			manager.tookScreenshot(RSManager::SCREENSHOT_HUDLESS);
		}

		manager.restoreRenderState();
		aaStepStarted = false;
		SDLOG(2, "DS2 post-processing complete.");
		// skip original FXAA
		return D3D_OK;
	}

	return  GamePlugin::redirectDrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT DS2Plugin::redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
	if(State == D3DRS_ALPHABLENDENABLE && Value == TRUE && (doAO && ssao) && hdrRT && !aoDone) {
		// now perform AO
		SDLOG(2, "Starting DS2 AO rendering.\n");
		manager.storeRenderState();
		defaultState->Apply();
		IDirect3DTexture9 *depth = NULL, *frameTex = NULL;
		depth = D3DGetSurfTexture(zBufferSurf); // the depth buffer, stored previously
		frameTex = D3DGetSurfTexture(hdrRT);
		if(depth && frameTex) {
			SDLOG(2, "- AO ready to go\n")
			ssao->goHDR(frameTex, depth, hdrRT);
			SDLOG(2, "- AO done\n")
		}
		else {
			SDLOG(0, "ERROR performing AO processing: could not find required surfaces/textures");
		}
		manager.restoreRenderState();
		aoDone = true;
		SDLOG(2, "DS2 AO rendering complete.\n");
	}

	return GamePlugin::redirectSetRenderState(State, Value);
}
