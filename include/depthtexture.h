//-----------------------------------------------------------------------------
// author: Dmytro Shchukin
//-----------------------------------------------------------------------------
#ifndef DEPTH_TEXTURE_H
#define DEPTH_TEXTURE_H

#include <d3dx9.h>

//--------------------------------------------------------------------------------------
class DepthTexture
{
	LPDIRECT3DTEXTURE9		m_pTexture;
	LPDIRECT3DSURFACE9		m_pSurface;
	bool					m_isRESZ;
	bool					m_isINTZ;
	bool					m_isRAWZ;
	bool					m_isSupported;
public:

	DepthTexture(const LPDIRECT3D9 d3d);
	~DepthTexture();

	void				createTexture(const LPDIRECT3DDEVICE9 device, int width, int height);
	void				resolveDepth(const LPDIRECT3DDEVICE9 device);

	LPDIRECT3DTEXTURE9	getTexture()	{ return m_pTexture; }
	LPDIRECT3DSURFACE9	getSurface()	{ return m_pSurface; }
	bool				isINTZ()		{ return m_isINTZ; }
	bool				isSupported()	{ return m_isSupported; }
};

#endif // DEPTH_TEXTURE_H