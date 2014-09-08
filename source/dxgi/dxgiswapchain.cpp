// wrapper for IDXGISwapChain in dxgi.h
// generated using wrapper_gen11.rb

#include "dxgi/dxgiswapchain.h"

#include "settings.h"
#include "renderstate_manager_dx11.h"

hkIDXGISwapChain::hkIDXGISwapChain(IDXGISwapChain **ppIDXGISwapChain, RSManagerDX11* rsMan) : rsMan(rsMan) {
	pWrapped = *ppIDXGISwapChain;
	*ppIDXGISwapChain = this;
}

HRESULT APIENTRY hkIDXGISwapChain::QueryInterface(REFIID riid, void **ppvObject) {
	SDLOG(20, "hkIDXGISwapChain::QueryInterface\n");
	return pWrapped->QueryInterface(riid, ppvObject);
}

ULONG APIENTRY hkIDXGISwapChain::AddRef() {
	SDLOG(20, "hkIDXGISwapChain::AddRef\n");
	return pWrapped->AddRef();
}

ULONG APIENTRY hkIDXGISwapChain::Release() {
	SDLOG(20, "hkIDXGISwapChain::Release\n");
	return pWrapped->Release();
}

HRESULT APIENTRY hkIDXGISwapChain::SetPrivateData(REFGUID Name, UINT DataSize, const void *pData) {
	SDLOG(20, "hkIDXGISwapChain::SetPrivateData\n");
	return pWrapped->SetPrivateData(Name, DataSize, pData);
}

HRESULT APIENTRY hkIDXGISwapChain::SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown) {
	SDLOG(20, "hkIDXGISwapChain::SetPrivateDataInterface\n");
	return pWrapped->SetPrivateDataInterface(Name, pUnknown);
}

HRESULT APIENTRY hkIDXGISwapChain::GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData) {
	SDLOG(20, "hkIDXGISwapChain::GetPrivateData\n");
	return pWrapped->GetPrivateData(Name, pDataSize, pData);
}

HRESULT APIENTRY hkIDXGISwapChain::GetParent(REFIID riid, void **ppParent) {
	SDLOG(20, "hkIDXGISwapChain::GetParent\n");
	return pWrapped->GetParent(riid, ppParent);
}

HRESULT APIENTRY hkIDXGISwapChain::GetDevice(REFIID riid, void **ppDevice) {
	SDLOG(20, "hkIDXGISwapChain::GetDevice\n");
	return pWrapped->GetDevice(riid, ppDevice);
}

HRESULT APIENTRY hkIDXGISwapChain::Present(UINT SyncInterval, UINT Flags) {
	SDLOG(20, "hkIDXGISwapChain::Present\n");
	return rsMan->redirectPresent(SyncInterval, Flags);
}

HRESULT APIENTRY hkIDXGISwapChain::GetBuffer(UINT Buffer, REFIID riid, void **ppSurface) {
	SDLOG(20, "hkIDXGISwapChain::GetBuffer\n");
	return pWrapped->GetBuffer(Buffer, riid, ppSurface);
}

HRESULT APIENTRY hkIDXGISwapChain::SetFullscreenState(BOOL Fullscreen, IDXGIOutput *pTarget) {
	SDLOG(20, "hkIDXGISwapChain::SetFullscreenState\n");
	return pWrapped->SetFullscreenState(Fullscreen, pTarget);
}

HRESULT APIENTRY hkIDXGISwapChain::GetFullscreenState(BOOL *pFullscreen, IDXGIOutput **ppTarget) {
	SDLOG(20, "hkIDXGISwapChain::GetFullscreenState\n");
	return pWrapped->GetFullscreenState(pFullscreen, ppTarget);
}

HRESULT APIENTRY hkIDXGISwapChain::GetDesc(DXGI_SWAP_CHAIN_DESC *pDesc) {
	SDLOG(20, "hkIDXGISwapChain::GetDesc\n");
	return pWrapped->GetDesc(pDesc);
}

HRESULT APIENTRY hkIDXGISwapChain::ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	SDLOG(20, "hkIDXGISwapChain::ResizeBuffers\n");
	return pWrapped->ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

HRESULT APIENTRY hkIDXGISwapChain::ResizeTarget(const DXGI_MODE_DESC *pNewTargetParameters) {
	SDLOG(20, "hkIDXGISwapChain::ResizeTarget\n");
	return pWrapped->ResizeTarget(pNewTargetParameters);
}

HRESULT APIENTRY hkIDXGISwapChain::GetContainingOutput(IDXGIOutput **ppOutput) {
	SDLOG(20, "hkIDXGISwapChain::GetContainingOutput\n");
	return pWrapped->GetContainingOutput(ppOutput);
}

HRESULT APIENTRY hkIDXGISwapChain::GetFrameStatistics(DXGI_FRAME_STATISTICS *pStats) {
	SDLOG(20, "hkIDXGISwapChain::GetFrameStatistics\n");
	return pWrapped->GetFrameStatistics(pStats);
}

HRESULT APIENTRY hkIDXGISwapChain::GetLastPresentCount(UINT *pLastPresentCount) {
	SDLOG(20, "hkIDXGISwapChain::GetLastPresentCount\n");
	return pWrapped->GetLastPresentCount(pLastPresentCount);
}
