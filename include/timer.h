#pragma once

#include "main.h"

#include "Settings.h"

class Timer {
	LARGE_INTEGER startTime;
	static double multiplicator;
	bool running = false;

public:
	Timer() {
		if(multiplicator<0) {
			LARGE_INTEGER frequency;
			if(QueryPerformanceFrequency(&frequency) == 0) SDLOG(-1, "ERROR: Could not get HPC frequency (not supported?)\n");
			multiplicator = 1000000.0/(double)frequency.QuadPart; // microseconds
		}
		start();
	}

	void start() {
		running = true;
		QueryPerformanceCounter(&startTime);
	}

	void stop() {
		running = false;
	}

	bool isRunning() const { return running; }

	double elapsed() const {
		LARGE_INTEGER time, diff;
		QueryPerformanceCounter(&time);
		diff.QuadPart = time.QuadPart - startTime.QuadPart;
		return (double)(diff.QuadPart)*multiplicator;
	}

	void moveStart(LONGLONG amount) {
		startTime.QuadPart += (LONGLONG)(amount/multiplicator);
	}
};

class SlidingAverage {
	double *vals;
	unsigned interval, current;
	bool filled;
public:
	SlidingAverage(unsigned interval) : interval(interval), current(0), filled(false) {
		vals = new double[interval]();
	}
	~SlidingAverage() {
		delete [] vals;
	}

	void add(double value) {
		if(++current == interval) {
			current = 0;
			filled = true;
		} else {
			filled = false;
		}
		vals[current] = value;
	}

	double get() const {
		double sum = 0;
		for(unsigned i=0; i<interval; ++i) { sum += vals[i]; }
		sum /= interval;
		return sum;
	}

	double maximum() const {
		double maxval = 0;
		for(unsigned i = 0; i < interval; ++i) { maxval = max(maxval, vals[i]); }
		return maxval;
	}

	bool justFilled() const {
		return filled;
	}
};
