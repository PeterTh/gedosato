#include "SSAO.h"

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <boost/filesystem.hpp>

#include "settings.h"
#include "renderstate_manager_dx9.h"

SSAO::SSAO(IDirect3DDevice9 *device, int width, int height, unsigned strength, Type type, Blur blur, bool useSRGB, bool readHWDepth)
	: Effect(device), width(width), height(height), strength(strength), type(type), blur(blur), useSRGB(useSRGB), readHWDepth(readHWDepth),
	  blurPasses(blur==BLUR_SHARP ? 3 : 1) {

	reloadShader();

	// Create buffers
	buffer1 = RSManager::getRTMan().createTexture(width, height, RenderTarget::FMT_ARGB_8);
	buffer2 = RSManager::getRTMan().createTexture(width, height, RenderTarget::FMT_ARGB_8);
	hdrBuffer = RSManager::getRTMan().createTexture(width, height, RenderTarget::FMT_ARGB_FP16);

	if(type == SSAO::SAO) halfZBuffer = RSManager::getRTMan().createTexture(width / 2, height / 2, RenderTarget::FMT_R32F);
}

SSAO::~SSAO() {
	SAFERELEASE(effect);
}

void SSAO::reloadShader() {

	// Setup the defines for compiling the effect
	vector<D3DXMACRO> defines;

	// Setup pixel size macro
	stringstream sp;
	sp << "float2(1.0 / " << width << ", 1.0 / " << height << ")";
	string pixelSizeText = sp.str();
	D3DXMACRO pixelSizeMacro = { "PIXEL_SIZE", pixelSizeText.c_str() };
	defines.push_back(pixelSizeMacro);

	// Setup screen size macro
	string screenSizeText = format("float2(%d,%d)", width, height);
	D3DXMACRO screenSizeMacro = { "SCREEN_SIZE", screenSizeText.c_str() };
	defines.push_back(screenSizeMacro);

	// Setup SRGB macro
	D3DXMACRO srgbMacro = { "USE_SRGB", useSRGB ? "true" : "false" };
	defines.push_back(srgbMacro);

	// Setup depth reading method
	D3DXMACRO readHWDepthMacro = { "USE_HWDEPTH", "true" };
	if(readHWDepth) defines.push_back(readHWDepthMacro);

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
	case HBAO: shaderFn = "HBAO.fx"; break;
	case MSSAO: shaderFn = "martinsh_ssao.fx"; break;
	case SAO: shaderFn = "SAO.fx"; break;
	case VSSAO2: shaderFn = "VSSAO2.fx"; break;
	}
	string customfn = getConfigFileName(getExeFileName() + "\\" + shaderFn);
	if(boost::filesystem::exists(customfn)) shaderFn = customfn;
	else shaderFn = getAssetFileName(shaderFn);
	SDLOG(0, "%s load, scale %s, strength %s\n", shaderFn.c_str(), scaleText.c_str(), strengthMacros[strength].Name);
	ID3DXBuffer* errors;
	ID3DXEffect* newEffect;
	HRESULT hr = D3DXCreateEffectFromFile(device, shaderFn.c_str(), &defines.front(), NULL, flags, NULL, &newEffect, &errors);
	if(hr != D3D_OK) {
		SDLOG(-1, "ERRORS:\n %s\n", errors->GetBufferPointer());
		Console::get().add(format("Error compiling %s:", shaderFn.c_str()));
		Console::get().add(static_cast<const char*>(errors->GetBufferPointer()));
	}
	else {
		SAFERELEASE(effect);
		effect = newEffect;
	}

	// get handles
	depthTexHandle = effect->GetParameterByName(NULL, "depthTex2D");
	frameTexHandle = effect->GetParameterByName(NULL, "frameTex2D");
	prevPassTexHandle = effect->GetParameterByName(NULL, "prevPassTex2D");
	isBlurHorizontalHandle = effect->GetParameterByName(NULL, "isBlurHorizontal");

	if(type == SSAO::SAO) blurPasses = 2;
}

void SSAO::go(IDirect3DTexture9 *frame, IDirect3DTexture9 *depth, IDirect3DSurface9 *dst) {
	device->SetVertexDeclaration(vertexDeclaration);
	
    mainSsaoPass(depth, buffer1->getSurf());
	
	for(size_t i = 0; i<blurPasses; ++i) {
		blurPass(depth, buffer1->getTex(), buffer2->getSurf(), true);
		blurPass(depth, buffer2->getTex(), buffer1->getSurf(), false);
	}

	combinePass(frame, buffer1->getTex(), dst);
	
	dumping = false;
}

void SSAO::goHDR(IDirect3DTexture9 *frame, IDirect3DTexture9 *depth, IDirect3DSurface9 *dst) {
	device->SetVertexDeclaration(vertexDeclaration);

	if(dumping) {
		RSManager::getDX9().dumpTexture("SSAO_PRE_frame", frame);
		RSManager::getDX9().dumpTexture("SSAO_PRE_depth", depth);
		RSManager::getDX9().dumpSurface("SSAO_PRE_dest", dst);
	}

	if(type == SSAO::SAO) {
		downsamplePass(depth, halfZBuffer->getSurf());
		mainSsaoPass(halfZBuffer->getTex(), buffer1->getSurf());
	}
	else {
		mainSsaoPass(depth, buffer1->getSurf());
	}

	if(dumping) {
		RSManager::getDX9().dumpTexture("SSAO_MD1_buffer1", buffer1->getTex());
	}

	for(size_t i = 0; i<blurPasses; ++i) {
		blurPass(depth, buffer1->getTex(), buffer2->getSurf(), true);
		blurPass(depth, buffer2->getTex(), buffer1->getSurf(), false);
	}

	if(dumping) {
		RSManager::getDX9().dumpTexture("SSAO_MD2_buffer1", buffer1->getTex());
	}

	combinePass(frame, buffer1->getTex(), hdrBuffer->getSurf());

	if(dumping) {
		RSManager::getDX9().dumpSurface("SSAO_END_buffer2", hdrBuffer->getSurf());
	}

	device->StretchRect(hdrBuffer->getSurf(), NULL, dst, NULL, D3DTEXF_NONE);
	dumping = false;
}

void SSAO::downsamplePass(IDirect3DTexture9 *depth, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);

	// Setup variables.
	effect->SetTexture(depthTexHandle, depth);

	// Do it!
	UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(0);
	quad(width / 2, height / 2);
	effect->EndPass();
	effect->End();
}

void SSAO::mainSsaoPass(IDirect3DTexture9* depth, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);

	// Setup variables.
	effect->SetTexture(depthTexHandle, depth);

    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(type == SSAO::SAO ? 2 : 0);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void SSAO::blurPass(IDirect3DTexture9 *depth, IDirect3DTexture9* src, IDirect3DSurface9* dst, bool horizontal) {
	device->SetRenderTarget(0, dst);

    // Setup variables.
    effect->SetTexture(prevPassTexHandle, src);
    effect->SetTexture(depthTexHandle, depth);
	effect->SetBool(isBlurHorizontalHandle, horizontal);
	
    // Do it!
	UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(type == SSAO::SAO ? 3 : horizontal ? 1 : 2);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void SSAO::combinePass(IDirect3DTexture9* frame, IDirect3DTexture9* ao, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);

    // Setup variables.
    effect->SetTexture(prevPassTexHandle, ao);
    effect->SetTexture(frameTexHandle, frame);
	
    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(type == SSAO::SAO ? 4 : 3);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void SSAO::dumpFrame() {
	dumping = true;
}
