#include "plugins/mitsurugi.h"

#include "renderstate_manager_dx9.h"

MitsurugiPlugin::~MitsurugiPlugin() {
	SAFERELEASE(depthSurf);
	SAFEDELETE(scaler);
}

void MitsurugiPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	GenericPlugin::initialize(Settings::get().getRenderWidth(), Settings::get().getRenderHeight(), bbformat, dssformat);

	d3ddev->CreateDepthStencilSurface(Settings::get().getRenderWidth(), Settings::get().getRenderHeight(), D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, false, &depthSurf, NULL);

	scaler = new Scaler(d3ddev, Settings::get().getRenderWidth(), Settings::get().getRenderHeight(), Settings::get().getPresentWidth(), Settings::get().getPresentHeight());

	Console::get().add("!! NOTE: In-game settings required for this plugin:");
	Console::get().add("!! 1920x1080, FPS display ON, in-game AA OFF, all other settings maximized");
}

void MitsurugiPlugin::prePresent() {
	GenericPlugin::prePresent();
	triggered = false;
	shaderCountA = 0;
	shaderCountB = 0;
}

HRESULT MitsurugiPlugin::redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	if(Usage & D3DUSAGE_RENDERTARGET) {
		if((Width == Settings::get().getPresentWidth() && Height == Settings::get().getPresentHeight()) || (Width == 1920 && Height == 1080)) {
			Width = Settings::get().getRenderWidth();
			Height = Settings::get().getRenderHeight();
		}
		else if(Width == 960 || Width == 480 || Width == 240 || Width == 120) {
			Width = Settings::get().getRenderWidth() / (1920 / Width);
			Height = Settings::get().getRenderHeight() / (1080 / Height);
		}
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
	//SetVertexShaderConstantF: start : 0, count : variable
	// 0.0010416667     0.0000000000     0.0000000000   -1.0000000000
	// 0.0000000000    -0.0018518518     0.0000000000    1.0000000000
	if(StartRegister == 0 && Vector4fCount >= 4 && Vector4fCount <= 10 && FLT_EPS(pConstantData[0], 0.0010416667, 0.0001) && FLT_EPS(pConstantData[5], -0.0018518518, 0.0001)) {

		if(Vector4fCount == 8 && pConstantData[3] == -1.0f) {
			triggered = true;
			SDLOG(8, "-> [mitsurugi] trigger!\n");
		}

		//SetVertexShaderConstantF: start : 0, count : 4
		//- 0.0010416667     0.0000000000     0.0000000000 - 1.0000000000
		//- 0.0000000000 - 0.0018518518     0.0000000000     1.0000000000
		//- 0.0000000000     0.0000000000 - 0.0000500000     0.5000000000
		//- 0.0000000000     0.0000000000     0.0000000000     1.0000000000
		if(Vector4fCount == 4 && pConstantData[3] == -1.0f) {
			float buffer[4 * 4];
			memcpy(buffer, pConstantData, 4 * Vector4fCount * sizeof(float));
			buffer[0] = 2.0f / Settings::get().getRenderWidth();
			buffer[5] = -2.0f / Settings::get().getRenderHeight();
			SDLOG(8, "-> [mitsurugi] replaced!\n");
			return GamePlugin::redirectSetVertexShaderConstantF(StartRegister, buffer, Vector4fCount);
		}
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
	//SetPixelShaderConstantF: start : 0, count : 4
	//	- 2.0399794579     0.0000000000     0.0000000000     0.0000000000
	//	- 0.0005208334     0.0000000000     0.0000000000     0.0000000000
	//	- 0.0009259259     0.0000000000     0.0000000000     0.0000000000
	//	- 0.5000050068     0.9902060032     0.4000000060     0.0000000000
	if(StartRegister == 0 && Vector4fCount == 4 && FLT_EPS(pConstantData[4], 0.0005208334, 0.0001) && FLT_EPS(pConstantData[8], 0.0009259259, 0.0001)) {
		SDLOG(8, "-> [mitsurugi] replaced!\n");
		float buffer[4 * 4];
		memcpy(buffer, pConstantData, 4 * 4 * sizeof(float));
		buffer[4] = 1.0f / Settings::get().getRenderWidth();
		buffer[8] = 1.0f / Settings::get().getRenderHeight();
		return GenericPlugin::redirectSetPixelShaderConstantF(StartRegister, buffer, Vector4fCount);
	}
	return GenericPlugin::redirectSetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT MitsurugiPlugin::redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
	void* data = const_cast<void*>(pVertexStreamZeroData);
	if(PrimitiveType == 5 && PrimitiveCount == 2) {
		if(VertexStreamZeroStride == 20) {
			float* floats = (float*)pVertexStreamZeroData;

			LOG_CHECK(8,
				SDLOG(2, "[mitsuuuu]\n");
				for(int i = 0; i < 4; ++i) {
					for(int j = 0; j < 5; ++j) {
						SDLOG(2, "%12.4f", floats[i * 5 + j]);
					}
					SDLOG(2,"\n");
				}
			);
			//[mitsuuuu]
			// 2559.5000   1439.5000      0.0000      1.0000      1.0000
			//	- 0.5000   1439.5000      0.0000      0.0000      1.0000
			// 2559.5000    - 0.5000      0.0000      1.0000      0.0000
			//	- 0.5000    - 0.5000      0.0000      0.0000      0.0000
			if(!isPostDone() && FLT_EPS(floats[0], (Settings::get().getPresentWidth() - 0.5f), 0.01)) {
				float buffer[20];
				memcpy(buffer, pVertexStreamZeroData, 20*sizeof(float));
				buffer[0]  = Settings::get().getRenderWidth() - 0.5f;
				buffer[10] = buffer[0];
				buffer[1]  = Settings::get().getRenderHeight() - 0.5f;
				buffer[6]  = buffer[1];
				data = buffer;
			}
		}
		if(triggered) {
			triggered = false;
			IDirect3DBaseTexture9* tex0 = NULL;
			d3ddev->GetTexture(0, &tex0);
			IDirect3DTexture9 *tex = NULL;
			if(tex0->QueryInterface(IID_IDirect3DTexture9, (void**)&tex) == S_OK) {
				IDirect3DSurface9 *surf = NULL, *rt = NULL;
				tex->GetSurfaceLevel(0, &surf);
				d3ddev->GetRenderTarget(0, &rt);
				if(surf != NULL && rt != NULL) {
					manager.storeRenderState();
					GenericPlugin::process(surf);
					scaler->go(tex, rt);
					if(manager.takingScreenshot(RSManager::SCREENSHOT_FULL) || manager.takingScreenshot(RSManager::SCREENSHOT_HUDLESS)) {
						manager.captureRTScreen("full", surf);
						manager.tookScreenshot(RSManager::SCREENSHOT_FULL);
						manager.tookScreenshot(RSManager::SCREENSHOT_HUDLESS);
					}
					manager.restoreRenderState();
				}
				SAFERELEASE(surf);
				SAFERELEASE(rt);
				SAFERELEASE(tex);
			}
			SAFERELEASE(tex0);
			return D3D_OK;
		}
	}
	return GenericPlugin::redirectDrawPrimitiveUP(PrimitiveType, PrimitiveCount, data, VertexStreamZeroStride);
}

HRESULT MitsurugiPlugin::redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
	D3DSURFACE_DESC desc;
	pNewZStencil->GetDesc(&desc);
	if(desc.Width == Settings::get().getRenderWidth() && desc.Height == Settings::get().getRenderHeight()) return d3ddev->SetDepthStencilSurface(pNewZStencil);
	HRESULT res = d3ddev->SetDepthStencilSurface(depthSurf);
	return res;
}

HRESULT MitsurugiPlugin::redirectSetVertexShader(IDirect3DVertexShader9* pvShader) {
	if(manager.getShaderManager().getName(pvShader) == string("a5840dcc")) {
		shaderCountA++;
		if(shaderCountA == 2) GenericPlugin::processCurrentBB();
	}
	return GamePlugin::redirectSetVertexShader(pvShader);
}

HRESULT MitsurugiPlugin::redirectSetPixelShader(IDirect3DPixelShader9* pShader) {
	if(manager.getShaderManager().getName(pShader) == string("8358759b")) {
		shaderCountB++;
		if(shaderCountB == 2) GenericPlugin::processCurrentBB();
	}
	return GamePlugin::redirectSetPixelShader(pShader);
}

void MitsurugiPlugin::toggleHUD() {
	hudEnabled = !hudEnabled;
	Console::get().add(string("HUD rendering ") + (hudEnabled ? "enabled" : "disabled"));
}
