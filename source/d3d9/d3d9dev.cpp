#include <fstream>
#include <ostream>
#include <iostream>
#include <list>

#include "utils/win_utils.h"
#include "main.h"
#include "d3d9.h"
#include "utils/d3d9_utils.h"
#include "renderstate_manager_dx9.h"
#include "window_manager.h"
#include "Settings.h"

#ifndef hkIDirect3D9 
#include "d3d9/d3d9swap.h" // only include if we're not in d3ddevex
#endif

using namespace std;

HRESULT APIENTRY hkIDirect3DDevice9::Present(CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion) {
	RSManager::setLatest(rsMan);
	SDLOG(3, "!!!!!!!!!!!!!!!!!!!!!!! Present !!!!!!!!!!!!!!!!!!\n");
	return rsMan->redirectPresent(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "SetVertexShaderConstantF: start: %u, count: %u\n", StartRegister, Vector4fCount);
	if(Settings::get().getLogLevel() > 13) {
		for(size_t i=0; i<Vector4fCount; ++i) {
			SDLOG(0, " - %16.10f %16.10f %16.10f %16.10f\n", pConstantData[i*4+0], pConstantData[i*4+1], pConstantData[i*4+2], pConstantData[i*4+3]);
		}
	}
	return rsMan->redirectSetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) {
	RSManager::setLatest(rsMan);
	SDLOG(3, "SetRenderTarget %5d, %p\n", RenderTargetIndex, pRenderTarget);
	return rsMan->redirectSetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9* pvShader) {
	RSManager::setLatest(rsMan);
	SDLOG(3, "SetVertexShader %p, name: %s\n", pvShader, rsMan->getShaderManager().getName(pvShader));
	return rsMan->redirectSetVertexShader(pvShader);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetViewport(CONST D3DVIEWPORT9 *pViewport) {
	RSManager::setLatest(rsMan);
	SDLOG(6, "SetViewport X / Y - W x H : %4lu / %4lu  -  %4lu x %4lu ; Z: %f - %f\n", pViewport->X, pViewport->Y, pViewport->Width, pViewport->Height, pViewport->MinZ, pViewport->MaxZ);
	return rsMan->redirectSetViewport(pViewport); 
}

HRESULT APIENTRY hkIDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
	RSManager::setLatest(rsMan);
	SDLOG(9, "DrawIndexedPrimitive\n");
	return rsMan->redirectDrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) {
	RSManager::setLatest(rsMan);
	SDLOG(9, "DrawIndexedPrimitiveUP(%d, %u, %u, %u, %u, %p, %d, %p, %d)\n", PrimitiveType, MinIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
	return rsMan->redirectDrawIndexedPrimitiveUP(PrimitiveType, MinIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT APIENTRY hkIDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
	RSManager::setLatest(rsMan);
	SDLOG(9, "DrawPrimitive\n");
	return rsMan->redirectDrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride) {
	RSManager::setLatest(rsMan);
	SDLOG(9, "DrawPrimitiveUP(%d, %u, %p, %u)\n", PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
	return rsMan->redirectDrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT APIENTRY hkIDirect3DDevice9::DrawRectPatch(UINT Handle, CONST float *pNumSegs, CONST D3DRECTPATCH_INFO *pRectPatchInfo) {
	RSManager::setLatest(rsMan);
	SDLOG(9, "DrawRectPatch\n");
	return m_pD3Ddev->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT APIENTRY hkIDirect3DDevice9::DrawTriPatch(UINT Handle, CONST float *pNumSegs, CONST D3DTRIPATCH_INFO *pTriPatchInfo) {
	RSManager::setLatest(rsMan);
	SDLOG(9, "DrawTriPatch\n");
	return m_pD3Ddev->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) {
	RSManager::setLatest(rsMan);
	if(Type != D3DBACKBUFFER_TYPE_MONO) SDLOG(0, "Backbuffer type not mono!!");
	SDLOG(4, "GetBackBuffer (swap %u) %u\n", iSwapChain, iBackBuffer);
	return rsMan->redirectGetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT APIENTRY hkIDirect3DDevice9::EndScene() {
	RSManager::setLatest(rsMan);
	SDLOG(7, "EndScene\n");
	return m_pD3Ddev->EndScene();
}

HRESULT APIENTRY hkIDirect3DDevice9::QueryInterface(REFIID riid, LPVOID *ppvObj) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->QueryInterface(riid, ppvObj);
}

ULONG APIENTRY hkIDirect3DDevice9::AddRef() {
	RSManager::setLatest(rsMan);	
	SDLOG(0, "Device %p add ref on thread %u\n", m_pD3Ddev, GetCurrentThreadId());
	ULONG ret = m_pD3Ddev->AddRef();
	SDLOG(0, " -> %u\n", ret);
	return ret;
}

HRESULT APIENTRY hkIDirect3DDevice9::BeginScene() {
	RSManager::setLatest(rsMan);
	SDLOG(7, "BeginScene\n");
	return m_pD3Ddev->BeginScene();
}

HRESULT APIENTRY hkIDirect3DDevice9::BeginStateBlock() {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->BeginStateBlock();
}

HRESULT APIENTRY hkIDirect3DDevice9::Clear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "Clear\n");
	return rsMan->redirectClear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT APIENTRY hkIDirect3DDevice9::ColorFill(IDirect3DSurface9* pSurface,CONST RECT* pRect, D3DCOLOR color) {	
	RSManager::setLatest(rsMan);
	SDLOG(6, "ColorFill\n");
	return m_pD3Ddev->ColorFill(pSurface,pRect,color);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain) {
	RSManager::setLatest(rsMan);
	SDLOG(0, "CreateAdditionalSwapChain\n");
	Console::get().add("WARNING: Application called CreateAdditionalSwapChain, not currently supported by GeDoSaTo!");
	return m_pD3Ddev->CreateAdditionalSwapChain(pPresentationParameters, ppSwapChain);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture,pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "CreateDepthStencilSurface w/h: %4u/%4u  format: %s\n", Width, Height, D3DFormatToString(Format));
	return rsMan->redirectCreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer,pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "CreateOffscreenPlainSurface w/h: %4u/%4u  format: %s\n", Width, Height, D3DFormatToString(Format));
	return m_pD3Ddev->CreateOffscreenPlainSurface(Width,Height,Format,Pool,ppSurface,pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreatePixelShader(CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader) {
	RSManager::setLatest(rsMan);
	HRESULT hr = m_pD3Ddev->CreatePixelShader(pFunction, ppShader);
	if(SUCCEEDED(hr)) {
		rsMan->getShaderManager().registerShader(pFunction, *ppShader);
	}
	return hr;
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->CreateQuery(Type, ppQuery);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	RSManager::setLatest(rsMan);
	HRESULT ret = rsMan->redirectCreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
	//HRESULT ret = m_pD3Ddev->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
	SDLOG(1, "CreateRenderTarget w/h: %4u/%4u  format: %s\n  pointer: %p", Width, Height, D3DFormatToString(Format), *ppSurface);
	return ret;
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->CreateStateBlock(Type, ppSB);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	RSManager::setLatest(rsMan);
	SDLOG(1, "CreateTexture w/h: %4u/%4u    format: %s    RENDERTARGET=%d\n", Width, Height, D3DFormatToString(Format), Usage & D3DUSAGE_RENDERTARGET);
	return rsMan->redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** VERTexBuffer, HANDLE* pSharedHandle) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "CreateVertexBuffer: length %d, usage: %p, fvf: %p, pool: %d\n", Length, Usage, FVF, Pool);
	HRESULT retval = m_pD3Ddev->CreateVertexBuffer(Length, Usage, FVF, Pool, VERTexBuffer, pSharedHandle);
	SDLOG(4, " --> %p\n", *VERTexBuffer);
	return retval;
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->CreateVertexDeclaration(pVertexElements,ppDecl);
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateVertexShader(CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader) {
	RSManager::setLatest(rsMan);
	HRESULT hr = m_pD3Ddev->CreateVertexShader(pFunction, ppShader);
	if(SUCCEEDED(hr)) {
		rsMan->getShaderManager().registerShader(pFunction, *ppShader);
	}
	return hr;
}

HRESULT APIENTRY hkIDirect3DDevice9::CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture,pSharedHandle);
}

HRESULT APIENTRY hkIDirect3DDevice9::DeletePatch(UINT Handle) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->DeletePatch(Handle);
}

