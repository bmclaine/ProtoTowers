///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Returns SUCCESS when the specified amount of distance has been traveled.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ConditionBehavior.h"
#include "MathLib.h"

class NavAgent;
class AIEntity;
class Transform;

class HasTavelledDistanceBehavior : public ConditionBehavior
{
private:
	AIEntity    *m_pEntity;
	NavAgent    *m_pAgent;
	Vector3      m_vPrevPosition;
	const float  m_fTargetDistance;
	float        m_fElapsedDistance;
	bool         m_bReset;

public:
	HasTavelledDistanceBehavior(AIEntity* _entity, NavAgent* _agent, float _distance, bool _resetDistOnInit = true, std::string& _name = std::string());
	~HasTavelledDistanceBehavior();

	void Initialize() override;
	void Destroy() override;
	Status Update() override;
};