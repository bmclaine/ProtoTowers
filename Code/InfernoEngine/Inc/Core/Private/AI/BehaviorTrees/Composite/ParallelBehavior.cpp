///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: ParallelBehavior is a child of CompositeBehavior.
//              It runs all children simultaneously.
//              Either returns SUCCESS when ALL children or ONE child returns SUCCESS.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Composite/ParallelBehavior.h"

ParallelBehavior::ParallelBehavior(Policy _success, Policy _failure, std::string& _name)
	: CompositeBehavior(_name)
	, m_eSuccessPolicy(_success)
	, m_eFailurePolicy(_failure)
{ }

ParallelBehavior::~ParallelBehavior()
{ }

Status ParallelBehavior::Update()
{
	unsigned int successCount = 0;
	unsigned int failureCount = 0;

	unsigned int size = (unsigned int)m_vChildren.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		Status status = m_vChildren[i]->Tick();

		if (status == Status::BT_SUCCESS)
		{
			++successCount;
			if (m_eSuccessPolicy == Policy::REQUIRE_ONE)
				return status;
		}
		else if (status == Status::BT_FAILURE)
		{
			++failureCount;
			if (m_eFailurePolicy == Policy::REQUIRE_ONE)
				return status;
		}

		if (m_eFailurePolicy == Policy::REQUIRE_ALL && failureCount == size)
			return Status::BT_FAILURE;
		else if (m_eSuccessPolicy == Policy::REQUIRE_ALL && successCount == size)
			return Status::BT_SUCCESS;
	}
	return Status::BT_RUNNING;
}

void ParallelBehavior::Destroy()
{ }

