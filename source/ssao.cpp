#include "SSAO.h"

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include "settings.h"
#include "renderstate_manager.h"

SSAO::SSAO(IDirect3DDevice9 *device, int width, int height, unsigned strength, Type type, Blur blur) 
	: Effect(device), width(width), height(height), dumping(false), 
	  blurPasses(blur==BLUR_SHARP ? 3 : 1) {
	
	// Setup the defines for compiling the effect
    vector<D3DXMACRO> defines;

    // Setup pixel size macro
	stringstream sp;
	sp << "float2(1.0 / " << width << ", 1.0 / " << height << ")";
	string pixelSizeText = sp.str();
	D3DXMACRO pixelSizeMacro = { "PIXEL_SIZE", pixelSizeText.c_str() };
	defines.push_back(pixelSizeMacro);
	
    // Setup scale macro
	stringstream ss;
	ss << Settings::get().getSsaoScale() << ".0";
	string scaleText = ss.str();
	D3DXMACRO scaleMacro = { "SCALE", scaleText.c_str() };
	defines.push_back(scaleMacro);

	// Setup strength macro
	D3DXMACRO strengthMacros[] = {
		{ "SSAO_STRENGTH_LOW", "1" },
		{ "SSAO_STRENGTH_MEDIUM", "1" },
		{ "SSAO_STRENGTH_HIGH", "1" }
	};
	defines.push_back(strengthMacros[strength]);

	// Setup blur macro
	if(blur == BLUR_SHARP) {
		D3DXMACRO sharpBlur = { "BLUR_SHARP", "1" };
		defines.push_back(sharpBlur);
	}

    D3DXMACRO null = { NULL, NULL };
    defines.push_back(null);

	DWORD flags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_OPTIMIZATION_LEVEL3;

	// Load effect from file
	string shaderFn;
	switch(type) {
		case VSSAO: shaderFn = "VSSAO.fx"; break;
		case HBAO: shaderFn = "HBAO.fx"; break;
		case SCAO: shaderFn = "SCAO.fx"; break;
		case VSSAO2: shaderFn = "VSSAO2.fx"; break;
	}
	shaderFn = getAssetFileName(shaderFn);
	SDLOG(0, "%s load, scale %s, strength %s\n", shaderFn.c_str(), scaleText.c_str(), strengthMacros[strength].Name);	
	ID3DXBuffer* errors;
	HRESULT hr = D3DXCreateEffectFromFile(device, shaderFn.c_str(), &defines.front(), NULL, flags, NULL, &effect, &errors);
	if(hr != D3D_OK) SDLOG(0, "ERRORS:\n %s\n", errors->GetBufferPointer());
	
	// Create buffers
	buffer1 = RSManager::getRTMan().createTexture(width, height, RenderTarget::FMT_ARGB_8);
	buffer2 = RSManager::getRTMan().createTexture(width, height, RenderTarget::FMT_ARGB_8);
	hdrBuffer = RSManager::getRTMan().createTexture(width, height, RenderTarget::FMT_ARGB_FP16);

	// get handles
	depthTexHandle = effect->GetParameterByName(NULL, "depthTex2D");
	frameTexHandle = effect->GetParameterByName(NULL, "frameTex2D");
    prevPassTexHandle = effect->GetParameterByName(NULL, "prevPassTex2D");
}

SSAO::~SSAO() {
	SAFERELEASE(effect);
}

void SSAO::go(IDirect3DTexture9 *frame, IDirect3DTexture9 *depth, IDirect3DSurface9 *dst) {
	device->SetVertexDeclaration(vertexDeclaration);
	
    mainSsaoPass(depth, buffer1->getSurf());
	
	for(size_t i = 0; i<blurPasses; ++i) {
		hBlurPass(depth, buffer1->getTex(), buffer2->getSurf());
		vBlurPass(depth, buffer2->getTex(), buffer1->getSurf());
	}

	combinePass(frame, buffer1->getTex(), dst);
	
	dumping = false;
}

void SSAO::goHDR(IDirect3DTexture9 *frame, IDirect3DTexture9 *depth, IDirect3DSurface9 *dst) {
	device->SetVertexDeclaration(vertexDeclaration);

	if(dumping) {
		RSManager::get().dumpTexture("SSAO_PRE_frame", frame);
		RSManager::get().dumpTexture("SSAO_PRE_depth", depth);
		RSManager::get().dumpSurface("SSAO_PRE_dest", dst);
	}
	
	mainSsaoPass(depth, buffer1->getSurf());

	if(dumping) {
		RSManager::get().dumpTexture("SSAO_MD1_buffer1", buffer1->getTex());
	}
	
	for(size_t i = 0; i<blurPasses; ++i) {
		hBlurPass(depth, buffer1->getTex(), buffer2->getSurf());
		vBlurPass(depth, buffer2->getTex(), buffer1->getSurf());
	}
	
	if(dumping) {
		RSManager::get().dumpTexture("SSAO_MD2_buffer1", buffer1->getTex());
	}

	combinePass(frame, buffer1->getTex(), hdrBuffer->getSurf());
	
	if(dumping) {
		RSManager::get().dumpSurface("SSAO_END_buffer2", hdrBuffer->getSurf());
	}

	device->StretchRect(hdrBuffer->getSurf(), NULL, dst, NULL, D3DTEXF_NONE);

	dumping = false;
}

void SSAO::mainSsaoPass(IDirect3DTexture9* depth, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);
    //device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 1.0f, 0);

    // Setup variables.
    effect->SetTexture(depthTexHandle, depth);

    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(0);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void SSAO::hBlurPass(IDirect3DTexture9 *depth, IDirect3DTexture9* src, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);

    // Setup variables.
    effect->SetTexture(prevPassTexHandle, src);
    effect->SetTexture(depthTexHandle, depth);
	
    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(1);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void SSAO::vBlurPass(IDirect3DTexture9 *depth, IDirect3DTexture9* src, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);

    // Setup variables.
    effect->SetTexture(prevPassTexHandle, src);
    effect->SetTexture(depthTexHandle, depth);
	
    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(2);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void SSAO::combinePass(IDirect3DTexture9* frame, IDirect3DTexture9* ao, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);
    //device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 255), 1.0f, 0);

    // Setup variables.
    effect->SetTexture(prevPassTexHandle, ao);
    effect->SetTexture(frameTexHandle, frame);
	
    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(3);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void SSAO::combineShadowPass(IDirect3DTexture9 *shadow, IDirect3DTexture9* ao, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);

    // Setup variables.
    effect->SetTexture(prevPassTexHandle, ao);
    effect->SetTexture(frameTexHandle, shadow);
	
    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(4);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void SSAO::dumpFrame() {
	dumping = true;
}
