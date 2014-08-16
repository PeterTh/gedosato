#include "renderstate_manager.h"

#include <time.h>
#include <intsafe.h>
#include <io.h>
#include <fstream>
#include <string>

#include <boost/filesystem.hpp>

#include "d3dutil.h"
#include "winutil.h"
#include "d3d9/d3d9dev_ex.h"
#include "settings.h"
#include "hash.h"
#include "detouring.h"
#include "window_manager.h"

#include "scaling.h"
#include "console.h"
#include "key_actions.h"

RSManager* RSManager::latest = NULL;
bool RSManager::forceDSoff = false;

RSManager& RSManager::get() {
	if(latest == NULL) SDLOG(0, "Getting NULL RSManager!!\n")
	return *latest;
}
void RSManager::setLatest(RSManager *man) {
	latest = man;
	Console::setLatest(&man->console);
}

RenderTargetManager& RSManager::getRTMan() {
	return *latest->rtMan;
}

Scaler* RSManager::getScaler() {
	if(Settings::get().getPluginHandlesDownsampling()) {
		return plugin->getScaler();
	}
	return scaler;
}

IDirect3D9* RSManager::getD3D() {
	IDirect3D9* d3d = NULL;
	d3ddev->GetDirect3D(&d3d);
	d3d->Release();
	return d3d;
}


bool RSManager::currentlyDownsampling() {
	if(forceDSoff) return false;
	return Settings::get().getForceAlwaysDownsamplingRes() || (latest && latest->downsampling);
}


void RSManager::showStatus() {
	console.add(format("%s %s", INTERCEPTOR_NAME, GeDoSaToVersion()));
	if(getScaler()) getScaler()->showStatus();
	else console.add("Not downsampling");
	console.add(format("Plugin loaded: %s", plugin->getName().c_str()));
	plugin->reportStatus();
}

void RSManager::initResources(bool downsampling, unsigned rw, unsigned rh, 
		unsigned numBBs, D3DFORMAT bbFormat, D3DMULTISAMPLE_TYPE multiSampleType, unsigned multiSampleQuality, 
		D3DSWAPEFFECT swapEff, bool autoDepthStencil, D3DFORMAT depthStencilFormat) {
	
	if(inited) releaseResources();
	SDLOG(0, "RenderstateManager resource initialization started\n");

	this->downsampling = downsampling;
	renderWidth = rw;
	renderHeight = rh;
	numBackBuffers = numBBs;
	if(bbFormat != D3DFMT_UNKNOWN) backbufferFormat = bbFormat;
	swapEffect = swapEff == D3DSWAPEFFECT_COPY ? SWAP_COPY : (swapEff == D3DSWAPEFFECT_DISCARD ? SWAP_DISCARD : SWAP_FLIP);
	if(swapEffect == SWAP_FLIP) numBackBuffers++; // account for the "front buffer" in the swap chain
	
	console.initialize(d3ddev, downsampling ? Settings::get().getPresentWidth() : rw, downsampling ? Settings::get().getPresentHeight() : rh);
	Console::setLatest(&console);
	imgWriter = std::unique_ptr<ImageWriter>(new ImageWriter(d3ddev, Settings::get().getRenderWidth(), Settings::get().getRenderHeight()));

	// performance measurement
	console.add(frameTimeText);
	perfMonitor = new D3DPerfMonitor(d3ddev, 60);
	
	// create state block for state save/restore
	d3ddev->CreateStateBlock(D3DSBT_ALL, &prevStateBlock);
	// create and capture default state block
	d3ddev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	d3ddev->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	d3ddev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);
	d3ddev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	d3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
	d3ddev->CreateStateBlock(D3DSBT_ALL, &initStateBlock);


	if(downsampling) {
		scaler = new Scaler(d3ddev, rw, rh, Settings::get().getPresentWidth(), Settings::get().getPresentHeight());

		// generate backbuffers
		SDLOG(2, "Generating backbuffers:\n")
		for(unsigned i=0; i<numBackBuffers; ++i) {
			backBuffers.push_back(rtMan->createTexture(rw, rh, backbufferFormat, multiSampleType, multiSampleQuality));
			SDLOG(2, "Backbuffer %u: %p\n", i, backBuffers[i]);
		}

		// set back buffer 0 as initial rendertarget
		d3ddev->SetRenderTarget(0, backBuffers[0]->getSurf());
		// generate additional buffer to emulate flip if required
		if(swapEffect == SWAP_FLIP && Settings::get().getEmulateFlipBehaviour()) {
			extraBuffer = rtMan->createSurface(rw, rh, backbufferFormat);
			SDLOG(2, "Extra backbuffer: %p\n", extraBuffer);
		}

		// if required, determine depth/stencil surf type and create
		if(autoDepthStencil) {
			d3ddev->CreateDepthStencilSurface(rw, rh, depthStencilFormat, multiSampleType, multiSampleQuality, FALSE, &depthStencilSurf, NULL);
			SDLOG(2, "Generated depth stencil surface - format: %s\n", D3DFormatToString(depthStencilFormat));
			// set our depth stencil surface
			d3ddev->SetDepthStencilSurface(depthStencilSurf);
		}
	}

	plugin = GamePlugin::getPlugin(d3ddev, *this);
	plugin->initialize(rw, rh, bbFormat);

	// restore initial state
	d3ddev->SetRenderState(D3DRS_ZENABLE, autoDepthStencil ? D3DZB_TRUE : D3DZB_FALSE);
	d3ddev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);
	d3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);
	
	SDLOG(0, "RenderstateManager resource initialization completed\n");
	inited = true;
}

