#include "plugins/mitsurugi.h"


MitsurugiPlugin::~MitsurugiPlugin() {

}

void MitsurugiPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat) {

}

void MitsurugiPlugin::reportStatus() {

}

//HRESULT MitsurugiPlugin::redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
//	if(PrimitiveType == D3DPT_TRIANGLESTRIP && PrimitiveCount == 2) {
//
//	}
//}

HRESULT MitsurugiPlugin::redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	if(Width == 1920 && Height == 1080 && (Usage & D3DUSAGE_RENDERTARGET)) {
		Width = Settings::get().getRenderWidth();
		Height = Settings::get().getRenderHeight();
	}
	return GamePlugin::redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
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
		float vals[] = { 2.0f / Settings::get().getRenderWidth(), 0.0f, 0.0f, -1.0f,
						 0.0f, -2.0f / Settings::get().getRenderHeight(), 0.0f, 1.0f };
		float buffer[4 * 8];
		memcpy(buffer, pConstantData, 4 * 8 * sizeof(float));
		memcpy(buffer, vals, 4 * 2 * sizeof(float));
		SDLOG(8, "-> [mitsurugi] replaced!\n");
		return GamePlugin::redirectSetVertexShaderConstantF(StartRegister, buffer, Vector4fCount);
	}
	return GamePlugin::redirectSetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}
HRESULT MitsurugiPlugin::redirectSetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
	//-     0.0005208334     0.0009259259  1920.0000000000  1080.0000000000
	//if(StartRegister == 0 && Vector4fCount == 1 && pConstantData[0] < 0.00053f && pConstantData[1] < 0.001f && pConstantData[2] == 1920.0f && pConstantData[3] == 1080.0f) {
	//	float vals[] = { 1.0f / Settings::get().getRenderWidth(), 1.0f / Settings::get().getRenderHeight(), 
	//		static_cast<float>(Settings::get().getRenderWidth()), static_cast<float>(Settings::get().getRenderHeight()) };
	//	SDLOG(8, "-> [mitsurugi] replaced!\n");
	//	return GamePlugin::redirectSetPixelShaderConstantF(StartRegister, vals, Vector4fCount);
	//}
	return GamePlugin::redirectSetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}
