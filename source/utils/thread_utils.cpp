
#include "utils/thread_utils.h"

#include "settings.h"

///////////////////////////// GlobalThreadPool

concurrency::task_group GlobalThreadPool::taskGroup;

void GlobalThreadPool::Loop(const std::function<void(int, int)>& loop, int lower, int upper) {

	static int hwConc = std::thread::hardware_concurrency();
	int chunks = hwConc ? hwConc : 1;

	int range = upper - lower;
	if(range >= chunks * 2) { // don't parallelize tiny loops (this could be better)
		// could do slightly better load balancing for the generic case,
		// but doesn't matter since our loops are usually power of 2
		int chunk = range / chunks;
		int s = lower;
		for(int i = 0; i < chunks - 1; ++i) {
			taskGroup.run([loop,s,chunk] { loop(s, s + chunk); });
			s += chunk;
		}
		// This is the final chunk.
		taskGroup.run_and_wait([loop,s,upper] { loop(s, upper); });
	}
	else {
		loop(lower, upper);
	}
}