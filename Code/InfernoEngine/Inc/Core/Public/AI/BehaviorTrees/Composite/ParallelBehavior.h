///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: ParallelBehavior is a child of CompositeBehavior.
//              It runs all children simultaneously.
//              Either returns SUCCESS when ALL children or ONE child returns SUCCESS.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Public/AI/BehaviorTrees/Composite/CompositeBehavior.h"

enum Policy
{
	REQUIRE_ONE,
	REQUIRE_ALL
};

class ParallelBehavior : public virtual CompositeBehavior
{
protected:
	Policy m_eSuccessPolicy;
	Policy m_eFailurePolicy;

public:
	ParallelBehavior( Policy _success, Policy _failure, std::string& _name = std::string());
	virtual ~ParallelBehavior( void );

	Status Update( void ) override;
	void Destroy(void);
};
