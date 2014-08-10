#pragma once

#include "rendertarget.h"

#include "d3d9.h"

class RenderTargetManager {
	IDirect3DDevice9* device;

	// can be used by plugins to override the default buffer format
	// e.g. in case they are applying effects to FP16 buffers
	RenderTarget::Format overrideFormat = RenderTarget::FMT_NONE;

public:
	RenderTargetManager(IDirect3DDevice9* device) : device(device) {}

	void setOverrideFormat(RenderTarget::Format format) { overrideFormat = format; }

	RenderTargetPtr createTexture(unsigned width, unsigned height, RenderTarget::Format fmt);
	RenderTargetPtr createTexture(unsigned width, unsigned height, D3DFORMAT fmt);
	RenderTargetPtr createTexture(unsigned width, unsigned height, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE mstype, unsigned msquality);
	RenderTargetPtr createTexture(unsigned width, unsigned height);

	RenderTargetPtr createSurface(unsigned width, unsigned height, RenderTarget::Format fmt);
	RenderTargetPtr createSurface(unsigned width, unsigned height, D3DFORMAT fmt);
	RenderTargetPtr createSurface(unsigned width, unsigned height);
};
