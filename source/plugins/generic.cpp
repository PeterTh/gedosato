
#include "plugins/generic.h"

#include "d3dutil.h"

GenericPlugin::~GenericPlugin() {
	SAFERELEASE(tmpSurf);
	SAFERELEASE(tmpTex);
	SAFEDELETE(fxaa);
	SAFEDELETE(smaa);
	SAFEDELETE(post);
}

void GenericPlugin::initialize(unsigned rw, unsigned rh) {
	unsigned drw = rw, drh = rh;
	if(Settings::get().getAAQuality() > 0) {
		if(Settings::get().getAAType() == "smaa") smaa = new SMAA(d3ddev, drw, drh, (SMAA::Preset)(Settings::get().getAAQuality() - 1), false);
		else fxaa = new FXAA(d3ddev, drw, drh, (FXAA::Quality)(Settings::get().getAAQuality() - 1), false);
	}
	if(Settings::get().getEnablePostprocessing()) post = new Post(d3ddev, drw, drh, false);

	d3ddev->CreateTexture(rw, rh, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &tmpTex, NULL);
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
		IDirect3DTexture9 *tex = D3DGetSurfTexture(backBuffer);
		if(doAA) {
			if(fxaa) {
				fxaa->go(tex, backBuffer);
			}
			else if(smaa) {
				smaa->go(tex, tex, tmpSurf, SMAA::INPUT_COLOR);
				d3ddev->StretchRect(tmpSurf, NULL, backBuffer, NULL, D3DTEXF_NONE);
			}
		}
		if(doPost && post) {
			post->go(tex, tmpSurf);
			d3ddev->StretchRect(tmpSurf, NULL, backBuffer, NULL, D3DTEXF_NONE);
		}
	}
	SDLOG(8, "Generic plugin preDownsample end\n");
}
