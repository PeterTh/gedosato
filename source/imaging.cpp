#include "imaging.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"
#undef STB_IMAGE_WRITE_IMPLEMENTATION


void ImageWriter::writeSurface(const string& fn, IDirect3DSurface9* surf) {
	D3DSURFACE_DESC desc;
	tempSurf->GetDesc(&desc);
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = desc.Width;
	r.bottom = desc.Height;
	device9->StretchRect(surf, NULL, tempSurf, &r, D3DTEXF_NONE);
	D3DLOCKED_RECT lockedR;
	tempSurf->LockRect(&lockedR, &r, D3DLOCK_READONLY);
	stbi_write_png(fn.c_str(), desc.Width, desc.Height, 4, lockedR.pBits, lockedR.Pitch);
	tempSurf->UnlockRect();
}
