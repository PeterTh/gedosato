#pragma once

#include "game_plugin.h"

#include "ssao.h"
#include "post.h"
#include "bloom.h"

class MitsurugiPlugin : public GamePlugin {

	SSAO* ssao;
	Post* post;
	Bloom* bloom;
	bool doAO, doPost, doBloom;

public:
	MitsurugiPlugin(IDirect3DDevice9* device, RSManager &manager) : GamePlugin(device, manager)
		, ssao(NULL), post(NULL), bloom(NULL)
		, doAO(true), doPost(true), doBloom(true)
	{ }

	virtual ~MitsurugiPlugin();

	virtual void initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat) override;
	virtual void reportStatus() override;

	//virtual void prePresent() override;

	//virtual HRESULT redirectSetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) override;
	//virtual HRESULT redirectSetPixelShader(IDirect3DPixelShader9* pShader) override;
	//virtual HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override;
	//virtual HRESULT redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) override;

	virtual HRESULT redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) override;

	virtual HRESULT redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) override;
	virtual HRESULT redirectSetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) override;

	virtual void toggleAO() override { if(ssao) { doAO = !doAO; Console::get().add(doAO ? "SSAO enabled" : "SSAO disabled"); } else { Console::get().add("SSAO disabled in configuration!"); } }
	virtual void toggleBloom() override { if(bloom) { doBloom = !doBloom; Console::get().add(doBloom ? "Bloom enabled" : "Bloom disabled"); } else { Console::get().add("Bloom disabled in configuration!"); } }
	virtual void togglePost() override { if(post) { doPost = !doPost; Console::get().add(doPost ? "Post-processing enabled" : "Post-processing disabled"); } else { Console::get().add("Post-processing disabled in configuration!"); } }
};
