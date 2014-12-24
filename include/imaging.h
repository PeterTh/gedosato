#pragma once

#include <future>
#include <queue>

#include "main.h"
#include "settings.h"
#include "d3d9.h"
#include "external/stb_image_write.h"

class ImageWriter {
	IDirect3DDevice9* device9;
	IDirect3DSurface9* tempSurf = nullptr;
	std::queue<std::future<void>> futures;
	int wmax, hmax;
	
public:
	ImageWriter(IDirect3DDevice9* dev, int wmax, int hmax);
	~ImageWriter();

	void writeSurface(const string& fn, IDirect3DSurface9* surf, bool discardAlpha = true);
};
