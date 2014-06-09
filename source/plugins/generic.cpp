
#include "plugins/generic.h"

#include "d3dutil.h"

GenericPlugin::~GenericPlugin() {
	SAFERELEASE(tmpSurf);
	SAFERELEASE(tmpTex);
	SAFEDELETE(fxaa);
	SAFEDELETE(smaa);
	SAFEDELETE(post);
}

void GenericPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat) {
	unsigned drw = rw, drh = rh;
	if(Settings::get().getAAQuality() > 0) {
		if(Settings::get().getAAType() == "smaa") smaa = new SMAA(d3ddev, drw, drh, (SMAA::Preset)(Settings::get().getAAQuality() - 1), false);
		else fxaa = new FXAA(d3ddev, drw, drh, (FXAA::Quality)(Settings::get().getAAQuality() - 1), false);
	}
	if(Settings::get().getEnablePostprocessing()) post = new Post(d3ddev, drw, drh, false);

	d3ddev->CreateTexture(rw, rh, 1, D3DUSAGE_RENDERTARGET, (bbformat == D3DFMT_UNKNOWN) ? D3DFMT_A8R8G8B8 : bbformat, D3DPOOL_DEFAULT, &tmpTex, NULL);
	tmpTex->GetSurfaceLevel(0, &tmpSurf);
}

void GenericPlugin::reportStatus() {
	if(doAA && (smaa || fxaa)) {
		if(smaa) Console::get().add(format("SMAA enabled, quality level %d", Settings::get().getAAQuality()));
		else Console::get().add(format("FXAA enabled, quality level %d", Settings::get().getAAQuality()));
	}
	else Console::get().add("AA disabled");
	if(post && doPost) Console::get().add(format("Postprocessing enabled, type %s", Settings::get().getPostProcessingType().c_str()));
	else Console::get().add("Postprocessing disabled");
}

void GenericPlugin::preDownsample(IDirect3DSurface9* backBuffer) {
	SDLOG(8, "Generic plugin preDownsample start\n");
	if(doAA || doPost) {
		d3ddev->StretchRect(backBuffer, NULL, tmpSurf, NULL, D3DTEXF_NONE);
		bool didAA = false;
		if(doAA && (fxaa || smaa)) {
			didAA = true;
			if(fxaa) {
				fxaa->go(tmpTex, backBuffer);
			}
			else if(smaa) {
				smaa->go(tmpTex, tmpTex, backBuffer, SMAA::INPUT_COLOR);
			}
		}
		if(doPost && post) {
			if(didAA) d3ddev->StretchRect(backBuffer, NULL, tmpSurf, NULL, D3DTEXF_NONE);
			post->go(tmpTex, backBuffer);
		}
	}
	SDLOG(8, "Generic plugin preDownsample end\n");
}
