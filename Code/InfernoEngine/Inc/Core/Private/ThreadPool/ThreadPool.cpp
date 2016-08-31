///////////////////////////////////////////////////////////////////////////////////////////////////
// Author: Doug Berg
//
// Description: A thread manager class. It's handed WorkerThreads and processess each one.
//              The class is thread safe.
///////////////////////////////////////////////////////////////////////////////////////////////////


// STL Includes
using namespace std;

// Engine Includes
#include "ThreadPool\ThreadPool.h"
#include "ThreadPool\PathfindingThread.h"
#include "AssertHelper.h"

ThreadPool::ThreadPool()
{
	Initialize();
}

ThreadPool::~ThreadPool()
{
	Shutdown();
}

// IN: VOID
//
// OUT: VOID
//
// Description: Initializes the class to defaults
//              Adds the systems max amounts of threads to a vector
void ThreadPool::Initialize()
{
	PRINT("Initializing Thread pool...");

	m_uiNumThreads = thread::hardware_concurrency();

	m_run.store(true);

	for (unsigned int i = 0; i < m_uiNumThreads; ++i)
		m_threads.push_back(new thread(&ThreadPool::Execute, this));

	PRINT("Success!\n");
}

// IN: _worker - WorkerPointer to be added to the list of work
//
// OUT: VOID
//
// Description: The parameter is added to the list of WorkerThreads
//              Thread safe
void ThreadPool::PostWork(PathfindingThread* _worker)
{
	m_queueMutex.lock();

	m_workers.push(_worker);
	m_testObjects.push_back(_worker);
	m_queueCondition.notify_all();
	
	m_queueMutex.unlock();
}

// IN: _worker - The worker that is attempting to get work from the list of workers
//
// OUT: VOID
//
// Description: The parameter tries to find work from the list of workers.
//              If there is work, the front of the worker list is removed and assigned to the parameter.
//              It there is no work, the parameter is assigned to nullptr.
//              Thread safe
void ThreadPool::GetWork(PathfindingThread** _worker)
{
	unique_lock<mutex> lock(m_queueMutex);
	m_queueCondition.wait(lock, [&](){ return !m_run || m_workers.empty() == false; });

	if (m_workers.empty() == false)
	{
		*_worker = m_workers.front();
		m_workers.pop();
	}
	else
	{
		*_worker = nullptr;
	}
	lock.unlock();
}

// IN: VOID
//
// OUT: WorkerThread* - The first WorkerThread in the queue of completed Workers
//
// Description: Once a thread has been completed, it's added to a queue.
//              This function returns the first item in that queue.
PathfindingThread* ThreadPool::GetFirstCompleted()
{
	PathfindingThread* temp = m_completed.front();
	m_completed.pop();
	for (unsigned int i = 0; i < m_testObjects.size(); ++i)
		if (m_testObjects[i] == temp)
			m_testObjects.erase(m_testObjects.begin() + i);
	return temp;
}

// IN: VOID
//
// OUT: BOOL - If an agent has completed
//
// Description: Returns true if any agent has completed it's path calculation(if the 'completed' queue has something in it)
bool ThreadPool::HasAgentCompleted()
{
	return m_completed.size() > 0 ? true : false;
}

// IN: VOID
//
// OUT: VOID
//
// Description: Clears the queue of workers.
//              Thread safe
void ThreadPool::ClearWork()
{
	while (m_workers.empty() == false)
	{
		m_queueMutex.lock();

		m_workers.pop();
		m_queueCondition.notify_all();
		
		m_queueMutex.unlock();
	}
}

// IN: VOID
//
// OUT: VOID
//
// Description: This function is called when the ThreadPool is no longer needed.
//              Cleans up all memory to do with the class.
//              Thread safe
void ThreadPool::Shutdown()
{
	PRINT("Shutting down Thread pool...");

	ClearWork();

	m_queueMutex.lock();
	m_run.store(false);
	m_queueCondition.notify_all();
	m_queueMutex.unlock();

	for (unsigned int i = 0; i < m_threads.size(); ++i)
	{
		m_threads[i]->join();
		delete m_threads[i];
	}
	m_threads.clear();


	for (auto &iter : m_testObjects)
		delete iter;
	m_testObjects.clear();

	PRINT("Success!\n");
}

// IN: VOID
//
// OUT: VOID
//
// Description: This is the functions that does the lifting.
//              It sends a worker to find work. If it finds work, it does it.
void ThreadPool::Execute()
{
	while (m_run == true)
	{
		PathfindingThread* temp = nullptr;

		GetWork(&temp);

		if (temp != nullptr && m_run)
		{
			temp->FindPath();

			// Gotta keep it thread safe
			m_completedMutex.lock();
			m_completed.push(temp);
			m_completedMutex.unlock();
		}
	}
}

