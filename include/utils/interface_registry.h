#pragma once

#include <boost/bimap.hpp>
#include <mutex>

#include <guiddef.h>
#include <Unknwn.h>

struct IDXGIObject;

bool operator<(REFIID lhs, REFIID rhs) noexcept;

/// Manages wrapped interfaces (thread-safe)
//  interface wrappers are responsible for their own registration and removal
//
class InterfaceRegistry {
	using IndexType = std::pair<void*, const IID>;
	boost::bimap<IndexType, void*> registry;
	std::recursive_mutex mutex;

	InterfaceRegistry() {}

public:
	static InterfaceRegistry& get() {
		static InterfaceRegistry instance;
		return instance;
	}

	HRESULT QueryInterface(const char* name, IUnknown* object, REFIID riid, void **ppvObject);
	HRESULT GetParent(const char* name, IDXGIObject* object, REFIID riid, void **ppvObject);

	void* getWrappedInterface(void* original, REFIID iid);
	void unregisterWrapper(void* wrapper);
};

