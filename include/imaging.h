#pragma once

#include <future>
#include <queue>

#include "main.h"
#include "settings.h"
#include "d3d9.h"
#include "external/stb_image_write.h"

class ImageWriter {
	IDirect3DDevice9* device9;
	IDirect3DSurface9* tempSurf = NULL;
	std::queue<std::future<void>> futures;
	int wmax, hmax;
	
public:
	ImageWriter(IDirect3DDevice9* dev, int wmax, int hmax) : device9(dev), tempSurf(NULL), wmax(wmax), hmax(hmax) {
		if(Settings::get().getMaxScreenshotParallelism() != -1)	device9->CreateRenderTarget(wmax, hmax, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 0, TRUE, &tempSurf, NULL);
	}
	~ImageWriter() {
		while(futures.size() > 0) {
			auto& f = futures.front();
			f.wait();
			futures.pop();
		}
		SAFERELEASE(tempSurf);
	}

	void writeSurface(const string& fn, IDirect3DSurface9* surf, bool discardAlpha = true);
};
