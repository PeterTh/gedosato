#include "SSAO.h"

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <boost/filesystem.hpp>

#include "settings.h"
#include "renderstate_manager_dx9.h"

SSAO::SSAO(IDirect3DDevice9 *device, int bbWidth, int bbHeight, int width, int height, unsigned strength, Type type, Blur blur, bool useSRGB, bool readHWDepth)
	: Effect(device), width(width), height(height), bbWidth(bbWidth), bbHeight(bbHeight), dwidth(width), dheight(height), strength(strength), type(type), blur(blur)
	, useSRGB(useSRGB), readHWDepth(readHWDepth), blurPasses(blur==BLUR_SHARP ? 3 : 1) {

	// Create buffers
	if(type == SSAO::SAO || type == SSAO::HBAO) { 
		if(Settings::get().getSsaoScale() == 2 
			&& (bbWidth > static_cast<int>(Settings::get().getPresentWidth()) && bbHeight > static_cast<int>(Settings::get().getPresentHeight()))) { 
			dwidth = width / 2; dheight = height / 2;
			bbWidth = bbWidth / 2; bbHeight = bbHeight / 2;
		}
		CSZBuffer = RSManager::getRTMan().createTexture(dwidth, dheight, RenderTarget::FMT_R32F);
	}

	reloadShader();

	// Games like Dark Souls 2 or RE Revelations always render in 16/9 internally no matter if the backbuffer is actually 16/10 so we have to accomodate that for us 16/10 users
	// TODO check what would happen when using very exotic AR with this flag on
	if(bbHeight * 16 > bbWidth * 9) {
		if(((float)width / height > (float)bbWidth / bbHeight)) {
			SDLOG(6, "[GenericDepthPlugin] Correcting aspect ratio for our depth texture, computing and applying new proper AR height (%d -> %d)\n", height, bbHeight);
			isAspectRatioFix = true;
		}
	}

	buffer1 = RSManager::getRTMan().createTexture(dwidth, dheight, RenderTarget::FMT_ARGB_8);
	buffer2 = RSManager::getRTMan().createTexture(dwidth, dheight, RenderTarget::FMT_ARGB_8);
	hdrBuffer = RSManager::getRTMan().createTexture(width, height, RenderTarget::FMT_ARGB_FP16);
}

SSAO::~SSAO() {
	SAFERELEASE(effect);
	if(type == SSAO::HBAO) SAFERELEASE(noiseTex);
}

