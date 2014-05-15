#include "bloom.h"

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include "settings.h"
#include "renderstate_manager.h"
#include "utils.h"

#define STEP_SKIP 3
#define STEP_MINSIZE 16
#define ADJUST_SPEED 0.025f

Bloom::Bloom(IDirect3DDevice9 *device, int width, int height, float cutoff, float strength, float dirtStrength)
	: Effect(device), width(width), height(height), cutoff(cutoff), power(0.0),
	  strength(strength), dirtStrength(dirtStrength), 
	  //steps(static_cast<unsigned>(floor(max(Log2(width), Log2(height))))), 
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
		stepSizes[i].w = sWidth; 
		stepSizes[i].h = sHeight;
		for(unsigned j=0; j<2; ++j) {
			device->CreateTexture(sWidth, sHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &stepBuffers[i].textures[j], NULL);
			stepBuffers[i].textures[j]->GetSurfaceLevel(0, &stepBuffers[i].surfaces[j]);
		}
		++i;
	}
	steps = i;
	//minWidth = sWidth; minHeight = sHeight;
	//SDLOG(2, "Bloom min: %4dx%4d\n", minWidth, minHeight);
	//device->CreateOffscreenPlainSurface(minWidth, minHeight, D3DFMT_A16B16G16R16F, D3DPOOL_SYSTEMMEM, &readbackSurf, NULL);

	// Get handles
	#define GETHANDLE(__name) \
	__name##Handle = effect->GetParameterByName(NULL, #__name); \
	if(__name##Handle == NULL) SDLOG(-1, "ERROR loading handle %s in Bloom effect\n", #__name);
	GETHANDLE(inputPixelMetrics)
	GETHANDLE(invSteps);
	GETHANDLE(totalPower);
	GETHANDLE(sampleTex);
	GETHANDLE(passTex);
	GETHANDLE(dirtTex);
	#undef GETHANDLE
	#define GETHANDLE(__name) \
	__name##Handle = effect->GetTechniqueByName(#__name); \
	if(__name##Handle == NULL) SDLOG(-1, "ERROR loading technique handle %s in Bloom effect\n", #__name);
	GETHANDLE(initialCutoffAndDownsample);
	GETHANDLE(gaussian);
	GETHANDLE(integrateUpwards);
	GETHANDLE(finalCompose);
	#undef GETHANDLE
}

Bloom::~Bloom() {
	SAFERELEASE(effect);
	SAFERELEASE(dirtTexture);
	SAFERELEASE(readbackSurf);
	for(unsigned i=0; i<steps; ++i) {
		for(unsigned j=0; j<2; ++j) {
			SAFERELEASE(stepBuffers[i].surfaces[j]);
			SAFERELEASE(stepBuffers[i].textures[j]);
		}
	}
}

void Bloom::go(IDirect3DTexture9 *hdrFrame, IDirect3DTexture9 *composeFrame, IDirect3DSurface9 *dst) {
	device->SetVertexDeclaration(vertexDeclaration);
	
	if(dumping) {
		RSManager::get().dumpTexture("Bloom_PRE_hdr", hdrFrame);
		RSManager::get().dumpTexture("Bloom_PRE_frame", composeFrame);
	}
	
    initialPass(hdrFrame, stepBuffers[0].surfaces[0]);
	
	if(dumping) {
		RSManager::get().dumpTexture("Bloom_INIT_out", stepBuffers[0].textures[0]);
	}
	
	// Traverse pyramid down
	unsigned cur = 0;
	for(; cur<steps-1; ++cur) {
		if(dumping) {
			RSManager::get().dumpTexture("Bloom_STEP_in", stepBuffers[cur].textures[0]);
		}
		if(cur<steps-STEP_SKIP) {
			// First Blur
			blurPass(stepBuffers[cur].textures[0], stepBuffers[cur].surfaces[1], stepSizes[cur].w, stepSizes[cur].h, true);
			blurPass(stepBuffers[cur].textures[1], stepBuffers[cur].surfaces[0], stepSizes[cur].w, stepSizes[cur].h, false);
			if(dumping) {
				RSManager::get().dumpTexture("Bloom_STEP_out", stepBuffers[cur].textures[0]);
			}
		}
		// Downsample
		device->StretchRect(stepBuffers[cur].surfaces[0], NULL, stepBuffers[cur+1].surfaces[0], NULL, D3DTEXF_LINEAR);
	}

	// Traverse back up
	for(cur=steps-STEP_SKIP; cur>0; --cur) {
		// Upsample
		upPass(stepBuffers[cur].textures[0], stepBuffers[cur-1].surfaces[0]);
	}

	// Get total bloom brightness
	//device->GetRenderTargetData(stepBuffers[steps-1].surfaces[0], readbackSurf);
	//if(dumping) {
	//	RSManager::get().dumpSurface("Bloom_READBACK", readbackSurf);
	//}
	//D3DLOCKED_RECT r;
	//HRESULT hr = readbackSurf->LockRect(&r, NULL, D3DLOCK_READONLY);
	//if(SUCCEEDED(hr)) {
	//	float cur_power = 0.0f;
	//	for(int y=0; y<minHeight; ++y) {
	//		D3DXVECTOR4_16F* line = reinterpret_cast<D3DXVECTOR4_16F*>(((BYTE*)r.pBits) + r.Pitch*y);
	//		for(int x=0; x<minWidth; ++x) {
	//			cur_power += line[x].x + line[x].y + line[x].z;
	//		}
	//	}
	//	cur_power /= minWidth*minHeight;
	//	power = power*(1.0f-ADJUST_SPEED) + cur_power*ADJUST_SPEED;
	//	SDLOG(10, "Bloom power: %12.10f, cur power: %12.10f\n", power, cur_power);
	//	readbackSurf->UnlockRect();
	//}
	
	// apply final combination pass
	if(dumping) {
		RSManager::get().dumpTexture("Bloom_FINAL_in", stepBuffers[0].textures[0]);
	}

	finalPass(stepBuffers[0].textures[0], composeFrame, dst);
	
	if(dumping) {
		RSManager::get().dumpSurface("Bloom_FINAL_out", dst);
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

void Bloom::upPass(IDirect3DTexture9* src, IDirect3DSurface9* dst) {
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

void Bloom::finalPass(IDirect3DTexture9* src, IDirect3DTexture9* frame, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);
	
    // Setup variables.
    effect->SetTexture(passTexHandle, src);
    effect->SetTexture(sampleTexHandle, frame);
    effect->SetTexture(dirtTexHandle, dirtTexture);
	effect->SetFloat(invStepsHandle, 1.0f/(steps-STEP_SKIP));
	effect->SetFloat(totalPowerHandle, power);

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