void RSManager::releaseResources() {
	SDLOG(0, "RenderstateManager release: going into neutral state\n");
	for(int i = 0; i < 16; ++i) d3ddev->SetTexture(i, NULL);
	IDirect3DSurface9 *bb = NULL;
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb);
	d3ddev->SetRenderTarget(0, bb);
	SAFERELEASE(bb);
	SDLOG(0, "RenderstateManager releasing resources\n");
	SAFEDELETE(plugin);
	SAFEDELETE(scaler);
	SAFEDELETE(perfMonitor);
	SAFERELEASE(depthStencilSurf);
	extraBuffer.reset(NULL);
	imgWriter.reset(NULL);
	SAFERELEASE(prevStateBlock);
	SAFERELEASE(initStateBlock);
	SAFERELEASE(prevVDecl);
	SAFERELEASE(prevDepthStencilSurf);
	SAFERELEASE(prevRenderTarget);
	backBuffers.clear();
	console.cleanup();
	SDLOG(0, "RenderstateManager resource release completed\n");
}

void RSManager::prePresent(bool doNotFlip) {
	// downsample offscreen backbuffer to screen
	if(downsampling) {
		storeRenderState();
		d3ddev->BeginScene();
		// restore neutral state
		initStateBlock->Apply();
		// apply plugin actions if specified
		plugin->preDownsample(backBuffers[0]->getSurf());

		// actual scaling
		SDLOG(2, "Scaling fake backbuffer (%p)\n", backBuffers[0]);
		IDirect3DSurface9* realBackBuffer;
		d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &realBackBuffer);
		SDLOG(2, "- to backbuffer %p\n", realBackBuffer);
		scaler->go(backBuffers[0]->getTex(), realBackBuffer);
		realBackBuffer->Release();
		SDLOG(2, "- scaling complete!\n");
		
		// emulate flip queue behaviour exactly if required
		if(swapEffect == SWAP_FLIP && Settings::get().getEmulateFlipBehaviour() && !doNotFlip) {
			d3ddev->StretchRect(backBuffers[0]->getSurf(), NULL, extraBuffer->getSurf(), NULL, D3DTEXF_NONE);
			for(unsigned bb=0; bb<numBackBuffers; ++bb) {
				d3ddev->StretchRect(backBuffers[bb+1]->getSurf(), NULL, backBuffers[bb]->getSurf(), NULL, D3DTEXF_NONE);
			}
			d3ddev->StretchRect(extraBuffer->getSurf(), NULL, backBuffers[numBackBuffers-1]->getSurf(), NULL, D3DTEXF_NONE);
			SDLOG(2, "Advanced flip queue\n");
		} else {
			SDLOG(2, "Not \"flipping\" backbuffers\n");
		}
		d3ddev->EndScene();
		restoreRenderState();
	}
	else {
		storeRenderState();
		d3ddev->BeginScene();
		IDirect3DSurface9* bb = NULL;
		d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb);
		if(dumpingFrame) {
			dumpSurface("framedump_preplugin", bb);
		}
		plugin->preDownsample(bb);
		if(dumpingFrame) {
			dumpSurface("framedump_postplugin", bb);
		}
		SAFERELEASE(bb);
		d3ddev->EndScene();
		restoreRenderState();
	}

	// Stop dumping
	if(dumpingFrame) {
		if(downsampling) {
			dumpSurface("framedump_prePresent", backBuffers[0]->getSurf());
		}
		SDLOG(0, "============================================\nFinished dumping frame.\n");
		Settings::get().restoreLogLevel();
		dumpingFrame = false;
		console.add("Finished dumping frame.");
	}


	// Full-size screenshots
	if(takeScreenshot == SCREENSHOT_FULL || takeScreenshot == SCREENSHOT_HUDLESS || (!downsampling && takeScreenshot == SCREENSHOT_STANDARD)) {
		tookScreenshot();
		storeRenderState();
		if(downsampling) d3ddev->SetRenderTarget(0, backBuffers[0]->getSurf());
		captureRTScreen("full resolution");
		restoreRenderState();
	}

	// Frame time measurements
	cpuFrameTimes.add(cpuFrameTimer.elapsed() / 1000.0);
	perfMonitor->end();
	double cpuTime = cpuFrameTimes.get(), gpuTime = perfMonitor->getCurrent();
	frameTimeText->text = format("  %s\nFrame times (avg/max):\n CPU: %6.2lf / %6.2lf ms\n GPU: %6.2f / %6.2lf ms\nFPS: %4.3lf", 
		getTimeString(true), cpuTime, cpuFrameTimes.maximum(), gpuTime, perfMonitor->getMax(), 1000.0/max(cpuTime, gpuTime));

	// Draw console
	if(console.needsDrawing()) {
		storeRenderState();
		// restore neutral state
		initStateBlock->Apply();
		d3ddev->BeginScene();
		IDirect3DSurface9* realBackBuffer;
		d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &realBackBuffer);
		d3ddev->SetRenderTarget(0, realBackBuffer);
		console.draw();
		realBackBuffer->Release();
		d3ddev->EndScene();
		restoreRenderState();
	}

	// Normal screenshots
	if(takeScreenshot == SCREENSHOT_STANDARD) {
		tookScreenshot();
		storeRenderState();
		IDirect3DSurface9* realBackBuffer = NULL;
		d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &realBackBuffer);
		if(realBackBuffer) d3ddev->SetRenderTarget(0, realBackBuffer);
		captureRTScreen();
		SAFERELEASE(realBackBuffer);
		restoreRenderState();
	}
	
	// reset per-frame vars
	renderTargetSwitches = 0;
	plugin->prePresent();

	//////////////////////////////////////////// IO
	KeyActions::get().processIO();
	//////////////////////////////////////////// IO

	if(dumpingFrame) {
		Settings::get().elevateLogLevel(50);
		SDLOG(0, "============================================\nStarting frame dump.\n");
	}

	SDLOG(2, "Pre-present complete\n");
}

