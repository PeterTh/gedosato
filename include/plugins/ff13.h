#pragma once

#include "game_plugin.h"

#include "ssao.h"
#include "post.h"
#include "bloom.h"
#include "scaling.h"
#include "generic.h"

class FF13Plugin : public GenericPlugin {
	bool triggered = false;

public:
	FF13Plugin(IDirect3DDevice9* device, RSManagerDX9 &manager) : GenericPlugin(device, manager)
	{ }

	virtual ~FF13Plugin();

	virtual void initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) override;
	virtual void FF13Plugin::reportStatus() override;

	virtual void prePresent() override;

	virtual HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override;

	virtual HRESULT redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) override;
	virtual HRESULT redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;
	virtual HRESULT redirectCreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;

	virtual HRESULT redirectSetVertexShader(IDirect3DVertexShader9* pvShader) override;
	virtual HRESULT redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) override;
	virtual HRESULT redirectSetPixelShader(IDirect3DPixelShader9* pShader) override;
	virtual HRESULT redirectSetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) override;

	virtual	HRESULT redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) override;

	virtual HRESULT redirectSetViewport(CONST D3DVIEWPORT9 * pViewport) override;

	virtual HRESULT redirectStretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) override;
	virtual HRESULT redirectScissorRect(CONST RECT* pRect) override;
};
