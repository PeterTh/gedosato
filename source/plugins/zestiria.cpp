#include "plugins/zestiria.h"

#include "renderstate_manager_dx9.h"
#include "d3d9/d3d9tex.h"
#include "utils/d3d9_utils.h"

#include "utils/win_utils.h"

#define SFACTOR 4

void ZestiriaPlugin::reportStatus() {
	Console::get().add("!! Zestiria Plugin: character shadow resolution improved");
}

void ZestiriaPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	GenericPlugin::initialize(rw, rh, bbformat, dssformat);
}

void ZestiriaPlugin::prePresent() {
	GenericPlugin::prePresent();
}

HRESULT ZestiriaPlugin::redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	if(Usage & D3DUSAGE_RENDERTARGET && ((Width == 64 && Height == 64) || (Width == 128 && Height == 128))) {
		SDLOG(-1, "CreateTexture override\n");
		Width *= SFACTOR;
		Height *= SFACTOR;
	}
	return GenericPlugin::redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT ZestiriaPlugin::redirectSetViewport(CONST D3DVIEWPORT9 * pViewport) {
	if((pViewport->Width == 64 && pViewport->Height == 64) || (pViewport->Width == 128 && pViewport->Height == 128)) {
		SDLOG(4, "Zestiria: SetViewport override");
		D3DVIEWPORT9 newView = *pViewport;
		newView.Width *= SFACTOR;
		newView.Height *= SFACTOR;
		return GenericPlugin::redirectSetViewport(&newView);
	}
	return GenericPlugin::redirectSetViewport(pViewport);
}

HRESULT ZestiriaPlugin::redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
	if(StartRegister == 240 && Vector4fCount == 1) {
		if(pConstantData[0] == -0.015625f) {
			SDLOG(4, "Zestiria: SetVertexShaderConstantF override\n");
			float repData[] = { -1.0f / (64 * SFACTOR), 1.0f / (64 * SFACTOR), 0.0f, 0.0f };
			return GenericPlugin::redirectSetVertexShaderConstantF(StartRegister, repData, Vector4fCount);
		}
		if(pConstantData[0] == -0.0078125f) {
			SDLOG(4, "Zestiria: SetVertexShaderConstantF override\n");
			float repData[] = { -1.0f / (128 * SFACTOR), 1.0f / (128 * SFACTOR), 0.0f, 0.0f };
			return GenericPlugin::redirectSetVertexShaderConstantF(StartRegister, repData, Vector4fCount);
		}
	}
	if(StartRegister == 0 && (Vector4fCount == 2 || Vector4fCount == 3)) {
		IDirect3DSurface9 *rt;
		if(RSManager::getDX9().getD3Ddev()->GetRenderTarget(0, &rt) == D3D_OK) {
			D3DSURFACE_DESC desc;
			rt->GetDesc(&desc);
			rt->Release();
			if(desc.Width == desc.Height) {
				SDLOG(4, "Zestiria: SetVertexShaderConstantF override\n");
				float repData[12];
				for(size_t i = 0; i < Vector4fCount * 2; ++i) {
					repData[i] = pConstantData[i] / SFACTOR;
				}
				return GenericPlugin::redirectSetVertexShaderConstantF(StartRegister, repData, Vector4fCount);
			}
		}
	}
	return GenericPlugin::redirectSetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}