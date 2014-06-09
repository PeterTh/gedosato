
#pragma once

#include "game_plugin.h"

#include "smaa.h"
#include "fxaa.h"
#include "post.h"

class GenericPlugin : public GamePlugin {

	Post* post;
	FXAA* fxaa;
	SMAA* smaa;
	bool doPost, doAA;
	IDirect3DTexture9* tmpTex;
	IDirect3DSurface9* tmpSurf;

public:
	GenericPlugin(IDirect3DDevice9* device, RSManager &manager) : GamePlugin(device, manager)
		, post(NULL), fxaa(NULL), smaa(NULL)
		, doPost(true), doAA(true)
		, tmpTex(NULL), tmpSurf(NULL)
	{ }

	virtual ~GenericPlugin();

	virtual void initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat);
	virtual void reportStatus();

	virtual void preDownsample(IDirect3DSurface9* backBuffer);

	virtual void toggleAA() { if(smaa || fxaa) { doAA = !doAA; Console::get().add(doAA ? "AA enabled" : "AA disabled"); } else { Console::get().add("AA disabled in configuration!"); } }
	virtual void togglePost() { if(post) { doPost = !doPost; Console::get().add(doPost ? "Post-processing enabled" : "Post-processing disabled"); } else { Console::get().add("Post-processing disabled in configuration!"); } }
};
