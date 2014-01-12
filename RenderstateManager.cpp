#include "RenderstateManager.h"

#include <time.h>
#include <intsafe.h>
#include <io.h>
#include <fstream>
#include <string>

#include "d3dutil.h"
#include "Settings.h"
#include "Hash.h"
#include "Detouring.h"
#include "WindowManager.h"

#include "SMAA.h"
#include "FXAA.h"
#include "Scaling.h"
#include "Console.h"

RSManager* RSManager::latest = NULL;

RSManager& RSManager::get() {
	if(latest == NULL) SDLOG(0, "Getting NULL RSManager!!\n")
	return *latest;
}
void RSManager::setLatest(RSManager *man) {
	latest = man;
	Console::setLatest(&man->console);
}

void RSManager::initResources(unsigned rw, unsigned rh) {
	if(inited) releaseResources();
	SDLOG(0, "RenderstateManager resource initialization started\n");
	renderWidth = rw;
	renderHeight = rh;
	//if(Settings::get().getAAQuality()) {
	//	if(Settings::get().getAAType() == "SMAA") {
	//		smaa = new SMAA(d3ddev, rw, rh, (SMAA::Preset)(Settings::get().getAAQuality()-1));
	//	} else {
	//		fxaa = new FXAA(d3ddev, rw, rh, (FXAA::Quality)(Settings::get().getAAQuality()-1));
	//	}
	//}
	
	console.initialize(d3ddev, downsampling ? Settings::get().getPresentWidth() : rw, downsampling ? Settings::get().getPresentHeight() : rh);
	Console::setLatest(&console);

	if(downsampling) {
		rw = Settings::get().getRenderWidth();
		rh = Settings::get().getRenderHeight();
		// generate backbuffers
		SDLOG(2, "Generating backbuffers:\n")
		backBuffers = new IDirect3DSurface9*[numBackBuffers];
		backBufferTextures = new IDirect3DTexture9*[numBackBuffers];
		for(unsigned i=0; i<numBackBuffers; ++i) {
			d3ddev->CreateTexture(rw, rh, 1, D3DUSAGE_RENDERTARGET, backbufferFormat, D3DPOOL_DEFAULT, &backBufferTextures[i], NULL);
			backBufferTextures[i]->GetSurfaceLevel(0, &backBuffers[i]);
			SDLOG(2, "Backbuffer %u: %p  tex: %p\n", i, backBuffers[i], backBufferTextures[i]);
		}
		d3ddev->CreateDepthStencilSurface(rw, rh, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, false, &depthStencilSurf, NULL);

		// set back buffer 0 as initial rendertarget
		d3ddev->SetRenderTarget(0, backBuffers[0]);
		// set our depth stencil surface
		d3ddev->SetDepthStencilSurface(depthStencilSurf);

		scaler = new Scaler(d3ddev, rw, rh, Settings::get().getPresentWidth(), Settings::get().getPresentHeight());
	
		d3ddev->CreateStateBlock(D3DSBT_ALL, &prevStateBlock);
	}

	SDLOG(0, "RenderstateManager resource initialization completed\n");
	inited = true;
}

void RSManager::releaseResources() {
	SDLOG(0, "RenderstateManager releasing resources\n");
	SAFERELEASE(depthStencilSurf);
	SAFERELEASE(prevStateBlock);
	SAFEDELETE(smaa);
	SAFEDELETE(fxaa);
	SAFEDELETE(scaler);
	if(backBuffers && backBufferTextures) {
		for(unsigned i=0; i<numBackBuffers; ++i) {
			SAFERELEASE(backBuffers[i]);
			SAFERELEASE(backBufferTextures[i]);
		}
	}
	SAFEDELETEARR(backBuffers);
	SAFEDELETEARR(backBufferTextures);
	SDLOG(0, "RenderstateManager resource release completed\n");
}

