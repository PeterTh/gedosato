#pragma once

#include <boost/noncopyable.hpp>

#include "utils/win_utils.h"
#include "console.h"
#include "detouring.h"

class TimeManager : boost::noncopyable {
	static TimeManager instance;

	bool frozen = false;
	LARGE_INTEGER perfCounterAtFreeze = {0, 0};
	LARGE_INTEGER startPerfCounter;
	DWORD startTimeGetTime = TruetimeGetTime();

public:
	TimeManager();

	static TimeManager& get() {
		return instance;
	}

	void toggleFreeze();

	BOOL redirectQueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount);

	DWORD redirectTimeGetTime();
};
