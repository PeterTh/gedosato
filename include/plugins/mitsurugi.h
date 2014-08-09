#pragma once

#include "game_plugin.h"

#include "ssao.h"
#include "post.h"
#include "bloom.h"
#include "scaling.h"
#include "generic.h"

class MitsurugiPlugin : public GenericPlugin {

	Scaler* scaler = NULL;
	bool triggered = false;
	unsigned shaderCount = 0;

	IDirect3DSurface9 *depthSurf = NULL;

public:
	MitsurugiPlugin(IDirect3DDevice9* device, RSManager &manager) : GenericPlugin(device, manager)
	{ }

	virtual ~MitsurugiPlugin();

	virtual void initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat) override;

	virtual void prePresent() override;
	
	virtual Scaler* getScaler() override { return scaler; }

	virtual HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override;

	virtual HRESULT redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) override;
	virtual HRESULT redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;

	virtual HRESULT redirectSetVertexShader(IDirect3DVertexShader9* pvShader) override;
	virtual HRESULT redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) override;
	virtual HRESULT redirectSetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) override;

	virtual	HRESULT redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) override;

	virtual void toggleHUD() override;
};
