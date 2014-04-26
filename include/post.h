#pragma once

#include <dxgi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "Effect.h"

class Post : public Effect {
public:
    Post(IDirect3DDevice9 *device, int width, int height);
    virtual ~Post();

	void go(IDirect3DTexture9 *frame, IDirect3DSurface9 *dst);

private:
	int width, height;

	ID3DXEffect *effect;

	D3DXHANDLE thisframeTexHandle;
};
