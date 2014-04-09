#pragma once

#include "main.h"

#include "Timer.h"

#include "stb_truetype.h"
#include <string>
using std::string;
#include <vector>
using std::vector;

class ConsoleLine {
	Timer t;
	string msg;
	float ypos;

public:
	ConsoleLine(const string& msg) : msg(msg), ypos(-1.0f) {
	}

	float draw(float y);
};

class Console {
	static Console* latest;
	static const unsigned MAX_LINES = 6;
	static const unsigned BMPSIZE = 512;

	vector<ConsoleLine> lines;
	int start, width, height;
	float lineHeight;

	IDirect3DDevice9* device;
    IDirect3DVertexDeclaration9* vertexDeclaration;
	IDirect3DTexture9* fontTex;
	LPD3DXEFFECT effect;
	D3DXHANDLE rectColorHandle, textTex2DHandle;
    static const D3DVERTEXELEMENT9 vertexElements[3];
	
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
	
	void quad(float x, float y, float w, float h);
	void quad(const stbtt_aligned_quad& q);

public:
	Console() : start(0), device(NULL), vertexDeclaration(NULL), fontTex(NULL), effect(NULL) {
	}
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

	void add(const string& msg) {
		SDLOG(1, "Console add: %s\n", msg.c_str());
		lines.push_back(ConsoleLine(msg));
		lineHeight = 1.0f;
	}

	bool needsDrawing() { return device && lineHeight > 0.0f; } 
	void draw();

	void print(float x, float y, const char *text);

	void initialize(IDirect3DDevice9* device, int w, int h);
	void cleanup();

	int getW() { return width; }
	int getH() { return height; }
};
