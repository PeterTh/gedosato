#pragma once

#include <dxgi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "effect.h"
#include "timer.h"
#include "rendertarget.h"

class PostprocessingPass : public Effect {
public:
	PostprocessingPass(const string& shaderFn, IDirect3DDevice9 *device, int width, int height, bool useSRGB = true);
    virtual ~PostprocessingPass();

	void go(IDirect3DTexture9 *frame, IDirect3DSurface9 *dst, double time);
	void reloadShader();
	const string& getFn();

private:
	string shaderFn;
	int width, height;
	bool useSRGB;

	ID3DXEffect *effect = NULL;
	IDirect3DTexture9* noiseTex;	// NoiseTex.png

	D3DXHANDLE thisframeTexHandle;
	D3DXHANDLE timerHandle;
	D3DXHANDLE noiseTexHandle;
};

class Post {
public:
	Post(IDirect3DDevice9 *device, int width, int height, bool useSRGB = true, RenderTarget* tmp = nullptr);

	void go(IDirect3DTexture9 *frame, IDirect3DSurface9 *dst);
	void reloadShaders();

	void addProcessingPass(const string& effectFileName);

private:
	IDirect3DDevice9 *device;
	RenderTarget* tmp;
	int width, height;
	bool useSRGB;
	Timer timer;

	std::vector<std::shared_ptr<PostprocessingPass>> passes;
};