HRESULT RSManager::redirectPresent(CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion) {
	prePresent(false);
	
	storeRenderState();
	HRESULT hr = d3ddev->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	restoreRenderState();
	
	cpuFrameTimer.start();
	perfMonitor->start();
	return hr;
}

HRESULT RSManager::redirectPresentEx(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags) {
	SDLOG(1, "- PresentEx flags: %s\n", D3DPresentExFlagsToString(dwFlags).c_str());
	prePresent((dwFlags & D3DPRESENT_DONOTFLIP) != 0);	
	
	storeRenderState();
	HRESULT hr = ((IDirect3DDevice9Ex*)d3ddev)->PresentEx(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
	restoreRenderState();

	cpuFrameTimer.start();
	perfMonitor->start();	
	return hr;	
}

void RSManager::captureRTScreen(const string& stype, IDirect3DSurface9 *rtArg) {
	SDLOG(0, "Capturing screenshot\n");
	static unsigned index = 0;
	char timebuf[128], dirbuff[512], buffer[512];
	time_t ltime;
	time(&ltime);
	struct tm *timeinfo;
	timeinfo = localtime(&ltime);
	strftime(timebuf, 128, "screenshot_%Y-%m-%d_%H-%M-%S", timeinfo);
	sprintf(dirbuff, "%sscreens\\%s", getInstallDirectory().c_str(), getExeFileName().c_str());
	CreateDirectory(dirbuff, NULL);
	sprintf(buffer, "%s\\%s_%u.png", dirbuff, timebuf, index++);
	SDLOG(0, " - to %s\n", buffer);
		
	IDirect3DSurface9 *render = rtArg;
	if(rtArg == NULL) d3ddev->GetRenderTarget(0, &render);
	if(render) {
		imgWriter->writeSurface(buffer, render);
		Console::get().add(format("Captured %s screenshot to %s", stype.c_str(), buffer));
	}
	if(rtArg == NULL) SAFERELEASE(render);
}

void RSManager::dumpSurface(const char* name, IDirect3DSurface9* surface) {
	auto dirname = format("%s\\tmp\\%s", getInstallDirectory().c_str(), getExeFileName().c_str());
	CreateDirectory(dirname.c_str(), NULL);
	auto fullname = format("%s\\dump%03d_%s_%p.tga", dirname.c_str(), dumpCaptureIndex++, name, surface);
	SDLOG(1, "!! dumped RT %p to %s\n", surface, fullname.c_str());
	D3DXSaveSurfaceToFile(fullname.c_str(), D3DXIFF_TGA, surface, NULL, NULL);
}

void RSManager::dumpTexture(const char* name, IDirect3DTexture9* tex) {
	IDirect3DSurface9* surf = NULL;
	tex->GetSurfaceLevel(0, &surf);
	if(surf) dumpSurface(name, surf);
	SAFERELEASE(surf);
}

HRESULT RSManager::redirectSetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) {

	if(dumpingFrame) {
		IDirect3DSurface9* rt;
		d3ddev->GetRenderTarget(RenderTargetIndex, &rt);
		if(rt) {
			dumpSurface(format("framedump_preswitch%03u_target%d_pointer%p", renderTargetSwitches, RenderTargetIndex, rt).c_str(), rt);
		}
		SAFERELEASE(rt);
	}

	HRESULT hr = plugin->redirectSetRenderTarget(RenderTargetIndex, pRenderTarget);
	renderTargetSwitches++;

	if(dumpingFrame) {
		IDirect3DSurface9* rt;
		d3ddev->GetRenderTarget(RenderTargetIndex, &rt);
		if(rt) {
			dumpSurface(format("framedump_postswitch%03u_target%d_pointer%p", renderTargetSwitches, RenderTargetIndex, rt).c_str(), rt);
		}
		SAFERELEASE(rt);
	}

	return hr;
}

