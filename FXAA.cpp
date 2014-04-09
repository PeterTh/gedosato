#include "FXAA.h"

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include "settings.h"
#include "renderstate_manager.h"

FXAA::FXAA(IDirect3DDevice9 *device, int width, int height, Quality quality) 
	: Effect(device), width(width), height(height) {
	
	// Setup the defines for compiling the effect
    vector<D3DXMACRO> defines;
    stringstream s;

    // Setup pixel size macro
    s << "float2(1.0 / " << width << ", 1.0 / " << height << ")";
    string pixelSizeText = s.str();
    D3DXMACRO pixelSizeMacro = { "PIXEL_SIZE", pixelSizeText.c_str() };
    defines.push_back(pixelSizeMacro);
	
	D3DXMACRO qualityMacros[] = {
		{ "FXAA_QUALITY__PRESET", "10" },
		{ "FXAA_QUALITY__PRESET", "20" },
		{ "FXAA_QUALITY__PRESET", "28" },
		{ "FXAA_QUALITY__PRESET", "39" }
	};
	defines.push_back(qualityMacros[(int)quality]);

    D3DXMACRO null = { NULL, NULL };
    defines.push_back(null);

	DWORD flags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_OPTIMIZATION_LEVEL3;

	// Load effect from file
	SDLOG(0, "FXAA load\n");	
	ID3DXBuffer* errors;
	HRESULT hr = D3DXCreateEffectFromFile(device, getAssetFileName("FXAA.fx").c_str(), &defines.front(), NULL, flags, NULL, &effect, &errors);
	if(hr != D3D_OK) SDLOG(0, "ERRORS:\n %s\n", errors->GetBufferPointer());
	
	// Create buffer
	device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &buffer1Tex, NULL);
    buffer1Tex->GetSurfaceLevel(0, &buffer1Surf);

	// get handles
	frameTexHandle = effect->GetParameterByName(NULL, "frameTex2D");
}

FXAA::~FXAA() {
	SAFERELEASE(effect);
	SAFERELEASE(buffer1Surf);
	SAFERELEASE(buffer1Tex);
}

void FXAA::go(IDirect3DTexture9 *frame, IDirect3DSurface9 *dst) {
	device->SetVertexDeclaration(vertexDeclaration);
	
    lumaPass(frame, buffer1Surf);
	fxaaPass(buffer1Tex, dst);
}

void FXAA::lumaPass(IDirect3DTexture9 *frame, IDirect3DSurface9 *dst) {
	device->SetRenderTarget(0, dst);

    // Setup variables
    effect->SetTexture(frameTexHandle, frame);

    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(0);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void FXAA::fxaaPass(IDirect3DTexture9 *src, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);

    // Setup variables
    effect->SetTexture(frameTexHandle, src);
	
    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(1);
	quad(width, height);
	effect->EndPass();
	effect->End();
}
