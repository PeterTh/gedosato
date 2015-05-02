#include "renderstate_manager_dx9.h"
#include "renderstate_manager_dx11.h"

////////// Static

RSManager* RSManager::latest = NULL;
bool RSManager::forceDSoff = false;

RSManager& RSManager::get() {
	if(latest == NULL) SDLOG(-1, "ERROR: Getting NULL RSManager!!\n");
	return *latest;
}

RSManagerDX9& RSManager::getDX9() {
	auto& ret = get();
	if(typeid(ret) != typeid(RSManagerDX9)) SDLOG(-1, "ERROR: Getting DX9 RSManager from non-DX9 source!!\n");
	return *dynamic_cast<RSManagerDX9*>(&ret);
}
RSManagerDX11& RSManager::getDX11() {
	auto& ret = get();
	if(typeid(ret) != typeid(RSManagerDX11)) SDLOG(-1, "ERROR: Getting DX11 RSManager from non-DX11 source!!\n");
	return *dynamic_cast<RSManagerDX11*>(&ret);
}

void RSManager::setLatest(RSManager *man) {
	latest = man;
	Console::setLatest(&man->console);
}

RenderTargetManager& RSManager::getRTMan() {
	return *latest->rtMan;
}

bool RSManager::currentlyDownsampling() {
	if(forceDSoff) return false;
	return Settings::get().getForceAlwaysDownsamplingRes() || (latest && latest->downsampling);
}

////////// Instance

void RSManager::enableTakeScreenshot(ScreenshotType type) {
	takeScreenshot |= type;
	SDLOG(0, "takeScreenshot: %d\n", type);
}

//// Performance tracing

void RSManager::startPerfTrace() {
	traceText->show = true;
	perfTrace = std::make_unique<PerfTrace>();
}
void RSManager::endPerfTrace() {
	traceText->show = false;
	perfTrace->storeResult();
	perfTrace.reset(nullptr);
}
void RSManager::togglePerfTrace() {
	if(perfTrace) {
		endPerfTrace();
	}
	else {
		startPerfTrace();
	}
}

//// Generic pre/post present actions

void RSManager::genericPrePresent() {
	// FPS limiting
	float fpsLimit = Settings::get().getFpsLimit();
	if(fpsLimit > 1.0f) {
		float desiredMinFrametime = 1000000.0f / fpsLimit; // cpuFrameTimer reports microseconds
		if(Settings::get().getFpsLimitBusy()) {
			while(cpuFrameTimer.elapsed() < desiredMinFrametime);
		}
		else {
			if(cpuFrameTimer.elapsed() < desiredMinFrametime) {
				std::this_thread::sleep_for(std::chrono::microseconds(static_cast<unsigned long>(desiredMinFrametime - cpuFrameTimer.elapsed())));
			}
		}
	}

	// Frame time measurements
	cpuFrameTimes.add(cpuFrameTimer.elapsed() / 1000.0);
	perfMonitor->end();
	double cpuTime = cpuFrameTimes.get(), gpuTime = perfMonitor->getCurrent();
	frameTimeText->text = format("  %s\nFrame times (avg/max):\n CPU: %6.2lf / %6.2lf ms\n GPU: %6.2f / %6.2lf ms\nFPS: %4.3lf",
		getTimeString(true), cpuTime, cpuFrameTimes.maximum(), gpuTime, perfMonitor->getMax(), 1000.0 / max(cpuTime, gpuTime));
	if(perfTrace) perfTrace->addFrame(cpuTime, gpuTime);
}
void RSManager::genericPostPresent() {
	cpuFrameTimer.start();
	perfMonitor->start();
}
