#pragma once

#include "game_plugin.h"

#include "ssao.h"
#include "post.h"
#include "bloom.h"
#include "scaling.h"
#include "generic.h"

class DW8Plugin : public GenericPlugin {
public:
	DW8Plugin(IDirect3DDevice9* device, RSManagerDX9 &manager) : GenericPlugin(device, manager)
	{ }

	virtual ~DW8Plugin() {};

	virtual void initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) override;
	virtual void reportStatus() override;

	virtual void prePresent() override;

	virtual HRESULT redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) override;
	virtual HRESULT redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;
	virtual HRESULT redirectCreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;

	virtual HRESULT redirectSetViewport(CONST D3DVIEWPORT9 * pViewport) override;
	virtual HRESULT redirectScissorRect(CONST RECT* pRect) override;
};
