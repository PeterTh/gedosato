#pragma once

#include <d3d11.h>
#include <string>

TCHAR* D3D11UsageToString(D3D11_USAGE usage);

std::string D3D11Texture2DDescToString(const D3D11_TEXTURE2D_DESC& desc);
