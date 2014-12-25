#pragma once

#include "d3d9.h"
#include <string>
using std::string;

IDirect3DTexture9* D3DGetSurfTexture(IDirect3DSurface9* pSurface);

TCHAR* D3DFormatToString(D3DFORMAT format, bool bWithPrefix = true);
TCHAR* D3DSamplerStateTypeToString(D3DSAMPLERSTATETYPE state);
TCHAR* D3DDeclTypeToString(D3DDECLTYPE type);
TCHAR* D3DDeclUsageToString(D3DDECLUSAGE type);
TCHAR* D3DDevTypeToString(D3DDEVTYPE type);
TCHAR* D3DResourceTypeToString(D3DRESOURCETYPE type);
TCHAR* D3DMultisampleTypeToString(D3DMULTISAMPLE_TYPE type);
TCHAR* D3DSwapEffectToString(D3DSWAPEFFECT swap);
TCHAR* D3DRenderStateTypeToString(D3DRENDERSTATETYPE s);
TCHAR* D3DPoolToString(D3DPOOL pool);
TCHAR* D3DPrimitiveTypeToString(D3DPRIMITIVETYPE prim);

string D3DMatrixToString(const D3DMATRIX* pMatrix);
string D3DPresentExFlagsToString(DWORD flags);
string D3DSurfaceDescToString(const D3DSURFACE_DESC& desc);

void logRenderState(IDirect3DDevice9* dev, int level=-1);
