#include "imaging.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"
#undef STB_IMAGE_WRITE_IMPLEMENTATION

void ImageWriter::writeSurface(const string& fn, IDirect3DSurface9* surf) {
	D3DSURFACE_DESC desc;
	surf->GetDesc(&desc);
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = desc.Width;
	r.bottom = desc.Height;
	device9->StretchRect(surf, &r, tempSurf, &r, D3DTEXF_NONE);
	D3DLOCKED_RECT lockedR;
	tempSurf->LockRect(&lockedR, &r, D3DLOCK_READONLY);
	BYTE* buffer = new BYTE[lockedR.Pitch*desc.Height];
	memcpy(buffer, lockedR.pBits, lockedR.Pitch*desc.Height);
	INT pitch = lockedR.Pitch;
	tempSurf->UnlockRect();
	futures.push_back(std::async([buffer, desc, pitch, fn] {
		for(unsigned i = 0; i < pitch*desc.Height; i += 4) {
			// A8R8G8B8 --> RGB
			BYTE tmp = buffer[i + 0];
			buffer[i + 0] = buffer[i + 2];
			buffer[i + 1] = buffer[i + 1];
			buffer[i + 2] = tmp;
			buffer[i + 3] = 255;
		}
		stbi_write_png(fn.c_str(), desc.Width, desc.Height, 4, buffer, pitch);
		delete[] buffer;
	}));
}
