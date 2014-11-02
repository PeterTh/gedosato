#include "console.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"
#undef STB_TRUETYPE_IMPLEMENTATION

#include <sstream>
#include <algorithm>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

Console* Console::latest = NULL;
	
const D3DVERTEXELEMENT9 Console::vertexElements[3] = {
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },
    D3DDECL_END()
};

float ConsoleLine::draw(float y) {
	if(ypos<0.0f) ypos = y;
	else ypos += (y-ypos)*0.2f; 
	//cout << "Printed: " << msg << " at " << (10.0f + ypos) << "\n";
	Console::get().print(25.0f, (39.0f + ypos), msg.c_str());
	return t.elapsed() > Settings::get().getMessageSeconds()*1000000.0 ? 0.0f : 38.0f + ypos;
}

void Console::initialize(IDirect3DDevice9* device, int w, int h) {
	SDLOG(0, "Initializing Console on device %p\n", device);
	width = w;
	height = h;
	this->device = device;
	
	// Create font
	SDLOG(2, " - creating console font\n");
	SAFERELEASE(fontTex);
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
	delete ttf_buffer;
	delete temp_bitmap;
	
	// Create vertex decl
	SDLOG(2, " - creating console vertex decl\n");
	SAFERELEASE(vertexDeclaration);
	device->CreateVertexDeclaration(vertexElements , &vertexDeclaration);

	// Load effect from file
	SDLOG(2, " - loading console effect file\n");
	SAFERELEASE(effect);
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

void Console::cleanup() {
	SDLOG(2, "Console cleanup\n")
	device = NULL;
	SAFERELEASE(vertexDeclaration);
	SAFERELEASE(effect);
	SAFERELEASE(fontTex);
}


void Console::drawBGQuad(float x0, float y0, float w, float h) {
	unsigned passes;
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
	effect->SetFloatArray(rectColorHandle, color, 4);
	effect->Begin(&passes, 0);
	effect->BeginPass(0);
	quad(x0, y0, w, h);
	effect->EndPass();
	effect->End();
}

void Console::draw() {
	SDLOG(5, "Drawing console\n");
	device->SetVertexDeclaration(vertexDeclaration);

	// draw background quad
	if(lineHeight > 0.0f) {
		drawBGQuad(-1.0f, 1.0f, 2.0f, -lineHeight / height);
	}
	float y = 0.0f;

	// draw lines
	if(lines.size()-start > MAX_LINES) start += lines.size()-start-MAX_LINES; 
	for(size_t i=start; i<lines.size(); ++i) {
		float ret = lines[i].draw(y);
		if(ret == 0.0f) start = i+1; // if text timed out increase start
		else y = ret + 2.0f;
	}
	if(y == 0.0f) {
		if(lineHeight>0.2f) lineHeight *= 0.6f;
		else lineHeight = 0.0f;
	}
	else lineHeight = y + 15.0f;

	// draw static text
	for(auto& t : statics) {
		if(t->show) {
			auto p = print(t->x, t->y, t->text.c_str(), true);
			float wborder = 4.0f / width, hborder = 4.0f / height;
			float bgx = t->x / width - 1.0f, bgy = -(t->y - 32.0f) / height + 1.0f;
			drawBGQuad(bgx - wborder, bgy + hborder, p.first - bgx + wborder*8, p.second - bgy - hborder*8);
			print(t->x, t->y, t->text.c_str());
		}
	}
}

Console::Position Console::print(float x, float y, const char *text, bool measure) {
	unsigned passes;
	float xstart = x;
	Position ret = std::make_pair(-100.0f, 100.0f);
	if(!measure) {
		effect->SetTexture(textTex2DHandle, fontTex);
		effect->Begin(&passes, 0);
		effect->BeginPass(1);
	}
	while(*text) {
		if(*text >= 32 && *text < 128) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(cdata, BMPSIZE, BMPSIZE, *text - 32, &x, &y, &q, 1); // 1=opengl, 0=old d3d
			if(!measure) {
				quad(q);
			}
			ret.first = max(ret.first, q.x0/width - 1.0f);
			ret.second = min(ret.second, -q.y0/height + 1.0f);
		}
		if(*text == '\n') {
			y += 36.0f;
			x = xstart;
		}
		++text;
	}
	if(!measure) {
		effect->EndPass();
		effect->End();
	}
	return ret;
}

void Console::quad(float x, float y, float w, float h) {
	float quad[4][5] = {
		{ x  , y  , 0.0f, 0.0f, 0.0f },
		{ x+w, y  , 0.0f, 1.0f, 0.0f },
		{ x  , y+h, 0.0f, 0.0f, 1.0f },
		{ x+w, y+h, 0.0f, 1.0f, 1.0f }
	};
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &quad[0], sizeof(quad[0]));
}

void Console::quad(const stbtt_aligned_quad& q) {
	float quad[4][5] = {
		{ -1.0f + q.x0/width, 1.0f - q.y0/height, 0.0f, q.s0, q.t0 },
		{ -1.0f + q.x1/width, 1.0f - q.y0/height, 0.0f, q.s1, q.t0 },
		{ -1.0f + q.x0/width, 1.0f - q.y1/height, 0.0f, q.s0, q.t1 },
		{ -1.0f + q.x1/width, 1.0f - q.y1/height, 0.0f, q.s1, q.t1 }
	};
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &quad[0], sizeof(quad[0]));
}

bool Console::needsDrawing() {
	return device && lineHeight > 0.0f 
		|| std::any_of(statics.begin(), statics.end(), [](const StaticTextPtr& p) { return p->show; });
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
