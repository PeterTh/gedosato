#include "console.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"
#undef STB_TRUETYPE_IMPLEMENTATION

#include <sstream>
#include <algorithm>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <d3dx11effect.h>
#include <d3dcompiler.h>

#include "renderstate_manager_dx11.h"

Console* Console::latest = nullptr;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// API independent

float ConsoleLine::draw(float y) {
	if(ypos<0.0f) ypos = y;
	else ypos += (y-ypos)*0.2f; 
	SDLOG(15, "Printed: %s at %f\n", msg, (10.0f + ypos));
	Console::get().print(25.0f, (39.0f + ypos), msg.c_str());
	return t.elapsed() > Settings::get().getMessageSeconds()*1000000.0 ? 0.0f : 38.0f + ypos;
}

Console::Console(int w, int h) : width(w), height(h) {}

Console& Console::get() {
	if(latest == NULL) SDLOG(0, "ERROR: NULL Console\n");
	return *latest;
}

void Console::setLatest(Console *c) {
	latest = c;
}

void Console::setSize(int w, int h) {
	width = w;
	height = h;
}

void Console::add(const string& msg) {
	SDLOG(1, "Console add: %s\n", msg.c_str());
	if(Settings::get().getMessageSeconds() > 0) {
		std::vector<std::string> splitstring;
		boost::algorithm::split(splitstring, msg, boost::is_any_of("\n\r"));
		for(const auto& s : splitstring) lines.push_back(ConsoleLine(s));
		lineHeight = 1.0f;
	}
}

void Console::add(StaticTextPtr text) {
	statics.push_back(text);
}

bool Console::needsDrawing() {
	return hasDevice() && (lineHeight > 0.0f 
		|| std::any_of(statics.begin(), statics.end(), [](const StaticTextPtr& p) { return p->show; }));
}

void Console::draw() {
	SDLOG(-1, "Drawing console\n");
	beginDrawing();

	// draw background quad
	if(lineHeight > 0.0f) {
		drawBGQuad(-1.0f, -1.0f, 2.0f, 2.0f);
	}
	float y = 0.0f;

	//// draw lines
	//if(lines.size() - start > MAX_LINES) start += lines.size() - start - MAX_LINES;
	//for(size_t i = start; i < lines.size(); ++i) {
	//	float ret = lines[i].draw(y);
	//	if(ret == 0.0f) start = i + 1; // if text timed out increase start
	//	else y = ret + 2.0f;
	//}
	//if(y == 0.0f) {
	//	if(lineHeight > 0.2f) lineHeight *= 0.6f;
	//	else {
	//		lineHeight = 0.0f;
	//		// clear lines
	//		lines.clear();
	//		start = 0;
	//	}
	//}
	//else lineHeight = y + 15.0f;

	//// draw static text
	//for(auto& t : statics) {
	//	if(t->show) {
	//		auto p = print(t->x, t->y, t->text.c_str(), true);
	//		float wborder = 4.0f / width, hborder = 4.0f / height;
	//		float bgx = t->x / width - 1.0f, bgy = -(t->y - 32.0f) / height + 1.0f;
	//		drawBGQuad(bgx - wborder, bgy + hborder, p.first - bgx + wborder * 8, p.second - bgy - hborder * 8);
	//		print(t->x, t->y, t->text.c_str());
	//	}
	//}
}

Console::Position Console::print(float x, float y, const char *text, bool measure) {
	float xstart = x;
	Position ret = std::make_pair(-100.0f, 100.0f);
	if(!measure) {
		beginText();
	}
	while(*text) {
		if(*text >= 32 && *text < 128) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(cdata, BMPSIZE, BMPSIZE, *text - 32, &x, &y, &q, 1); // 1=opengl, 0=old d3d
			if(!measure) {
				quad(q);
			}
			ret.first = max(ret.first, q.x0 / width - 1.0f);
			ret.second = min(ret.second, -q.y0 / height + 1.0f);
		}
		if(*text == '\n') {
			y += 36.0f;
			x = xstart;
		}
		++text;
	}
	if(!measure) {
		endText();
	}
	return ret;
}

int Console::getW() {
	return width;
}

