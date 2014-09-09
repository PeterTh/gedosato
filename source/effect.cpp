#include "Effect.h"
	
const D3DVERTEXELEMENT9 Effect::vertexElements[3] = {
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },
    D3DDECL_END()
};

void Effect::quad(int width, int height) {
	D3DXVECTOR2 pixelSize = D3DXVECTOR2(1.0f / float(width), 1.0f / float(height));
	float tri[3][5] = {
		{ -1.0f - pixelSize.x, 1.0f + pixelSize.y, 0.5f, 0.0f, 0.0f },
		{ 3.0f - pixelSize.x, 1.0f + pixelSize.y, 0.5f, 2.0f, 0.0f },
		{ -1.0f - pixelSize.x, -3.0f + pixelSize.y, 0.5f, 0.0f, 2.0f }
	};
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 1, tri, sizeof(tri[0]));
}