HRESULT RSManager::redirectPresent(CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion) {
	if(takeScreenshot == SCREENSHOT_FULL || (!downsampling && takeScreenshot == SCREENSHOT_STANDARD)) {
		takeScreenshot = SCREENSHOT_NONE;
		captureRTScreen("full resolution");
	}

	if(downsampling) {
		// downsample offscreen backbuffer to screen
		storeRenderState();
		d3ddev->BeginScene();
		SDLOG(2, "Scaling fake backbuffer %u (%p)\n", lastBackBuffer, backBuffers[lastBackBuffer]);
		IDirect3DSurface9* realBackBuffer;
		d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &realBackBuffer);
		SDLOG(2, "- to backbuffer %p\n", realBackBuffer);
		scaler->go(backBufferTextures[lastBackBuffer], realBackBuffer);
		realBackBuffer->Release();
		SDLOG(2, "- scaling complete!\n");
		d3ddev->EndScene();
		if(takeScreenshot == SCREENSHOT_STANDARD) {
			takeScreenshot = SCREENSHOT_NONE;
			captureRTScreen();
		}
		restoreRenderState();
	}

	// Draw console
	if(console.needsDrawing()) {
		storeRenderState();
		d3ddev->BeginScene();
		IDirect3DSurface9* realBackBuffer;
		d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &realBackBuffer);
		d3ddev->SetRenderTarget(0, realBackBuffer);
		console.draw();
		realBackBuffer->Release();
		d3ddev->EndScene();
		restoreRenderState();
	}
	
	return d3ddev->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

void RSManager::captureRTScreen(string stype) {
	SDLOG(0, "Capturing screenshot\n");
	char timebuf[128], dirbuff[512], buffer[512];
	time_t ltime;
	time(&ltime);
	struct tm *timeinfo;
	timeinfo = localtime(&ltime);
	strftime(timebuf, 128, "screenshot_%Y-%m-%d_%H-%M-%S.bmp", timeinfo);
	sprintf(dirbuff, "%sscreens\\%s", getInstallDirectory().c_str(), getExeFileName().c_str());
	CreateDirectory(dirbuff, NULL);
	sprintf(buffer, "%s\\%s", dirbuff, timebuf);
	SDLOG(0, " - to %s\n", buffer);
		
	IDirect3DSurface9 *render = NULL;
	d3ddev->GetRenderTarget(0, &render);
	if(render) {
		D3DXSaveSurfaceToFile(buffer, D3DXIFF_BMP, render, NULL, NULL);
		Console::get().add(format("Captured %s screenshot to %s", stype.c_str(), buffer));
	}
	SAFERELEASE(render);
}

void RSManager::dumpSurface(const char* name, IDirect3DSurface9* surface) {
	char fullname[128];
	sprintf_s(fullname, 128, "dump%03d_%s.tga", dumpCaptureIndex++, name);
	D3DXSaveSurfaceToFile(fullname, D3DXIFF_TGA, surface, NULL, NULL);
}

HRESULT RSManager::redirectSetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) {
	// if we know the main surface, and switch to the backbuffer, apply AA
	//if(mainSurface && onBackbuffer && doAA && (smaa || fxaa) && !aaDone) { 
	//	SDLOG(3, "~~ applying AA now\n");
	//	// final renderbuffer has to be from texture, just making sure here
	//	if(IDirect3DTexture9* tex = getSurfTexture(mainSurface)) {
	//		// check size just to make even more sure
	//		D3DSURFACE_DESC desc;
	//		mainSurface->GetDesc(&desc);
	//		if(desc.Width == Settings::get().getRenderWidth() && desc.Height == Settings::get().getRenderHeight()) {
	//			storeRenderState();
	//			d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	//			d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//			d3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
	//			if(smaa) smaa->go(tex, tex, rgbaBuffer1Surf, SMAA::INPUT_COLOR);
	//			else fxaa->go(tex, rgbaBuffer1Surf);
	//			d3ddev->StretchRect(rgbaBuffer1Surf, NULL, mainSurface, NULL, D3DTEXF_NONE);
	//			restoreRenderState();
	//		}
	//		tex->Release();				
	//	}
	//	aaDone = true;
	//}

	return d3ddev->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}

void RSManager::registerD3DXCreateTextureFromFileInMemory(LPCVOID pSrcData, UINT SrcDataSize, LPDIRECT3DTEXTURE9 pTexture) {
	SDLOG(1, "RenderstateManager: registerD3DXCreateTextureFromFileInMemory %p | %p (size %d)\n", pTexture, pSrcData, SrcDataSize);
	//if(Settings::get().getEnableTextureDumping()) {
	//	UINT32 hash = SuperFastHash((char*)const_cast<void*>(pSrcData), SrcDataSize);
	//	SDLOG(1, " - size: %8u, hash: %8x\n", SrcDataSize, hash);

	//	IDirect3DSurface9* surf;
	//	((IDirect3DTexture9*)pTexture)->GetSurfaceLevel(0, &surf);
	//	char buffer[128];
	//	sprintf_s(buffer, "dpfix/tex_dump/%08x.tga", hash);
	//	D3DXSaveSurfaceToFile(GetDirectoryFile(buffer), D3DXIFF_TGA, surf, NULL, NULL);
	//	surf->Release();
	//}
	registerKnowTexture(pSrcData, SrcDataSize, pTexture);
}

