
#include "utils/d3d11_utils.h"

#include "utils/dxgi_utils.h"
#include "utils/string_utils.h"

TCHAR* D3D11UsageToString(D3D11_USAGE usage) {
	switch(usage) {
	case D3D11_USAGE_DEFAULT: return "D3D11_USAGE_DEFAULT";
	case D3D11_USAGE_IMMUTABLE: return "D3D11_USAGE_IMMUTABLE";
	case D3D11_USAGE_DYNAMIC: return "D3D11_USAGE_DYNAMIC";
	case D3D11_USAGE_STAGING: return "D3D11_USAGE_STAGING";
	}
	return "UNKNOWN_D3D11_USAGE";
}

std::string D3D11Texture2DDescToString(const D3D11_TEXTURE2D_DESC& desc) {
	typedef struct D3D11_TEXTURE2D_DESC {
		UINT             Width;
		UINT             Height;
		UINT             MipLevels;
		UINT             ArraySize;
		DXGI_FORMAT      Format;
		DXGI_SAMPLE_DESC SampleDesc;
		D3D11_USAGE      Usage;
		UINT             BindFlags;
		UINT             CPUAccessFlags;
		UINT             MiscFlags;
	} D3D11_TEXTURE2D_DESC;

	return format("Texture2DDesc:\n - size: %4ux%4u | mip: %2u | arr_s: %3u\n - format: %s\n - usage: %s | rt: %u | depth/stencil: %u\n - flags: bind=%p, cpu=%p, misc=%p\n",
		desc.Width, desc.Height, desc.MipLevels, desc.ArraySize,
		DxgiFormatToString(desc.Format), D3D11UsageToString(desc.Usage), desc.BindFlags & D3D11_BIND_RENDER_TARGET, desc.BindFlags & D3D11_BIND_DEPTH_STENCIL, 
		desc.BindFlags, desc.CPUAccessFlags, desc.MiscFlags);
}
