#pragma once

#include "MathLib.h"
#include "Rigidbody.h"
#include "QuadTree.h"
#include <vector>

class Physics{

private:
	static Vector3 m_f3Gravity;

	QuadTree* root;
	std::vector<Rigidbody*> m_bodies;
	std::vector<Collider*> m_dynColliders;
	std::vector<Collider*> m_staticColliders;
	std::vector<CollisionData> m_CollisionData;
	static Flags<char> s_LayerMatrix;
	std::vector<Collider*> m_removeList;
	std::vector<Collider*> m_addList;

	Color color[MAX_LEVEL];

public:
	Physics();
	~Physics();

	static Vector3 Gravity();

	// IN: void
	// OUT: void
	//
	// Update only called every 1/60th of second
	void FixedUpdate();

	// IN: Rigidbody - pointer to rigidbody to add
	// OUT: void
	//
	// Adds a rigidbody to the registry
	void AddRigidbody(Rigidbody* body);

	// IN: Rigidbody - pointer to rigidbody to remove
	// OUT: void
	//
	// Removes a rigidbody from the registry
	void RemoveRigidbody(Rigidbody* body);

	// IN: Collider - pointer to collider to add
	// OUT: void
	//
	// Adds a Collider to the registry
	void AddCollider(Collider* collider);

	// IN: Collider - pointer to collider to remove
	// OUT: void
	//
	// Removes a collider from the registry
	void RemoveCollider(Collider* collider);

	// IN: Ray - ray to check against colliders
	// OUT: RayCastHit - information about what the ray hit
	// 
	// Checks to see if a ray collides with an object
	bool Raycast(Ray& ray, RayCastHit& hitInfo);

	// IN: origin - the origin of the sphere
	//	   radius - the radius of the sphere
	// OUT: hitinfo - vector of information about what the sphere overlapped with
	// 
	// Checks to see if a ray collides with an object
	bool SphereOverlap(const Vector3& origin, const float radius, std::vector<RayCastHit>& hitInfo);

	// IN: int, int - layers to check
	// OUT: bool - returns true if two layers can collide
	//
	// Returns true if two layers can collide
	static bool CheckLayer(int layerX, int layerY);

	void OnGizmosDraw();

private:
	Physics(const Physics&) = delete;
	Physics& operator=(const Physics&) = delete;

	void UpdateRigidbodies();
	void UpdateTree();
	void UpdateCollisions();
	void UpdateAddList();
	void UpdateRemoveList();
	void CollisionResolution();
	void CollisionEvents();
	bool ContainsCollider(Collider* collider);

	void SetCollisionBetweenLayers(unsigned int _layerX, unsigned int _layerY, bool _collision);
};