HRESULT APIENTRY hkIDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->EndStateBlock(ppSB);
}

HRESULT APIENTRY hkIDirect3DDevice9::EvictManagedResources() {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->EvictManagedResources();
}

UINT APIENTRY hkIDirect3DDevice9::GetAvailableTextureMem() {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetAvailableTextureMem();
}

HRESULT APIENTRY hkIDirect3DDevice9::GetClipPlane(DWORD Index, float *pPlane) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetClipPlane(Index, pPlane);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetClipStatus(pClipStatus);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetCreationParameters(pParameters);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetCurrentTexturePalette(UINT *pPaletteNumber) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetCurrentTexturePalette(pPaletteNumber);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "GetDepthStencilSurface\n");
	return rsMan->redirectGetDepthStencilSurface(ppZStencilSurface);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetDeviceCaps(D3DCAPS9 *pCaps) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetDeviceCaps(pCaps);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetDirect3D(IDirect3D9 **ppD3D9) {
	RSManager::setLatest(rsMan);
	HRESULT hRet = m_pD3Ddev->GetDirect3D(ppD3D9);
	if( SUCCEEDED(hRet) )
		*ppD3D9 = m_pD3Dint;
	return hRet;
}

HRESULT APIENTRY hkIDirect3DDevice9::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "GetDisplayMode %u\n", iSwapChain);
	return rsMan->redirectGetDisplayMode(iSwapChain, pMode);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "GetFrontBufferData %u\n", iSwapChain);
	return m_pD3Ddev->GetFrontBufferData(iSwapChain, pDestSurface);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetFVF(DWORD* pFVF) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetFVF(pFVF);
}

