#include "ThreadPool\WorkerTypes\WorkerThreadConfigSave.h"
#include "../../Interface/Public/Engine.h"

WorkerThreadConfigSave::WorkerThreadConfigSave() : WorkerThreadFuctionPointers()
{
	DeleteOnFinish = true;
}

WorkerThreadConfigSave::~WorkerThreadConfigSave()
{
}

void WorkerThreadConfigSave::DoWork()
{
	Inferno::UpdateConfigFile(ConfigVector);
}