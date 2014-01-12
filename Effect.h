#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "main.h"

// Base class for effects
class Effect {
protected:
	IDirect3DDevice9* device;
    IDirect3DVertexDeclaration9 *vertexDeclaration;

    static const D3DVERTEXELEMENT9 vertexElements[3];

public:
	Effect(IDirect3DDevice9* device) : device(device) {
		device->CreateVertexDeclaration(vertexElements , &vertexDeclaration);
	}

	virtual ~Effect() {
		SAFERELEASE(vertexDeclaration);
	}

	void quad(int width, int height) {
		// Draw aligned fullscreen quad
		D3DXVECTOR2 pixelSize = D3DXVECTOR2(1.0f / float(width), 1.0f / float(height));
		float quad[4][5] = {
			{ -1.0f - pixelSize.x,  1.0f + pixelSize.y, 0.5f, 0.0f, 0.0f },
			{  1.0f - pixelSize.x,  1.0f + pixelSize.y, 0.5f, 1.0f, 0.0f },
			{ -1.0f - pixelSize.x, -1.0f + pixelSize.y, 0.5f, 0.0f, 1.0f },
			{  1.0f - pixelSize.x, -1.0f + pixelSize.y, 0.5f, 1.0f, 1.0f }
		};
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(quad[0]));
	}
};
