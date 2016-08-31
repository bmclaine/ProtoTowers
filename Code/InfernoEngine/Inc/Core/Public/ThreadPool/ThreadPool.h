///////////////////////////////////////////////////////////////////////////////////////////////////
// Author: Doug Berg
//
// Description: A thread manager class. It's handed WorkerThreads and processess each one.
//              The class is thread safe.
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <vector>
#include <atomic>
#include <list>

// Forward Declaration
class PathfindingThread;



class ThreadPool
{
private:
	unsigned int                      m_uiNumThreads;  // -> The number of threads in the class

	std::mutex                        m_queueMutex;	   // -> The mutex to protect variables
	std::mutex                        m_completedMutex;// -> The mutex to protect the m_completed queue

	std::condition_variable           m_queueCondition;// -> Makes threads wait until there is work to be had
	std::atomic_bool                  m_run;           // -> Determines if the thread pool is still running or not

	std::vector< std::thread* >       m_threads;       // -> The vector of system threads
	
	std::queue < PathfindingThread* > m_workers;       // -> The queue of workers
	std::vector< PathfindingThread* > m_testObjects;   // -> Used for safe deletion of WorkerThreads
	std::queue < PathfindingThread* > m_completed;     // -> A queue of completed workers


	void Initialize( void );                           // -> Initializes class
	void Execute   ( void );                           // -> Continually finds work to do
	void GetWork   ( PathfindingThread** _worker );    // -> Sends a worker to get work from the list
	void ClearWork ( void );                           // -> Clears the list of workers

public:
	// Default constructor
	ThreadPool ( void );
	~ThreadPool( void );

	// IN: _worker - WorkerPointer to be added to the list of work
	//
	// OUT: VOID
	//
	// Description: The parameter is added to the list of WorkerThreads
	//              Thread safe
	void PostWork ( PathfindingThread*  _worker );

	// IN: VOID
	//
	// OUT: WorkerThread* - The first WorkerThread in the queue of completed Workers
	//
	// Description: Once a thread has been completed, it's added to a queue.
	//              This function returns the first item in that queue.
	PathfindingThread* GetFirstCompleted( void );

	// IN: VOID
	//
	// OUT: BOOL - If an agent has completed
	//
	// Description: Returns true if any agent has completed it's path calculation(if the 'completed' queue has something in it)
	bool HasAgentCompleted( void );
	
	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: This function is called when the ThreadPool is no longer needed.
	//              Cleans up all memory to do with the class.
	//              Thread safe
	void Shutdown ( void );
};

