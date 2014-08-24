#pragma once

#include "generic.h"
#include "ssao.h"
#include "depth_texture.h"

class GenericDepthPlugin : public GenericPlugin {

	SSAO* ssao = NULL;
	bool doAO = true;

	unsigned drw, drh;
	std::unique_ptr<DepthTexture> depthTexture;
	IDirect3DSurface9* depthStencilTarget = NULL;

	virtual void process(IDirect3DSurface9* backBuffer) override;

public:
	GenericDepthPlugin(IDirect3DDevice9* device, RSManager &manager) : GenericPlugin(device, manager)
	{ }

	virtual ~GenericDepthPlugin();

	virtual void initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat) override;
	virtual void reportStatus() override;

	virtual HRESULT redirectClear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) override;
	virtual HRESULT redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;
	virtual HRESULT redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) override;

	virtual void toggleAO() override { if(ssao) { doAO = !doAO; Console::get().add(doAO ? "SSAO enabled" : "SSAO disabled"); } else { Console::get().add("SSAO disabled in configuration!"); } }
	virtual void dumpSSAO() override { if(ssao) { ssao->dumpFrame(); Console::get().add("AO dumped"); } else { Console::get().add("SSAO disabled in configuration!"); } }

	virtual void reloadShaders() override;
};
