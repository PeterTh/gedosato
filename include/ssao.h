#pragma once

#include <dxgi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "effect.h"
#include "rendertarget.h"

class SSAO : public Effect {
public:
	enum Type { VSSAO, HBAO, SCAO, VSSAO2 };
	enum Blur { BLUR_GAUSSIAN, BLUR_SHARP };

	SSAO(IDirect3DDevice9 *device, int width, int height, unsigned strength, Type type, Blur blur, bool useSRGB, bool readHWDepth);
    virtual ~SSAO();

	void go(IDirect3DTexture9 *frame, IDirect3DTexture9 *depth, IDirect3DSurface9 *dst);
	void goHDR(IDirect3DTexture9 *frame, IDirect3DTexture9 *depth, IDirect3DSurface9 *dst);

	void dumpFrame();

private:
	int width, height;
	size_t blurPasses;
	bool dumping;

	ID3DXEffect *effect;
	
	RenderTargetPtr buffer1, buffer2, hdrBuffer;

	D3DXHANDLE depthTexHandle, frameTexHandle, prevPassTexHandle;
	
	void mainSsaoPass(IDirect3DTexture9 *depth, IDirect3DSurface9 *dst);
	void vBlurPass(IDirect3DTexture9 *depth, IDirect3DTexture9* src, IDirect3DSurface9* dst);
	void hBlurPass(IDirect3DTexture9 *depth, IDirect3DTexture9* src, IDirect3DSurface9* dst);
	void combinePass(IDirect3DTexture9* frame, IDirect3DTexture9* ao, IDirect3DSurface9* dst);
	void combineShadowPass(IDirect3DTexture9 *shadow, IDirect3DTexture9* ao, IDirect3DSurface9* dst);

};
