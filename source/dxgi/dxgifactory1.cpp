// wrapper for IDXGIFactory1 in dxgi.h
// generated using wrapper_gen11.rb

#include "dxgi/dxgifactory1.h"

#include "dxgi/dxgiswapchain.h"
#include "dxgi/dxgiadapter.h"
#include "dxgi/dxgiadapter1.h"
#include "settings.h"

hkIDXGIFactory1::hkIDXGIFactory1(IDXGIFactory1 **ppIDXGIFactory1) {
	pWrapped = *ppIDXGIFactory1;
	*ppIDXGIFactory1 = this;
}

#define hkIDXGIFactory hkIDXGIFactory1
#include "dxgifactory.cpp"
#undef hkIDXGIFactory

HRESULT APIENTRY hkIDXGIFactory1::EnumAdapters1(UINT Adapter, IDXGIAdapter1 **ppAdapter) {
	SDLOG(20, "hkIDXGIFactory1::EnumAdapters1\n");
	HRESULT res = pWrapped->EnumAdapters1(Adapter, ppAdapter);
	if(SUCCEEDED(res)) {
		SDLOG(20, "--> Success, wrapping %p\n", *ppAdapter);
		new hkIDXGIAdapter1(ppAdapter);
	}
	return res;
}

BOOL APIENTRY hkIDXGIFactory1::IsCurrent() {
	SDLOG(20, "hkIDXGIFactory1::IsCurrent\n");
	return pWrapped->IsCurrent();
}
