
#include "rendertarget_manager.h"

#include "renderstate_manager.h"

RenderTargetPtr RenderTargetManager::createTexture(unsigned width, unsigned height, RenderTarget::Format fmt) {
	return RenderTargetPtr(new RenderTarget(device, width, height, RenderTarget::d3dformatFromFormat(fmt), RenderTarget::TEXTURE_USE));
}

RenderTargetPtr RenderTargetManager::createTexture(unsigned width, unsigned height, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE mstype, unsigned msquality) {
	return RenderTargetPtr(new RenderTarget(device, width, height, fmt, RenderTarget::TEXTURE_USE, mstype, msquality));
}

RenderTargetPtr RenderTargetManager::createTexture(unsigned width, unsigned height, D3DFORMAT fmt) {
	return RenderTargetPtr(new RenderTarget(device, width, height, fmt, RenderTarget::TEXTURE_USE));
}

RenderTargetPtr RenderTargetManager::createTexture(unsigned width, unsigned height) {
	if(overrideFormat == RenderTarget::FMT_NONE) {
		return RenderTargetPtr(new RenderTarget(device, width, height, RenderTarget::d3dformatWithAlpha(RSManager::get().getBackBufferFormat()), RenderTarget::TEXTURE_USE));
	}
	else {
		return RenderTargetPtr(new RenderTarget(device, width, height, RenderTarget::d3dformatFromFormat(overrideFormat), RenderTarget::TEXTURE_USE));
	}
}

RenderTargetPtr RenderTargetManager::createSurface(unsigned width, unsigned height, RenderTarget::Format fmt) {
	return RenderTargetPtr(new RenderTarget(device, width, height, RenderTarget::d3dformatFromFormat(fmt), RenderTarget::SURFACE_USE));
}

RenderTargetPtr RenderTargetManager::createSurface(unsigned width, unsigned height, D3DFORMAT fmt) {
	return RenderTargetPtr(new RenderTarget(device, width, height, fmt, RenderTarget::SURFACE_USE));
}

RenderTargetPtr RenderTargetManager::createSurface(unsigned width, unsigned height) {
	if(overrideFormat == RenderTarget::FMT_NONE) {
		return RenderTargetPtr(new RenderTarget(device, width, height, RenderTarget::d3dformatWithAlpha(RSManager::get().getBackBufferFormat()), RenderTarget::SURFACE_USE));
	}
	else {
		return RenderTargetPtr(new RenderTarget(device, width, height, RenderTarget::d3dformatFromFormat(overrideFormat), RenderTarget::TEXTURE_USE));
	}
}
