#pragma once

#include <memory>

#include "d3d9.h"
#include "settings.h"

// Multi-API abstraction of a rendertarget
class RenderTarget {
public:
	typedef enum { SURFACE_USE = 0, TEXTURE_USE = 1 } Usage;
	typedef enum { FMT_NONE, FMT_ARGB_8, FMT_RGB_8, FMT_ARGB_16, FMT_RGB_16, FMT_ARGB_FP16, FMT_RGB_FP16 } Format;

	static D3DFORMAT d3dformatFromFormat(Format fmt) {
		switch(fmt) {
		case FMT_ARGB_8: return D3DFMT_A8R8G8B8;
		case FMT_RGB_8: return D3DFMT_R8G8B8;
		case FMT_ARGB_16: return D3DFMT_A16B16G16R16;
		case FMT_RGB_16: return D3DFMT_A16B16G16R16;
		case FMT_ARGB_FP16: return D3DFMT_A16B16G16R16F;
		case FMT_RGB_FP16: return D3DFMT_A16B16G16R16F;
		}
		SDLOG(-1, "ERROR: RenderTarget - unknown format\n");
		return D3DFMT_UNKNOWN;
	}
	static D3DFORMAT d3dformatWithAlpha(D3DFORMAT fmt) {
		switch(fmt) {
		case D3DFMT_X8R8G8B8: return D3DFMT_A8R8G8B8;
		case D3DFMT_X8B8G8R8: return D3DFMT_A8B8G8R8;
		}
		return fmt;
	}

	~RenderTarget() {
		SAFERELEASE(surf9);
		SAFERELEASE(tex9);
	}

	// TODO remove once migration is complete
	IDirect3DSurface9 *getSurf() {
		if(multisampling) dirty = true;
		return surf9; 
	};
	IDirect3DTexture9 *getTex() {
		if(multisampling && dirty) {
			IDirect3DSurface9* texsurf = NULL;
			tex9->GetSurfaceLevel(0, &texsurf);
			d3ddev->StretchRect(surf9, NULL, texsurf, NULL, D3DTEXF_NONE);
			SAFERELEASE(texsurf);
			dirty = false;
		}
		return tex9; 
	};

private:
	friend class RenderTargetManager;

	IDirect3DDevice9 *d3ddev;
	IDirect3DSurface9 *surf9;
	IDirect3DTexture9 *tex9;
	bool multisampling, dirty;

	RenderTarget(IDirect3DDevice9* dev, unsigned width, unsigned height, D3DFORMAT fmt, Usage usage, D3DMULTISAMPLE_TYPE mstype = D3DMULTISAMPLE_NONE, unsigned msquality = 0)
			: d3ddev(dev), surf9(NULL), tex9(NULL), multisampling(false), dirty(true) {
		multisampling = mstype != D3DMULTISAMPLE_NONE;
		if(usage == TEXTURE_USE) {
			dev->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, fmt, D3DPOOL_DEFAULT, &tex9, NULL);
			if(multisampling) dev->CreateRenderTarget(width, height, fmt, mstype, msquality, false, &surf9, NULL);
			else tex9->GetSurfaceLevel(0, &surf9);
		}
		else {
			dev->CreateRenderTarget(width, height, fmt, mstype, msquality, false, &surf9, NULL);
		}
	}

	RenderTarget(const RenderTarget& rt);
};

typedef std::unique_ptr<RenderTarget> RenderTargetPtr;
