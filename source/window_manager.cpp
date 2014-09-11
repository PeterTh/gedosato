#include "window_manager.h"

#include "detouring.h"
#include "settings.h"
#include "utils/d3d9_utils.h"
#include "utils/win_utils.h"
#include "utils/string_utils.h"

WindowManager WindowManager::instance;

void WindowManager::applyCursorCapture() {
	if(captureCursor) {
		RECT clientrect;
		HWND hwnd = ::GetActiveWindow();
		TrueGetClientRect(hwnd, &clientrect);
		::ClientToScreen(hwnd, (LPPOINT)&clientrect.left);
		::ClientToScreen(hwnd, (LPPOINT)&clientrect.right);
		::ClipCursor(&clientrect);
	} else {
		::ClipCursor(NULL);
	}
}

void WindowManager::toggleCursorCapture() {
	captureCursor = !captureCursor;
}

void WindowManager::toggleCursorVisibility() {	
	cursorVisible = !cursorVisible;
	::ShowCursor(cursorVisible);
}

void WindowManager::toggleBorderlessFullscreen() {
	borderlessFullscreen = !borderlessFullscreen;
	HWND hwnd = ::GetActiveWindow();
	if(borderlessFullscreen) {
		SDLOG(1, "WindowManager::toggleBorderlessFullscreen A hwnd: %p\n", hwnd);
		// store previous rect
		TrueGetClientRect(hwnd, &prevWindowRect);
		// set styles
		LONG lStyle = TrueGetWindowLongA(hwnd, GWL_STYLE);
		prevStyle = lStyle;
		lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
		TrueSetWindowLongA(hwnd, GWL_STYLE, lStyle);
		LONG lExStyle = TrueGetWindowLongA(hwnd, GWL_EXSTYLE);
		prevExStyle = lExStyle;
		lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		TrueSetWindowLongA(hwnd, GWL_EXSTYLE, lExStyle);
		SDLOG(1, "WindowManager::toggleBorderlessFullscreen B\n", hwnd);
		// adjust size & position
		HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO info;
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(monitor, &info);
		int monitorWidth = info.rcMonitor.right - info.rcMonitor.left;
		int monitorHeight = info.rcMonitor.bottom - info.rcMonitor.top;
		::SetWindowPos(hwnd, NULL, info.rcMonitor.left, info.rcMonitor.top, monitorWidth, monitorHeight, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);
		BringWindowToTop(hwnd);
		SDLOG(1, "WindowManager::toggleBorderlessFullscreen C\n", hwnd);
	} else {
		// restore previous window
		::SetWindowLong(hwnd, GWL_STYLE, prevStyle);
		::SetWindowLong(hwnd, GWL_EXSTYLE, prevExStyle);
		RECT desiredRect = prevWindowRect;
		::AdjustWindowRect(&desiredRect, prevStyle, false);
		int wWidth = desiredRect.right - desiredRect.left, wHeight = desiredRect.bottom - desiredRect.top;
		::SetWindowPos(hwnd, NULL, prevWindowRect.left, prevWindowRect.top, wWidth, wHeight, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}
}

void WindowManager::maintainBorderlessFullscreen() {
	if(borderlessFullscreen) {
		HWND hwnd = ::GetActiveWindow();
		RECT rect;
		TrueGetWindowRect(hwnd, &rect);
		HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO info;
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(monitor, &info);
		if(rect.left != info.rcMonitor.left || rect.top != info.rcMonitor.top || rect.right != info.rcMonitor.right || rect.bottom != info.rcMonitor.bottom) {
			SDLOG(0, "Restoring borderless window, previous rect: %s, monitor: %s\n", RectToString(&rect).c_str(), RectToString(&info.rcMonitor).c_str());
			borderlessFullscreen = false;
			toggleBorderlessFullscreen();
		}
	}
}

void WindowManager::forceBorderlessFullscreen() {
	borderlessFullscreen = true;
	maintainBorderlessFullscreen();
}

void WindowManager::resize(unsigned clientW, unsigned clientH) {
	SDLOG(19, "WindowManager::resize A\n");
	HWND hwnd = ::GetActiveWindow();
	SDLOG(19, "WindowManager::resize B %p\n", hwnd);
	// Store current window rect
	TrueGetClientRect(hwnd, &prevWindowRect);
	SDLOG(19, "WindowManager::resize C\n");
	// Get monitor size
	HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);
	int monitorWidth = info.rcMonitor.right - info.rcMonitor.left;
	int monitorHeight = info.rcMonitor.bottom - info.rcMonitor.top;
	SDLOG(1, "WindowManager::resize D\n");

	// How much do we overlap or are smaller than the actual screen size
	int widthDiff = monitorWidth - (clientW ? clientW : prevWindowRect.right);
	int heightDiff = monitorHeight - (clientH ? clientH : prevWindowRect.bottom);

 	RECT desiredRect;
	desiredRect.left = widthDiff / 2;
	desiredRect.top = heightDiff / 2;
	desiredRect.right = monitorWidth - (widthDiff / 2);
	desiredRect.bottom = monitorHeight - (heightDiff / 2);
	SDLOG(19, "WindowManager::resize E\n");
	LONG lStyle = TrueGetWindowLongA(hwnd, GWL_STYLE);
	SDLOG(19, "WindowManager::resize F style: %d desiredRect: %s\n", lStyle, RectToString(&desiredRect).c_str());
	::AdjustWindowRect(&desiredRect, lStyle, false);
	SDLOG(19, "WindowManager::resize G\n");
	::SetWindowPos(hwnd, NULL, desiredRect.left, desiredRect.top, desiredRect.right - desiredRect.left, desiredRect.bottom - desiredRect.top, SWP_NOZORDER);
	SDLOG(19, "WindowManager::resize H\n");
	SDLOG(0, "Set Window rect to %s\n", RectToString(&desiredRect).c_str());
}

void WindowManager::addCaption() {
	HWND hwnd = ::GetActiveWindow();
	// set styles
	LONG lStyle = TrueGetWindowLongA(hwnd, GWL_STYLE);
	lStyle |= WS_OVERLAPPEDWINDOW;
	TrueSetWindowLongA(hwnd, GWL_STYLE, lStyle);
	LONG lExStyle = TrueGetWindowLongA(hwnd, GWL_EXSTYLE);
	lExStyle |= WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE;
	TrueSetWindowLongA(hwnd, GWL_EXSTYLE, lExStyle);
	SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void WindowManager::maintainWindowSize() {
	if(!borderlessFullscreen) {
		HWND hwnd = ::GetActiveWindow();
		RECT rect;
		::GetWindowRect(hwnd, &rect);
		
		if(rect.right-rect.left < (long)Settings::get().getPresentWidth() || rect.bottom-rect.top < (long)Settings::get().getPresentHeight()) {
			SDLOG(2, "Restoring window size, previous rect: %s\n", RectToString(&rect).c_str());
			resize(Settings::get().getPresentWidth(), Settings::get().getPresentHeight());
		}
	}
}

void WindowManager::setFakeFullscreen(unsigned w, unsigned h) {
	fakeWidth = w;
	fakeHeight = h;
	forceBorderlessFullscreen();
}

void WindowManager::interceptGetDisplayMode(D3DDISPLAYMODE* pMode) {
	if(borderlessFullscreen) {
		pMode->Width = fakeWidth;
		pMode->Height = fakeHeight;
		SDLOG(2, "WindowManager faking GetDisplaMode\n");
	}
}
