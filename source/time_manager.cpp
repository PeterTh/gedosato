#include "time_manager.h"

TimeManager TimeManager::instance;

TimeManager::TimeManager() {
	TrueQueryPerformanceCounter(&startPerfCounter);
}

void TimeManager::toggleFreeze() {
	if(frozen) {
		frozen = false;
		Console::get().add("TimeManager: time unfrozen");
	}
	else {
		TrueQueryPerformanceCounter(&perfCounterAtFreeze);
		frozen = true;
		Console::get().add("ZA WARUDO");
	}
}

BOOL TimeManager::redirectQueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount) {
	if(frozen) {
		*lpPerformanceCount = perfCounterAtFreeze;
	}
	else if(Settings::get().getTimePerfCounterFactor() != 1.0f) {
		TrueQueryPerformanceCounter(lpPerformanceCount);
		LARGE_INTEGER dt;
		dt.QuadPart = lpPerformanceCount->QuadPart - startPerfCounter.QuadPart;
		dt.QuadPart = static_cast<LONGLONG>(Settings::get().getTimePerfCounterFactor() * dt.QuadPart);
		lpPerformanceCount->QuadPart = startPerfCounter.QuadPart + dt.QuadPart;
	}
	else {
		TrueQueryPerformanceCounter(lpPerformanceCount);
	}
	return TRUE;
}

DWORD TimeManager::redirectTimeGetTime() {
	if(Settings::get().getTimeGetTimeFactor() != 1.0f) {
		auto dt = TruetimeGetTime() - startTimeGetTime;
		dt = static_cast<DWORD>(Settings::get().getTimeGetTimeFactor() * dt);
		return startTimeGetTime + dt;
	}
	return TruetimeGetTime();
}
