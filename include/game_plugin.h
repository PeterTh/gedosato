#pragma once

#include "main.h"
#include "settings.h"
#include "d3d9.h"
#include "settings.h"
#include "console.h"

class GamePlugin {
protected:
	IDirect3DDevice9 *d3ddev;
	RSManager &manager;
	string name;

public:
	static GamePlugin* getPlugin(IDirect3DDevice9 *device, RSManager &manager);

	GamePlugin(IDirect3DDevice9 *device, RSManager &manager) : d3ddev(device), manager(manager), name("None") { }
	virtual ~GamePlugin() { }
	string getName() { return name; }
	virtual void reportStatus() { }
	virtual void initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat) { }
	virtual void prePresent() { }
	virtual void preDownsample(IDirect3DSurface9* backBuffer) { }

	virtual HRESULT redirectSetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) {
		return d3ddev->SetRenderTarget(RenderTargetIndex, pRenderTarget);
	}

	virtual HRESULT redirectSetPixelShader(IDirect3DPixelShader9* pShader) {
		return d3ddev->SetPixelShader(pShader);
	}

	virtual HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
		return d3ddev->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
	}

	virtual HRESULT redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
		return d3ddev->SetRenderState(State, Value);
	}

	virtual HRESULT redirectDrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
		return d3ddev->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
	}

	virtual void toggleAA() { Console::get().add("AA not supported by this plugin!"); }
	virtual void toggleAO() { Console::get().add("AO not supported by this plugin!"); }
	virtual void toggleDOF() { Console::get().add("DoF not supported by this plugin!"); }
	virtual void toggleBloom() { Console::get().add("Bloom not supported by this plugin!"); }
	virtual void togglePost() { Console::get().add("Postprocessing not supported by this plugin!"); }
	virtual void toggleHUD() { Console::get().add("HUD toggling not supported by this plugin!"); }

	virtual void dumpSSAO() { Console::get().add("AO not supported by this plugin!"); }
	virtual void dumpBloom() { Console::get().add("Bloom not supported by this plugin!"); }
};
