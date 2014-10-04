#pragma once

#include <dxgi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "effect.h"
#include "rendertarget.h"

class SSAO : public Effect {
public:
	enum Type { HBAO, VSSAO2, MSSAO, SAO };
	enum Blur { BLUR_GAUSSIAN, BLUR_SHARP };

	SSAO(IDirect3DDevice9 *device, int width, int height, unsigned strength, Type type, Blur blur, bool useSRGB, bool readHWDepth);
    virtual ~SSAO();

	void go(IDirect3DTexture9 *frame, IDirect3DTexture9 *depth, IDirect3DSurface9 *dst);
	void goHDR(IDirect3DTexture9 *frame, IDirect3DTexture9 *depth, IDirect3DSurface9 *dst);

	void dumpFrame();
	void reloadShader();

private:
	int width, height;
	size_t blurPasses;
	bool dumping = false;
	unsigned strength;
	Type type;
	Blur blur;
	bool useSRGB, readHWDepth;

	ID3DXEffect *effect = NULL;
	
	IDirect3DTexture9* noiseTex; // RandomNoiseB.png
	RenderTargetPtr buffer1, buffer2, hdrBuffer;
	IDirect3DTexture9 * pMipMap;

	D3DXHANDLE depthTexHandle, frameTexHandle, prevPassTexHandle, noiseTexHandle, isBlurHorizontalHandle;
	
	void mipMapPass(IDirect3DTexture9 *depth);
	void mainSsaoPass(IDirect3DTexture9 *depth, IDirect3DSurface9 *dst);
	void blurPass(IDirect3DTexture9 *depth, IDirect3DTexture9* src, IDirect3DSurface9* dst, bool horizontal);
	void combinePass(IDirect3DTexture9* frame, IDirect3DTexture9* ao, IDirect3DSurface9* dst);
	void combineShadowPass(IDirect3DTexture9 *shadow, IDirect3DTexture9* ao, IDirect3DSurface9* dst);
};
