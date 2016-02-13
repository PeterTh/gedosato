// wrapper for IDXGIDevice1 in dxgi.h
// generated using wrapper_gen11.rb

#include "settings.h"

// could be included from dxgidevice2
#ifndef hkIDXGIDevice1

#include "dxgi/dxgidevice1.h"

hkIDXGIDevice1::hkIDXGIDevice1(IDXGIDevice1 **ppIDXGIDevice1) {
	SDLOG(5, "hkIDXGIDevice1::hkIDXGIDevice1(%p)\n", *ppIDXGIDevice1);
	pWrapped = *ppIDXGIDevice1;
	*ppIDXGIDevice1 = this;
}

#endif // not def hkIDXGIDevice1  

#define hkIDXGIDevice hkIDXGIDevice1
#include "../../source/dxgi/dxgidevice.cpp"
#undef hkIDXGIDevice

HRESULT APIENTRY hkIDXGIDevice1::SetMaximumFrameLatency(UINT MaxLatency) {
	SDLOG(20, "hkIDXGIDevice1::SetMaximumFrameLatency\n");
	return pWrapped->SetMaximumFrameLatency(MaxLatency);
}

HRESULT APIENTRY hkIDXGIDevice1::GetMaximumFrameLatency(UINT *pMaxLatency) {
	SDLOG(20, "hkIDXGIDevice1::GetMaximumFrameLatency\n");
	return pWrapped->GetMaximumFrameLatency(pMaxLatency);
}
