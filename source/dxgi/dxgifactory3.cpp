// wrapper for IDXGIFactory3 in dxgi1_3.h
// generated using wrapper_gen11.rb

#include "settings.h"

// could be included from dxgifactory4
#ifndef hkIDXGIFactory3

#include "dxgi/dxgifactory3.h"

hkIDXGIFactory3::hkIDXGIFactory3(IDXGIFactory3 **ppIDXGIFactory3) {
	SDLOG(20, "hkIDXGIFactory3::hkIDXGIFactory3\n");
	pWrapped = *ppIDXGIFactory3;
	*ppIDXGIFactory3 = this;
}

#endif // not def hkIDXGIFactory3

#define hkIDXGIFactory2 hkIDXGIFactory3
#include "dxgifactory2.cpp"
#undef hkIDXGIFactory


UINT APIENTRY hkIDXGIFactory3::GetCreationFlags() {
	SDLOG(20, "hkIDXGIFactory3::GetCreationFlags\n");
	return pWrapped->GetCreationFlags();
}