void SSAO::reloadShader() {

	// Setup the defines for compiling the effect
	vector<D3DXMACRO> defines;

	// Setup pixel size macro
	stringstream sp;
	sp << "float2(1.0 / " << dwidth << ", 1.0 / " << dheight << ")";
	string pixelSizeText = sp.str();
	D3DXMACRO pixelSizeMacro = { "PIXEL_SIZE", pixelSizeText.c_str() };
	defines.push_back(pixelSizeMacro);

	// Setup screen size macro
	string screenSizeText = format("float2(%d,%d)", dwidth, dheight);
	D3DXMACRO screenSizeMacro = { "SCREEN_SIZE", screenSizeText.c_str() };
	defines.push_back(screenSizeMacro);

	// Setup SRGB macro
	D3DXMACRO srgbMacro = { "USE_SRGB", useSRGB ? "true" : "false" };
	defines.push_back(srgbMacro);

	// Setup depth reading method
	D3DXMACRO readHWDepthMacro = { "USE_HWDEPTH", "true" };
	if(readHWDepth) defines.push_back(readHWDepthMacro);

	// Setup scale macro
	stringstream ss;
	ss << Settings::get().getSsaoScale() << ".0";
	string scaleText = ss.str();
	D3DXMACRO scaleMacro = { "SCALE", scaleText.c_str() };
	defines.push_back(scaleMacro);

	// Setup strength macro
	D3DXMACRO strengthMacros[] = {
		{ "SSAO_STRENGTH_LOW", "1" },
		{ "SSAO_STRENGTH_MEDIUM", "1" },
		{ "SSAO_STRENGTH_HIGH", "1" }
	};
	defines.push_back(strengthMacros[strength]);

	// Setup blur macro
	if(blur == BLUR_SHARP) {
		D3DXMACRO sharpBlur = { "BLUR_SHARP", "1" };
		defines.push_back(sharpBlur);
	}

	D3DXMACRO null = { NULL, NULL };
	defines.push_back(null);

	DWORD flags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_OPTIMIZATION_LEVEL3;

	// Load effect from file
	string shaderFn;
	switch(type) {
	case HBAO: shaderFn = "HBAO.fx"; break;
	case MSSAO: shaderFn = "martinsh_ssao.fx"; break;
	case SAO: shaderFn = "SAO.fx"; break;
	case VSSAO2: shaderFn = "VSSAO2.fx"; break;
	}
	string customfn = getConfigFileName(getExeFileName() + "\\" + shaderFn);
	if(boost::filesystem::exists(customfn)) shaderFn = customfn;
	else shaderFn = getAssetFileName(shaderFn);
	SDLOG(0, "%s load, scale %s, strength %s\n", shaderFn.c_str(), scaleText.c_str(), strengthMacros[strength].Name);
	ID3DXBuffer* errors;
	ID3DXEffect* newEffect;
	HRESULT hr = D3DXCreateEffectFromFile(device, shaderFn.c_str(), &defines.front(), NULL, flags, NULL, &newEffect, &errors);
	if(hr != D3D_OK) {
		SDLOG(-1, "ERRORS:\n %s\n", errors->GetBufferPointer());
		Console::get().add(format("Error compiling %s:", shaderFn.c_str()));
		Console::get().add(static_cast<const char*>(errors->GetBufferPointer()));
	}
	else {
		SAFERELEASE(effect);
		effect = newEffect;
	}

	// get handles
	depthTexHandle = effect->GetParameterByName(NULL, "depthTex2D");
	frameTexHandle = effect->GetParameterByName(NULL, "frameTex2D");
	prevPassTexHandle = effect->GetParameterByName(NULL, "prevPassTex2D");
	isBlurHorizontalHandle = effect->GetParameterByName(NULL, "isBlurHorizontal");

	if(type == SSAO::SAO) {
		projInfoHandle = effect->GetParameterByName(NULL, "projInfo");

		// 1 iteration is not enough imo
		blurPasses = 2;

		D3DXMATRIX matProjection;
		D3DXVECTOR4 projInfo;
		//width = 1280; height = 720;
		// values from http://www.rjdown.co.uk/projects/bfbc2/fovcalculator.php. 16/9 : 59, 16/10 : 65
		// D3DX_PI/2 or D3DX_PI/4 could suffice I don't know for sure
		int FOV = dheight * 16 > dwidth * 9 ? 65 : 59;
		D3DXMatrixPerspectiveFovLH(&matProjection, (FLOAT)D3DXToRadian(FOV), ((FLOAT)dwidth / (FLOAT)dheight), (FLOAT)0.01, (FLOAT)100.0); // Dummy nearZ/farZ
	
		projInfo.x = -2.0f / ((float)dwidth * matProjection._11);
		projInfo.y = -2.0f / ((float)dheight * matProjection._22),
		projInfo.z = ((1.0f - matProjection._13) / matProjection._11) + projInfo.x * 0.5f;
		projInfo.w = ((1.0f + matProjection._23) / matProjection._22) + projInfo.y * 0.5f;

		effect->SetVector(projInfoHandle, &projInfo);
	}
	else if(type == SSAO::HBAO) {
		string noiseFile = getAssetFileName("RandomNoiseB.png");
		hr = D3DXCreateTextureFromFile(device, noiseFile.c_str(), &noiseTex);

		//int fovY = dheight * 16 > dwidth * 9 ? 65 : 59;
		int fovY = static_cast<int>(D3DX_PI/3.0f);
		float m_FocalLen[2];
		m_FocalLen[0] = 1.0f / tanf(fovY * 0.5f) *  (float)dwidth / (float)dheight;
		m_FocalLen[1] = 1.0f / tanf(fovY * 0.5f);

		m_pFocalLenHandle = effect->GetParameterByName(NULL, "g_FocalLen");
		noiseTexHandle = effect->GetParameterByName(NULL, "noiseTexture");

		effect->SetFloatArray(m_pFocalLenHandle, m_FocalLen, 2);
	}
}

