#pragma once

#include "d3d9.h"
#include <string>
using std::string;

TCHAR* D3DFormatToString(D3DFORMAT format, bool bWithPrefix = true);
TCHAR* D3DSamplerStateTypeToString(D3DSAMPLERSTATETYPE state);
TCHAR* D3DDeclTypeToString(D3DDECLTYPE type);
TCHAR* D3DDeclUsageToString(D3DDECLUSAGE type);
TCHAR* D3DDevTypeToString(D3DDEVTYPE type);
TCHAR* D3DResourceTypeToString(D3DRESOURCETYPE type);
TCHAR* D3DMultisampleTypeToString(D3DMULTISAMPLE_TYPE type);

string D3DMatrixToString(const D3DMATRIX* pMatrix);
