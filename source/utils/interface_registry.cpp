#include "utils/interface_registry.h"

#include "settings.h"

#include "dxgi/dxgioutput.h"
#include "dxgi/dxgidevice.h"
#include "dxgi/dxgidevice1.h"
#include "dxgi/dxgidevice2.h"
#include "dxgi/dxgidevice3.h"
#include "dxgi/dxgiadapter.h"
#include "dxgi/dxgiadapter1.h"
#include "dxgi/dxgiadapter2.h"
#include "dxgi/dxgifactory.h"
#include "dxgi/dxgifactory1.h"
#include "dxgi/dxgifactory2.h"
#include "dxgi/dxgifactory3.h"
#include "dxgi/dxgiswapchain.h"

#include "d3d11/d3d11dev.h"
#include "d3d11/d3d11devcontext.h"

GUID g_unwrapUUID = { 31337, 42, 42, {0} };

bool operator<(const GUID& lhs, const GUID& rhs) noexcept {
	return memcmp(&lhs, &rhs, sizeof(GUID)) < 0;
}

HRESULT InterfaceRegistry::QueryInterface(const char* name, IUnknown* object, REFIID riid, void **ppvObject) {
	SDLOG(20, "%s::QueryInterface id: %d  ;  oIn: %p\n", name, riid, ppvObject ? *ppvObject : 0);
	SDLOG(20, " (this: %p  ;  pWrapped: %p)\n", this, object);
	if(riid == g_unwrapUUID) {
		SDLOG(20, " -> unwrap request, returning %p\n", object);
		*ppvObject = object;
		return S_OK;
	}
	auto hr = object->QueryInterface(riid, ppvObject);
	SDLOG(20, " -> hr: %d  ;  oOut: %p\n", hr, ppvObject ? *ppvObject : 0);
	if(SUCCEEDED(hr)) {
		*ppvObject = getWrappedInterface(*ppvObject, riid);
	}
	return hr;
}

HRESULT InterfaceRegistry::GetParent(const char* name, IDXGIObject* object, REFIID riid, void **ppvObject) {
	SDLOG(20, "%s::GetParent id: %d  ;  oIn: %p\n", name, riid, ppvObject ? *ppvObject : 0);
	SDLOG(20, " (this: %p  ;  pWrapped: %p)\n", this, object);
	auto hr = object->GetParent(riid, ppvObject);
	SDLOG(20, " -> hr: %d  ;  oOut: %p\n", hr, ppvObject ? *ppvObject : 0);
	if(SUCCEEDED(hr)) {
		*ppvObject = getWrappedInterface(*ppvObject, riid);
	}
	return hr;
}

void* InterfaceRegistry::getWrappedInterface(void* original, REFIID refiid) {
	std::lock_guard<std::recursive_mutex> guard(mutex);
	SDLOG(5, "InterfaceRegistry::getWrappedInterface %p // %s\n", original, refiid);
	IndexType id {original, refiid};
	SDLOG(10, " - looking for id: (%p, %s)\n", id.first, id.second);
	auto it = registry.find(id);
	if(it != registry.end()) {
		SDLOG(5, " -> found id (%p, %s), returning %p\n", it->first.first, it->first.second, it->second);
		return it->second;
	}
	SDLOG(5, " -> not found, creating\n");

	void *wrapper = nullptr;
	#define UUID_OBJECT_HOOKED(__UUID_OBJECT_) \
	if(refiid == __uuidof(__UUID_OBJECT_)) { \
		__UUID_OBJECT_ **orig_ptr = (__UUID_OBJECT_**)&original; \
		wrapper = new hk##__UUID_OBJECT_(orig_ptr); \
	}
	#include "uuids.def"

	if(!wrapper) {
		SDLOG(5, "WARNING InterfaceRegistry::getWrappedInterface: unknown UUID, no wrapper created (returning original %p)\n", original);
		return original;
	}
	else {
		SDLOG(5, " -> created, returning %p\n", wrapper);
		registry.insert({id, wrapper});
		reverseReg.insert({wrapper, id});
		return wrapper;
	}
}
void InterfaceRegistry::unregisterWrapper(void* wrapper) {
	std::lock_guard<std::recursive_mutex> guard(mutex);
	SDLOG(5, "InterfaceRegistry::unregisterWrapper %p\n", wrapper);
	// look up ids and erase in reg
	auto range = reverseReg.equal_range(wrapper);
	for(auto i = range.first; i!=range.second; ++i) {
		registry.erase(i->second);
	}
	// erase in reverse reg
	auto num = reverseReg.erase(wrapper);
	if(num == 0) {
		SDLOG(5, "WARNING InterfaceRegistry::unregisterWrapper: unexpected number of deletions: %d\n", num);
	}
}

bool InterfaceRegistry::isWrapper(void* loc) {
	std::lock_guard<std::recursive_mutex> guard(mutex);
	auto found = reverseReg.find(loc);
	return found != reverseReg.end();
}