void SSAO::goHDR(IDirect3DTexture9 *frame, IDirect3DTexture9 *depth, IDirect3DSurface9 *dst) {
	device->SetVertexDeclaration(vertexDeclaration);

	if(dumping) {
		RSManager::getDX9().dumpTexture("SSAO_PRE_frame", frame);
		RSManager::getDX9().dumpTexture("SSAO_PRE_depth", depth);
		RSManager::getDX9().dumpSurface("SSAO_PRE_dest", dst);
	}

	if(type == SSAO::SAO || type == SSAO::HBAO) {
		reconstructCSZPass(depth, CSZBuffer->getSurf());
		mainSsaoPass(CSZBuffer->getTex(), buffer1->getSurf());
	}
	else {
		mainSsaoPass(depth, buffer1->getSurf());
	}

	if(dumping) {
		RSManager::getDX9().dumpTexture("SSAO_MD1_buffer1", buffer1->getTex());
	}

	for(size_t i = 0; i<blurPasses; ++i) {
		blurPass(depth, buffer1->getTex(), buffer2->getSurf(), true);
		blurPass(depth, buffer2->getTex(), buffer1->getSurf(), false);
	}

	if(dumping) {
		RSManager::getDX9().dumpTexture("SSAO_MD2_buffer1", buffer1->getTex());
	}

	if((type == SSAO::SAO || type == SSAO::HBAO) && isAspectRatioFix) {
		aspectRatioPass(buffer1->getTex(), hdrBuffer->getSurf());
		combinePass(frame, hdrBuffer->getTex(), hdrBuffer->getSurf());
	}
	else
		combinePass(frame, buffer1->getTex(), hdrBuffer->getSurf());

	if(dumping) {
		RSManager::getDX9().dumpSurface("SSAO_END_buffer2", hdrBuffer->getSurf());
	}

	device->StretchRect(hdrBuffer->getSurf(), NULL, dst, NULL, D3DTEXF_NONE);
	dumping = false;
}

void SSAO::reconstructCSZPass(IDirect3DTexture9 *depth, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);

	// Setup variables.
	effect->SetTexture(depthTexHandle, depth);

	// Do it!
	UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(0);
	quad(dwidth, dheight);
	effect->EndPass();
	effect->End();
}

void SSAO::mainSsaoPass(IDirect3DTexture9* depth, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);

	// Setup variables.
	effect->SetTexture(depthTexHandle, depth);
	if(type == SSAO::HBAO) effect->SetTexture(noiseTexHandle, noiseTex);

    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(type == SSAO::SAO || type == SSAO::HBAO ? 1 : 0);

	quad(dwidth, dheight);
	effect->EndPass();
	effect->End();
}

void SSAO::blurPass(IDirect3DTexture9 *depth, IDirect3DTexture9* src, IDirect3DSurface9* dst, bool horizontal) {
	device->SetRenderTarget(0, dst);

    // Setup variables.
    effect->SetTexture(prevPassTexHandle, src);
    effect->SetTexture(depthTexHandle, depth);
	effect->SetBool(isBlurHorizontalHandle, horizontal);
	
    // Do it!
	UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(type == SSAO::SAO || type == SSAO::HBAO ? 2 : horizontal ? 1 : 2);
	quad(dwidth, dheight);
	effect->EndPass();
	effect->End();
}

// (Boulotaur2024) Doing a separate pass here because using aspectQuad() within the ssaoPass
// degrades the quality of SSAO quite a bit
void SSAO::aspectRatioPass(IDirect3DTexture9* src, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);

    // Setup variables.
    effect->SetTexture(prevPassTexHandle, src);
	
    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(3);
	aspectQuad(dwidth, dheight, bbWidth, bbHeight);
	effect->EndPass();
	effect->End();
}

void SSAO::combinePass(IDirect3DTexture9* frame, IDirect3DTexture9* ao, IDirect3DSurface9* dst) {
	device->SetRenderTarget(0, dst);

    // Setup variables.
    effect->SetTexture(prevPassTexHandle, ao);
    effect->SetTexture(frameTexHandle, frame);
	
    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(type == SSAO::SAO || type == SSAO::HBAO ? 4 : 3);

	quad(dwidth, dheight);
	effect->EndPass();
	effect->End();
}

void SSAO::dumpFrame() {
	dumping = true;
}
