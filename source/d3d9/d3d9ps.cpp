// wrapper for IDirect3DPixelShader9 in d3d9.h
// generated using wrapper_gen.rb

#include "d3d9/d3d9ps.h"

#include "settings.h"

hkIDirect3DPixelShader9::hkIDirect3DPixelShader9(IDirect3DPixelShader9 **ppIDirect3DPixelShader9) {
	m_pWrapped = *ppIDirect3DPixelShader9;
	*ppIDirect3DPixelShader9 = this;
}

HRESULT APIENTRY hkIDirect3DPixelShader9::QueryInterface(REFIID riid, void** ppvObj) {
	SDLOG(20, "hkIDirect3DPixelShader9::QueryInterface\n");
	return m_pWrapped->QueryInterface(riid, ppvObj);
}

ULONG APIENTRY hkIDirect3DPixelShader9::AddRef() {
	SDLOG(20, "hkIDirect3DPixelShader9::AddRef\n");
	return m_pWrapped->AddRef();
}

ULONG APIENTRY hkIDirect3DPixelShader9::Release() {
	SDLOG(20, "hkIDirect3DPixelShader9::Release\n");
	ULONG ret = m_pWrapped->Release();
	if(ret == 0) {
		delete this;
	}
	return ret;
}

HRESULT APIENTRY hkIDirect3DPixelShader9::GetDevice(IDirect3DDevice9** ppDevice) {
	SDLOG(20, "hkIDirect3DPixelShader9::GetDevice\n");
	return m_pWrapped->GetDevice(ppDevice);
}

HRESULT APIENTRY hkIDirect3DPixelShader9::GetFunction(void* param_1, UINT* pSizeOfData) {
	SDLOG(20, "hkIDirect3DPixelShader9::GetFunction\n");
	return m_pWrapped->GetFunction(param_1, pSizeOfData);
}
