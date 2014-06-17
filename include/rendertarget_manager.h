#pragma once

#include "rendertarget.h"

#include "d3d9.h"

class RenderTargetManager {
	IDirect3DDevice9* device;

public:
	RenderTargetManager(IDirect3DDevice9* device) : device(device) {
	}

	RenderTargetPtr createTexture(unsigned width, unsigned height, RenderTarget::Format fmt) {
		return RenderTargetPtr(new RenderTarget(device, width, height, RenderTarget::d3dformatFromFormat(fmt), RenderTarget::TEXTURE_USE));
	}
	RenderTargetPtr createSurface(unsigned width, unsigned height, RenderTarget::Format fmt) {
		return RenderTargetPtr(new RenderTarget(device, width, height, RenderTarget::d3dformatFromFormat(fmt), RenderTarget::SURFACE_USE));
	}
	RenderTargetPtr createSurface(unsigned width, unsigned height, D3DFORMAT fmt) {
		return RenderTargetPtr(new RenderTarget(device, width, height, fmt, RenderTarget::SURFACE_USE));
	}
};
