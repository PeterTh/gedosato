#include "plugins/symphonia.h"

#include "renderstate_manager_dx9.h"
#include "d3d9/d3d9tex.h"
#include "utils/d3d9_utils.h"

#include "utils/win_utils.h"

void SymphoniaPlugin::reportStatus() {
	Console::get().add("!! Symphonia Plugin: oh come on");
	Console::get().add(string("!! black outlines: ") + (Settings::get().getSymphoniaBlackOutlines()?"enabled":"disabled"));
}

void SymphoniaPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	GenericPlugin::initialize(rw, rh, bbformat, dssformat);
}

void SymphoniaPlugin::prePresent() {
	GenericPlugin::prePresent();
}

HRESULT SymphoniaPlugin::redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	if(Width == 1280 && Height == 720 && (((Usage & D3DUSAGE_RENDERTARGET) && Format == D3DFMT_A8R8G8B8) || Format == D3DFMT_D24S8)) {
		SDLOG(-1, "CreateTexture BB override\n");
		Width = Settings::get().getRenderWidth();
		Height = Settings::get().getRenderHeight();
	}
	else if(Width == 512 && Height == 256 && (Usage & D3DUSAGE_RENDERTARGET)) {
		SDLOG(-1, "CreateTexture effect override\n");
		Width = Settings::get().getRenderWidth() / 2;
		Height = Settings::get().getRenderHeight() / 2;
	}
	return GenericPlugin::redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT SymphoniaPlugin::redirectSetViewport(CONST D3DVIEWPORT9* pViewport) {
	if(pViewport->Width == 1280 && pViewport->Height == 720) {
		SDLOG(4, "Symphonia: SetViewport BB override");
		D3DVIEWPORT9 newView = *pViewport;
		newView.Width = Settings::get().getRenderWidth();
		newView.Height = Settings::get().getRenderHeight();
		return GenericPlugin::redirectSetViewport(&newView);
	} 
	else if(pViewport->Width == 512 && pViewport->Height == 256) {
		SDLOG(4, "Symphonia: SetViewport effect override");
		D3DVIEWPORT9 newView = *pViewport;
		newView.Width = Settings::get().getRenderWidth() / 2;
		newView.Height = Settings::get().getRenderHeight() / 2;
		return GenericPlugin::redirectSetViewport(&newView);
	}
	return GenericPlugin::redirectSetViewport(pViewport);
}

HRESULT SymphoniaPlugin::redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
	if(StartRegister == 240 && Vector4fCount == 1) {
		D3DVIEWPORT9 vp;
		d3ddev->GetViewport(&vp);
		float vals[4] = { -1.0f/vp.Width, 1.0f/vp.Height, 0.0f, 0.0f };
		return GenericPlugin::redirectSetVertexShaderConstantF(StartRegister, vals, Vector4fCount);
	}
	return GenericPlugin::redirectSetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT SymphoniaPlugin::redirectScissorRect(CONST RECT* pRect) {
	SDLOG(4, "Symphonia: Rect override");
	RECT newR = *pRect;
	double sFactor = Settings::get().getRenderWidth()/1280.0;
	newR.right = static_cast<long>(newR.right*sFactor);
	newR.bottom = static_cast<long>(newR.bottom*sFactor);
	return GenericPlugin::redirectScissorRect(&newR);
}

HRESULT SymphoniaPlugin::redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
	if(Settings::get().getSymphoniaBlackOutlines()) {
		if(State == D3DRS_SRCBLEND || State == D3DRS_DESTBLEND) {
			D3DCULL cullmode;
			d3ddev->GetRenderState(D3DRS_CULLMODE, (DWORD*)(&cullmode));
			if(cullmode == D3DCULL_CW) {
				if(State == D3DRS_SRCBLEND) return GenericPlugin::redirectSetRenderState(State, D3DBLEND_ONE);
				if(State == D3DRS_DESTBLEND) return GenericPlugin::redirectSetRenderState(State, D3DBLEND_ZERO);
			}
		}
	}
	return GenericPlugin::redirectSetRenderState(State, Value);
}
