///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: This is a child class of WorkerThread. It will be used by the AIModule class
//              to task a thread to handle an agents pathfinding.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// STL Includes
using namespace std;


// Engine Includes
#include "GlobalIncludes.h"
#include "ThreadPool\PathfindingThread.h"
#include "AI\AIModule.h"
#include "AI\NavNode.h"
#include "AI\Triangle.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "Physics\Collision.h"
#include "GameObject.h"
#include "Physics\CapsuleCollider.h"
#include "Components\Transform.h"

#include "Components\NavAgent.h"
#include "GameObject.h"

using namespace InfernoEngine_Physics;

// Defaults
PathfindingThread::PathfindingThread()
	: m_pRequest(nullptr)
	, m_currentNode(nullptr)
	, m_neighborNode(nullptr)
{ }

PathfindingThread::~PathfindingThread()
{ }

// IN: VOID
//
// OUT: VOID
//
// Description: THIS IS THE FUNCTION THAT WILL BE PUT ON ITS OWN THREAD.
//              This is the beef of the class, where all the heavy lifting will be done.
void PathfindingThread::FindPath()
{
	bool result = false;
	
	while (result == false)
		result = Loop();

	CalculateAgentPath();
}

// IN: VOID
//
// OUT: NavAgent* - The NavAgent in the Request
//
// Description: Returns the NavAgent member of the Request class member this is keeping track of
Request* PathfindingThread::GetRequest() const
{
	return m_pRequest;
}
Request* PathfindingThread::GetRequest()
{
	return m_pRequest;
}

// IN: Request* - The Request to keep track of
//
// OUT: VOID
//
// Description: Sets the class member to this Request
void PathfindingThread::Initialize(Request* _request)
{
	m_pRequest = _request;
	m_pRequest->navMesh->InitializeTileCosts(_request->agent);
	m_pRequest->navMesh->GetStartNode()->SetVisited(true);
	m_openList.push_back(m_pRequest->navMesh->GetStartNode());
}

bool PathfindingThread::Loop()
{
	if (m_openList.size() == 0)
		return true;

	m_currentNode = m_openList.back();
	m_openList.pop_back();

	// We've found the end
	if (m_currentNode->GetCentroid() == m_pRequest->navMesh->GetGoalNode()->GetCentroid())
		return true;

	unsigned int numNeighbors = (unsigned int)m_currentNode->GetNeighbors().size();
	float tempG = 0;
	for (unsigned int i = 0; i < numNeighbors; ++i)
	{
		m_neighborNode = m_currentNode->GetNeighbors()[i];

		if (m_neighborNode->BeenVisited() == false)
		{
			m_neighborNode->SetGivenCost(FLT_MAX);
			m_neighborNode->SetParent(nullptr);
			UpdateVertex(m_currentNode, m_neighborNode);
		}
	}
	sort(m_openList.begin(), m_openList.end(), SortByFinalCost());
	return false;
}

void PathfindingThread::CalculateAgentPath()
{
	NavNode* temp = m_currentNode;
	m_pRequest->navMesh->GetStartNode()->SetParent(nullptr);
	while (temp != nullptr)
	{
		m_pRequest->path->push_back(temp->GetCentroid());
		temp = temp->GetParent();
	}
	m_openList.clear();
	m_pRequest->navMesh->Reset();
}

void PathfindingThread::ComputeCost(NavNode* _current, NavNode* _neighbor)
{
	float dist = (_current->GetCentroid() - _neighbor->GetCentroid()).LengthSq();

	// if lineofsight
	if (_current->GetParent() != nullptr)// CanSeeParent(_current->GetParent(), _neighbor))
	{
		if (_current->GetParent()->GetGivenCost() + dist < _neighbor->GetGivenCost())
		{
			_neighbor->SetParent(_current->GetParent());
			_neighbor->SetGivenCost(_current->GetParent()->GetGivenCost() + dist);
		}
	}
	else
	{
		if (_current->GetGivenCost() + dist < _neighbor->GetGivenCost())
		{
			_neighbor->SetParent(_current);
			_neighbor->SetGivenCost(_current->GetGivenCost() + dist);
		}
	}
}

void PathfindingThread::UpdateVertex(NavNode* _current, NavNode* _neighbor)
{
	float given = _neighbor->GetGivenCost();

	ComputeCost(_current, _neighbor);
	if (_neighbor->GetGivenCost() < given)
	{
		if (_neighbor->BeenVisited())
		{
			_neighbor->SetVisited(false);
			for (unsigned int i = 0; i < m_openList.size(); ++i)
				if (m_openList[i] == _neighbor)
					m_openList.erase(m_openList.begin() + i);
		}
		else
		{
			_neighbor->SetVisited(true);
		}
		m_openList.push_back(_neighbor);
	}
}

bool PathfindingThread::CanSeeParent(NavNode* _current, NavNode* _neighbor)
{
	// Parent == nullptr means that it's the starting node. It can easily see itself.
	if (_current == nullptr)
		return false;

	CapsuleCollider* collider = m_pRequest->agent->gameObject->GetComponent<CapsuleCollider>();
	float colliderRadius = collider->GetRadius();

	Vector3 origin    = _neighbor->GetCentroid() + Vector3(0.0f, 1.0f, 0.0f);// -> Raise the point off the ground to make more realistic RayCast
	Vector3 direction = _current->GetCentroid() - _neighbor->GetCentroid();
	float distanceToParent = direction.LengthSq();
	direction.Normalize();

	// Move the origin in said direction for the length of the collidersRadius
	origin += (direction * colliderRadius);

	Ray ray(origin, direction);
	RayCastHit hit;

	if (RayCast(ray, hit))
	{
		float distanceToObject = (hit.collider->transform->GetPosition() - _neighbor->GetCentroid()).LengthSq();
			if (distanceToObject > distanceToParent)
				return true;
	}
	return false;
}

