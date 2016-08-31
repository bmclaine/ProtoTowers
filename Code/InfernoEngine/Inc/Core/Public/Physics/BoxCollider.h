#pragma once

#include "Physics\Collider.h"

class BoxCollider : public Collider{

private:
	Vector3 m_f3Size;
	Vector3 m_pBoundPoints[8];

protected:
	virtual IComponent& operator=(IComponent& _rhs) override;
	BoxCollider& operator=(const BoxCollider& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	virtual ~BoxCollider() override;

public:
	BoxCollider(GameObject* const gameObject, Transform* const transform);

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init() override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	// IN: void
	// OUT: void
	//
	// Draws Gizmos for the component
	virtual void OnDrawGizmos() override;

	// IN: void
	// OUT: Vector3* - the bounding points
	//
	// Calculates and returns the bounding points
	Vector3* const GetBoundPoints();

	// IN: void
	// OUT: void
	//
	// Caluclates the bound points
	void CalculateBoundPoints();

	// IN: void
	// OUT: Vector3 - size
	//
	// returns the size of the bounding box
	Vector3 GetSize() const;

	// IN: Vector3* - the array of edges / should have three elements
	// OUT: Vector3 - the array of edges
	//
	// Caluclates the edges and stores them in the array of edges
	void GetEdges(Vector3* edges);

	// IN: Vector3* - the array of face normals / should have three elements
	// OUT: Vector3 - the array of face normals
	//
	// Caluclates the face normals and stores them in the array of face normals
	void GetFaceNormals(Vector3* faceNormals);

	// IN: BoxCollider, BoxCollider, BoxColliders to test collision for
	// IN: Vector3 - axis to project on
	// IN: Vector3 - vector from the center of first box to center of second box
	// OUT: float penetration value
	//
	// returns the overlap , positive indicates overlap, negative indicates seperation
	static float GetPenetrationOnAxis(BoxCollider* lhs, BoxCollider* rhs, const Vector3& axis, const Vector3& toCenter);

	// IN: BoxCollider - collider to get projected points from
	// IN: Vector3 - axis to project points on
	// OUt: float - half length of box along the axis
	//
	// returns half length of box along the axis
	static float TransformToAxis(BoxCollider* collider, const Vector3& axis);

	void SetSize(const Vector3& size);

	virtual const AABB GetBoundingBox() const;

	virtual const Box2D GetBox2D();

	virtual int GetColliderType();

	virtual void OnEnable() override;
};