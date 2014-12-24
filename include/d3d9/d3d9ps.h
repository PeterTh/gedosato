// wrapper for IDirect3DPixelShader9 in d3d9.h
// generated using wrapper_gen.rb

#include "d3d9.h"

interface hkIDirect3DPixelShader9 : public IDirect3DPixelShader9 {
	IDirect3DPixelShader9 *m_pWrapped;
	
public:
	hkIDirect3DPixelShader9(IDirect3DPixelShader9 **ppIDirect3DPixelShader9);
	
	// original interface
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObj);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice);
	STDMETHOD(GetFunction)(void*, UINT* pSizeOfData);
};

