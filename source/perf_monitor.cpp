#include "perf_monitor.h"

#include <fstream>

PerfTrace::PerfTrace() {
	total.start();
	t.start();
}

void PerfTrace::addFrame(double cpu_t, double gpu_t) {
	frame_cpu_times.push_back(cpu_t);
	frame_gpu_times.push_back(gpu_t);
	t.stop();
	frame_eff_times.push_back(t.elapsed() / 1000.0);
	t.start();
}

void PerfTrace::storeResult() {
	total.stop();
	double tot_ms = total.elapsed() / 1000.0;
	string logFn = format("logs\\%s_%s_perf.txt", getExeFileName().c_str(), getTimeString().c_str());
	std::replace(logFn.begin(), logFn.end(), ':', '-');
	std::replace(logFn.begin(), logFn.end(), ' ', '_');
	logFn = getInstalledFileName(logFn);
	std::ofstream outf(logFn.c_str());
	outf << "Total frames: " << frame_cpu_times.size() << "\n";
	outf << "Total time: " << tot_ms << "\n";
	outf << "Average FPS: " << frame_cpu_times.size()*1000 / tot_ms << "\n\n";

	auto eff_copy = frame_eff_times;
	std::sort(eff_copy.begin(), eff_copy.end());
	auto l = eff_copy.size();
	outf << "99 % frame time: " << eff_copy[static_cast<int>(floor(l * 0.99))] << "\n";
	outf << "95 % frame time: " << eff_copy[static_cast<int>(floor(l * 0.95))] << "\n";
	outf << "75 % frame time: " << eff_copy[static_cast<int>(floor(l * 0.75))] << "\n\n";

	outf << "Individual frames (cpu;gpu;effective):\n";
	for(size_t i = 0; i < l; ++i) {
		outf << format("%8.4f,%8.4f,%8.4f\n", frame_cpu_times[i], frame_gpu_times[i], frame_eff_times[i]);
	}
	outf << "\n";
}
