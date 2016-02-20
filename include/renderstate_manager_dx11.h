#pragma once

#include "renderstate_manager.h"

#include "d3d11/d3d11.h"
#include "key_actions.h"
#include "utils/interface_registry.h"

class RSManagerDX11 : public RSManager {
	ID3D11Device* d3ddev = NULL;
	IDXGIFactory* dxgiFactory = NULL;
	IDXGISwapChain* dxgiSwapChain = NULL;

	ID3D11Texture2D* backBuffers[10];

	ULONG internalRefCount = 0;

public:
	RSManagerDX11(ID3D11Device* d3ddev);
	virtual ~RSManagerDX11();

	void addInternalReferences(ULONG count);
	static ULONG getInternalRefCount();

	HRESULT redirectCreateSwapChain(IDXGIFactory* factory, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain);
	HRESULT redirectPresent(UINT SyncInterval, UINT Flags);

	virtual void showStatus() override;
};


class DX11InternalRefHelper {
	ID3D11Device* device;
	RSManagerDX11* manager;
	ULONG startCount;
public:
	DX11InternalRefHelper(ID3D11Device* device, RSManagerDX11* manager) : device(device), manager(manager) {
		device->AddRef();
		startCount = device->Release();
	}
	~DX11InternalRefHelper() {
		device->AddRef();
		ULONG newRefs = device->Release() - startCount;
		SDASSERT(newRefs >= 0, "DX11InternalRefHelper: lower final ref count");
		manager->addInternalReferences(newRefs);
	}
};
