
#include "utils/thread_utils.h"

#include "settings.h"

///////////////////////////// WorkerThread

WorkerThread::WorkerThread() : active(true), started(false) {
	thread = new std::thread(std::bind(&WorkerThread::WorkFunc, this));
	doneMutex.lock();
	while(!started) {};
}

WorkerThread::~WorkerThread() {
	mutex.lock();
	active = false;
	signal.notify_one();
	mutex.unlock();
	thread->join();
	delete thread;
}

void WorkerThread::Process(const std::function<void()>& work) {
	mutex.lock();
	work_ = work;
	signal.notify_one();
	mutex.unlock();
}

void WorkerThread::WaitForCompletion() {
	std::unique_lock<std::recursive_mutex> lock(doneMutex);
	done.wait(lock);
}

void WorkerThread::WorkFunc() {
	mutex.lock();
	std::unique_lock<std::recursive_mutex> lock(mutex);
	started = true;
	while(active) {
		signal.wait(lock);
		if(active) {
			work_();
			doneMutex.lock();
			done.notify_one();
			doneMutex.unlock();
		}
	}
}

LoopWorkerThread::LoopWorkerThread() : WorkerThread(true) {
	thread = new std::thread(std::bind(&LoopWorkerThread::WorkFunc, this));
	doneMutex.lock();
	while(!started) {};
}

void LoopWorkerThread::Process(const std::function<void(int, int)> &work, int start, int end) {
	mutex.lock();
	work_ = work;
	start_ = start;
	end_ = end;
	signal.notify_one();
	mutex.unlock();
}

void LoopWorkerThread::WorkFunc() {
	mutex.lock();
	started = true;
	while(active) {
		signal.wait(mutex);
		if(active) {
			work_(start_, end_);
			doneMutex.lock();
			done.notify_one();
			doneMutex.unlock();
		}
	}
}

///////////////////////////// ThreadPool

ThreadPool::ThreadPool(int numThreads) : workersStarted(false) {
	if(numThreads <= 0) {
		numThreads_ = 1;
		SDLOG(-1, "ThreadPool: Bad number of threads %i\n", numThreads);
	}
	else if(numThreads > 8) {
		SDLOG(-1, "ThreadPool: Capping number of threads to 8 (was %i)\n", numThreads);
		numThreads_ = 8;
	}
	else {
		numThreads_ = numThreads;
	}
}

void ThreadPool::StartWorkers() {
	if(!workersStarted) {
		for(int i = 0; i < numThreads_; ++i) {
			workers.push_back(std::make_shared<LoopWorkerThread>());
		}
		workersStarted = true;
	}
}

void ThreadPool::ParallelLoop(const std::function<void(int, int)> &loop, int lower, int upper) {
	// FIXME: disabled, deadlocking, look into it
	// or better: replace with standard loop parallelism

	//int range = upper - lower;
	//if(range >= numThreads_ * 2) { // don't parallelize tiny loops (this could be better, maybe add optional parameter that estimates work per iteration)
	//	std::lock_guard<std::recursive_mutex> guard(mutex);
	//	StartWorkers();

	//	// could do slightly better load balancing for the generic case, 
	//	// but doesn't matter since all our loops are power of 2
	//	int chunk = range / numThreads_;
	//	int s = lower;
	//	for(int i = 0; i < numThreads_ - 1; ++i) {
	//		workers[i]->Process(loop, s, s + chunk);
	//		s += chunk;
	//	}
	//	// This is the final chunk.
	//	loop(s, upper);
	//	for(int i = 0; i < numThreads_ - 1; ++i) {
	//		workers[i]->WaitForCompletion();
	//	}
	//}
	//else {
		loop(lower, upper);
	//}
}

///////////////////////////// GlobalThreadPool

std::shared_ptr<ThreadPool> GlobalThreadPool::pool;
bool  GlobalThreadPool::initialized = false;

void GlobalThreadPool::Loop(const std::function<void(int, int)>& loop, int lower, int upper) {
	Inititialize();
	pool->ParallelLoop(loop, lower, upper);
}

void GlobalThreadPool::Inititialize() {
	if(!initialized) {
		pool = std::make_shared<ThreadPool>(std::thread::hardware_concurrency());
		initialized = true;
	}
}
