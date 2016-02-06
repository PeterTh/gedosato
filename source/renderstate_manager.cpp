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
	Console::setLatest(man->console.get());
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

namespace {
	double limitFPS(const Timer& cpuFrameTimer, double waitTimeAvg, bool pre) {
		Timer waitTime;
		float fpsLimit = Settings::get().getFpsLimit();
		if(fpsLimit > 1.0f) {
			double targetTime = 1000000.0 / fpsLimit; // cpuFrameTimer reports microseconds
			if(!pre) {
				// we want to do some of our waiting predictively
				targetTime = Settings::get().getFpsPredictiveLimitRatio() * waitTimeAvg;
			}
			// perform waiting
			waitTime.start();
			if(Settings::get().getFpsLimitBusy()) {
				while(cpuFrameTimer.elapsed() < targetTime);
			}
			else {
				if(cpuFrameTimer.elapsed() < targetTime) {
					std::this_thread::sleep_for(std::chrono::microseconds(static_cast<unsigned long>(targetTime - cpuFrameTimer.elapsed())));
				}
			}
		}
		return waitTime.elapsed();
	}
	void flushGPU(LPDIRECT3DDEVICE9 dev) {
		IDirect3DQuery9* query = NULL;
		dev->CreateQuery(D3DQUERYTYPE_EVENT, &query);

		if(query != NULL) {
			query->Issue(D3DISSUE_END);
			while(S_FALSE == query->GetData(NULL, 0, D3DGETDATA_FLUSH));
		}

		query->Release();
	}
}

void RSManager::genericPrePresent() {
	// Frame time measurements
	cpuFrameTimes.add(cpuFrameTimer.elapsed() / 1000.0);
	perfMonitor->end();
	double cpuTime = cpuFrameTimes.get(), gpuTime = perfMonitor->getCurrent();
	frameTimeText->text = format("  %s\nFrame times (avg/max):\n CPU: %6.2lf / %6.2lf ms\n GPU: %6.2f / %6.2lf ms\nFPS: %4.3lf",
		getTimeString(true), cpuTime, cpuFrameTimes.maximum(), gpuTime, perfMonitor->getMax(), 1000.0 / max(cpuTime, gpuTime));
	if(perfTrace) perfTrace->addFrame(cpuTime, gpuTime);

	// FPS limiting
	currentWaitTime += limitFPS(fpsFrameTimer, 0.0, true);
	fpsWaitTimes.add(currentWaitTime);
}

void RSManager::genericPostPresent() {
	currentWaitTime = 0.0;
	fpsFrameTimer.start();

	if(Settings::get().getFlushGPUEveryFrame()) flushGPU(getDX9().getD3Ddev());
	// FPS limiting
	if(Settings::get().getFpsPredictiveLimitRatio() > 0.0f) {
		//traceText->text = "";
		//traceText->show = true;
		//traceText->text += format("start wait : %f\n", fpsFrameTimer.elapsed());
		limitFPS(fpsFrameTimer, fpsWaitTimes.get(), false);
		currentWaitTime += fpsFrameTimer.elapsed();
		//traceText->text += format("Pre waiting: %f\n", currentWaitTime);
		//traceText->text += format("Current AVG: %f\n", fpsWaitTimes.get());
	}

	cpuFrameTimer.start();
	perfMonitor->start();
}
