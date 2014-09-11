#include "scaling.h"

#include <string>
#include <sstream>
#include <vector>
#include <boost/algorithm/string/predicate.hpp>
using namespace std;

#include "console.h"
#include "utils.h"
#include "settings.h"
#include "utils/string_utils.h"
#include "renderstate_manager.h"

Scaler::Scaler(IDirect3DDevice9 *device, int inputwidth, int inputheight, int width, int height, bool useSRGB)
	: Effect(device), inputwidth(inputwidth), inputheight(inputheight), width(width), height(height),
	  levels(0), levelInputSizes(NULL), useSRGB(useSRGB) {
	SDLOG(0, "Scaler construct\n");
	// Determine initial scaling type from settings
	if(boost::iequals(Settings::get().getScalingType(), string("bicubic"))) {
		sType = BICUBIC;
	} else if(boost::iequals(Settings::get().getScalingType(), string("lanczos"))) {
		sType = LANCZOS;
	} else if(boost::iequals(Settings::get().getScalingType(), string("nearest"))) {
		sType = NEAREST;
	} else {
		sType = BILINEAR;
	}

	reloadShader();

	stringstream ss;

	// generate multi-level buffers
	levels = max( max(0, static_cast<int>(ceil(Log2(static_cast<double>(inputwidth)/width)-1))), 
		          max(0, static_cast<int>(ceil(Log2(static_cast<double>(inputheight)/height)-1))) );
	SDLOG(2, "Scaler: Generating %u multi-level buffers\n", levels);
	ss << "Downsampling: " << levels+1 << " level" << (levels?"s ":" ");
	levelInputSizes = new FLOAT[levels*2+2];
	levelInputSizes[levels*2+0] = static_cast<FLOAT>(width*2); 
	levelInputSizes[levels*2+1] = static_cast<FLOAT>(height*2); 
	levelBuffers.resize(levels);
	for(int i=levels-1; i>=0; --i) {
		unsigned w = static_cast<unsigned>(levelInputSizes[(i+1)*2+0]);
		unsigned h = static_cast<unsigned>(levelInputSizes[(i+1)*2+1]);
		levelInputSizes[i*2+0] = static_cast<FLOAT>(w*2);
		levelInputSizes[i*2+1] = static_cast<FLOAT>(h*2);
		levelBuffers[i] = RSManager::getRTMan().createTexture(w, h, RenderTarget::FMT_ARGB_8);
		SDLOG(2, "Multi-level buffer %u: %p - size: %ux%u\n", i, &*levelBuffers[i], w, h);
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
	delete [] levelInputSizes;
}

void Scaler::reloadShader() {
	// Setup the defines for compiling the effect
	vector<D3DXMACRO> defines;

	D3DXMACRO srgbMacro = { "USE_SRGB", useSRGB ? "true" : "false" };
	defines.push_back(srgbMacro);

	D3DXMACRO null = { NULL, NULL };
	defines.push_back(null);

	DWORD flags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_OPTIMIZATION_LEVEL3;

	// Load effect from file
	SDLOG(0, "Scaler load\n");
	ID3DXBuffer* errors;
	ID3DXEffect* newEffect = NULL;
	HRESULT hr = D3DXCreateEffectFromFile(device, getAssetFileName("scaling.fx").c_str(), &defines.front(), NULL, flags, NULL, &newEffect, &errors);
	if(hr != D3D_OK) {
		SDLOG(0, "ERRORS:\n %s\n", errors->GetBufferPointer());
		Console::get().add("Error compiling scaling.fx:");
		Console::get().add(static_cast<const char*>(errors->GetBufferPointer()));
	} else {
		SAFERELEASE(effect);
		effect = newEffect;
	}

	// get handles
	frameTexHandle = effect->GetParameterByName(NULL, "frameTex2D");
	inputSizeHandle = effect->GetParameterByName(NULL, "inputSize");
}


void Scaler::go(IDirect3DTexture9 *input, IDirect3DSurface9 *dst) {
	if(sType == NEAREST) {
		IDirect3DSurface9* is = NULL;
		input->GetSurfaceLevel(0, &is);
		device->StretchRect(is, NULL, dst, NULL, D3DTEXF_NONE);
		SAFERELEASE(is);
		return;
	}

	device->SetVertexDeclaration(vertexDeclaration);
	IDirect3DSurface9 *finalDst = dst;

	for(unsigned i=0; i<=levels; i++) {
		if(i==levels) dst = finalDst;
		else dst = levelBuffers[i]->getSurf();
		UINT passes;
		device->SetRenderTarget(0, dst);
		effect->SetTexture(frameTexHandle, input);
		effect->SetFloatArray(inputSizeHandle, levelInputSizes + i * 2, 2);
		effect->Begin(&passes, 0);
		effect->BeginPass(sType);
		if(Settings::get().getMaintainAspectRatio() && i == 0) aspectQuad();
		else quad(width, height);
		effect->EndPass();
		effect->End();
		if(i < levels) input = levelBuffers[i]->getTex();
	}
}

void Scaler::setScaling(scalingType t) {
	sType = t;
	Console::get().add(format("Set scaling type to %s", getScalingName()));
}

void Scaler::nextScaling() {
	scalingType newSType = static_cast<scalingType>(sType+1);
	if(newSType == STYPE_END) newSType = BILINEAR;
	setScaling(newSType);
}

void Scaler::showStatus() {
	Console::get().add(format("%s (%s scaling)", status.c_str(), getScalingName()));
}

const char* Scaler::getScalingName() {
	switch(sType) {
	case BILINEAR: return "bilinear";
	case BICUBIC: return "bicubic";
	case LANCZOS: return "lanczos";
	case NEAREST: return "nearest";
	}
	return "unknown";
}

void Scaler::aspectQuad() {
	float w = 1.0, h = 1.0;
	double renderRatio = static_cast<double>(inputwidth) / inputheight;
	double targetRatio = static_cast<double>(width) / height;
	if(renderRatio > targetRatio) { // render res is wider
		h /= static_cast<float>(renderRatio / targetRatio);
	}
	else if(renderRatio < targetRatio) { // render res is taller
		w /= static_cast<float>(targetRatio / renderRatio);
	}

	D3DXVECTOR2 pixelSize = D3DXVECTOR2(1.0f / float(width), 1.0f / float(height));
	float quad[4][5] = {
		{ -w - pixelSize.x,  h + pixelSize.y, 0.5f, 0.0f, 0.0f },
		{  w - pixelSize.x,  h + pixelSize.y, 0.5f, 1.0f, 0.0f },
		{ -w - pixelSize.x, -h + pixelSize.y, 0.5f, 0.0f, 1.0f },
		{  w - pixelSize.x, -h + pixelSize.y, 0.5f, 1.0f, 1.0f }
	};
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(quad[0]));
}