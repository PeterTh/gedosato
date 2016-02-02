#include "plugins/symphonia.h"

#include "renderstate_manager_dx9.h"
#include "d3d9/d3d9tex.h"
#include "utils/d3d9_utils.h"

#include "utils/win_utils.h"

void SymphoniaPlugin::reportStatus() {
	Console::get().add("!! Symphonia Plugin: oh come on");
}

void SymphoniaPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	GenericPlugin::initialize(rw, rh, bbformat, dssformat);
}

void SymphoniaPlugin::prePresent() {
	GenericPlugin::prePresent();
}

HRESULT SymphoniaPlugin::redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	if(Width == 1280 && Height == 720 && (((Usage & D3DUSAGE_RENDERTARGET) && Format == D3DFMT_A8R8G8B8) || Format == D3DFMT_D24S8)) {
		SDLOG(-1, "CreateTexture override\n");
		Width = Settings::get().getRenderWidth();
		Height = Settings::get().getRenderHeight();
	}
	return GenericPlugin::redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT SymphoniaPlugin::redirectSetViewport(CONST D3DVIEWPORT9* pViewport) {
	if(pViewport->Width == 1280 && pViewport->Height == 720) {
		SDLOG(4, "Symphonia: SetViewport override");
		D3DVIEWPORT9 newView = *pViewport;
		newView.Width = Settings::get().getRenderWidth();
		newView.Height = Settings::get().getRenderHeight();
		return GenericPlugin::redirectSetViewport(&newView);
	}
	return GenericPlugin::redirectSetViewport(pViewport);
}

HRESULT SymphoniaPlugin::redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
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
