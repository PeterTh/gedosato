#pragma once

#include "main.h"
#include "timer.h"
#include "external/stb_truetype.h"

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <memory>

class ConsoleLine {
	Timer t;
	string msg;
	float ypos;

public:
	ConsoleLine(const string& msg) : msg(msg), ypos(-1.0f) {
	}

	float draw(float y);
};

struct StaticText {
	string text;
	float x, y;
	bool show;

	StaticText(string text, float x, float y)
		: text(text), x(x), y(y), show(false) {
	}
};

typedef std::shared_ptr<StaticText> StaticTextPtr;

class Console {
	typedef std::pair<float, float> Position;
	static Console* latest;
	static const unsigned MAX_LINES = 16;
	static const unsigned BMPSIZE = 512;

	vector<ConsoleLine> lines;
	vector<StaticTextPtr> statics;
	size_t start = 0;
	int width, height;
	float lineHeight;

	IDirect3DDevice9* device = nullptr;
    IDirect3DVertexDeclaration9* vertexDeclaration = nullptr;
	IDirect3DTexture9* fontTex = nullptr;
	LPD3DXEFFECT effect = nullptr;
	D3DXHANDLE rectColorHandle, textTex2DHandle;
    static const D3DVERTEXELEMENT9 vertexElements[3];
	
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
	
	void quad(float x, float y, float w, float h);
	void quad(const stbtt_aligned_quad& q);

	void drawBGQuad(float x0, float y0, float x1, float y1);

public:
	Console() {	}
	~Console() {
		SDLOG(0, "Deleting Console on device %p\n", device);
		cleanup();
	}

	static Console& get() { 
		if(latest == NULL) SDLOG(0, "ERROR: NULL Console\n");
		return *latest; 
	}
	static void setLatest(Console *c) {
		latest = c;
	}

	void add(const string& msg);

	void add(StaticTextPtr text) {
		statics.push_back(text);
	}

	bool needsDrawing(); 
	void draw();

	Position print(float x, float y, const char *text, bool measure = false);

	void initialize(IDirect3DDevice9* device, int w, int h);
	void cleanup();

	int getW() { return width; }
	int getH() { return height; }
};
