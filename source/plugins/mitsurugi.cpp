#include "plugins/mitsurugi.h"

#include "renderstate_manager.h"

MitsurugiPlugin::~MitsurugiPlugin() {
	SAFERELEASE(depthSurf);
	SAFEDELETE(scaler);
}

void MitsurugiPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat) {
	GenericPlugin::initialize(Settings::get().getRenderWidth(), Settings::get().getRenderHeight(), bbformat);

	d3ddev->CreateDepthStencilSurface(Settings::get().getRenderWidth(), Settings::get().getRenderHeight(), D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, false, &depthSurf, NULL);

	scaler = new Scaler(d3ddev, Settings::get().getRenderWidth(), Settings::get().getRenderHeight(), Settings::get().getPresentWidth(), Settings::get().getPresentHeight());
}

void MitsurugiPlugin::prePresent() {
	GenericPlugin::prePresent();
	triggered = false;
}

HRESULT MitsurugiPlugin::redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	if(Width == Settings::get().getPresentWidth() && Height == Settings::get().getPresentHeight() && (Usage & D3DUSAGE_RENDERTARGET)) {
		Width = Settings::get().getRenderWidth();
		Height = Settings::get().getRenderHeight();
	}
	return GenericPlugin::redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT MitsurugiPlugin::redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	return GenericPlugin::redirectCreateDepthStencilSurface(Settings::get().getRenderWidth(), Settings::get().getRenderHeight(), Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}


HRESULT MitsurugiPlugin::redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
	//if(StartRegister == 0 && Vector4fCount == 1 && pConstantData[1] == 0.0f && pConstantData[2] == 0.0f) {
	//	float vals[] = { 1.0f/Settings::get().getRenderWidth(), 0.0f, 0.0f, 1.0f/Settings::get().getRenderHeight() };
	//	SDLOG(8, "-> [mitsurugi] replaced!\n");
	//	return GamePlugin::redirectSetVertexShaderConstantF(StartRegister, vals, Vector4fCount);
	//}
	//SetVertexShaderConstantF: start : 0, count : 8
	// 0.0010416667     0.0000000000     0.0000000000   -1.0000000000
	// 0.0000000000    -0.0018518518     0.0000000000    1.0000000000
	if(StartRegister == 0 && Vector4fCount == 8 && FLT_EPS(pConstantData[0], 0.0010416667, 0.0001) && pConstantData[1] == 0.0f  && pConstantData[2] == 0.0f && pConstantData[3] == -1.0f
												&& pConstantData[4] == 0.0f && FLT_EPS(pConstantData[5], -0.0018518518, 0.0001) && pConstantData[6] == 0.0f && pConstantData[7] ==  1.0f) {
	//	float vals[] = { 2.0f / Settings::get().getRenderWidth(), 0.0f, 0.0f, -1.0f,
	//					 0.0f, -2.0f / Settings::get().getRenderHeight(), 0.0f, 1.0f };
	//	float buffer[4 * 8];
	//	memcpy(buffer, pConstantData, 4 * 8 * sizeof(float));
	//	memcpy(buffer, vals, 4 * 2 * sizeof(float));
	//	SDLOG(8, "-> [mitsurugi] replaced!\n");
	//	return GamePlugin::redirectSetVertexShaderConstantF(StartRegister, buffer, Vector4fCount);
		triggered = true;
		SDLOG(8, "-> [mitsurugi] trigger!\n");
	}
	return GenericPlugin::redirectSetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}
HRESULT MitsurugiPlugin::redirectSetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
	////-     0.0005208334     0.0009259259  1920.0000000000  1080.0000000000
	//if(StartRegister == 0 && Vector4fCount == 1 && pConstantData[0] < 0.00053f && pConstantData[1] < 0.001f && pConstantData[2] == 1920.0f && pConstantData[3] == 1080.0f) {
	//	float vals[] = { 1.0f / Settings::get().getRenderWidth(), 1.0f / Settings::get().getRenderHeight(), 
	//		static_cast<float>(Settings::get().getRenderWidth()), static_cast<float>(Settings::get().getRenderHeight()) };
	//	SDLOG(8, "-> [mitsurugi] replaced!\n");
	//	return GamePlugin::redirectSetPixelShaderConstantF(StartRegister, vals, Vector4fCount);
	//}
	return GenericPlugin::redirectSetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT MitsurugiPlugin::redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
	if(triggered && PrimitiveType == 5 && PrimitiveCount == 2) {
		triggered = false;
		IDirect3DBaseTexture9* tex0 = NULL;
		d3ddev->GetTexture(0, &tex0);
		IDirect3DTexture9 *tex = NULL;
		if(tex0->QueryInterface(IID_IDirect3DTexture9, (void**)&tex) == S_OK) {
			IDirect3DSurface9 *surf = NULL, *rt = NULL;
			tex->GetSurfaceLevel(0, &surf);
			d3ddev->GetRenderTarget(0, &rt);
			if(surf != NULL && rt != NULL) {
				RSManager::get().storeRenderState();
				GenericPlugin::process(surf);
				scaler->go(tex, rt);
				if(RSManager::get().getTakeScreenshot() == RSManager::SCREENSHOT_FULL || RSManager::get().getTakeScreenshot() == RSManager::SCREENSHOT_HUDLESS) {
					RSManager::get().captureRTScreen("full", surf);
				}
				RSManager::get().tookScreenshot();
				RSManager::get().restoreRenderState();
			}
			SAFERELEASE(surf);
			SAFERELEASE(rt);
			SAFERELEASE(tex);
		}
		SAFERELEASE(tex0);
		return D3D_OK;
	}
	return GenericPlugin::redirectDrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT MitsurugiPlugin::redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
	D3DSURFACE_DESC desc;
	pNewZStencil->GetDesc(&desc);
	if(desc.Width == Settings::get().getRenderWidth() && desc.Height == Settings::get().getRenderHeight()) return d3ddev->SetDepthStencilSurface(pNewZStencil);
	HRESULT res = d3ddev->SetDepthStencilSurface(depthSurf);
	return res;
}