void RSManager::registerD3DXCreateTextureFromFileInMemory(LPCVOID pSrcData, UINT SrcDataSize, LPDIRECT3DTEXTURE9 pTexture) {
	SDLOG(1, "RenderstateManager: registerD3DXCreateTextureFromFileInMemory %p | %p (size %d)\n", pTexture, pSrcData, SrcDataSize);
	if(Settings::get().getEnableTextureDumping()) {
		UINT32 hash = SuperFastHash((char*)const_cast<void*>(pSrcData), SrcDataSize);
		SDLOG(1, " - size: %8u, hash: %8x\n", SrcDataSize, hash);

		IDirect3DSurface9* surf;
		((IDirect3DTexture9*)pTexture)->GetSurfaceLevel(0, &surf);
		string directory = getInstalledFileName(format("textures\\%s\\dump\\", getExeFileName().c_str()));
		SDLOG(0, "%s\n", boost::filesystem::path(directory).string().c_str())
		try {
			boost::filesystem::create_directories(boost::filesystem::path(directory));
			D3DXSaveSurfaceToFile(format("%s%08x.tga", directory.c_str(), hash).c_str(), D3DXIFF_TGA, surf, NULL, NULL);
		} catch(boost::filesystem::filesystem_error e) {
			SDLOG(0, "ERROR - Filesystem error while trying to create directory:\n%s\n", e.what());
		}
		SAFERELEASE(surf);
	}
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

void RSManager::enableTakeScreenshot(ScreenshotType type) {
	takeScreenshot = type; 
	SDLOG(0, "takeScreenshot: %d\n", type);
}

void RSManager::storeRenderState() {
	SDLOG(8, "storing render state\n");
	prevStateBlock->Capture();
	prevVDecl = NULL;
	prevDepthStencilSurf = NULL;
	prevRenderTarget = NULL;
	d3ddev->GetVertexDeclaration(&prevVDecl);
	d3ddev->GetDepthStencilSurface(&prevDepthStencilSurf);
	d3ddev->SetDepthStencilSurface(depthStencilSurf);
	d3ddev->GetRenderTarget(0, &prevRenderTarget);
	SDLOG(8, " - completed\n");
}

void RSManager::restoreRenderState() {
	SDLOG(8, "restore render state\n");
	d3ddev->SetVertexDeclaration(prevVDecl);
	SAFERELEASE(prevVDecl);
	d3ddev->SetDepthStencilSurface(prevDepthStencilSurf); // also restore NULL!
	SAFERELEASE(prevDepthStencilSurf);
	d3ddev->SetRenderTarget(0, prevRenderTarget);
	SAFERELEASE(prevRenderTarget);
	prevStateBlock->Apply();
	SDLOG(8, " - completed\n");
}

const char* RSManager::getTextureName(IDirect3DBaseTexture9* pTexture) {
	#define TEXTURE(_name, _hash) \
	if(texture##_name == pTexture) return #_name;
	#include "Textures.def"
	#undef TEXTURE
	return "Unknown";
}

HRESULT RSManager::redirectD3DXCreateTextureFromFileInMemoryEx(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, 
															   D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE9* ppTexture) {
	HRESULT hr = D3DERR_NOTAVAILABLE;
	// calculate hash
	UINT ssize;
	UINT32 hash;
	if(Settings::get().getEnableTextureOverride() || Settings::get().getEnableTextureMarking()) {
		ssize = (SrcDataSize == 2147483647u) ? (Width*Height/2) : SrcDataSize;
		hash = SuperFastHash((char*)const_cast<void*>(pSrcData), ssize);
	}
	// try override
	if(Settings::get().getEnableTextureOverride()) {
		SDLOG(4, "Trying texture override size: %8u, hash: %8x\n", ssize, hash);
		string fn = getInstalledFileName(format("textures\\%s\\override\\%08x.dds", getExeFileName().c_str(), hash));
		if(fileExists(fn.c_str())) {
			SDLOG(3, "Texture override (dds)! hash: %8x\n", hash);
			hr = D3DXCreateTextureFromFileEx(pDevice, fn.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
		}
		fn = getInstalledFileName(format("textures\\%s\\override\\%08x.png", getExeFileName().c_str(), hash));
		if(fileExists(fn.c_str())) {
			SDLOG(3, "Texture override (png)! hash: %8x\n", hash);
			hr = D3DXCreateTextureFromFileEx(pDevice, fn.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
		}
	}
	if(hr == D3DERR_NOTAVAILABLE) {
		hr = TrueD3DXCreateTextureFromFileInMemoryEx(pDevice, pSrcData, SrcDataSize, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
	}
	// add text
	if(hr == D3D_OK && Settings::get().getEnableTextureMarking()) {
		if(pSrcInfo->Width >= 32 && pSrcInfo->Height >= 32) {
			D3DXSaveTextureToFileA(getInstalledFileName("tmp\\temp.png").c_str(), D3DXIFF_PNG, *ppTexture, NULL);
			(*ppTexture)->Release();
			string fn = getInstalledFileName("tmp\\temp.png");
			string outfn = getInstalledFileName("tmp\\temp_out.png");
			string command = format("\"\"%s\" \"%s\" %08p \"%s\"\"", getAssetFileName("GeDoSaToTexM.exe").c_str(), fn.c_str(), hash, outfn.c_str());
			SDLOG(4, "Texture marking, executing command: \n : %s", command.c_str());
			RunSilent(command.c_str());
			D3DXCreateTextureFromFileExA(pDevice, outfn.c_str(), Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
		}
	}
	return hr;
}

HRESULT RSManager::redirectGetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) {
	if(downsampling) {
		SDLOG(4, "redirectGetBackBuffer\n");
		*ppBackBuffer = backBuffers[iBackBuffer]->getSurf();
		(*ppBackBuffer)->AddRef();
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
		pMode->RefreshRate = Settings::getResSettings().getActiveHz();
	}
	if(Settings::get().getForceBorderlessFullscreen()) WindowManager::get().interceptGetDisplayMode(pMode);
	if(!SUCCEEDED(res)) SDLOG(0, " -> redirectGetDisplayMode NOT SUCCEEDED - %x", res);
	return res;
}
HRESULT RSManager::redirectGetDisplayModeEx(UINT iSwapChain, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) {
	HRESULT res;
	res = ((IDirect3DDevice9Ex*)d3ddev)->GetDisplayModeEx(iSwapChain, pMode, pRotation);
	if(downsampling) {
		SDLOG(2, " -> faked\n");
		pMode->Width = Settings::get().getRenderWidth();
		pMode->Height = Settings::get().getRenderHeight();
		pMode->RefreshRate = Settings::getResSettings().getActiveHz();
		pMode->ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
	}
	if(Settings::get().getForceBorderlessFullscreen()) WindowManager::get().interceptGetDisplayMode((D3DDISPLAYMODE*)pMode);
	if(!SUCCEEDED(res)) SDLOG(0, " -> redirectGetDisplayMode NOT SUCCEEDED - %x", res);
	return res;
}

HRESULT RSManager::redirectGetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface) {
	if(downsampling && depthStencilSurf != NULL) {
		SDLOG(4, "redirectGetDepthStencilSurface\n");
		*ppZStencilSurface = depthStencilSurf;
		(*ppZStencilSurface)->AddRef();
		return D3D_OK;
	}
	return d3ddev->GetDepthStencilSurface(ppZStencilSurface);
}

////////////////////////////////////////////////////////////////////////// CreateDevice/Reset helpers
namespace {
	void logMode(const char* operation, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode = NULL) {
		SDLOG(0, " - %s requested mode:\n", operation);
		SDLOG(0, " - - Backbuffer(s): %4u x %4u %16s *%d \n", pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight, D3DFormatToString(pPresentationParameters->BackBufferFormat), pPresentationParameters->BackBufferCount);
		SDLOG(0, " - - PresentationInterval: %2u   Windowed: %5s    Refresh: %3u Hz    SwapEffect: %s\n", pPresentationParameters->PresentationInterval,
			pPresentationParameters->Windowed ? "true" : "false", pPresentationParameters->FullScreen_RefreshRateInHz, D3DSwapEffectToString(pPresentationParameters->SwapEffect));
		SDLOG(0, " - - Autodepthstencil: %8s  Format: %s  Discard: %s\n", 
			pPresentationParameters->EnableAutoDepthStencil ? "true" : "false", D3DFormatToString(pPresentationParameters->AutoDepthStencilFormat), pPresentationParameters->Flags&D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL ? "true" : "false");
		SDLOG(0, " - - Multisampling Type: %s  Quality: %d\n", D3DMultisampleTypeToString(pPresentationParameters->MultiSampleType), pPresentationParameters->MultiSampleQuality)
		if(pFullscreenDisplayMode != NULL) {
			SDLOG(0, " - D3DDISPLAYMODEEX set\n")
			SDLOG(0, " - - FS: %4u x %4u @ %3u Hz %16s\n", pFullscreenDisplayMode->Width, pFullscreenDisplayMode->Height, pFullscreenDisplayMode->RefreshRate, D3DFormatToString(pFullscreenDisplayMode->Format));
		}
	}
	bool isDownsamplingRequest(D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode = NULL) {
		if( Settings::getResSettings().setDSRes(pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight)
		|| (pFullscreenDisplayMode && Settings::getResSettings().setDSRes(pFullscreenDisplayMode->Width, pFullscreenDisplayMode->Height))) {
			SDLOG(0, "===================\n!!!!! requested downsampling resolution!\n");

			return true;
		}
		return false;
	}
	void adjustPresentInterval(D3DPRESENT_PARAMETERS* pPresentationParameters) {
		switch(Settings::get().getPresentInterval()) {
		case 0: pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; break;
		case 1: pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_ONE; break;
		case 2: pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_TWO; break;
		case 3: pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_THREE; break;
		case 4: pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_FOUR; break;
		}
		if(Settings::get().getForcePresentRes()) {
			pPresentationParameters->BackBufferWidth = Settings::get().getPresentWidth();
			pPresentationParameters->BackBufferHeight = Settings::get().getPresentHeight();
		}
	}
	void initPresentationParams(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DPRESENT_PARAMETERS* copy) {
		if(pPresentationParameters->BackBufferCount == 0) pPresentationParameters->BackBufferCount = 1;
		if(pPresentationParameters->BackBufferFormat == D3DFMT_UNKNOWN) pPresentationParameters->BackBufferFormat = D3DFMT_A8R8G8B8;
		pPresentationParameters->BackBufferWidth = Settings::get().getRenderWidth();
		pPresentationParameters->BackBufferHeight = Settings::get().getRenderHeight();
		pPresentationParameters->FullScreen_RefreshRateInHz = Settings::getResSettings().getActiveHz();;
		*copy = *pPresentationParameters;
		copy->BackBufferWidth = Settings::get().getPresentWidth();
		copy->BackBufferHeight = Settings::get().getPresentHeight();
		copy->FullScreen_RefreshRateInHz = Settings::get().getPresentHz();
		copy->BackBufferCount = 1;
		copy->EnableAutoDepthStencil = FALSE; // we take care of depth/stencil generation
		copy->Flags = copy->Flags & (~D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL); // this flag is only valid if stencil is generated by D3D
		if(pPresentationParameters->Windowed) copy->FullScreen_RefreshRateInHz = 0;
	}
	void initDisplayMode(D3DDISPLAYMODEEX* d, D3DDISPLAYMODEEX** copy) {
		if(d) {
			if(d->RefreshRate == 0) d->RefreshRate = Settings::getResSettings().getActiveHz();;
			*(*copy) = *d;
			(*copy)->Width = Settings::get().getPresentWidth();
			(*copy)->Height = Settings::get().getPresentHeight();
			(*copy)->RefreshRate = Settings::get().getPresentHz();
		} else {
			*copy = NULL;
		}
	}
	bool forceBorderlessFS(D3DPRESENT_PARAMETERS *pPresentationParameters) {
		if(pPresentationParameters && pPresentationParameters->Windowed == FALSE && Settings::get().getForceBorderlessFullscreen()) {
			SDLOG(1, "Forcing borderless windowed fullscreen mode.\n");
			pPresentationParameters->Windowed = TRUE;
			pPresentationParameters->FullScreen_RefreshRateInHz = 0;
			return true;
		}
		return false;
	}
	void initResourcesWrapper(bool downsampling, const D3DPRESENT_PARAMETERS *pPresentationParameters) {
		RSManager::get().initResources(downsampling, 
			downsampling ? Settings::get().getRenderWidth() : pPresentationParameters->BackBufferWidth,
			downsampling ? Settings::get().getRenderHeight() : pPresentationParameters->BackBufferHeight,
			downsampling ? pPresentationParameters->BackBufferCount : 0,
			downsampling ? pPresentationParameters->BackBufferFormat : D3DFMT_UNKNOWN,
			pPresentationParameters->MultiSampleType, pPresentationParameters->MultiSampleQuality,
			pPresentationParameters->SwapEffect, 
			pPresentationParameters->EnableAutoDepthStencil?true:false, pPresentationParameters->AutoDepthStencilFormat);
	}
}

HRESULT RSManager::redirectCreateDevice(IDirect3D9* d3d9, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9 **ppReturnedDeviceInterface) {
	logMode("redirectCreateDevice", pPresentationParameters);

	D3DPRESENT_PARAMETERS params = *pPresentationParameters;
	pPresentationParameters = &params;
	adjustPresentInterval(pPresentationParameters);
	bool fs = forceBorderlessFS(pPresentationParameters);

	if(isDownsamplingRequest(pPresentationParameters)) {
		if(fs) setDisplayFaking(true);
		D3DPRESENT_PARAMETERS copy;
		initPresentationParams(pPresentationParameters, &copy);
		forceDSoff = true;
		HRESULT ret = d3d9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, &copy, ppReturnedDeviceInterface);
		forceDSoff = false;
		if(SUCCEEDED(ret)) {
			new hkIDirect3DDevice9(ppReturnedDeviceInterface, &copy, d3d9);
			initResourcesWrapper(true, pPresentationParameters);
			if(fs) WindowManager::get().setFakeFullscreen(Settings::get().getRenderWidth(), Settings::get().getRenderHeight());
		} else SDLOG(0, "FAILED creating downsampling device -- error: %s\n description: %s\n", DXGetErrorString(ret), DXGetErrorDescription(ret)); 
		return ret;
	}

	forceDSoff = true;
	HRESULT ret = d3d9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
	forceDSoff = false;
	if(SUCCEEDED(ret)) {
		new hkIDirect3DDevice9(ppReturnedDeviceInterface, pPresentationParameters, d3d9);
		initResourcesWrapper(false, pPresentationParameters);
		if(fs) WindowManager::get().setFakeFullscreen(pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);
	} else SDLOG(0, "FAILED creating non-downsampling device -- error: %s\n description: %s\n", DXGetErrorString(ret), DXGetErrorDescription(ret)); 
	return ret;
}

HRESULT RSManager::redirectCreateDeviceEx(IDirect3D9Ex* d3d9ex, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, 
										  D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface) {
	logMode("redirectCreateDeviceEx", pPresentationParameters, pFullscreenDisplayMode);
	
	D3DPRESENT_PARAMETERS params = *pPresentationParameters;
	pPresentationParameters = &params;
	adjustPresentInterval(pPresentationParameters);
	bool fs = forceBorderlessFS(pPresentationParameters);
	
	if(isDownsamplingRequest(pPresentationParameters, pFullscreenDisplayMode)) {
		if(fs) setDisplayFaking(true);
		D3DPRESENT_PARAMETERS copy;
		initPresentationParams(pPresentationParameters, &copy);
		D3DDISPLAYMODEEX copyEx, *pCopyEx = &copyEx;
		initDisplayMode(pFullscreenDisplayMode, &pCopyEx);
		forceDSoff = true;
		HRESULT ret = d3d9ex->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, &copy, pCopyEx, ppReturnedDeviceInterface);
		forceDSoff = false;
		if(SUCCEEDED(ret)) {
			new hkIDirect3DDevice9Ex(ppReturnedDeviceInterface, &copy, d3d9ex);
			initResourcesWrapper(true, pPresentationParameters);
			if(fs) WindowManager::get().setFakeFullscreen(Settings::get().getRenderWidth(), Settings::get().getRenderHeight());
		} else SDLOG(0, "FAILED creating downsampling ex device -- error: %s\n description: %s\n", DXGetErrorString(ret), DXGetErrorDescription(ret)); 
		return ret;
	}

	forceDSoff = true;
	HRESULT ret = d3d9ex->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);
	forceDSoff = false;
	if(SUCCEEDED(ret)) {
		new hkIDirect3DDevice9Ex(ppReturnedDeviceInterface, pPresentationParameters, d3d9ex);
		initResourcesWrapper(false, pPresentationParameters);
		if(fs) WindowManager::get().setFakeFullscreen(pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);
	} else SDLOG(0, "FAILED creating non-downsampling ex device -- error: %s\n description: %s\n", DXGetErrorString(ret), DXGetErrorDescription(ret)); 
	return ret;
}

HRESULT RSManager::redirectReset(D3DPRESENT_PARAMETERS * pPresentationParameters) {
	logMode("redirectReset", pPresentationParameters);
	
	D3DPRESENT_PARAMETERS params = *pPresentationParameters;
	pPresentationParameters = &params;
	adjustPresentInterval(pPresentationParameters);
	bool fs = forceBorderlessFS(pPresentationParameters);

	releaseResources();
	
	if(isDownsamplingRequest(pPresentationParameters)) {
		if(fs) setDisplayFaking(true);
		D3DPRESENT_PARAMETERS copy;
		initPresentationParams(pPresentationParameters, &copy);
		downsampling = false;
		forceDSoff = true;
		HRESULT ret = d3ddev->Reset(&copy);
		forceDSoff = false;
		if(SUCCEEDED(ret)) {
			initResourcesWrapper(true, pPresentationParameters);
			if(fs) WindowManager::get().setFakeFullscreen(Settings::get().getRenderWidth(), Settings::get().getRenderHeight());
		}
		else {
			SDLOG(0, "FAILED resetting to downsampling -- error: %s\n description: %s\n", DXGetErrorString(ret), DXGetErrorDescription(ret));
		}
		return ret;
	}

	forceDSoff = true;
	HRESULT ret = d3ddev->Reset(pPresentationParameters);
	forceDSoff = false;
	if(SUCCEEDED(ret)) {
		initResourcesWrapper(false, pPresentationParameters);
		if(fs) WindowManager::get().setFakeFullscreen(pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);
	} else SDLOG(0, "FAILED resetting to non-downsampling -- error: %s\n description: %s\n", DXGetErrorString(ret), DXGetErrorDescription(ret)); 
	return ret;
}

HRESULT RSManager::redirectResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode) {
	logMode("redirectResetEx", pPresentationParameters);
	
	D3DPRESENT_PARAMETERS params = *pPresentationParameters;
	pPresentationParameters = &params;
	adjustPresentInterval(pPresentationParameters);
	bool fs = forceBorderlessFS(pPresentationParameters);

	releaseResources();

	if(isDownsamplingRequest(pPresentationParameters)) {
		if(fs) setDisplayFaking(true);
		D3DPRESENT_PARAMETERS copy;
		initPresentationParams(pPresentationParameters, &copy);
		D3DDISPLAYMODEEX copyEx, *pCopyEx = &copyEx;
		initDisplayMode(pFullscreenDisplayMode, &pCopyEx);
		downsampling = false;
		forceDSoff = true;
		HRESULT ret = ((IDirect3DDevice9Ex*)d3ddev)->ResetEx(&copy, pCopyEx);
		forceDSoff = false;
		if(SUCCEEDED(ret)) {
			initResourcesWrapper(true, pPresentationParameters);
			if(fs) WindowManager::get().setFakeFullscreen(Settings::get().getRenderWidth(), Settings::get().getRenderHeight());
		} else SDLOG(0, "FAILED resetting to downsampling ex -- error: %s\n description: %s\n", DXGetErrorString(ret), DXGetErrorDescription(ret)); 
		return ret;
	}

	forceDSoff = true;
	HRESULT ret = ((IDirect3DDevice9Ex*)d3ddev)->ResetEx(pPresentationParameters, pFullscreenDisplayMode);
	forceDSoff = false;
	if(SUCCEEDED(ret)) {
		initResourcesWrapper(false, pPresentationParameters);
		if(fs) WindowManager::get().setFakeFullscreen(pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);
	} else SDLOG(0, "FAILED resetting to non-downsampling ex -- error: %s\n description: %s\n", DXGetErrorString(ret), DXGetErrorDescription(ret)); 
	return ret;
}

