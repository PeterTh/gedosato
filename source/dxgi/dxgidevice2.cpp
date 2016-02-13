// wrapper for IDXGIDevice2 in dxgi1_2.h
// generated using wrapper_gen11.rb

#include "settings.h"

// could be included from dxgidevice3
#ifndef hkIDXGIDevice2

#include "dxgi/dxgidevice2.h"

hkIDXGIDevice2::hkIDXGIDevice2(IDXGIDevice2 **ppIDXGIDevice2) {
	SDLOG(5, "hkIDXGIDevice2::hkIDXGIDevice2(%p)\n", *ppIDXGIDevice2);
	pWrapped = *ppIDXGIDevice2;
	*ppIDXGIDevice2 = this;
}

#endif // not def hkIDXGIDevice2  

#define hkIDXGIDevice1 hkIDXGIDevice2
#include "../../source/dxgi/dxgidevice1.cpp"
#undef hkIDXGIDevice

HRESULT APIENTRY hkIDXGIDevice2::OfferResources(UINT NumResources, IDXGIResource *const *ppResources, DXGI_OFFER_RESOURCE_PRIORITY Priority) {
	SDLOG(20, "hkIDXGIDevice2::OfferResources\n");
	return pWrapped->OfferResources(NumResources, ppResources, Priority);
}

HRESULT APIENTRY hkIDXGIDevice2::ReclaimResources(UINT NumResources, IDXGIResource *const *ppResources, BOOL *pDiscarded) {
	SDLOG(20, "hkIDXGIDevice2::ReclaimResources\n");
	return pWrapped->ReclaimResources(NumResources, ppResources, pDiscarded);
}

HRESULT APIENTRY hkIDXGIDevice2::EnqueueSetEvent(HANDLE hEvent) {
	SDLOG(20, "hkIDXGIDevice2::EnqueueSetEvent\n");
	return pWrapped->EnqueueSetEvent(hEvent);
}
