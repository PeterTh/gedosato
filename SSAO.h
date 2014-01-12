#pragma once

#include <dxgi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "Effect.h"

class SSAO : public Effect {
public:
	enum Type { VSSAO, HBAO, SCAO, VSSAO2 };

    SSAO(IDirect3DDevice9 *device, int width, int height, unsigned strength, Type type);
    virtual ~SSAO();

	void go(IDirect3DTexture9 *frame, IDirect3DTexture9 *depth, IDirect3DSurface9 *dst);

private:
	int width, height;

	ID3DXEffect *effect;
	
	IDirect3DTexture9* buffer1Tex;
	IDirect3DSurface9* buffer1Surf;
	IDirect3DTexture9* buffer2Tex;
	IDirect3DSurface9* buffer2Surf;

	D3DXHANDLE depthTexHandle, frameTexHandle, prevPassTexHandle;
	
	void mainSsaoPass(IDirect3DTexture9 *depth, IDirect3DSurface9 *dst);
	void vBlurPass(IDirect3DTexture9 *depth, IDirect3DTexture9* src, IDirect3DSurface9* dst);
	void hBlurPass(IDirect3DTexture9 *depth, IDirect3DTexture9* src, IDirect3DSurface9* dst);
	void combinePass(IDirect3DTexture9* frame, IDirect3DTexture9* ao, IDirect3DSurface9* dst);
};