void RSManager::redirectSetCursorPosition(int X, int Y, DWORD Flags) {
	SDLOG(2, "redirectSetCursorPosition\n");
	if(downsampling) {
		X = X * Settings::get().getPresentWidth() / Settings::get().getRenderWidth();
		Y = Y * Settings::get().getPresentHeight() / Settings::get().getRenderHeight();
	}
	d3ddev->SetCursorPosition(X, Y, Flags);
}

HRESULT RSManager::redirectSetPixelShader(IDirect3DPixelShader9* pShader) {
	return plugin->redirectSetPixelShader(pShader);
}

HRESULT RSManager::redirectSetVertexShader(IDirect3DVertexShader9* pvShader) {
	return plugin->redirectSetVertexShader(pvShader);
}

HRESULT RSManager::redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
	return plugin->redirectSetRenderState(State, Value);
}
HRESULT RSManager::redirectSetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) {
	return d3ddev->SetSamplerState(Sampler, Type, Value);
}

HRESULT RSManager::redirectClear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) {
	return plugin->redirectClear(Count, pRects, Flags, Color, Z, Stencil);
}
HRESULT RSManager::redirectDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) {
	return plugin->redirectDrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}
HRESULT RSManager::redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) {
	return plugin->redirectDrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}
HRESULT RSManager::redirectDrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
	return plugin->redirectDrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}
HRESULT RSManager::redirectDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, CONST void * pIndexData, D3DFORMAT IndexDataFormat, CONST void * pVertexStreamZeroData, UINT VertexStreamZeroStride) {
	return plugin->redirectDrawIndexedPrimitiveUP(PrimitiveType, MinIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT RSManager::redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
	return plugin->redirectSetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}
HRESULT RSManager::redirectSetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) {
	return plugin->redirectSetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT RSManager::redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) {
	return plugin->redirectCreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
}

HRESULT RSManager::redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) {
	return plugin->redirectCreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT RSManager::redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil){
	return plugin->redirectSetDepthStencilSurface(pNewZStencil);
}
