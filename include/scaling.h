#pragma once

#include <dxgi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "Effect.h"

class Scaler : public Effect {
public:
	typedef enum { BILINEAR, BICUBIC } scalingType;
	
	Scaler(IDirect3DDevice9 *device, int inputwidth, int inputheight, int width, int height, bool useSRGB = false);
    virtual ~Scaler();

	void go(IDirect3DTexture9 *input, IDirect3DSurface9 *dst);

	void setScaling(scalingType t);

	void showStatus();

private:
	int inputwidth, inputheight, width, height;
	scalingType sType;

	ID3DXEffect *effect;
	
	D3DXHANDLE frameTexHandle;
	D3DXHANDLE inputSizeHandle;
	
	unsigned levels;
	IDirect3DTexture9** levelBufferTextures;
	IDirect3DSurface9** levelBuffers;
	FLOAT *levelInputSizes;

	string status;

	const char* getScalingName();
};
