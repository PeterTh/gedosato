
#pragma once

#include "game_plugin.h"

#include "smaa.h"
#include "fxaa.h"
#include "post.h"
#include "ssao.h"
#include "depthTexture.h"
#include "rendertarget.h"

#include <functional>

class GenericPlugin : public GamePlugin {

	Post* post;
	FXAA* fxaa;
	SMAA* smaa;
	SSAO *ssao;

	unsigned drw, drh;

	bool doPost, doAA, doAO;
	RenderTargetPtr tmp;
	bool postDone, postReady, hudEnabled;
	DWORD injectRSType, injectRSValue;
	IDirect3DSurface9* processedBB;

	DepthTexture* depthTexture;
	IDirect3DSurface9* depthStencilTarget;

	void process(IDirect3DSurface9* backBuffer);
	void processCurrentBB();
	void performInjection();
	HRESULT drawingStep(std::function<HRESULT(void)> drawCall);

public:
	GenericPlugin(IDirect3DDevice9* device, RSManager &manager) : GamePlugin(device, manager)
		, post(NULL), fxaa(NULL), smaa(NULL), ssao(NULL)
		, depthTexture(NULL), depthStencilTarget(NULL)
		, doPost(true), doAA(true), doAO(true)
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
	virtual void toggleAO() override { if(ssao) { doAO = !doAO; Console::get().add(doAO ? "SSAO enabled" : "SSAO disabled"); } else { Console::get().add("SSAO disabled in configuration!"); } }
	virtual void toggleHUD() override;

	virtual HRESULT redirectSetPixelShader(IDirect3DPixelShader9* pShader) override;
	virtual HRESULT redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) override;
	virtual HRESULT redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;
	virtual HRESULT redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) override;

	virtual HRESULT redirectDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) override;
	virtual HRESULT redirectDrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) override;
	virtual HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override;
	virtual HRESULT redirectDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, CONST void * pIndexData, D3DFORMAT IndexDataFormat, CONST void * pVertexStreamZeroData, UINT VertexStreamZeroStride) override;
};
