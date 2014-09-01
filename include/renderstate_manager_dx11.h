#pragma once

#include "renderstate_manager.h"

#include "d3d11/d3d11.h"

class RSManagerDX11 : public RSManager {
	ID3D11Device* d3ddev;

public:
	RSManagerDX11(ID3D11Device* d3ddev) : RSManager(), d3ddev(d3ddev) {
		rtMan.reset(new RenderTargetManager(d3ddev));
	}
};
