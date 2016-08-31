#pragma once

#include "WorkerThread.h"

#include <functional>
#include <future>

class WorkerThreadFuctionPointers : public WorkerThread
{
public:
	std::function<void(void)> WorkFunction;
	std::function<void(void)> CallbackFunction;
	std::promise<bool> Promise;

	WorkerThreadFuctionPointers();
	virtual ~WorkerThreadFuctionPointers();

	virtual void DoWork() override;
	virtual void DoneCallback() override;
};

