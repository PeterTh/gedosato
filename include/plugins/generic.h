
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
	bool postDone, postReady;
	DWORD injectRSType, injectRSValue;

	void process(IDirect3DSurface9* backBuffer);
	void processCurrentBB();
	void performInjection();

public:
	GenericPlugin(IDirect3DDevice9* device, RSManager &manager) : GamePlugin(device, manager)
		, post(NULL), fxaa(NULL), smaa(NULL)
		, doPost(true), doAA(true)
		, tmpTex(NULL), tmpSurf(NULL)
		, postDone(false), postReady(false)
		, injectRSType(0), injectRSValue(0)
	{ }

	virtual ~GenericPlugin() override;

	virtual void initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat) override;
	virtual void reportStatus() override;

	virtual void preDownsample(IDirect3DSurface9* backBuffer) override;
	virtual void prePresent() override;

	virtual void toggleAA() override { if(smaa || fxaa) { doAA = !doAA; Console::get().add(doAA ? "AA enabled" : "AA disabled"); } else { Console::get().add("AA disabled in configuration!"); } }
	virtual void togglePost() override { if(post) { doPost = !doPost; Console::get().add(doPost ? "Post-processing enabled" : "Post-processing disabled"); } else { Console::get().add("Post-processing disabled in configuration!"); } }

	virtual HRESULT redirectSetPixelShader(IDirect3DPixelShader9* pShader) override;
	virtual HRESULT redirectDrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) override;
	virtual HRESULT redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) override;

};
