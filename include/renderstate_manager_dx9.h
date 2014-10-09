#pragma once

#include "renderstate_manager.h"

#include "game_plugin.h"

class RSManagerDX9 : public RSManager {
	bool inited = false, multisampling = false;
	IDirect3DDevice9 *d3ddev = NULL;
	GamePlugin *plugin = NULL;

	ShaderManager shaderMan;

	enum { SWAP_COPY, SWAP_FLIP, SWAP_DISCARD } swapEffect = SWAP_DISCARD;
	D3DFORMAT backbufferFormat = D3DFMT_X8R8G8B8;
	vector<RenderTargetPtr> backBuffers;
	RenderTargetPtr extraBuffer;
	IDirect3DSurface9* depthStencilSurf = NULL;

	unsigned renderTargetSwitches = 0;

	#define TEXTURE(_name, _hash) \
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
	
	void prePresent(bool doNotFlip);

public:
	RSManagerDX9(IDirect3DDevice9* d3ddev) : RSManager(), d3ddev(d3ddev) {
		#define TEXTURE(_name, _hash) ++numKnownTextures;
		#include "Textures.def"
		#undef TEXTURE
		rtMan.reset(new RenderTargetManager(d3ddev));
	}

	~RSManagerDX9() {
		releaseResources();
	}

	D3DFORMAT getBackBufferFormat() { return backbufferFormat; }
	bool usingMultisampling() { return multisampling; }

	void storeRenderState();
	void restoreRenderState();
	void setNeutralRenderState() { initStateBlock->Apply(); }

	void initResources(bool downsampling, unsigned rw, unsigned rh, unsigned numBBs, D3DFORMAT bbFormat, D3DMULTISAMPLE_TYPE multiSampleType, unsigned multiSampleQuality, D3DSWAPEFFECT swapEff, bool autoDepthStencil, D3DFORMAT depthStencilFormat);
	void releaseResources();
	
	void captureRTScreen(const std::string &stype = "normal", IDirect3DSurface9 *rtArg = NULL);

	void dumpSurface(const char* name, IDirect3DSurface9* surface);
	void dumpTexture(const char* name, IDirect3DTexture9* tex);

	virtual void showStatus() override;

	virtual void toggleHUD() override { plugin->toggleHUD(); }
	virtual void toggleAA() override { plugin->toggleAA(); }
	virtual void toggleAO() override { plugin->toggleAO(); }
	virtual void toggleDOF() override { plugin->toggleDOF(); }
	virtual void toggleBloom() override { plugin->toggleBloom(); }
	virtual void togglePost() override { plugin->togglePost(); }

	virtual void dumpSSAO() override { plugin->dumpSSAO(); }
	virtual void dumpBloom() override { plugin->dumpBloom(); }

	virtual void reloadShaders() override;

	virtual Scaler* getScaler() override;
	ShaderManager& getShaderManager() { return shaderMan; }
	IDirect3D9* getD3D();

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

	HRESULT redirectClear(DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
	HRESULT redirectDrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
	HRESULT redirectDrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
	HRESULT redirectDrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
	HRESULT redirectDrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinIndex, UINT NumVertices, UINT PrimitiveCount, CONST void * pIndexData, D3DFORMAT IndexDataFormat, CONST void * pVertexStreamZeroData, UINT VertexStreamZeroStride);

	HRESULT redirectSetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);
	HRESULT redirectSetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount);
	HRESULT redirectSetDepthStencilSurface(IDirect3DSurface9* pNewZStencil);

	HRESULT redirectCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle);
	HRESULT redirectCreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
	HRESULT redirectCreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
	HRESULT redirectSetViewport(CONST D3DVIEWPORT9 * pViewport);
	HRESULT redirectStretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter);
};

