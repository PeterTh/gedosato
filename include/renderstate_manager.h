#pragma once

#include <map>
#include <set>
#include <vector>

#include "d3d9.h"
#include "d3dperf.h"
#include "console.h"
#include "shader_manager.h"
#include "game_plugin.h"

#pragma region Forward Declarations
class SMAA;
class FXAA;
class DOF;
class SSAO;
class Post;
class Bloom;
class Scaler;
#pragma endregion

class RSManager {
public:
	typedef enum { SCREENSHOT_NONE, SCREENSHOT_STANDARD, SCREENSHOT_FULL, SCREENSHOT_HUDLESS } ScreenshotType;

private:
	static RSManager* latest;

	bool inited, downsampling;
	IDirect3DDevice9 *d3ddev;
	GamePlugin *plugin;

	bool dumpingFrame;
	Scaler* scaler;
	Console console;
	ShaderManager shaderMan;
	
	ScreenshotType takeScreenshot;

	enum { SWAP_COPY, SWAP_FLIP, SWAP_DISCARD } swapEffect;
	unsigned numBackBuffers, renderWidth, renderHeight;
	D3DFORMAT backbufferFormat;
	IDirect3DTexture9** backBufferTextures;
	IDirect3DSurface9** backBuffers;
	IDirect3DSurface9* extraBuffer;
	IDirect3DSurface9* depthStencilSurf;
	
	unsigned dumpCaptureIndex, renderTargetSwitches;

	#define TEXTURE(_name, _hash) \
	private: \
	static const UINT32 texture##_name##Hash = _hash; \
	IDirect3DTexture9* texture##_name; \
	bool isTexture##_name(IDirect3DBaseTexture9* pTexture) { return texture##_name && ((IDirect3DTexture9*)pTexture) == texture##_name; };
	#include "Textures.def"
	#undef TEXTURE
	const char* getTextureName(IDirect3DBaseTexture9* pTexture);
	
	void registerKnowTexture(LPCVOID pSrcData, UINT SrcDataSize, LPDIRECT3DTEXTURE9 pTexture);
	unsigned numKnownTextures, foundKnownTextures;

	// Render state store/restore data
	IDirect3DVertexDeclaration9* prevVDecl;
	IDirect3DSurface9* prevDepthStencilSurf;
	IDirect3DSurface9* prevRenderTarget;
	IDirect3DStateBlock9* prevStateBlock;

	// Performance measurement
	Timer cpuFrameTimer;
	SlidingAverage cpuFrameTimes;
	D3DPerfMonitor* perfMonitor;
	StaticTextPtr frameTimeText;

	void prePresent(bool doNotFlip);
		
public:
	static RSManager& get();
	static void setLatest(RSManager *man);
	static bool currentlyDownsampling() {
		return latest && latest->downsampling;
	}

	RSManager() : inited(false), downsampling(false), dumpingFrame(false), scaler(NULL), takeScreenshot(SCREENSHOT_NONE),
				  swapEffect(SWAP_DISCARD), numBackBuffers(0), renderWidth(0), renderHeight(0),
				  backbufferFormat(D3DFMT_X8R8G8B8), backBufferTextures(NULL), backBuffers(NULL), extraBuffer(NULL), depthStencilSurf(NULL),
				  dumpCaptureIndex(0), renderTargetSwitches(0), numKnownTextures(0), foundKnownTextures(0),
				  prevVDecl(NULL), prevDepthStencilSurf(NULL), prevRenderTarget(NULL), prevStateBlock(NULL),
				  cpuFrameTimes(120), perfMonitor(NULL), frameTimeText(std::make_shared<StaticText>("", 20.0f, 100.0f))
	{
		#define TEXTURE(_name, _hash) ++numKnownTextures;
		#include "Textures.def"
		#undef TEXTURE
	}

	~RSManager() {
		releaseResources();
	}

	void showStatus();

	unsigned getRenderWidth() { return renderWidth; }
	unsigned getRenderHeight() { return renderHeight; }
	ScreenshotType getTakeScreenshot() { return takeScreenshot; }
	void tookScreenshot() { takeScreenshot = SCREENSHOT_NONE; }

	void storeRenderState();
	void restoreRenderState();
	
	void setD3DDevice(IDirect3DDevice9 *pD3Ddev) { d3ddev = pD3Ddev; }

	void initResources(bool downsampling, unsigned rw, unsigned rh, unsigned numBBs, D3DFORMAT bbFormat, D3DSWAPEFFECT swapEff, bool autoDepthStencil, D3DFORMAT depthStencilFormat);
	void releaseResources();

	void enableTakeScreenshot(ScreenshotType type);
	bool takingScreenshot() { return takeScreenshot != SCREENSHOT_NONE; }

	void dumpFrame() { dumpingFrame = true; }
	void captureRTScreen(const std::string &stype = "normal");
	
	void dumpSurface(const char* name, IDirect3DSurface9* surface);
	void dumpTexture(const char* name, IDirect3DTexture9* tex);
	
	void toggleAA() { plugin->toggleAA(); }
	void toggleAO() { plugin->toggleAO(); }
	void toggleDOF() { plugin->toggleDOF(); }
	void toggleBloom() { plugin->toggleBloom(); }
	void togglePost() { plugin->togglePost(); }
	void dumpSSAO() { plugin->dumpSSAO(); }
	void dumpBloom() { plugin->dumpBloom(); }

	void togglePerfInfo() { frameTimeText->show = !frameTimeText->show; }

	Scaler* getScaler() { return scaler; }
	ShaderManager& getShaderManager() { return shaderMan; }
	
	bool downsamplingEnabled() { return downsampling; }

	static HRESULT redirectCreateDevice(IDirect3D9* d3d9, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9 **ppReturnedDeviceInterface);
	static HRESULT redirectCreateDeviceEx(IDirect3D9Ex * m_pD3Dint, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface);
	HRESULT redirectReset(D3DPRESENT_PARAMETERS * pPresentationParameters);
	HRESULT redirectResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode);
	
	void registerD3DXCreateTextureFromFileInMemory(LPCVOID pSrcData, UINT SrcDataSize, LPDIRECT3DTEXTURE9 pTexture);
	void registerD3DXCompileShader(LPCSTR pSrcData, UINT srcDataLen, const D3DXMACRO *pDefines, LPD3DXINCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile, DWORD Flags, LPD3DXBUFFER * ppShader, LPD3DXBUFFER * ppErrorMsgs, LPD3DXCONSTANTTABLE * ppConstantTable);
	HRESULT redirectD3DXCreateTextureFromFileInMemoryEx(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE9* ppTexture);
	
	HRESULT redirectSetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget);
	HRESULT redirectPresent(CONST RECT * pSourceRect, CONST RECT * pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
	HRESULT redirectPresentEx(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags);
	HRESULT redirectGetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer);
	HRESULT redirectGetRenderTarget(DWORD renderTargetIndex, IDirect3DSurface9** ppRenderTarget);
	HRESULT redirectGetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode);
	HRESULT redirectGetDisplayModeEx(UINT iSwapChain, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation);
	HRESULT redirectGetDepthStencilSurface(IDirect3DSurface9 ** ppZStencilSurface);
	void redirectSetCursorPosition(int X, int Y, DWORD Flags);

	HRESULT redirectSetPixelShader(IDirect3DPixelShader9* pShader);
	HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
	HRESULT redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
};
