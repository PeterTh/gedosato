#pragma once

#include "main.h"
#include "settings.h"
#include "d3d9.h"
#include "settings.h"
#include "console.h"
#include "scaling.h"

class GamePlugin {
protected:
	IDirect3DDevice9 *d3ddev = NULL;
	RSManagerDX9 &manager;
	string name = "None";

public:
	static GamePlugin* getPlugin(IDirect3DDevice9 *device, RSManagerDX9 &manager);

	GamePlugin(IDirect3DDevice9 *device, RSManagerDX9 &manager) : d3ddev(device), manager(manager) { }
	virtual ~GamePlugin() { }
	string getName() { return name; }
	virtual void reportStatus() { }
	virtual void initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) { }
	virtual void prePresent() { }
	virtual void preDownsample(IDirect3DSurface9* backBuffer) { }

	virtual void toggleAA() { Console::get().add("AA not supported by this plugin!"); }
	virtual void toggleAO() { Console::get().add("AO not supported by this plugin!"); }
	virtual void toggleDOF() { Console::get().add("DoF not supported by this plugin!"); }
	virtual void toggleBloom() { Console::get().add("Bloom not supported by this plugin!"); }
	virtual void togglePost() { Console::get().add("Postprocessing not supported by this plugin!"); }
	virtual void toggleHUD() { Console::get().add("HUD toggling not supported by this plugin!"); }

	virtual void dumpSSAO() { Console::get().add("AO not supported by this plugin!"); }
	virtual void dumpBloom() { Console::get().add("Bloom not supported by this plugin!"); }

	virtual void reloadShaders() { }

	virtual Scaler* getScaler() { return NULL; }

	virtual HRESULT redirectSetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) {
		return d3ddev->SetRenderTarget(RenderTargetIndex, pRenderTarget);
	}
	virtual HRESULT redirectSetPixelShader(IDirect3DPixelShader9* pShader) {
		return d3ddev->SetPixelShader(pShader);
	}
	virtual HRESULT redirectSetVertexShader(IDirect3DVertexShader9* pvShader) {
		return d3ddev->SetVertexShader(pvShader);
	}
	virtual HRESULT redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
		return d3ddev->SetRenderState(State, Value);
	}

	virtual HRESULT redirectClear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
		return d3ddev->Clear(Count, pRects, Flags, Color, Z, Stencil);
	}
	virtual HRESULT redirectDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
		return d3ddev->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
	}
	virtual HRESULT redirectDrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
		return d3ddev->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
	}
	virtual HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
		return d3ddev->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
	}
	virtual HRESULT redirectDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, CONST void * pIndexData, D3DFORMAT IndexDataFormat, CONST void * pVertexStreamZeroData, UINT VertexStreamZeroStride) {
		return d3ddev->DrawIndexedPrimitiveUP(PrimitiveType, MinIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
	}

	virtual HRESULT redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
		return d3ddev->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
	}
	virtual HRESULT redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
		return d3ddev->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
	}
	virtual	HRESULT redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
		return d3ddev->SetDepthStencilSurface(pNewZStencil);
	}

	virtual HRESULT redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
		return d3ddev->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
	}
	virtual HRESULT redirectSetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
		return d3ddev->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
	}
	virtual HRESULT redirectCreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
		return d3ddev->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
	}

	virtual HRESULT redirectSetViewport(CONST D3DVIEWPORT9 * pViewport) {
		return d3ddev->SetViewport(pViewport);
	}
	virtual HRESULT redirectStretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) {
		return d3ddev->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
	}
	virtual HRESULT redirectScissorRect(CONST RECT* pRect) {
		return d3ddev->SetScissorRect(pRect);
	}

	virtual HRESULT redirectBeginScene() {
		return d3ddev->BeginScene();
	}
};
