// wrapper for IDXGIDevice3 in dxgi1_3.h
// generated using wrapper_gen11.rb

#include "settings.h"

// could be included from dxgidevice4
#ifndef hkIDXGIDevice3

#include "dxgi/dxgidevice3.h"

hkIDXGIDevice3::hkIDXGIDevice3(IDXGIDevice3 **ppIDXGIDevice3) {
	SDLOG(5, "hkIDXGIDevice3::hkIDXGIDevice3(%p)\n", *ppIDXGIDevice3);
	pWrapped = *ppIDXGIDevice3;
	*ppIDXGIDevice3 = this;
}

#endif // not def hkIDXGIDevice3 

#define hkIDXGIDevice2 hkIDXGIDevice3
#include "../../source/dxgi/dxgidevice2.cpp"
#undef hkIDXGIDevice

void APIENTRY hkIDXGIDevice3::Trim() {
	SDLOG(20, "hkIDXGIDevice3::Trim\n");
	return pWrapped->Trim();
}
