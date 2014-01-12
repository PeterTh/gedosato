#pragma once

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxerr.lib")

#include <d3d9.h>
#include <d3dx9.h>
#include "d3d9int.h"
#include "d3d9dev.h"

#define EXPORTED(__rettype, __convention, __name, ...) \
__rettype __convention __name(__VA_ARGS__);
#include "Exports.def"
