// wrapper for IDXGIAdapter1 in dxgi.h
// generated using wrapper_gen11.rb

#include "settings.h"

// could be included from dxgiadapter2
#ifndef hkIDXGIAdapter1

#include "dxgi/dxgiadapter1.h"

hkIDXGIAdapter1::hkIDXGIAdapter1(IDXGIAdapter1 **ppIDXGIAdapter1) {
	SDLOG(5, "hkIDXGIAdapter1::hkIDXGIAdapter1(%p)\n", *ppIDXGIAdapter1);
	pWrapped = *ppIDXGIAdapter1;
	*ppIDXGIAdapter1 = this;
}

#endif // not def hkIDXGIAdapter1

#define hkIDXGIAdapter hkIDXGIAdapter1
#include "dxgiadapter.cpp"
#undef hkIDXGIAdapter

HRESULT APIENTRY hkIDXGIAdapter1::GetDesc1(DXGI_ADAPTER_DESC1 *pDesc) {
	SDLOG(20, "hkIDXGIAdapter1::GetDesc1\n");
	return pWrapped->GetDesc1(pDesc);
}
