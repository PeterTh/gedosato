#pragma once

#include <d3dx9.h>

class DepthTexture {
	IDirect3DTexture9* m_pTexture = NULL;
	IDirect3DSurface9* m_pSurface = NULL;
	bool m_isRESZ = false;
	bool m_isINTZ = false;
	bool m_isRAWZ = false;
	bool m_isSupported = false;


public:
	DepthTexture(const LPDIRECT3D9 d3d);
	~DepthTexture();

	void createTexture(const LPDIRECT3DDEVICE9 device, int width, int height);
	void resolveDepth(const LPDIRECT3DDEVICE9 device);

	IDirect3DTexture9* getTexture()	{ return m_pTexture; }
	IDirect3DSurface9* getSurface()	{ return m_pSurface; }
	bool isINTZ() { return m_isINTZ; }
	bool isSupported() { return m_isSupported; }
};
