#include "renderstate_manager_dx11.h"


RSManagerDX11::RSManagerDX11(ID3D11Device* d3ddev) : RSManager(), d3ddev(d3ddev) {
	rtMan.reset(new RenderTargetManager(d3ddev));
	console.reset(new ConsoleDX11(d3ddev, this, renderWidth, renderHeight));
	Console::setLatest(console.get());
}

RSManagerDX11::~RSManagerDX11() {
	SDLOG(-1, "~RSManagerDX11\n");
	setLatest(nullptr);
}

void RSManagerDX11::addInternalReferences(ULONG count) {
	internalRefCount += count;
}

ULONG RSManagerDX11::getInternalRefCount() {
	if(latest == nullptr) return 0;
	return getDX11().internalRefCount;
}

HRESULT RSManagerDX11::redirectCreateSwapChain(IDXGIFactory* factory, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain) {
	SDLOG(-1, "RSManagerDX11::redirectCreateSwapChain\n");

	// unwrap device
	if(InterfaceRegistry::get().isWrapper(pDevice)) {
		pDevice->QueryInterface(g_unwrapUUID, (void**)&pDevice);
	}
	else {
		SDLOG(-1, "WARNING: redirectCreateSwapChain on non-intercepted device");
	}

	dxgiFactory = factory;
	HRESULT ret;
	renderWidth = pDesc->BufferDesc.Width;
	renderHeight = pDesc->BufferDesc.Height;
	SDLOG(2, "RedirectCreateSwapChain dev: %p | RSDev: %p\n", pDevice, d3ddev);
	console->setSize(renderWidth, renderHeight);

	if(Settings::getResSettings().setDSRes(pDesc->BufferDesc.Width, pDesc->BufferDesc.Height)) {
		SDLOG(1, " -> Downsampling resolution!\n");
		downsampling = true;
		numBackBuffers = pDesc->BufferCount;
		// modify swap chain settings
		DXGI_SWAP_CHAIN_DESC copy = *pDesc;
		copy.BufferDesc.Width = Settings::get().getPresentWidth();
		copy.BufferDesc.Height = Settings::get().getPresentHeight();
		copy.BufferDesc.RefreshRate = { Settings::get().getPresentHz(), 1 };
		copy.BufferCount = 1;
		// create swap chain
		SDLOG(1, " - redirectCreateSwapChain create downsampling swapchain\n");
		ret = dxgiFactory->CreateSwapChain(pDevice, &copy, ppSwapChain);
		if(SUCCEEDED(ret)) {
			SDLOG(1, " - redirectCreateSwapChain allocate backbuffers\n");
			// allocate downsampling size backbuffers
			D3D11_TEXTURE2D_DESC bufferDesc;
			bufferDesc.Width = renderWidth;
			bufferDesc.Height = renderHeight;
			bufferDesc.Format = pDesc->BufferDesc.Format;
			bufferDesc.SampleDesc = pDesc->SampleDesc;
			bufferDesc.MipLevels = 1;
			bufferDesc.ArraySize = 1;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			for(unsigned i = 0; i < numBackBuffers; ++i) {
				d3ddev->CreateTexture2D(&bufferDesc, NULL, &backBuffers[i]);
				SDLOG(1, " - redirectCreateSwapChain allocated backbuffers %u\n", i);
			}
		}
	}
	else {
		ret = dxgiFactory->CreateSwapChain(pDevice, pDesc, ppSwapChain);
	}

	if(SUCCEEDED(ret)) {
		dxgiSwapChain = *ppSwapChain;
	}
	return ret;
}

HRESULT RSManagerDX11::redirectPresent(UINT SyncInterval, UINT Flags) {
	SDLOG(-1, "RSManagerDX11::redirectPresent\n");

	//////////////////////////////////////////// IO
	KeyActions::get().processIO();
	//////////////////////////////////////////// IO

	if(downsampling) {
		ID3D11DeviceContext *context;
		d3ddev->GetImmediateContext(&context);
		ID3D11Texture2D *realBackBuffer;
		dxgiSwapChain->GetBuffer(0, __uuidof(realBackBuffer), reinterpret_cast<void**>(&realBackBuffer));
		ID3D11RenderTargetView* rtView;
		d3ddev->CreateRenderTargetView(realBackBuffer, NULL, &rtView);
		context->OMSetRenderTargets(1, &rtView, NULL);

		// TODO downsample

		context->Release();
	}
	if(console->needsDrawing()) {
		SDLOG(-1, "Draw stage 0");
		ID3D11DeviceContext *context = nullptr;
		d3ddev->GetImmediateContext(&context);
		if(context) {
			SDLOG(-1, "Draw stage 1");
			ID3D11Texture2D *realBackBuffer = nullptr;
			dxgiSwapChain->GetBuffer(0, __uuidof(realBackBuffer), reinterpret_cast<void**>(&realBackBuffer));
			if(realBackBuffer) {
				SDLOG(-1, "Draw stage 2");
				ID3D11RenderTargetView* rtView = nullptr;
				d3ddev->CreateRenderTargetView(realBackBuffer, NULL, &rtView);
				if(rtView) {
					SDLOG(-1, "Draw stage 3");
					context->OMSetRenderTargets(1, &rtView, nullptr);
					FLOAT clearCol[] = { 0.0f,0.0f,0.0f,0.0f };
					//context->ClearRenderTargetView(rtView, clearCol);
					//D3D11_VIEWPORT vp = {0.0f, 0.0f, renderWidth, renderHeight, 0.0f, 1.0f};
					//context->RSSetViewports(1, &vp);


					console->draw();

					rtView->Release();
				}
				realBackBuffer->Release();
			}
			context->Release();
		}
	}
	return dxgiSwapChain->Present(SyncInterval, Flags);
}

void RSManagerDX11::showStatus() {
	console->add("Hello DX11 World");
}
