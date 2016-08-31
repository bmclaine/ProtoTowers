//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#pragma once

#include "Components\IComponent.h"

class CutSceneWaypoint : public IComponent
{
public:
	CutSceneWaypoint(GameObject* const _gameObject, Transform* const _transform);
	~CutSceneWaypoint();

	void SetNextWaypoint(CutSceneWaypoint* _waypoint);
	CutSceneWaypoint* GetNextWaypoint() const;
	CutSceneWaypoint* GetTangentWaypoint() const;

	float GetWaypointTime() const;

private:
	CutSceneWaypoint* m_pNextWaypoint;
	CutSceneWaypoint* m_pTangetWaypoint;

	float m_fTime;

protected:
	IComponent& operator=(IComponent&) override;
	IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	CutSceneWaypoint& operator=(const CutSceneWaypoint& _assign);

	void Init() override;
	void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	void OnDestroy() override;

};