#pragma once

#include <dxgi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "Effect.h"

class DOF : public Effect {
public:
	enum Type { BOKEH, BASIC };

    DOF(IDirect3DDevice9 *device, int width, int height, Type type, float baseRadius);
    virtual ~DOF();

	void go(IDirect3DTexture9 *frame, IDirect3DTexture9 *depth, IDirect3DSurface9 *dst);

private:
	int width, height;

	ID3DXEffect *effect;
	
	IDirect3DTexture9* buffer1Tex;
	IDirect3DSurface9* buffer1Surf;
	IDirect3DTexture9* buffer2Tex;
	IDirect3DSurface9* buffer2Surf;

	D3DXHANDLE thisframeTexHandle, depthTexHandle, lastpassTexHandle;
	
	void cocPass(IDirect3DTexture9 *frame, IDirect3DTexture9 *depth, IDirect3DSurface9 *dst);
	void dofPass(IDirect3DTexture9 *prev, IDirect3DSurface9* dst);
	void hBlurPass(IDirect3DTexture9 *prev, IDirect3DSurface9* dst);
	void vBlurPass(IDirect3DTexture9 *prev, IDirect3DSurface9* dst);
};
