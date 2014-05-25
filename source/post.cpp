#include "post.h"

#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include "settings.h"
#include "renderstate_manager.h"

Post::Post(IDirect3DDevice9 *device, int width, int height) 
	: Effect(device), width(width), height(height) {
	
	// Setup the defines for compiling the effect
    vector<D3DXMACRO> defines;

    // Setup pixel size macro
	stringstream sp;
	sp << "float2(1.0 / " << width << ", 1.0 / " << height << ")";
	string pixelSizeText = sp.str();
	D3DXMACRO pixelSizeMacro = { "PIXEL_SIZE", pixelSizeText.c_str() };
	defines.push_back(pixelSizeMacro);
	
    D3DXMACRO null = { NULL, NULL };
    defines.push_back(null);

	DWORD flags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_OPTIMIZATION_LEVEL3;

	// Load effect from file
	string shaderFn = "post.fx";
	if(Settings::get().getPostProcessingType() == "asmodean") shaderFn = "post_asmodean.fx";
	shaderFn = getAssetFileName(shaderFn);
	SDLOG(0, "%s load\n", shaderFn.c_str());	
	ID3DXBuffer* errors;
	HRESULT hr = D3DXCreateEffectFromFile(device, shaderFn.c_str(), &defines.front(), NULL, flags, NULL, &effect, &errors);
	if(hr != D3D_OK) SDLOG(0, "ERRORS:\n %s\n", errors->GetBufferPointer());
	
	// get handles
	thisframeTexHandle = effect->GetParameterByName(NULL, "thisframeTex");
}

Post::~Post() {
	SAFERELEASE(effect);
}

void Post::go(IDirect3DTexture9 *frame, IDirect3DSurface9 *dst) {
	device->SetVertexDeclaration(vertexDeclaration);
	
    device->SetRenderTarget(0, dst);
    effect->SetTexture(thisframeTexHandle, frame);

    // Do it!
    UINT passes;
	effect->Begin(&passes, 0);
	effect->BeginPass(0);
	quad(width, height);
	effect->EndPass();
	effect->End();
}
