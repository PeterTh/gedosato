// wrapper for IDXGIAdapter2 in dxgi1_3.h
// generated using wrapper_gen11.rb

#include "settings.h"

// could be included from dxgiadapter3
#ifndef hkIDXGIAdapter2

#include "dxgi/dxgiadapter2.h"

hkIDXGIAdapter2::hkIDXGIAdapter2(IDXGIAdapter2 **ppIDXGIAdapter2) {
	SDLOG(5, "hkIDXGIAdapter2::hkIDXGIAdapter2(%p)\n", *ppIDXGIAdapter2);
	pWrapped = *ppIDXGIAdapter2;
	*ppIDXGIAdapter2 = this;
}

#endif // not def hkIDXGIAdapter2  

#define hkIDXGIAdapter1 hkIDXGIAdapter2
#include "dxgiadapter1.cpp"
#undef hkIDXGIAdapter

HRESULT APIENTRY hkIDXGIAdapter2::GetDesc2(DXGI_ADAPTER_DESC2 *pDesc) {
	SDLOG(20, "hkIDXGIAdapter2::GetDesc2\n");
	return pWrapped->GetDesc2(pDesc);
}