void APIENTRY hkIDirect3DDevice9::GetGammaRamp(UINT iSwapChain,D3DGAMMARAMP* pRamp) {
	RSManager::setLatest(rsMan);
	m_pD3Ddev->GetGammaRamp(iSwapChain,pRamp);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9** ppIndexData) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetIndices(ppIndexData);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetLight(DWORD Index, D3DLIGHT9 *pLight) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetLight(Index, pLight);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetLightEnable(DWORD Index, BOOL *pEnable) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetLightEnable(Index, pEnable);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetMaterial(D3DMATERIAL9 *pMaterial) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetMaterial(pMaterial);
}

float APIENTRY hkIDirect3DDevice9::GetNPatchMode() {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetNPatchMode();
}

unsigned int APIENTRY hkIDirect3DDevice9::GetNumberOfSwapChains() {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetNumberOfSwapChains();
}

HRESULT APIENTRY hkIDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY *pEntries) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9** ppShader) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetPixelShader(ppShader);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetPixelShaderConstantB(StartRegister,pConstantData,BoolCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetRasterStatus(UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetRasterStatus(iSwapChain,pRasterStatus);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD *pValue) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetRenderState(State, pValue);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetRenderTarget(DWORD renderTargetIndex, IDirect3DSurface9** ppRenderTarget) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "GetRenderTarget %u\n", renderTargetIndex);
	return rsMan->redirectGetRenderTarget(renderTargetIndex, ppRenderTarget);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9* renderTarget,IDirect3DSurface9* pDestSurface) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetRenderTargetData(renderTarget,pDestSurface);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetSamplerState(Sampler,Type,pValue);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetScissorRect(RECT* pRect) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetScissorRect(pRect);
}

BOOL APIENTRY hkIDirect3DDevice9::GetSoftwareVertexProcessing() {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetSoftwareVertexProcessing();
}

