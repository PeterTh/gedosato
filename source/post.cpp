#include "post.h"

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <boost/filesystem.hpp>

#include "settings.h"
#include "renderstate_manager.h"

// Pass ---------------------------------------------------------------------------------------------------------------

PostprocessingPass::PostprocessingPass(const string& shaderFn, IDirect3DDevice9 *device, int width, int height, bool useSRGB) 
	: Effect(device), shaderFn(shaderFn), width(width), height(height), useSRGB(useSRGB) {
	
	reloadShader();
}

void PostprocessingPass::reloadShader() {
	// Setup the defines for compiling the effect
	vector<D3DXMACRO> defines;

	// Setup pixel size macro
	string pixelSizeText = format("float2(1.0/%d, 1.0/%d)", width, height);
	D3DXMACRO pixelSizeMacro = { "PIXEL_SIZE", pixelSizeText.c_str() };
	defines.push_back(pixelSizeMacro);

	// Setup screen size macro
	string screenSizeText = format("float2(%d,%d)", width, height);
	D3DXMACRO screenSizeMacro = { "SCREEN_SIZE", screenSizeText.c_str() };
	defines.push_back(screenSizeMacro);

	D3DXMACRO srgbMacro = { "USE_SRGB", useSRGB ? "true" : "false" };
	defines.push_back(srgbMacro);

	D3DXMACRO null = { NULL, NULL };
	defines.push_back(null);

	DWORD flags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_OPTIMIZATION_LEVEL3;

	// Load effect from file
	string customfn = getConfigFileName(getExeFileName() + "\\" + shaderFn);
	if(!boost::filesystem::exists(customfn)) customfn = getAssetFileName(shaderFn);
	SDLOG(0, "%s load\n", customfn.c_str());
	ID3DXBuffer* errors;
	ID3DXEffect* newEffect = NULL;
	HRESULT hr = D3DXCreateEffectFromFile(device, customfn.c_str(), &defines.front(), NULL, flags, NULL, &newEffect, &errors);
	if(hr != D3D_OK) {
		SDLOG(-1, "ERRORS:\n %s\n", (char*)errors->GetBufferPointer());
		Console::get().add(format("Error compiling %s:", customfn.c_str()));
		Console::get().add(static_cast<const char*>(errors->GetBufferPointer()));
	}
	else {
		SAFERELEASE(effect);
		effect = newEffect;
	}

	// Get handles
	thisframeTexHandle = effect->GetParameterByName(NULL, "thisframeTex");
	timerHandle = effect->GetParameterByName(NULL, "timer");
	noiseTexHandle = effect->GetParameterByName(NULL, "noiseTexture");

	// Load noise
	if(noiseTexHandle != NULL) {
		string noiseFile = getAssetFileName("NoiseTex.png");
		hr = D3DXCreateTextureFromFile(device, noiseFile.c_str(), &noiseTex);
	}
}

const string& PostprocessingPass::getFn() {
	return shaderFn;
}

PostprocessingPass::~PostprocessingPass() {
	SAFERELEASE(effect);
	if(noiseTexHandle != NULL) SAFERELEASE(noiseTex);
}

void PostprocessingPass::go(IDirect3DTexture9 *frame, IDirect3DSurface9 *dst, double time) {
	device->SetVertexDeclaration(vertexDeclaration);
	
    device->SetRenderTarget(0, dst);
    effect->SetTexture(thisframeTexHandle, frame);
	if(timerHandle != NULL) effect->SetFloat(timerHandle, static_cast<float>(time / 1000000.0));
	if(noiseTexHandle != NULL) effect->SetTexture(noiseTexHandle, noiseTex);

    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(0);
	quad(width, height);
	effect->EndPass();
	effect->End();
}


// Post ---------------------------------------------------------------------------------------------------------------


Post::Post(IDirect3DDevice9 *device, int width, int height, bool useSRGB, RenderTarget* tmp)
	: device(device), width(width), height(height), useSRGB(useSRGB), tmp(tmp) {

	timer.start();

	// Base processing pass
	string shaderFn = "post.fx";
	if(Settings::get().getPostProcessingType() == "asmodean") shaderFn = "post_asmodean.fx";
	passes.push_back(make_shared<PostprocessingPass>(shaderFn, device, width, height, useSRGB));

	// Additional processing passes
	for(const auto& passFn : Settings::get().getProcessingPasses()) {
		addProcessingPass(passFn);
	}
}

void Post::reloadShaders() {
	for(auto& pass : passes) {
		pass->reloadShader();
	}
}

void Post::addProcessingPass(const string& effectFileName) {
	if(passes.size() != 0 && tmp == nullptr) {
		SDLOG(-1, "ERROR: Post::addProcessingPass with more than 1 pass, but no temporary storage specified");
		return;
	}
	passes.push_back(make_shared<PostprocessingPass>(effectFileName, device, width, height, useSRGB));
}

void Post::go(IDirect3DTexture9 *frame, IDirect3DSurface9 *dst) {
	if(passes.size() == 0) {
		SDLOG(-1, "ERROR: Post::go called with no postprocessing chain");
		return;
	}
	// -- one or more passes
	// first from source to dst
	SDLOG(8, "Post processing: first pass\n");
	passes[0]->go(frame, dst, timer.elapsed());
	// remaining passes from tmp with back copy
	for(size_t curPass = 1; curPass < passes.size(); ++curPass) {
		SDLOG(8, "Post processing: prepare pass %u\n", curPass);
		device->StretchRect(dst, NULL, tmp->getSurf(), NULL, D3DTEXF_NONE);
		SDLOG(8, "Post processing: pass %u (%s)\n", curPass, passes[curPass]->getFn());
		passes[curPass]->go(tmp->getTex(), dst, timer.elapsed());
	}
}


