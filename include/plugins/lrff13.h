#pragma once

#include "game_plugin.h"

#include "ssao.h"
#include "post.h"
#include "bloom.h"
#include "scaling.h"
#include "generic_depth.h"

using PluginBase = GenericDepthPlugin;

class LightningReturnsPlugin : public PluginBase {

	//std::unique_ptr<FXAA> dofAA;
	//std::unique_ptr<Bloom> bloom;
	//std::unique_ptr<Scaler> scaler;
	//RenderTargetPtr temp;

	//bool isBloom = false, isDofGeneration = false, finishedDofGeneration = false;

public:
	LightningReturnsPlugin(IDirect3DDevice9* device, RSManagerDX9 &manager) : PluginBase(device, manager)
	{ }

	virtual ~LightningReturnsPlugin() {};

	virtual void initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) override;
	virtual void reportStatus() override;

	virtual void prePresent() override;

	virtual HRESULT redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) override;
	virtual HRESULT redirectSetViewport(CONST D3DVIEWPORT9 * pViewport) override;
	virtual HRESULT redirectSetPixelShader(IDirect3DPixelShader9* pShader) override;
	virtual HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override;
	virtual HRESULT redirectStretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) override;

	virtual void preDownsample(IDirect3DSurface9* backBuffer) override;

};