HRESULT APIENTRY hkIDirect3DDevice9::GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* OffsetInBytes,UINT* pStride) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetStreamSource(StreamNumber, ppStreamData,OffsetInBytes, pStride);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber,UINT* Divider) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetStreamSourceFreq(StreamNumber,Divider);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetSwapChain(UINT iSwapChain,IDirect3DSwapChain9** pSwapChain) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "GetSwapChain %u\n", iSwapChain);
	if(iSwapChain == 0) {
		if(hookedSwapChain0 == NULL) {
			SDLOG(1, "Hooking swapchain 0\n");
			IDirect3DSwapChain9 *sw;
			m_pD3Ddev->GetSwapChain(iSwapChain, &sw);
			hookedSwapChain0 = new hkIDirect3DSwapChain9(&sw, this);
		}
		SDLOG(4, "-> Hooked swapchain\n");
		*pSwapChain = hookedSwapChain0;
		return D3D_OK;
	}
	return m_pD3Ddev->GetSwapChain(iSwapChain,pSwapChain);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetTexture(DWORD Stage, IDirect3DBaseTexture9 **ppTexture) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetTexture(Stage, ppTexture);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pValue) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetTextureStageState(Stage, Type, pValue);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX *pMatrix) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetTransform(State, pMatrix);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetVertexDeclaration(ppDecl);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetVertexShader(ppShader);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetVertexShaderConstantB(StartRegister,pConstantData,BoolCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetVertexShaderConstantF(StartRegister,pConstantData,Vector4fCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetVertexShaderConstantI(StartRegister,pConstantData,Vector4iCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::GetViewport(D3DVIEWPORT9 *pViewport) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->GetViewport(pViewport);
}

HRESULT APIENTRY hkIDirect3DDevice9::LightEnable(DWORD LightIndex, BOOL bEnable) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->LightEnable(LightIndex, bEnable);
}

HRESULT APIENTRY hkIDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix) {
	RSManager::setLatest(rsMan);
	SDLOG(5, "MultiplyTransform state: %u matrix: \n%s\n", State, D3DMatrixToString(pMatrix).c_str());
	return m_pD3Ddev->MultiplyTransform(State, pMatrix);
}

HRESULT APIENTRY hkIDirect3DDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer,pVertexDecl, Flags);
}

ULONG APIENTRY hkIDirect3DDevice9::Release() {
	SDLOG(0, "Device %p release on thread %u\n", m_pD3Ddev, GetCurrentThreadId());
	ULONG ret = m_pD3Ddev->Release();
	SDLOG(0, " -> %u\n", ret);
	if(ret == 0) {
		rsMan->releaseResources();
		delete rsMan;
		RSManager::setLatest(NULL);
	}
	return ret;
}

