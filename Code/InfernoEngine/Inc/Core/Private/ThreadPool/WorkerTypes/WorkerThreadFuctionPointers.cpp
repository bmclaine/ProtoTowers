#include "ThreadPool\WorkerTypes\WorkerThreadFuctionPointers.h"
#include "AssertHelper.h"

WorkerThreadFuctionPointers::WorkerThreadFuctionPointers()
{
	WorkFunction = nullptr;
	CallbackFunction = nullptr;
	DeleteOnFinish = false;
}

WorkerThreadFuctionPointers::~WorkerThreadFuctionPointers()
{
}

void WorkerThreadFuctionPointers::DoWork()
{
	if (WorkFunction)
		WorkFunction();
}
void WorkerThreadFuctionPointers::DoneCallback()
{
	if (CallbackFunction)
		CallbackFunction();

	//PRINT("Setting Promise\n");
	Promise.set_value(true);
	//PRINT("Promise Set to 'True'\n");
}