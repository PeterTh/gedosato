#pragma once

#include "renderstate_manager.h"

#include "d3d11/d3d11.h"

class RSManagerDX11 : public RSManager {
	ID3D11Device* d3ddev = NULL;
	IDXGIFactory* dxgiFactory = NULL;
	IDXGISwapChain* dxgiSwapChain = NULL;

	ID3D11Texture2D* backBuffers[10];

public:
	RSManagerDX11(ID3D11Device* d3ddev) : RSManager(), d3ddev(d3ddev) {
		rtMan.reset(new RenderTargetManager(d3ddev));
	}

	HRESULT redirectCreateSwapChain(IDXGIFactory* factory, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain) {
		dxgiFactory = factory;
		HRESULT ret;
		SDLOG(2, "RedirectCreateSwapChain dev: %p | RSDev: %p\n", pDevice, d3ddev);
		if(Settings::getResSettings().setDSRes(pDesc->BufferDesc.Width, pDesc->BufferDesc.Height)) {
			SDLOG(1, " -> Downsampling resolution!\n");
			downsampling = true;
			renderWidth = pDesc->BufferDesc.Width;
			renderHeight = pDesc->BufferDesc.Height;
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

	HRESULT redirectPresent(UINT SyncInterval, UINT Flags) {
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
		return dxgiSwapChain->Present(SyncInterval, Flags);
	}


};
