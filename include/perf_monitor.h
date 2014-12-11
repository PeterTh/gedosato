#pragma once

#include "main.h"
#include "timer.h"

class PerfTrace {
	std::vector<double> frame_cpu_times;
	std::vector<double> frame_gpu_times;
	std::vector<double> frame_eff_times;
	Timer t, total;

public:
	PerfTrace();

	void addFrame(double cpu_t, double gpu_t);
	void storeResult();
};
