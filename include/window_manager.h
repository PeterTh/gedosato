#pragma once

#include <Windows.h>
#include <d3d9.h>

class WindowManager {
	static WindowManager instance;
	
	bool captureCursor, cursorVisible;

	bool borderlessFullscreen;
	RECT prevWindowRect;
	long prevStyle, prevExStyle;

	unsigned fakeWidth, fakeHeight;

public:
	static WindowManager& get() {
		return instance;
	}

	WindowManager() : captureCursor(false), cursorVisible(true), borderlessFullscreen(false) { }
	void applyCursorCapture();
	void toggleCursorCapture();
	void toggleCursorVisibility();
	void toggleBorderlessFullscreen();
	void forceBorderlessFullscreen();
	void maintainBorderlessFullscreen();
	void maintainWindowSize();
	void resize(unsigned clientW, unsigned clientH);

	void setFakeFullscreen(unsigned w, unsigned h);
	void interceptGetDisplayMode(D3DDISPLAYMODE* pMode);
};
