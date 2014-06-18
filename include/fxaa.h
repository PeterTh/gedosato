#pragma once

#include <dxgi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "effect.h"
#include "rendertarget.h"

class FXAA : public Effect {
public:
	enum Quality { QualityLow, QualityMedium, QualityHigh, QualityUltra };

	FXAA(IDirect3DDevice9 *device, int width, int height, Quality quality, bool useSRGB = true);
    virtual ~FXAA();

	void go(IDirect3DTexture9 *frame, IDirect3DSurface9 *dst);

private:
	int width, height;

	ID3DXEffect *effect;
	
	RenderTargetPtr buffer;

	D3DXHANDLE frameTexHandle;
	
	void lumaPass(IDirect3DTexture9 *frame, IDirect3DSurface9 *dst);
	void fxaaPass(IDirect3DTexture9 *src, IDirect3DSurface9* dst);
};
