#pragma once

#include <map>
#include <mutex>

#include <guiddef.h>
#include <Unknwn.h>

struct IDXGIObject;

extern GUID g_unwrapUUID;

bool operator<(const GUID& lhs, const GUID& rhs) noexcept;

/// Manages wrapped interfaces (thread-safe)
//  interface wrappers are responsible for their own registration and removal
//
class InterfaceRegistry {
	using IndexType = std::pair<void*, const IID>;
	std::map<IndexType, void*> registry;
	std::multimap<void*, IndexType> reverseReg;
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

	template<typename T>
	T* getWrappedInterface(void* original) {
		return static_cast<T*>(getWrappedInterface(original, __uuidof(T)));
	}

	bool isWrapper(void* loc);
};

