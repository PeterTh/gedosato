#include "Scaling.h"

#include "Console.h"

#include <string>
#include <sstream>
#include <vector>
#include <boost/algorithm/string/predicate.hpp>
using namespace std;

#include "Settings.h"

namespace {
	double log2(double n) {  
		return log(n)/log(2.0);  
	}
};

Scaler::Scaler(IDirect3DDevice9 *device, int inputwidth, int inputheight, int width, int height) 
	: Effect(device), inputwidth(inputwidth), inputheight(inputheight), width(width), height(height),
	  levels(0), levelBufferTextures(NULL), levelBuffers(NULL), levelInputSizes(NULL) {
	SDLOG(0, "Scaler construct\n");
	// Determine initial scaling type from settings
	if(boost::iequals(Settings::get().getScalingType(), string("bicubic"))) {
		sType = BICUBIC; 
	} else {
		sType = BILINEAR;
	}

	// Setup the defines for compiling the effect
    vector<D3DXMACRO> defines;
    D3DXMACRO null = { NULL, NULL };
    defines.push_back(null);

	DWORD flags = D3DXFX_NOT_CLONEABLE;

	// Load effect from file
	SDLOG(0, "Scaler load\n");
	ID3DXBuffer* errors;
	HRESULT hr = D3DXCreateEffectFromFile(device, getAssetFileName("scaling.fx").c_str(), &defines.front(), NULL, flags, NULL, &effect, &errors);
	if(hr != D3D_OK) SDLOG(0, "ERRORS:\n %s\n", errors->GetBufferPointer());
		
	// get handles
	frameTexHandle = effect->GetParameterByName(NULL, "frameTex2D");
	inputSizeHandle = effect->GetParameterByName(NULL, "inputSize");
	
	stringstream ss;

	// generate multi-level buffers
	levels = max( max(0, static_cast<int>(ceil(log2(static_cast<double>(inputwidth)/width)-1))), 
		          max(0, static_cast<int>(ceil(log2(static_cast<double>(inputheight)/height)-1))) );
	SDLOG(2, "Scaler: Generating %u multi-level buffers\n", levels);
	ss << "Downsampling: " << levels+1 << " level" << (levels?"s ":" ");
	levelBuffers = new IDirect3DSurface9*[levels];
	levelBufferTextures = new IDirect3DTexture9*[levels];
	levelInputSizes = new FLOAT[levels*2+2];
	levelInputSizes[levels*2+0] = static_cast<FLOAT>(width*2); 
	levelInputSizes[levels*2+1] = static_cast<FLOAT>(height*2); 
	for(int i=levels-1; i>=0; --i) {
		unsigned w = static_cast<unsigned>(levelInputSizes[(i+1)*2+0]);
		unsigned h = static_cast<unsigned>(levelInputSizes[(i+1)*2+1]);
		levelInputSizes[i*2+0] = static_cast<FLOAT>(w*2);
		levelInputSizes[i*2+1] = static_cast<FLOAT>(h*2);
		device->CreateTexture(w, h, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &levelBufferTextures[i], NULL);
		levelBufferTextures[i]->GetSurfaceLevel(0, &levelBuffers[i]);
		SDLOG(2, "Multi-level buffer %u: %p  tex: %p, size: %ux%u\n", i, levelBuffers[i], levelBufferTextures[i], w, h);
	}
	levelInputSizes[0] = static_cast<FLOAT>(inputwidth);
	levelInputSizes[1] = static_cast<FLOAT>(inputheight);
	for(unsigned i=0; i<levels+1; i++) {
		SDLOG(2, "Scaling step %u: input size: %5.0f x %5.0f\n", i, levelInputSizes[i*2+0], levelInputSizes[i*2+1]);
		ss << format("%5.0f x %5.0f -> ", levelInputSizes[i*2+0], levelInputSizes[i*2+1]);
	}
	ss << format("%5d x %5d", Settings::get().getPresentWidth(), Settings::get().getPresentHeight());
	status = ss.str();
	showStatus();
}

Scaler::~Scaler() {
	SAFERELEASE(effect);
	for(unsigned i=0; i<levels; ++i) {
		SAFERELEASE(levelBuffers[i]);
		SAFERELEASE(levelBufferTextures[i]);
	}
	delete [] levelBuffers;
	delete [] levelBufferTextures;
	delete [] levelInputSizes;
}

void Scaler::go(IDirect3DTexture9 *input, IDirect3DSurface9 *dst) {
	device->SetVertexDeclaration(vertexDeclaration);
	
	IDirect3DSurface9 *finalDst = dst;

	for(unsigned i=0; i<=levels; i++) {
		dst = levelBuffers[i];
		if(i==levels) dst = finalDst;
		UINT passes;
		if(sType == BICUBIC) { 
			device->SetRenderTarget(0, dst);
			effect->SetTexture(frameTexHandle, input);
			effect->SetFloatArray(inputSizeHandle, levelInputSizes+i*2, 2);
			effect->Begin(&passes, 0);
			effect->BeginPass(1);
			quad(width, height);
			effect->EndPass();
			effect->End();
		} else { // default to bilinear
			device->SetRenderTarget(0, dst);
			effect->SetTexture(frameTexHandle, input);
			effect->Begin(&passes, 0);
			effect->BeginPass(0);
			quad(width, height);
			effect->EndPass();
			effect->End();
		}
		input = levelBufferTextures[i];
	}
}

void Scaler::setScaling(scalingType t) {
	sType = t;
	Console::get().add(format("Set scaling type to %s", getScalingName()));
}

void Scaler::showStatus() {
	Console::get().add(format("%s (%s scaling)", status.c_str(), getScalingName()));
}

const char* Scaler::getScalingName() {
	switch(sType) {
	case BILINEAR: return "bilinear";
	case BICUBIC: return "bicubic";
	}
	return "unknown";
}