int Console::getH() {
	return height;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DirectX 9
	
const D3DVERTEXELEMENT9 ConsoleDX9::vertexElements[3] = {
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },
    D3DDECL_END()
};

ConsoleDX9::ConsoleDX9(IDirect3DDevice9* device, int w, int h) : Console(w, h), device(device) {
	SDLOG(0, "Initializing DX9 Console on device %p\n", device);
	
	// Create font
	SDLOG(2, " - creating console font\n");
	FILE* ff = fopen(getAssetFileName("font.ttf").c_str(), "rb");
	unsigned char* ttf_buffer = new unsigned char[1<<20];
	unsigned char* temp_bitmap = new unsigned char[BMPSIZE*BMPSIZE];
	fread(ttf_buffer, 1, 1<<20, ff);
	fclose(ff);
	stbtt_BakeFontBitmap(ttf_buffer, 0, 34.0, temp_bitmap, BMPSIZE, BMPSIZE, 32, 96, cdata); // no guarantee this fits!
	device->CreateTexture(BMPSIZE, BMPSIZE, 0, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8, D3DPOOL_DEFAULT, &fontTex, NULL);
	IDirect3DTexture9 *tempTex;
	device->CreateTexture(BMPSIZE, BMPSIZE, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8, D3DPOOL_SYSTEMMEM, &tempTex, NULL);
	D3DLOCKED_RECT rect;
	tempTex->LockRect(0, &rect, NULL, 0);
	memcpy(rect.pBits, temp_bitmap, BMPSIZE*BMPSIZE);
	tempTex->UnlockRect(0);
	device->UpdateTexture(tempTex, fontTex);
	tempTex->Release();
	delete [] ttf_buffer;
	delete [] temp_bitmap;
	
	// Create vertex decl
	SDLOG(2, " - creating console vertex decl\n");
	device->CreateVertexDeclaration(vertexElements , &vertexDeclaration);

	// Load effect from file
	SDLOG(2, " - loading console effect file\n");
	vector<D3DXMACRO> defines;
	std::stringstream s;
	D3DXMACRO null = { NULL, NULL };
	defines.push_back(null);
	DWORD flags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_OPTIMIZATION_LEVEL3;

	SDLOG(2, " - actually load effect\n");	
	ID3DXBuffer* errors;
	HRESULT hr = D3DXCreateEffectFromFile(device, getAssetFileName("console.fx").c_str(), &defines.front(), NULL, flags, NULL, &effect, &errors);
	if(hr != D3D_OK) SDLOG(0, "ERRORS:\n %s\n", errors->GetBufferPointer());

	// get handles
	rectColorHandle = effect->GetParameterByName(NULL, "rectColor");
	textTex2DHandle = effect->GetParameterByName(NULL, "textTex2D");

	SDLOG(0, " - done\n");
}

ConsoleDX9::~ConsoleDX9() {
	SDLOG(2, "ConsoleDX9 cleanup\n")
	SAFERELEASE(vertexDeclaration);
	SAFERELEASE(effect);
	SAFERELEASE(fontTex);
}

void ConsoleDX9::beginDrawing() {
	device->SetVertexDeclaration(vertexDeclaration);
}

void ConsoleDX9::beginText() {
	unsigned passes;
	effect->SetTexture(textTex2DHandle, fontTex);
	effect->Begin(&passes, 0);
	effect->BeginPass(1);
}

void ConsoleDX9::endText() {
	effect->EndPass();
	effect->End();
}

void ConsoleDX9::quad(float x, float y, float w, float h) {
	float quad[4][5] = {
		{ x  , y  , 0.0f, 0.0f, 0.0f },
		{ x+w, y  , 0.0f, 1.0f, 0.0f },
		{ x  , y+h, 0.0f, 0.0f, 1.0f },
		{ x+w, y+h, 0.0f, 1.0f, 1.0f }
	};
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &quad[0], sizeof(quad[0]));
}

