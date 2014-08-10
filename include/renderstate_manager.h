#pragma once

#include <map>
#include <set>
#include <vector>

#include "d3d9.h"
#include "d3dperf.h"
#include "console.h"
#include "shader_manager.h"
#include "rendertarget_manager.h"
#include "imaging.h"
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
	static bool forceDSoff;

	bool inited = false, downsampling = false;
	IDirect3DDevice9 *d3ddev = NULL;
	GamePlugin *plugin = NULL;

	bool dumpingFrame = false;
	Scaler* scaler = NULL;
	Console console;
	ShaderManager shaderMan;
	std::unique_ptr<RenderTargetManager> rtMan;
	std::unique_ptr<ImageWriter> imgWriter;
	
	ScreenshotType takeScreenshot = SCREENSHOT_NONE;

	enum { SWAP_COPY, SWAP_FLIP, SWAP_DISCARD } swapEffect = SWAP_DISCARD;
	unsigned numBackBuffers = 0, renderWidth = 0, renderHeight = 0;
	D3DFORMAT backbufferFormat = D3DFMT_X8R8G8B8;
	vector<RenderTargetPtr> backBuffers;
	RenderTargetPtr extraBuffer;
	IDirect3DSurface9* depthStencilSurf = NULL;
	
	unsigned dumpCaptureIndex = 0, renderTargetSwitches = 0;

	#define TEXTURE(_name, _hash) \
	private: \
	static const UINT32 texture##_name##Hash = _hash; \
	IDirect3DTexture9* texture##_name; \
	bool isTexture##_name(IDirect3DBaseTexture9* pTexture) { return texture##_name && ((IDirect3DTexture9*)pTexture) == texture##_name; };
	#include "Textures.def"
	#undef TEXTURE
	const char* getTextureName(IDirect3DBaseTexture9* pTexture);
	
	void registerKnowTexture(LPCVOID pSrcData, UINT SrcDataSize, LPDIRECT3DTEXTURE9 pTexture);
	unsigned numKnownTextures = 0, foundKnownTextures = 0;

	// Render state store/restore data
	IDirect3DVertexDeclaration9* prevVDecl = NULL;
	IDirect3DSurface9* prevDepthStencilSurf = NULL;
	IDirect3DSurface9* prevRenderTarget = NULL;
	IDirect3DStateBlock9* prevStateBlock = NULL;
	IDirect3DStateBlock9* initStateBlock = NULL;

	// Performance measurement
	Timer cpuFrameTimer;
	SlidingAverage cpuFrameTimes{ 120 };
	D3DPerfMonitor* perfMonitor = NULL;
	StaticTextPtr frameTimeText{ new StaticText("", 20.0f, 100.0f) };

	void prePresent(bool doNotFlip);
		
public:
	static RSManager& get();
	static RenderTargetManager& getRTMan();
	static void setLatest(RSManager *man);
	static bool currentlyDownsampling();

	RSManager(IDirect3DDevice9* d3ddev) : d3ddev(d3ddev), rtMan(new RenderTargetManager(d3ddev)) {
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
	D3DFORMAT getBackBufferFormat() { return backbufferFormat; }
	void tookScreenshot() { takeScreenshot = SCREENSHOT_NONE; }

	void storeRenderState();
	void restoreRenderState();
	
	void initResources(bool downsampling, unsigned rw, unsigned rh, unsigned numBBs, D3DFORMAT bbFormat, D3DMULTISAMPLE_TYPE multiSampleType, unsigned multiSampleQuality, D3DSWAPEFFECT swapEff, bool autoDepthStencil, D3DFORMAT depthStencilFormat);
	void releaseResources();

	void enableTakeScreenshot(ScreenshotType type);
	bool takingScreenshot() { return takeScreenshot != SCREENSHOT_NONE; }

	void dumpFrame() { dumpingFrame = true; }
	void captureRTScreen(const std::string &stype = "normal", IDirect3DSurface9 *rtArg = NULL);
	
	void dumpSurface(const char* name, IDirect3DSurface9* surface);
	void dumpTexture(const char* name, IDirect3DTexture9* tex);

	void toggleHUD() { plugin->toggleHUD(); }
	void toggleAA() { plugin->toggleAA(); }
	void toggleAO() { plugin->toggleAO(); }
	void toggleDOF() { plugin->toggleDOF(); }
	void toggleBloom() { plugin->toggleBloom(); }
	void togglePost() { plugin->togglePost(); }

	void dumpSSAO() { plugin->dumpSSAO(); }
	void dumpBloom() { plugin->dumpBloom(); }

	void togglePerfInfo() { frameTimeText->show = !frameTimeText->show; }

	Scaler* getScaler();
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
	HRESULT redirectSetVertexShader(IDirect3DVertexShader9* pvShader);
	HRESULT redirectSetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	HRESULT redirectSetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);

	HRESULT redirectDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
	HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
	HRESULT redirectDrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
	HRESULT redirectDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, CONST void * pIndexData, D3DFORMAT IndexDataFormat, CONST void * pVertexStreamZeroData, UINT VertexStreamZeroStride);
	
	HRESULT redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);
	HRESULT redirectSetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);

	HRESULT redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle);
	HRESULT redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
	HRESULT redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil);
};
