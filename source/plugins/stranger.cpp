#include "plugins/stranger.h"

#include "renderstate_manager_dx9.h"
#include "d3d9/d3d9tex.h"
#include "utils/d3d9_utils.h"

#include "utils/win_utils.h"

using PluginBase = GenericPlugin;

void StrangerOfSwordCityPlugin::reportStatus() {
	PluginBase::reportStatus();
	Console::get().add("!! Stranger of Sword City Plugin");
}

StrangerOfSwordCityPlugin::StrangerOfSwordCityPlugin(IDirect3DDevice9* device, RSManagerDX9 &manager) : PluginBase(device, manager) {

}

void StrangerOfSwordCityPlugin::initialize(unsigned rw, unsigned rh, D3DFORMAT bbformat, D3DFORMAT dssformat) {
	PluginBase::initialize(rw, rh, bbformat, dssformat);
}

void StrangerOfSwordCityPlugin::prePresent() {
	PluginBase::prePresent();
	finishedFrame = false;
}

HRESULT StrangerOfSwordCityPlugin::redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	if((Usage & D3DUSAGE_RENDERTARGET || Format == D3DFMT_D24S8) && Width == 1280 && Height == 720) {
		SDLOG(-1, "Sosc: CreateTexture override\n");
		Width *= 3;
		Height *= 3;
	}
	return PluginBase::redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT StrangerOfSwordCityPlugin::redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	if(Width == 1280 && Height == 720) {
		Width*=3;
		Height*=3;
	}
	return PluginBase::redirectCreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT StrangerOfSwordCityPlugin::redirectSetViewport(CONST D3DVIEWPORT9 * pViewport) {
	SDLOG(-1, "Sosc: SetViewport override");
	D3DVIEWPORT9 newView = *pViewport;
	newView.X *= 3;
	newView.Y *= 3;
	newView.Width *= 3;
	newView.Height *= 3;
	return GenericPlugin::redirectSetViewport(&newView);
}

HRESULT StrangerOfSwordCityPlugin::redirectSetPixelShader(IDirect3DPixelShader9* pShader) {
	return PluginBase::redirectSetPixelShader(pShader);
}

HRESULT StrangerOfSwordCityPlugin::redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
	if(!finishedFrame && PrimitiveCount==2) {
		// scale draw calls for 2D UI elements
		size_t floatStride = VertexStreamZeroStride / sizeof(float);
		float* vertexData = (float*)pVertexStreamZeroData;
		// first, determine whether we are dealing with flipped textures
		float orientation_v = 1.0f;
		if(vertexData[6 + 1 * floatStride] < vertexData[6 + 2 * floatStride]) orientation_v = -1.0f;
		float orientation_h = 1.0f;
		if(vertexData[5 + 0 * floatStride] < vertexData[5 + 2 * floatStride]) orientation_h = -1.0f;
		// then adjust coordiantes
		for(int vert = 0; vert < 4; ++vert) {
			for(size_t i = 0; i < floatStride; ++i) {
				SDLOG(20, "%8.4f ", vertexData[i]);
				float& val = vertexData[i + vert*floatStride];
				// vertex coords
				if(i == 0) val *= 3;
				if(i == 1) val *= 3;
				// texture coords
				if(i == 5 && vert <= 1) val -= orientation_h * (1.0f / (1280.0f * 1.333f));
				if(i == 6 && vert%2==1) val -= orientation_v * (1.0f / ( 720.0f * 1.666f));
			}
			SDLOG(20, "\n");
		}
	}
	return PluginBase::redirectDrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT StrangerOfSwordCityPlugin::redirectSetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) {
	IDirect3DTexture9 *tex;
	if(pTexture->QueryInterface(IID_IDirect3DTexture9, (void**)&tex) == S_OK) {
		D3DSURFACE_DESC desc;
		tex->GetLevelDesc(0, &desc);
		if(desc.Width == 3840 && desc.Height == 2160) finishedFrame = true;
		tex->Release();
	}
	return PluginBase::redirectSetTexture(Stage, pTexture);
}
