#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <vector>
#include <atomic>

class WorkerThread;

class WorkerThreadPool
{
private:
	unsigned int m_uiNumThreads;
	std::mutex m_mQueueMutex;
	std::condition_variable m_cvWorkCondition;
	std::queue<WorkerThread*> m_qWorkers;
	std::vector<std::thread*> m_vThreads;
	std::atomic_bool m_bRun;

	void Execute();

public:
	WorkerThreadPool();
	~WorkerThreadPool();

	void PostWork(WorkerThread* work);
	void GetWork(WorkerThread** work);
	void ClearWork();
	void Shutdown();
};