void ConsoleDX9::quad(const stbtt_aligned_quad& q) {
	float quad[4][5] = {
		{ -1.0f + q.x0/width, 1.0f - q.y0/height, 0.0f, q.s0, q.t0 },
		{ -1.0f + q.x1/width, 1.0f - q.y0/height, 0.0f, q.s1, q.t0 },
		{ -1.0f + q.x0/width, 1.0f - q.y1/height, 0.0f, q.s0, q.t1 },
		{ -1.0f + q.x1/width, 1.0f - q.y1/height, 0.0f, q.s1, q.t1 }
	};
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &quad[0], sizeof(quad[0]));
}

void ConsoleDX9::drawBGQuad(float x0, float y0, float w, float h) {
	unsigned passes;
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
	effect->SetFloatArray(rectColorHandle, color, 4);
	effect->Begin(&passes, 0);
	effect->BeginPass(0);
	quad(x0, y0, w, h);
	effect->EndPass();
	effect->End();
}

bool ConsoleDX9::hasDevice() {
	return device != nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DirectX 11

struct VertexType {
	D3DXVECTOR3 position;
	D3DXVECTOR2 texture;
};

ConsoleDX11::ConsoleDX11(ID3D11Device* device, RSManagerDX11* manager, int w, int h) : Console(w,h), device(device) {
	SDLOG(0, "Initializing DX11 Console on device %p, wxh: %dx%d\n", device, width, height);
	DX11InternalRefHelper helper(device, manager);

	// Create font
	SDLOG(2, " - creating console font\n");
	FILE* ff = fopen(getAssetFileName("font.ttf").c_str(), "rb");
	unsigned char* ttf_buffer = new unsigned char[1 << 20];
	unsigned char* temp_bitmap = new unsigned char[BMPSIZE*BMPSIZE];
	fread(ttf_buffer, 1, 1 << 20, ff);
	fclose(ff);
	stbtt_BakeFontBitmap(ttf_buffer, 0, 34.0, temp_bitmap, BMPSIZE, BMPSIZE, 32, 96, cdata); // no guarantee this fits!
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = BMPSIZE;
	texDesc.Height = BMPSIZE;
	texDesc.Format = DXGI_FORMAT_A8_UNORM;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc = {1,0};
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = temp_bitmap;
	data.SysMemPitch = BMPSIZE;
	data.SysMemSlicePitch = BMPSIZE*BMPSIZE;
	HRESULT texHr = device->CreateTexture2D(&texDesc, &data, &fontTex);
	CHECKHR(texHr, "Console: failed to create font texture\n");
	delete[] ttf_buffer;
	delete[] temp_bitmap;

	// Create vertex buffer
	SDLOG(2, " - creating console vertex buffer\n");
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	auto vbuffHr = device->CreateBuffer(&vertexBufferDesc, nullptr, &vertexBuffer);
	CHECKHR(vbuffHr, "Console: failed to create vertex buffer\n");

	// Create the index buffer
	D3D11_SUBRESOURCE_DATA indexData;
	unsigned long indices[4] = { 0, 1, 2, 3};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * 4;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	auto ibufferHr = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	CHECKHR(ibufferHr, "Console: failed to create index buffer\n");
	
	// Load effect from file
	SDLOG(2, " - loading console effect file\n");
	D3D_SHADER_MACRO defines[] = { { "DX11", "1" }, {nullptr, nullptr} };
	ID3DBlob *errors;
	auto effectHr = D3DX11CompileEffectFromFile(strToWStr(getAssetFileName("console.fx")).c_str(), defines, nullptr, D3DCOMPILE_OPTIMIZATION_LEVEL2 | D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY, 0, device, &effect, &errors);
	CHECKHR(effectHr, "Console: failed to create effect:\n%s", (char*)errors->GetBufferPointer());
	
	// Set variables
	auto rectColorVar = effect->GetVariableByName("rectColor")->AsVector();
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
	CHECKHR(rectColorVar->SetFloatVector(color), "Console: failed to set rectColorVar\n");
	auto texVar = effect->GetVariableByName("textTex2D")->AsShaderResource();
	CHECKHR(device->CreateShaderResourceView(fontTex, nullptr, &fontTexView), "Console: failed to create shader resource view for font texture\n");
	CHECKHR(texVar->SetResource(fontTexView), "Console: failed to set texVar\n");

	// Create input layout
	D3DX11_PASS_SHADER_DESC passShaderDesc;
	effect->GetTechniqueByIndex(0)->GetPassByIndex(0)->GetVertexShaderDesc(&passShaderDesc);
	D3DX11_EFFECT_SHADER_DESC effectVsDesc;
	passShaderDesc.pShaderVariable->GetShaderDesc(passShaderDesc.ShaderIndex, &effectVsDesc);
	const void *vsCodePtr = effectVsDesc.pBytecode;
	unsigned vsCodeLen = effectVsDesc.BytecodeLength;

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	CHECKHR(device->CreateInputLayout(inputDesc, _countof(inputDesc), vsCodePtr, vsCodeLen, &inputLayout), "Console: failed to create input layout\n");
	
	SDLOG(0, " - done\n");
}

ConsoleDX11::~ConsoleDX11() {
	SDLOG(-1, "Console DX11 delete\n");
	SAFERELEASE(inputLayout);
	SAFERELEASE(effect);
	SAFERELEASE(fontTexView);
	SAFERELEASE(fontTex);
	SAFERELEASE(vertexBuffer);
	SAFERELEASE(indexBuffer);
}

void ConsoleDX11::beginDrawing() {
	ID3D11DeviceContext *immediateContext;
	device->GetImmediateContext(&immediateContext);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->Release();
}

void ConsoleDX11::beginText() {
	ID3D11DeviceContext *immediateContext;
	device->GetImmediateContext(&immediateContext);
	effect->GetTechniqueByIndex(0)->GetPassByIndex(1)->Apply(0, immediateContext);
	immediateContext->Release();
}

void ConsoleDX11::endText() {

}

void ConsoleDX11::quad(float x, float y, float w, float h) {
	ID3D11DeviceContext *immediateContext;
	device->GetImmediateContext(&immediateContext);
	float quad[4][5] = {
		{ x    , y    , 0.0f, 0.0f, 0.0f },
		{ x + w, y    , 0.0f, 1.0f, 0.0f },
		{ x    , y + h, 0.0f, 0.0f, 1.0f },
		{ x + w, y + h, 0.0f, 1.0f, 1.0f }
	};
	D3D11_MAPPED_SUBRESOURCE mapped;
	immediateContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, quad, sizeof(quad));
	immediateContext->Unmap(vertexBuffer, 0);

	UINT strides[] = { sizeof(VertexType) }, offsets[] = { 0 };
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
	//immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//immediateContext->DrawIndexed(4, 0, 0);
	immediateContext->Draw(4, 0);
	immediateContext->Release();
}

