#pragma once

#include <future>

#include "main.h"
#include "settings.h"
#include "d3d9.h"
#include "external/stb_image_write.h"

class ImageWriter {
	IDirect3DDevice9* device9;
	IDirect3DSurface9* tempSurf;
	std::vector<std::future<void>> futures;

	static const unsigned MAX_PARALLEL = 8;

public:
	ImageWriter(IDirect3DDevice9* dev, int wmax, int hmax) : device9(dev), tempSurf(NULL) {
		device9->CreateRenderTarget(wmax, hmax, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 0, TRUE, &tempSurf, NULL);
	}
	~ImageWriter() {
		for(auto& f : futures) {
			f.wait();
		}
		SAFERELEASE(tempSurf);
	}

	void writeSurface(const string& fn, IDirect3DSurface9* surf, bool discardAlpha = true);
};
