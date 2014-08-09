
#pragma once

#include "game_plugin.h"

#include "smaa.h"
#include "fxaa.h"
#include "post.h"
#include "rendertarget.h"

#include <functional>

class GenericPlugin : public GamePlugin {

	Post* post;
	FXAA* fxaa;
	SMAA* smaa;
	bool doPost, doAA;
	RenderTargetPtr tmp;
	bool postDone, postReady, hudEnabled;
	DWORD injectRSType, injectRSValue;
	IDirect3DSurface9* processedBB;


	HRESULT drawingStep(std::function<HRESULT(void)> drawCall);

protected:
	void process(IDirect3DSurface9* backBuffer);
	void processCurrentBB();
	void performInjection();

public:
	GenericPlugin(IDirect3DDevice9* device, RSManager &manager) : GamePlugin(device, manager)
		, post(NULL), fxaa(NULL), smaa(NULL)
		, doPost(true), doAA(true)
		, postDone(false), postReady(false), hudEnabled(true)
		, injectRSType(0), injectRSValue(0)
		, processedBB(NULL)
	{ }

	virtual ~GenericPlugin() override;

	virtual void initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat) override;
	virtual void reportStatus() override;

	virtual void preDownsample(IDirect3DSurface9* backBuffer) override;
	virtual void prePresent() override;

	virtual void toggleAA() override { if(smaa || fxaa) { doAA = !doAA; Console::get().add(doAA ? "AA enabled" : "AA disabled"); } else { Console::get().add("AA disabled in configuration!"); } }
	virtual void togglePost() override { if(post) { doPost = !doPost; Console::get().add(doPost ? "Post-processing enabled" : "Post-processing disabled"); } else { Console::get().add("Post-processing disabled in configuration!"); } }
	virtual void toggleHUD() override;

	virtual HRESULT redirectSetPixelShader(IDirect3DPixelShader9* pShader) override;
	virtual HRESULT redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) override;

	virtual HRESULT redirectDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) override;
	virtual HRESULT redirectDrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) override;
	virtual HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override;
	virtual HRESULT redirectDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, CONST void * pIndexData, D3DFORMAT IndexDataFormat, CONST void * pVertexStreamZeroData, UINT VertexStreamZeroStride) override;
};