void ConsoleDX11::quad(const stbtt_aligned_quad& q) {
	ID3D11DeviceContext *immediateContext;
	device->GetImmediateContext(&immediateContext);
	float quad[4][5] = {
		{ -1.0f + q.x0 / width, 1.0f - q.y0 / height, 0.0f, q.s0, q.t0 },
		{ -1.0f + q.x1 / width, 1.0f - q.y0 / height, 0.0f, q.s1, q.t0 },
		{ -1.0f + q.x0 / width, 1.0f - q.y1 / height, 0.0f, q.s0, q.t1 },
		{ -1.0f + q.x1 / width, 1.0f - q.y1 / height, 0.0f, q.s1, q.t1 }
	};
	D3D11_MAPPED_SUBRESOURCE mapped;
	immediateContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, quad, sizeof(quad));
	immediateContext->Unmap(vertexBuffer, 0);

	UINT strides[] = { sizeof(VertexType) }, offsets[] = { 0 };
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
	//immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//immediateContext->DrawIndexed(4, 0, 0);
	immediateContext->Draw(4, 0);
	immediateContext->Release();
}

void ConsoleDX11::drawBGQuad(float x0, float y0, float w, float h) {
	ID3D11DeviceContext *immediateContext;
	device->GetImmediateContext(&immediateContext);
	effect->GetTechniqueByIndex(0)->GetPassByIndex(0)->Apply(0, immediateContext);
	quad(x0, y0, w, h);
	immediateContext->Release();
}

bool ConsoleDX11::hasDevice() {
	return device != nullptr;
}