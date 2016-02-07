#pragma once

#include "main.h"
#include "timer.h"
#include "external/stb_truetype.h"

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <memory>

#include <d3d11.h>

//////////////////////////////////////////////////////////////////// API independent

class ConsoleLine {
	Timer t;
	string msg;
	float ypos = -1.0f;
public:
	ConsoleLine(const string& msg) : msg(msg) {}
	float draw(float y);
};

struct StaticText {
	string text;
	float x, y;
	bool show = false;
	StaticText(string text, float x, float y) : text(text), x(x), y(y) {}
};
typedef std::shared_ptr<StaticText> StaticTextPtr;

class Console {
protected:
	typedef std::pair<float, float> Position;
	static Console* latest;
	static const unsigned MAX_LINES = 16;
	static const unsigned BMPSIZE = 512;

	vector<ConsoleLine> lines;
	vector<StaticTextPtr> statics;
	size_t start = 0;
	int width, height;
	float lineHeight;

	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs

	Console(int w, int h);

	virtual void beginDrawing() = 0;
	virtual void beginText() = 0;
	virtual void endText() = 0;
	virtual void quad(float x, float y, float w, float h) = 0;
	virtual void quad(const stbtt_aligned_quad& q) = 0;
	virtual void drawBGQuad(float x0, float y0, float x1, float y1) = 0;
	virtual bool hasDevice() = 0;

public:
	static Console& get();
	static void setLatest(Console *c);

	void add(const string& msg);
	void add(StaticTextPtr text);
	bool needsDrawing();
	void draw();

	Position print(float x, float y, const char *text, bool measure = false);
	
	int getW();
	int getH();

	virtual ~Console() {}
};

//////////////////////////////////////////////////////////////////// DirectX 9

class ConsoleDX9 : public Console {
	IDirect3DDevice9* device = nullptr;
	IDirect3DVertexDeclaration9* vertexDeclaration = nullptr;
	IDirect3DTexture9* fontTex = nullptr;
	LPD3DXEFFECT effect = nullptr;
	D3DXHANDLE rectColorHandle, textTex2DHandle;
	static const D3DVERTEXELEMENT9 vertexElements[3];

protected:
	virtual void beginDrawing() override;
	virtual void beginText() override;
	virtual void endText() override;
	virtual void quad(float x, float y, float w, float h) override;
	virtual void quad(const stbtt_aligned_quad& q) override;
	virtual void drawBGQuad(float x0, float y0, float x1, float y1) override;

	virtual bool hasDevice() override;

public:
	ConsoleDX9(IDirect3DDevice9* device, int w, int h);
	virtual ~ConsoleDX9();
};


//////////////////////////////////////////////////////////////////// DirectX 11

class RSManagerDX11;
struct ID3DX11Effect;

class ConsoleDX11 : public Console {
	ID3D11Texture2D* fontTex = nullptr;
	ID3D11ShaderResourceView* fontTexView = nullptr;
	ID3D11Buffer *vertexBuffer = nullptr, *indexBuffer = nullptr;
	ID3DX11Effect *effect = nullptr;

protected:
	ID3D11Device* device = nullptr;

	virtual void beginDrawing() override;
	virtual void beginText() override;
	virtual void endText() override;
	virtual void quad(float x, float y, float w, float h) override;
	virtual void quad(const stbtt_aligned_quad& q) override;
	virtual void drawBGQuad(float x0, float y0, float x1, float y1) override;

	virtual bool hasDevice() override;

public:
	ConsoleDX11(ID3D11Device* device, RSManagerDX11* manager, int w, int h);
	virtual ~ConsoleDX11();
};
