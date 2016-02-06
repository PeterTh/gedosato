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
#include "perf_monitor.h"

class RSManagerDX9;
class RSManagerDX11;
class Scaler;

class RSManager {
public:
	static const unsigned SCREENSHOT_NONE = 0;
	static const unsigned SCREENSHOT_STANDARD = 1 << 1;
	static const unsigned SCREENSHOT_FULL = 1 << 2;
	static const unsigned SCREENSHOT_HUDLESS = 1 << 3;
	typedef unsigned ScreenshotType;

protected:
	static RSManager* latest;
	static bool forceDSoff;

	ScreenshotType takeScreenshot = SCREENSHOT_NONE;

	bool downsampling = false, dumpingFrame = false;
	unsigned dumpCaptureIndex = 0;

	unsigned renderWidth = 0, renderHeight = 0;
	unsigned numBackBuffers = 0;

	// Performance measurement
	double currentWaitTime = 0.0;
	Timer cpuFrameTimer, fpsFrameTimer;
	SlidingAverage cpuFrameTimes{ 120 };
	SlidingAverage fpsWaitTimes { 60 };
	std::unique_ptr<D3DPerfMonitor> perfMonitor;
	StaticTextPtr frameTimeText{ new StaticText("", 20.0f, 100.0f) };
	StaticTextPtr traceText{ new StaticText("Tracing", 300.0f, 30.0f) };
	std::unique_ptr<PerfTrace> perfTrace;

	// Utilities
	std::unique_ptr<Console> console;
	std::unique_ptr<RenderTargetManager> rtMan;
	std::unique_ptr<ImageWriter> imgWriter;
	std::unique_ptr<Scaler> scaler;

public:
	static bool exists() { return latest != NULL; }
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
	bool takingScreenshot(ScreenshotType type) { return (takeScreenshot & type) != SCREENSHOT_NONE; }
	void tookScreenshot(ScreenshotType type) { takeScreenshot &= ~type; }

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

	virtual void startPerfTrace();
	virtual void endPerfTrace();
	virtual void togglePerfTrace();

	virtual void genericPrePresent();
	virtual void genericPostPresent();
};
