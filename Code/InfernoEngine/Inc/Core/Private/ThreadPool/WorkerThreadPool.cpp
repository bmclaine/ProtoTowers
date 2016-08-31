#include "ThreadPool\WorkerThreadPool.h"
#include "ThreadPool\WorkerTypes\WorkerThread.h"
#include "AssertHelper.h"

WorkerThreadPool::WorkerThreadPool()
{
	PRINT("Starting Worker Thread Pool...");
	m_vThreads.resize(std::thread::hardware_concurrency());

	m_bRun.store(true);

	for (size_t i = 0; i < m_vThreads.size(); i++)
		m_vThreads[i] = new std::thread(&WorkerThreadPool::Execute, this);

	PRINT("Success!\n");
}

WorkerThreadPool::~WorkerThreadPool()
{
	Shutdown();
}

void WorkerThreadPool::Execute()
{
	while (m_bRun.load())
	{
		WorkerThread* work = nullptr;
		GetWork(&work);

		if (work)
		{
			work->DoWork();
			work->DoneCallback();

			if (work->DeleteOnFinish)
				delete work;
		}
	}
}

void WorkerThreadPool::PostWork(WorkerThread* work)
{
	if (work)
	{
		m_mQueueMutex.lock();
		m_qWorkers.push(work);
		m_mQueueMutex.unlock();
		m_cvWorkCondition.notify_all();
	}
}
void WorkerThreadPool::GetWork(WorkerThread** work)
{
	std::unique_lock<std::mutex> uLock(m_mQueueMutex);
	m_cvWorkCondition.wait(uLock, [&](){ return (m_qWorkers.size() > 0 || !m_bRun.load()); });

	if (m_bRun.load())
	{
		(*work) = m_qWorkers.front();
		m_qWorkers.pop();
	}
	else
		work = nullptr;

	uLock.unlock();
}
void WorkerThreadPool::ClearWork()
{
	m_mQueueMutex.lock();

	while (m_qWorkers.size() > 0)
		m_qWorkers.pop();

	m_mQueueMutex.unlock();
}
void WorkerThreadPool::Shutdown()
{
	ClearWork();

	m_bRun.store(false);
	m_cvWorkCondition.notify_all();

	for (auto& i : m_vThreads)
	{
		i->join();
		delete i;
	}
	m_vThreads.clear();
}