HRESULT APIENTRY hkIDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters) {
	RSManager::setLatest(rsMan);
	SDLOG(0, "Reset ------\n");
	return rsMan->redirectReset(pPresentationParameters);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetClipPlane(DWORD Index, CONST float *pPlane) {
	RSManager::setLatest(rsMan);
	SDLOG(7, "SetClipPlane %d: %f / %f / %f / %f\n", 0, pPlane[0], pPlane[1], pPlane[2], pPlane[3]);
	return m_pD3Ddev->SetClipPlane(Index, pPlane);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetClipStatus(CONST D3DCLIPSTATUS9 *pClipStatus) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->SetClipStatus(pClipStatus);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->SetCurrentTexturePalette(PaletteNumber);
}

void APIENTRY hkIDirect3DDevice9::SetCursorPosition(int X, int Y, DWORD Flags) {
	RSManager::setLatest(rsMan);
	SDLOG(5, "SetCursorPosition\n");
	rsMan->redirectSetCursorPosition(X, Y, Flags);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9 *pCursorBitmap) {
	RSManager::setLatest(rsMan);
	SDLOG(3, "SetCursorProperties\n");
	return m_pD3Ddev->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
	RSManager::setLatest(rsMan);
	SDLOG(5, "SetDepthStencilSurface %p\n", pNewZStencil);
	return rsMan->redirectSetDepthStencilSurface(pNewZStencil);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->SetDialogBoxMode(bEnableDialogs);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetFVF(DWORD FVF) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->SetFVF(FVF);
}

void APIENTRY hkIDirect3DDevice9::SetGammaRamp(UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp) {
	RSManager::setLatest(rsMan);
	SDLOG(3, "SetGammaRamp\n");
	m_pD3Ddev->SetGammaRamp(iSwapChain,Flags, pRamp);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->SetIndices(pIndexData);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetLight(DWORD Index, CONST D3DLIGHT9 *pLight) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->SetLight(Index, pLight);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetMaterial(CONST D3DMATERIAL9 *pMaterial) {	
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->SetMaterial(pMaterial);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetNPatchMode(float nSegments) {	
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->SetNPatchMode(nSegments);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY *pEntries) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9* pShader) {
	RSManager::setLatest(rsMan);
	SDLOG(3, "SetPixelShader %p, name: %s\n", pShader, rsMan->getShaderManager().getName(pShader));
	return rsMan->redirectSetPixelShader(pShader);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) {
	RSManager::setLatest(rsMan);
	SDLOG(6, "SetPixelShaderConstantB: start: %u, count: %u\n", StartRegister, BoolCount);
	if(Settings::get().getLogLevel() > 13) {
		for(size_t i=0; i<BoolCount; ++i) {
			SDLOG(0, " - % 5s\n", pConstantData[i] ? "true" : "false");
		}
	}
	return m_pD3Ddev->SetPixelShaderConstantB(StartRegister,pConstantData,BoolCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) {
	RSManager::setLatest(rsMan);
	SDLOG(6, "SetPixelShaderConstantF: start: %u, count: %u\n", StartRegister, Vector4fCount);
	if(Settings::get().getLogLevel() > 13) {
		for(size_t i=0; i<Vector4fCount; ++i) {
			SDLOG(0, " - %16.10f %16.10f %16.10f %16.10f\n", pConstantData[i*4+0], pConstantData[i*4+1], pConstantData[i*4+2], pConstantData[i*4+3]);
		}
	}
	return rsMan->redirectSetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) {
	RSManager::setLatest(rsMan);
	SDLOG(6, "SetPixelShaderConstantI: start: %u, count: %u\n", StartRegister, Vector4iCount);
	if(Settings::get().getLogLevel() > 13) {
		for(size_t i=0; i<Vector4iCount; ++i) {
			SDLOG(0, " - % 16d % 16d % 16d % 16d\n", pConstantData[i*4+0], pConstantData[i*4+1], pConstantData[i*4+2], pConstantData[i*4+3]);
		}
	}
	return m_pD3Ddev->SetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
	RSManager::setLatest(rsMan);
	SDLOG(6, "SetRenderState: state: %s, value: %u\n", D3DRenderStateTypeToString(State), Value);
	return rsMan->redirectSetRenderState(State, Value);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) {
	RSManager::setLatest(rsMan);
	SDLOG(14, "SetSamplerState sampler %lu:   state type: %s   value: %lu\n", Sampler, D3DSamplerStateTypeToString(Type), Value);
	return rsMan->redirectSetSamplerState(Sampler, Type, Value);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetScissorRect(CONST RECT* pRect) {
	RSManager::setLatest(rsMan);
	SDLOG(5, "SetScissorRect %s\n", RectToString(pRect).c_str());
	return m_pD3Ddev->SetScissorRect(pRect);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->SetSoftwareVertexProcessing(bSoftware);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride) {
	RSManager::setLatest(rsMan);
	SDLOG(8, "SetStreamSource %d: %p (%u, %u)\n", StreamNumber, pStreamData, OffsetInBytes, Stride);
	return m_pD3Ddev->SetStreamSource(StreamNumber, pStreamData,OffsetInBytes, Stride);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber,UINT Divider) {
	RSManager::setLatest(rsMan);
	SDLOG(8, "SetStreamSourceFreq %d: %u\n", StreamNumber, Divider);
	return m_pD3Ddev->SetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9 *pTexture) {
	RSManager::setLatest(rsMan);
	SDLOG(6, "setTexture %d, %p\n", Stage, pTexture);
	if(Settings::get().getLogLevel() > 10 && pTexture) {
		IDirect3DTexture9 *tex;
		if(pTexture->QueryInterface(IID_IDirect3DTexture9, (void**)&tex) == S_OK) {
			D3DSURFACE_DESC desc;
			tex->GetLevelDesc(0, &desc);
			SDLOG(10, " -- size: %dx%d RT? %s\n", desc.Width, desc.Height, (desc.Usage & D3DUSAGE_RENDERTARGET) ? "true" : "false");
			tex->Release();
		}
	}
	return m_pD3Ddev->SetTexture(Stage, pTexture);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->SetTextureStageState(Stage, Type, Value);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix) {
	RSManager::setLatest(rsMan);
	SDLOG(0, "SetTransform state: %u matrix: \n%s\n", State, D3DMatrixToString(pMatrix).c_str());
	return m_pD3Ddev->SetTransform(State, pMatrix);	
}

HRESULT APIENTRY hkIDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->SetVertexDeclaration(pDecl);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "SetVertexShaderConstantB: start: %u, count: %u\n", StartRegister, BoolCount);
	if(Settings::get().getLogLevel() > 13) {
		for(size_t i=0; i<BoolCount; ++i) {
			SDLOG(0, " - %s\n", pConstantData[i] ? "true" : "false");
		}
	}
	return m_pD3Ddev->SetVertexShaderConstantB(StartRegister,pConstantData,BoolCount);
}

HRESULT APIENTRY hkIDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) {
	RSManager::setLatest(rsMan);
	SDLOG(4, "SetVertexShaderConstantI: start: %u, count: %u\n", StartRegister, Vector4iCount);
	if(Settings::get().getLogLevel() > 13) {
		for(size_t i=0; i<Vector4iCount; ++i) {
			SDLOG(0, " - % 16d % 16d % 16d % 16d\n", pConstantData[i*4+0], pConstantData[i*4+1], pConstantData[i*4+2], pConstantData[i*4+3]);
		}
	}
	return m_pD3Ddev->SetVertexShaderConstantI(StartRegister,pConstantData,Vector4iCount);
}

BOOL APIENTRY hkIDirect3DDevice9::ShowCursor(BOOL bShow) {
	RSManager::setLatest(rsMan);
	SDLOG(0, "ShowCursor %d\n", bShow);
	if(Settings::get().getHideMouseCursor()) return m_pD3Ddev->ShowCursor(false);
	return m_pD3Ddev->ShowCursor(bShow);
}

HRESULT APIENTRY hkIDirect3DDevice9::StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter) {
	RSManager::setLatest(rsMan);
	SDLOG(5, "StretchRect src -> dest, sR -> dR : %p -> %p,  %s -> %s\n", pSourceSurface, pDestSurface, RectToString(pSourceRect).c_str(), RectToString(pDestRect).c_str());
	return rsMan->redirectStretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT APIENTRY hkIDirect3DDevice9::TestCooperativeLevel() {
	RSManager::setLatest(rsMan);
	SDLOG(8, "TestCooperativeLevel");
	HRESULT hr = m_pD3Ddev->TestCooperativeLevel();
	SDLOG(8, " - returned %s\n", (hr ==  D3D_OK ? "OK" : (D3DERR_DEVICELOST ? "DEVICELOST" : (hr == D3DERR_DEVICENOTRESET ? "DEVICENOTRESET" : "DRIVERINTERNALERROR"))));
	return hr;
}

HRESULT APIENTRY hkIDirect3DDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint) {
	RSManager::setLatest(rsMan);
	SDLOG(6, "UpdateSurface source: %p - %s   dest: %p - %d/%d\n", pSourceSurface, RectToString(pSourceRect).c_str(), pDestinationSurface, pDestPoint->x, pDestPoint->y);
	return m_pD3Ddev->UpdateSurface(pSourceSurface,pSourceRect,pDestinationSurface,pDestPoint);
}

HRESULT APIENTRY hkIDirect3DDevice9::UpdateTexture(IDirect3DBaseTexture9 *pSourceTexture, IDirect3DBaseTexture9 *pDestinationTexture) {
	RSManager::setLatest(rsMan);
	SDLOG(6, "UpdateTexture source: %p    dest: %p\n", pSourceTexture, pDestinationTexture);
	return m_pD3Ddev->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT APIENTRY hkIDirect3DDevice9::ValidateDevice(DWORD *pNumPasses) {
	RSManager::setLatest(rsMan);
	return m_pD3Ddev->ValidateDevice(pNumPasses);
}
