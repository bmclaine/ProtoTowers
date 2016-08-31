#pragma once

#include "WorkerThreadFuctionPointers.h"

#include <vector>

class WorkerThreadConfigSave : public WorkerThreadFuctionPointers
{
public:
	std::vector<std::pair<std::string, std::string>> ConfigVector;

	WorkerThreadConfigSave();
	virtual ~WorkerThreadConfigSave() override;

	virtual void DoWork() override;
};

