#pragma once

#include <dxgi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "Effect.h"

class Bloom : public Effect {
public:
    Bloom(IDirect3DDevice9 *device, int width, int height, float cutoff, float strength, float dirtStrength);
    virtual ~Bloom();

	void go(IDirect3DTexture9 *hdrFrame, IDirect3DTexture9 *composeFrame, IDirect3DSurface9 *dst);

	void dumpFrame();

private:
	int width, height, minWidth, minHeight;
	float cutoff, strength, dirtStrength, power;
	unsigned steps;
	bool dumping;
	struct Size { int w; int h; };

	ID3DXEffect *effect;
	IDirect3DTexture9 *dirtTexture;
	IDirect3DSurface9 *readbackSurf;
	
	struct StepBuffer {
		IDirect3DTexture9* textures[2];
		IDirect3DSurface9* surfaces[2];
	};

	// we can afford this
	StepBuffer stepBuffers[20];
	Size stepSizes[20];

	D3DXHANDLE inputPixelMetricsHandle, invStepsHandle, totalPowerHandle;
	D3DXHANDLE sampleTexHandle, passTexHandle, dirtTexHandle;
	D3DXHANDLE initialCutoffAndDownsampleHandle, gaussianHandle, integrateUpwardsHandle, finalComposeHandle;
	
	void initialPass(IDirect3DTexture9* src, IDirect3DSurface9* dst);
	void blurPass(IDirect3DTexture9* src, IDirect3DSurface9* dst, int sw, int sh, bool horizontal);
	void upPass(IDirect3DTexture9* src, IDirect3DSurface9* dst);
	void finalPass(IDirect3DTexture9* src, IDirect3DTexture9* frame, IDirect3DSurface9* dst);
};
