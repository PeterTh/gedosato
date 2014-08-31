#include "bloom.h"

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include "settings.h"
#include "renderstate_manager_dx9.h"
#include "utils.h"

#define STEP_SKIP 5
#define STEP_MINSIZE 16

Bloom::Bloom(IDirect3DDevice9 *device, int width, int height, float cutoff, float strength, float dirtStrength)
	: Effect(device), width(width), height(height), cutoff(cutoff),
	  strength(strength), dirtStrength(dirtStrength), 
	  steps(0),
	  dumping(false) {
	
	// Setup the defines for compiling the effect
    vector<D3DXMACRO> defines;
	//string cutoffText = format("%lf", cutoff);
	//D3DXMACRO cutoffMacro = { "CUTOFF", cutoffText.c_str() };
	//defines.push_back(cutoffMacro);
	//string strengthText = format("%lf", strength);
	//D3DXMACRO strengthMacro = { "STRENGTH", strengthText.c_str() };
	//defines.push_back(strengthMacro);
	//string dirtStrengthText = format("%lf", dirtStrength);
	//D3DXMACRO dirtStrengthMacro = { "DIRT_STRENGTH", dirtStrengthText.c_str() };
	//defines.push_back(dirtStrengthMacro);	
    D3DXMACRO null = { NULL, NULL };
    defines.push_back(null);

	DWORD flags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_OPTIMIZATION_LEVEL3;

	// Load effect from file
	string shaderFn = getAssetFileName("bloom.fx");
	SDLOG(0, "%s load\n", shaderFn.c_str());
	ID3DXBuffer* errors;
	HRESULT hr = D3DXCreateEffectFromFile(device, shaderFn.c_str(), &defines.front(), NULL, flags, NULL, &effect, &errors);
	if(hr != D3D_OK) SDLOG(-1, "ERRORS compiling bloom.fx:\n %s\n", errors->GetBufferPointer());
	
	// Load texture
	hr = D3DXCreateTextureFromFile(device, getAssetFileName("lensdirt.dds").c_str(), &dirtTexture);
	if(hr != D3D_OK) SDLOG(-1, "ERROR loading bloom texture lensdirt.dds\n");

	// Create buffers
	int sWidth = width, sHeight = height;
	unsigned i = 0;
	while(max(sWidth,sHeight) > 1) {
		if(min(sWidth,sHeight) <= STEP_MINSIZE && sWidth != sHeight) {
			sWidth = STEP_MINSIZE; sHeight = STEP_MINSIZE;
		} else {
			sWidth /= 2; sHeight /= 2;
		}
		SDLOG(2, "Generating bloom buffer %2u - %4dx%4d\n", i, sWidth, sHeight);
		Size s = { sWidth, sHeight };
		stepSizes.push_back(s);
		for(unsigned j=0; j<2; ++j) {
			stepBuffers[j].emplace_back();
			stepBuffers[j][i] = RSManager::getRTMan().createTexture(sWidth, sHeight, D3DFMT_A16B16G16R16F);
		}
		++i;
	}
	steps = i;

	// Get handles
	#define GETHANDLE(__name) \
	__name##Handle = effect->GetParameterByName(NULL, #__name); \
	if(__name##Handle == NULL) SDLOG(-1, "ERROR loading handle %s in Bloom effect\n", #__name);
	GETHANDLE(inputPixelMetrics)
	GETHANDLE(invSteps);
	GETHANDLE(sampleTex);
	GETHANDLE(passTex);
	GETHANDLE(dirtTex);
	GETHANDLE(avgTex);
	#undef GETHANDLE
	#define GETHANDLE(__name) \
	__name##Handle = effect->GetTechniqueByName(#__name); \
	if(__name##Handle == NULL) SDLOG(-1, "ERROR loading technique handle %s in Bloom effect\n", #__name);
	GETHANDLE(initialCutoffAndDownsample);
	GETHANDLE(gaussian);
	GETHANDLE(integrateUpwards);
	GETHANDLE(eyeAdaption);
	GETHANDLE(finalCompose);
	#undef GETHANDLE
}

Bloom::~Bloom() {
	SAFERELEASE(effect);
	SAFERELEASE(dirtTexture);
}

void Bloom::go(IDirect3DTexture9 *hdrFrame, IDirect3DTexture9 *composeFrame, IDirect3DSurface9 *dst) {
	device->SetVertexDeclaration(vertexDeclaration);
	
	if(dumping) {
		RSManager::getDX9().dumpTexture("Bloom_PRE_hdr", hdrFrame);
		RSManager::getDX9().dumpTexture("Bloom_PRE_frame", composeFrame);
	}
	
    initialPass(hdrFrame, stepBuffers[0][0]->getSurf());
	
	if(dumping) {
		RSManager::getDX9().dumpTexture("Bloom_INIT_out", stepBuffers[0][0]->getTex());
	}
	
	// traverse pyramid down
	unsigned cur = 0;
	for(; cur<steps-1; ++cur) {
		if(dumping) {
			RSManager::getDX9().dumpTexture("Bloom_STEP_in", stepBuffers[0][cur]->getTex());
		}
		if(cur<steps-STEP_SKIP) {
			// first Blur
			blurPass(stepBuffers[0][cur]->getTex(), stepBuffers[1][cur]->getSurf(), stepSizes[cur].w, stepSizes[cur].h, true);
			blurPass(stepBuffers[1][cur]->getTex(), stepBuffers[0][cur]->getSurf(), stepSizes[cur].w, stepSizes[cur].h, false);
			if(dumping) {
				RSManager::getDX9().dumpTexture("Bloom_STEP_out", stepBuffers[0][cur]->getTex());
			}
		}
		// downsample
		device->StretchRect(stepBuffers[0][cur]->getSurf(), NULL, stepBuffers[0][cur+1]->getSurf(), NULL, D3DTEXF_LINEAR);
	}

	// traverse back up
	for(cur=steps-STEP_SKIP; cur>0; --cur) {
		// Upsample
		upPass(stepBuffers[0][cur]->getTex(), stepBuffers[0][cur-1]->getSurf(), stepSizes[cur].w, stepSizes[cur].h);
	}

	// perform eye adaption
	eyePass(stepBuffers[0][steps-1]->getTex(), stepBuffers[1][steps-1]->getSurf());
	
	// apply final combination pass
	if(dumping) {
		RSManager::getDX9().dumpTexture("Bloom_FINAL_in", stepBuffers[0][0]->getTex());
	}

	finalPass(stepBuffers[0][0]->getTex(), stepBuffers[1][steps-1]->getTex(), composeFrame, dst);
	
	if(dumping) {
		RSManager::getDX9().dumpSurface("Bloom_FINAL_out", dst);
	}
	
	dumping = false;
}

void Bloom::initialPass(IDirect3DTexture9* src, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);

    // Setup variables.
    effect->SetTexture(sampleTexHandle, src);
	D3DXVECTOR4 ps(1.0f/width, 1.0f/height, static_cast<FLOAT>(width), static_cast<FLOAT>(height));
    effect->SetVector(inputPixelMetricsHandle, &ps);
	
    // Do it!
    effect->SetTechnique(initialCutoffAndDownsampleHandle);
	UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(0);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void Bloom::blurPass(IDirect3DTexture9* src, IDirect3DSurface9* dst, int sw, int sh, bool horizontal) {
	device->SetRenderTarget(0, dst);

    // Setup variables.
    effect->SetTexture(passTexHandle, src);
	D3DXVECTOR4 ps(1.0f/sw, 1.0f/sh, static_cast<FLOAT>(sw), static_cast<FLOAT>(sh));
    effect->SetVector(inputPixelMetricsHandle, &ps);
	
    // Do it!
    effect->SetTechnique(gaussianHandle);
	UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(horizontal ? 0 : 1);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void Bloom::upPass(IDirect3DTexture9* src, IDirect3DSurface9* dst, int sw, int sh) {
	device->SetRenderTarget(0, dst);
	
    // Setup variables.
    effect->SetTexture(passTexHandle, src);

    // Do it!
    effect->SetTechnique(integrateUpwardsHandle);
	UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(0);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void Bloom::eyePass(IDirect3DTexture9* src, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);
	
    // Setup variables.
    effect->SetTexture(avgTexHandle, src);

    // Do it!
    effect->SetTechnique(eyeAdaptionHandle);
	UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(0);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void Bloom::finalPass(IDirect3DTexture9* src, IDirect3DTexture9* eye, IDirect3DTexture9* frame, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);
	
    // Setup variables.
    effect->SetTexture(passTexHandle, src);
    effect->SetTexture(sampleTexHandle, frame);
    effect->SetTexture(dirtTexHandle, dirtTexture);
    effect->SetTexture(avgTexHandle, eye);
	effect->SetFloat(invStepsHandle, 1.0f/(steps-STEP_SKIP));

    // Do it!
    effect->SetTechnique(finalComposeHandle);
	UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(0);
	quad(width, height);
	effect->EndPass();
	effect->End();
}

void Bloom::dumpFrame() {
	dumping = true;
}
