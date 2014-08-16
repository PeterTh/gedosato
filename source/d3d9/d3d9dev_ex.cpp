#include "d3d9/d3d9dev_ex.h"

#include "d3d9/d3d9swap.h"

// This is pretty terrible, but less terrible than lots of duplicate code
#define hkIDirect3D9 hkIDirect3D9Ex
#define hkIDirect3DDevice9 hkIDirect3DDevice9Ex
#include "d3d9dev.cpp"
#undef hkIDirect3D9

hkIDirect3DDevice9Ex::hkIDirect3DDevice9Ex(IDirect3DDevice9Ex **ppReturnedDeviceInterface, D3DPRESENT_PARAMETERS *pPresentParam, IDirect3D9Ex *pIDirect3D9Ex) {
	m_pD3Ddev = *ppReturnedDeviceInterface;
	*ppReturnedDeviceInterface = this;
	m_pD3Dint = pIDirect3D9Ex;
	rsMan = new RSManager(m_pD3Ddev, m_pD3Dint);
	RSManager::setLatest(rsMan);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// EX

HRESULT APIENTRY hkIDirect3DDevice9Ex::SetConvolutionMonoKernel(UINT width,UINT height,float* rows,float* columns) {
	RSManager::setLatest(rsMan);
	SDLOG(7, "SetConvolutionMonoKernel\n");
	return m_pD3Ddev->SetConvolutionMonoKernel(width, height, rows, columns);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::ComposeRects(IDirect3DSurface9* pSrc, IDirect3DSurface9* pDst, IDirect3DVertexBuffer9* pSrcRectDescs, UINT NumRects, 
													IDirect3DVertexBuffer9* pDstRectDescs, D3DCOMPOSERECTSOP Operation, int Xoffset, int Yoffset) {
	RSManager::setLatest(rsMan);
	SDLOG(7, "ComposeRects\n");
	return m_pD3Ddev->ComposeRects(pSrc, pDst, pSrcRectDescs, NumRects, pDstRectDescs, Operation, Xoffset, Yoffset);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::PresentEx(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags) {
	RSManager::setLatest(rsMan);
	SDLOG(3, "!!!!!!!!!!!!!!!!!!!!!!! PresentEx !!!!!!!!!!!!!!!!!!\n");
	return rsMan->redirectPresentEx(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::GetGPUThreadPriority(INT* pPriority) {
	RSManager::setLatest(rsMan);
	SDLOG(5, "GetGPUThreadPriority\n");
	return m_pD3Ddev->GetGPUThreadPriority(pPriority);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::SetGPUThreadPriority(INT Priority) {
	RSManager::setLatest(rsMan);
	SDLOG(5, "SetGPUThreadPriority\n");
	return m_pD3Ddev->SetGPUThreadPriority(Priority);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::WaitForVBlank(UINT iSwapChain) {
	RSManager::setLatest(rsMan);
	SDLOG(3, "WaitForVBlank\n");
	return m_pD3Ddev->WaitForVBlank(iSwapChain);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::CheckResourceResidency(IDirect3DResource9** pResourceArray, UINT32 NumResources) {
	RSManager::setLatest(rsMan);
	SDLOG(7, "CheckResourceResidency\n");
	return m_pD3Ddev->CheckResourceResidency(pResourceArray, NumResources);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::SetMaximumFrameLatency(UINT MaxLatency) {
	RSManager::setLatest(rsMan);
	SDLOG(2, "SetMaximumFrameLatency\n");
	return m_pD3Ddev->SetMaximumFrameLatency(MaxLatency);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::GetMaximumFrameLatency(UINT* pMaxLatency) {
	RSManager::setLatest(rsMan);
	SDLOG(2, "GetMaximumFrameLatency\n");
	return m_pD3Ddev->GetMaximumFrameLatency(pMaxLatency);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::CheckDeviceState(HWND hDestinationWindow) {
	RSManager::setLatest(rsMan);
	SDLOG(2, "CheckDeviceState\n");
	return m_pD3Ddev->CheckDeviceState(hDestinationWindow);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::CreateRenderTargetEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, 
															BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "CreateRenderTargetEx\n");
	return m_pD3Ddev->CreateRenderTargetEx(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle, Usage);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::CreateOffscreenPlainSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "CreateOffscreenPlainSurfaceEx\n");
	return m_pD3Ddev->CreateOffscreenPlainSurfaceEx(Width, Height, Format, Pool, ppSurface, pSharedHandle, Usage);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::CreateDepthStencilSurfaceEx(UINT Width, UINT Height,  D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, 
																   DWORD MultisampleQuality, BOOL Discard,IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "CreateDepthStencilSurfaceEx\n");
	return m_pD3Ddev->CreateDepthStencilSurfaceEx(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle, Usage);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::ResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode) {
	RSManager::setLatest(rsMan);
	SDLOG(0, "Reset ------\n");
	return rsMan->redirectResetEx(pPresentationParameters, pFullscreenDisplayMode);
}

HRESULT APIENTRY hkIDirect3DDevice9Ex::GetDisplayModeEx(UINT iSwapChain, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "GetDisplayModeEx %u\n", iSwapChain);
	return rsMan->redirectGetDisplayModeEx(iSwapChain, pMode, pRotation);
}