void RSManager::registerKnowTexture(LPCVOID pSrcData, UINT SrcDataSize, LPDIRECT3DTEXTURE9 pTexture) {
	if(foundKnownTextures < numKnownTextures) {
		UINT32 hash = SuperFastHash((char*)const_cast<void*>(pSrcData), SrcDataSize);
		#define TEXTURE(_name, _hash) \
		if(hash == _hash) { \
			texture##_name = pTexture; \
			++foundKnownTextures; \
			SDLOG(1, "RenderstateManager: recognized known texture %s at %u\n", #_name, pTexture); \
		}
		#include "Textures.def"
		#undef TEXTURE
		if(foundKnownTextures == numKnownTextures) {
			SDLOG(1, "RenderstateManager: all known textures found!\n");
		}
	}
}

void RSManager::registerD3DXCompileShader(LPCSTR pSrcData, UINT srcDataLen, const D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile, DWORD Flags, LPD3DXBUFFER * ppShader, LPD3DXBUFFER * ppErrorMsgs, LPD3DXCONSTANTTABLE * ppConstantTable) {
	SDLOG(0, "RenderstateManager: registerD3DXCompileShader %p, fun: %s, profile: %s", *ppShader, pFunctionName, pProfile);
	SDLOG(0, "============= source:\n%s\n====================", pSrcData);
}

IDirect3DTexture9* RSManager::getSurfTexture(IDirect3DSurface9* pSurface) {
	IUnknown *pContainer = NULL;
	HRESULT hr = pSurface->GetContainer(IID_IDirect3DTexture9, (void**)&pContainer);
	if(D3D_OK == hr) return (IDirect3DTexture9*)pContainer;
	SAFERELEASE(pContainer);
	return NULL;
}

void RSManager::enableTakeScreenshot(screenshotType type) {
	takeScreenshot = type; 
	SDLOG(0, "takeScreenshot: %d\n", type);
}

void RSManager::reloadAA() {
	//SAFEDELETE(smaa); 
	//SAFEDELETE(fxaa); 
	//if(Settings::get().getAAType() == "SMAA") {
	//	smaa = new SMAA(d3ddev, Settings::get().getRenderWidth(), Settings::get().getRenderHeight(), (SMAA::Preset)(Settings::get().getAAQuality()-1));
	//} else {
	//	fxaa = new FXAA(d3ddev, Settings::get().getRenderWidth(), Settings::get().getRenderHeight(), (FXAA::Quality)(Settings::get().getAAQuality()-1));
	//}
	SDLOG(0, "Reloaded AA\n");
}

void RSManager::storeRenderState() {
	prevStateBlock->Capture();
	prevVDecl = NULL;
	prevDepthStencilSurf = NULL;
	d3ddev->GetVertexDeclaration(&prevVDecl);
	d3ddev->GetDepthStencilSurface(&prevDepthStencilSurf);
	d3ddev->SetDepthStencilSurface(depthStencilSurf);
	d3ddev->GetRenderTarget(0, &prevRenderTarget);
}

void RSManager::restoreRenderState() {
	if(prevVDecl) {
		d3ddev->SetVertexDeclaration(prevVDecl);
		prevVDecl->Release();
	}
	d3ddev->SetDepthStencilSurface(prevDepthStencilSurf); // also restore NULL!
	if(prevDepthStencilSurf) {
		prevDepthStencilSurf->Release();
	}
	d3ddev->SetRenderTarget(0, prevRenderTarget);
	if(prevRenderTarget) {
		prevRenderTarget->Release();
	}
	prevStateBlock->Apply();
}

const char* RSManager::getTextureName(IDirect3DBaseTexture9* pTexture) {
	#define TEXTURE(_name, _hash) \
	if(texture##_name == pTexture) return #_name;
	#include "Textures.def"
	#undef TEXTURE
	return "Unknown";
}

HRESULT RSManager::redirectD3DXCreateTextureFromFileInMemoryEx(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE9* ppTexture) {
	//if(Settings::get().getEnableTextureOverride()) {
	//	UINT ssize = (SrcDataSize == 2147483647u) ? (Width*Height/2) : SrcDataSize;
	//	UINT32 hash = SuperFastHash((char*)const_cast<void*>(pSrcData), ssize);
	//	SDLOG(4, "Trying texture override size: %8u, hash: %8x\n", ssize, hash);
	//	
	//	char buffer[128];
	//	sprintf_s(buffer, "dpfix/tex_override/%08x.dds", hash);
	//	if(fileExists(buffer)) {
	//		SDLOG(3, "Texture override (dds)! hash: %8x\n", hash);
	//		return D3DXCreateTextureFromFileEx(pDevice, buffer, D3DX_DEFAULT, D3DX_DEFAULT, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
	//	}
	//	sprintf_s(buffer, "dpfix/tex_override/%08x.png", hash);
	//	if(fileExists(buffer)) {
	//		SDLOG(3, "Texture override (png)! hash: %8x\n", hash);
	//		return D3DXCreateTextureFromFileEx(pDevice, buffer, D3DX_DEFAULT, D3DX_DEFAULT, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
	//	}
	//}
	return TrueD3DXCreateTextureFromFileInMemoryEx(pDevice, pSrcData, SrcDataSize, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
}

HRESULT RSManager::redirectGetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) {
	if(downsampling) {
		SDLOG(4, "redirectGetBackBuffer\n");
		*ppBackBuffer = backBuffers[iBackBuffer];
		(*ppBackBuffer)->AddRef();
		lastBackBuffer = iBackBuffer;
		return D3D_OK;
	}
	return d3ddev->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT RSManager::redirectGetRenderTarget(DWORD renderTargetIndex, IDirect3DSurface9** ppRenderTarget) {
	HRESULT res = d3ddev->GetRenderTarget(renderTargetIndex, ppRenderTarget);
	SDLOG(2, " -> %p\n", *ppRenderTarget);
	return res; 
}

HRESULT RSManager::redirectGetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) {
	HRESULT res;
	res = d3ddev->GetDisplayMode(iSwapChain, pMode);
	if(downsampling) {
		SDLOG(2, " -> faked\n");
		pMode->Width = Settings::get().getRenderWidth();
		pMode->Height = Settings::get().getRenderHeight();
		pMode->RefreshRate = Settings::get().getReportedHz();
		return D3D_OK;
	}
	return res;
}

HRESULT RSManager::redirectGetDepthStencilSurface(IDirect3DSurface9 ** ppZStencilSurface) {
	HRESULT res;
	res = d3ddev->GetDepthStencilSurface(ppZStencilSurface);
	if(downsampling) {
		*ppZStencilSurface = depthStencilSurf;
	}
	return res;
}

// TODO: refactor code duplication between this and redirectReset
HRESULT RSManager::redirectCreateDevice(IDirect3D9* d3d9, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9 **ppReturnedDeviceInterface) {
	SDLOG(0, " - redirectCreateDevice requested mode:\n");
	SDLOG(0, " - - Backbuffer(s): %4u x %4u %16s *%d \n", pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight, D3DFormatToString(pPresentationParameters->BackBufferFormat), pPresentationParameters->BackBufferCount);
	SDLOG(0, " - - PresentationInterval: %2u   Windowed: %5s    Refresh: %3u Hz\n", pPresentationParameters->PresentationInterval, pPresentationParameters->Windowed ? "true" : "false", pPresentationParameters->FullScreen_RefreshRateInHz);
	
	if(pPresentationParameters->BackBufferWidth == Settings::get().getRenderWidth() && pPresentationParameters->BackBufferHeight == Settings::get().getRenderHeight()) {
		SDLOG(0, "===================\n!!! requested downsampling resolution!\n")
		if(pPresentationParameters->BackBufferCount == 0) pPresentationParameters->BackBufferCount = 1;
		if(pPresentationParameters->BackBufferFormat == D3DFMT_UNKNOWN) pPresentationParameters->BackBufferFormat = D3DFMT_A8R8G8B8;
		pPresentationParameters->BackBufferWidth = Settings::get().getRenderWidth();
		pPresentationParameters->BackBufferHeight = Settings::get().getRenderHeight();
		pPresentationParameters->FullScreen_RefreshRateInHz = Settings::get().getReportedHz();
		D3DPRESENT_PARAMETERS copy;
		memcpy(&copy, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
		copy.BackBufferWidth = Settings::get().getPresentWidth();
		copy.BackBufferHeight = Settings::get().getPresentHeight();
		copy.FullScreen_RefreshRateInHz = Settings::get().getPresentHz();
		copy.BackBufferCount = 1;
		HRESULT ret = d3d9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, &copy, ppReturnedDeviceInterface);
		if(SUCCEEDED(ret)) {
			new hkIDirect3DDevice9(ppReturnedDeviceInterface, &copy, d3d9);
			get().downsampling = true;
			get().numBackBuffers = pPresentationParameters->BackBufferCount;
			get().backbufferFormat = pPresentationParameters->BackBufferFormat;
			get().initResources(Settings::get().getRenderWidth(), Settings::get().getRenderHeight());
		} else SDLOG(0, "FAILED creating downsampling device -- error: %s\n description: %s\n", DXGetErrorString(ret), DXGetErrorDescription(ret)); 
		return ret;
	}
	HRESULT ret =  d3d9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
	if(SUCCEEDED(ret)) {
		new hkIDirect3DDevice9(ppReturnedDeviceInterface, pPresentationParameters, d3d9);
		get().downsampling = false;
		get().numBackBuffers = 0;
		get().initResources(pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);
	} else SDLOG(0, "FAILED creating non-downsampling device -- error: %s\n description: %s\n", DXGetErrorString(ret), DXGetErrorDescription(ret)); 
	return ret;
}

HRESULT RSManager::redirectReset(D3DPRESENT_PARAMETERS * pPresentationParameters) {
	SDLOG(0, " - redirectReset requested mode:\n");
	SDLOG(0, " - - Backbuffer(s): %4u x %4u %16s *%d \n", pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight, D3DFormatToString(pPresentationParameters->BackBufferFormat), pPresentationParameters->BackBufferCount);
	SDLOG(0, " - - PresentationInterval: %2u   Windowed: %5s    Refresh: %3u Hz\n", pPresentationParameters->PresentationInterval, pPresentationParameters->Windowed ? "true" : "false", pPresentationParameters->FullScreen_RefreshRateInHz);
	
	releaseResources();

	if(pPresentationParameters->BackBufferWidth == Settings::get().getRenderWidth() && pPresentationParameters->BackBufferHeight == Settings::get().getRenderHeight()) {
		SDLOG(0, "===================\n!!! requested downsampling resolution!\n")
		downsampling = true;
		if(pPresentationParameters->BackBufferCount == 0) pPresentationParameters->BackBufferCount = 1;
		numBackBuffers = pPresentationParameters->BackBufferCount;
		if(pPresentationParameters->BackBufferFormat == D3DFMT_UNKNOWN) pPresentationParameters->BackBufferFormat = D3DFMT_A8R8G8B8;
		backbufferFormat = pPresentationParameters->BackBufferFormat;
		pPresentationParameters->BackBufferWidth = Settings::get().getRenderWidth();
		pPresentationParameters->BackBufferHeight = Settings::get().getRenderHeight();
		pPresentationParameters->FullScreen_RefreshRateInHz = Settings::get().getReportedHz();
		D3DPRESENT_PARAMETERS copy;
		memcpy(&copy, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
		copy.BackBufferWidth = Settings::get().getPresentWidth();
		copy.BackBufferHeight = Settings::get().getPresentHeight();
		copy.FullScreen_RefreshRateInHz = Settings::get().getPresentHz();
		copy.BackBufferCount = 1;
		HRESULT ret = d3ddev->Reset(&copy);
		if(!SUCCEEDED(ret)) { 
			SDLOG(0, "FAILED resetting to downsampling -- error: %s\n description: %s\n", DXGetErrorString(ret), DXGetErrorDescription(ret)); 
		} else initResources(Settings::get().getRenderWidth(), Settings::get().getRenderHeight());
		return ret;
	}
	SDLOG(0, "Reset after check dev: %p\n", d3ddev);
	numBackBuffers = 0;
	downsampling = false;
	HRESULT ret =  d3ddev->Reset(pPresentationParameters);
	SDLOG(0, "After Reset\n");
	if(!SUCCEEDED(ret)) { 
		SDLOG(0, "FAILED resetting to non-downsampling -- error: %s\n description: %s\n", DXGetErrorString(ret), DXGetErrorDescription(ret)); 
	} else initResources(pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);
	return ret;
}

void RSManager::redirectSetCursorPosition(int X, int Y, DWORD Flags) {
	SDLOG(2, "redirectSetCursorPosition")
	if(downsampling) {
		X = X * Settings::get().getPresentWidth() / Settings::get().getRenderWidth();
		Y = Y * Settings::get().getPresentHeight() / Settings::get().getRenderHeight();
	}
	d3ddev->SetCursorPosition(X, Y, Flags);
}
