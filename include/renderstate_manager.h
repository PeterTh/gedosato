#pragma once

#include <map>
#include <set>
#include <vector>

#include "d3dperf.h"
#include "console.h"
#include "shader_manager.h"
#include "rendertarget_manager.h"
#include "imaging.h"
#include "game_plugin.h"

class RSManagerDX9;
class RSManagerDX11;
class Scaler;

class RSManager {
public:
	typedef enum { SCREENSHOT_NONE, SCREENSHOT_STANDARD, SCREENSHOT_FULL, SCREENSHOT_HUDLESS } ScreenshotType;

protected:
	static RSManager* latest;
	static bool forceDSoff;

	ScreenshotType takeScreenshot = SCREENSHOT_NONE;

	bool downsampling = false, dumpingFrame = false;
	unsigned dumpCaptureIndex = 0;

	unsigned renderWidth = 0, renderHeight = 0;
	unsigned numBackBuffers = 0;

	// Performance measurement
	Timer cpuFrameTimer;
	SlidingAverage cpuFrameTimes{ 120 };
	D3DPerfMonitor* perfMonitor = NULL;
	StaticTextPtr frameTimeText{ new StaticText("", 20.0f, 100.0f) };

	// Utilities
	Console console;
	std::unique_ptr<RenderTargetManager> rtMan;
	std::unique_ptr<ImageWriter> imgWriter;
	std::unique_ptr<Scaler> scaler;

public:
	static RSManager& get();
	static RSManagerDX9& getDX9();
	static RSManagerDX11& getDX11();
	static RenderTargetManager& getRTMan();
	static void setLatest(RSManager *man);
	static bool currentlyDownsampling();

	RSManager() {}

	virtual ~RSManager() {}

	unsigned getRenderWidth() { return renderWidth; }
	unsigned getRenderHeight() { return renderHeight; }
	virtual Scaler* getScaler() { return &*scaler; }

	void enableTakeScreenshot(ScreenshotType type);
	ScreenshotType getTakeScreenshot() { return takeScreenshot; }
	bool takingScreenshot() { return takeScreenshot != SCREENSHOT_NONE; }
	void tookScreenshot() { takeScreenshot = SCREENSHOT_NONE; }

	void dumpFrame() { dumpingFrame = true; }

	void togglePerfInfo() { frameTimeText->show = !frameTimeText->show; }

	bool downsamplingEnabled() { return downsampling; }

	virtual void showStatus() {}

	virtual void toggleHUD() {}
	virtual void toggleAA() {}
	virtual void toggleAO() {}
	virtual void toggleDOF() {}
	virtual void toggleBloom() {}
	virtual void togglePost() {}

	virtual void dumpSSAO() {}
	virtual void dumpBloom() {}

	virtual void reloadShaders() {}
};
