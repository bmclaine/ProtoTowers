///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: BehaviorTree is the class that will be the base of everything.
//              It will have one BaseBehavior child that will act as the Tree's root.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Components\IComponent.h"
#include "../AI/BehaviorTrees/BaseBehavior.h"

#include <string>

class AIEntity;
class NavAgent;

class BehaviorTree : public IComponent
{
private:
	BaseBehavior *m_pChild;
	BaseBehavior* m_pCutsceneChild;
	AIEntity     *m_pEntity;
	NavAgent     *m_pAgent;
	const float   m_fRequestTime;
	std::string   m_sName;
	bool          m_bBuiltTree;
	bool          m_bPlayCutsceneTree;

	// == Private Interface
	void CreateMeleeTree   ( void );
	void CreateRangedTree  ( void );
	void CreateTankTree    ( void );
	void CreateBossTree    ( void );
	void CreateTutorialTree( void );
	void CreateCutsceneTree( void );
	// ==

public:
	BehaviorTree( GameObject* const _gameObject, Transform* const _transform );
	virtual ~BehaviorTree( void );
	BehaviorTree& operator = ( const BehaviorTree& _assign );

	virtual IComponent& operator=( IComponent& _assign );
	virtual IComponent* CreateCopy( GameObject* const _gameObject, Transform* const _transform );
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	virtual void OnEnable() override;

	// == Interface
	void Destroy( void );
	Status Tick( void );
	void Reset( void );
	void SwitchToCutsceneTree();
	void SwitchToNormalTree();
	// ==
};

