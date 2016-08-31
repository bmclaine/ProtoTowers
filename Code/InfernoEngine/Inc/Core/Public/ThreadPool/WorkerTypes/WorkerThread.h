#pragma once
class WorkerThread
{
public:
	bool DeleteOnFinish;

	virtual void DoWork() = 0;
	virtual void DoneCallback() {}
};