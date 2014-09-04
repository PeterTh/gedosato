#pragma once

#include <boost/noncopyable.hpp>

#include "winutil.h"
#include "console.h"
#include "detouring.h"

class TimeManager : boost::noncopyable {
	static TimeManager instance;

	bool frozen = false;
	LARGE_INTEGER perfCounterAtFreeze{ { 0 } };

public:
	static TimeManager& get() {
		return instance;
	}

	void toggleFreeze() {
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

	BOOL redirectQueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount) {
		if(frozen) {
			*lpPerformanceCount = perfCounterAtFreeze;
		}
		else {
			TrueQueryPerformanceCounter(lpPerformanceCount);
		}
		return TRUE;
	}
};
