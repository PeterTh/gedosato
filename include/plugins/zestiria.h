#pragma once

#include "game_plugin.h"

#include "ssao.h"
#include "post.h"
#include "bloom.h"
#include "scaling.h"
#include "generic.h"

class ZestiriaPlugin : public GenericPlugin {
public:
	ZestiriaPlugin(IDirect3DDevice9* device, RSManagerDX9 &manager) : GenericPlugin(device, manager)
	{ }

	virtual ~ZestiriaPlugin() {};

	virtual void initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) override;
	virtual void reportStatus() override;

	virtual void prePresent() override;

	virtual HRESULT redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) override;
	virtual HRESULT redirectSetViewport(CONST D3DVIEWPORT9 * pViewport) override;
	virtual HRESULT redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) override;
};
