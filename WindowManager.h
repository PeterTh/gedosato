#pragma once

#include <Windows.h>

class WindowManager {
	static WindowManager instance;
	
	bool captureCursor, cursorVisible;

	bool borderlessFullscreen;
	RECT prevWindowRect;
	long prevStyle, prevExStyle;

public:
	static WindowManager& get() {
		return instance;
	}

	WindowManager() : captureCursor(false), cursorVisible(true), borderlessFullscreen(false) { }
	void applyCursorCapture();
	void toggleCursorCapture();
	void toggleCursorVisibility();
	void toggleBorderlessFullscreen();
	void maintainBorderlessFullscreen();
	void maintainWindowSize();
	void resize(unsigned clientW, unsigned clientH);
};
