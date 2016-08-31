#pragma once

#include "Collision.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"

class CollisionLibrary {

private:
	CollisionLibrary() = delete;
	~CollisionLibrary() = delete;
	CollisionLibrary(const CollisionLibrary&) = delete;
	CollisionLibrary& operator=(const CollisionLibrary&) = delete;

public:

	static bool CheckCollision(Collider* lCollider, Collider* rCollider, CollisionData& collisionData);

	static bool CheckBoxToBoxCollision(BoxCollider* lCollider, BoxCollider* rCollider, CollisionData& collisionData, float direction = 1);

	static bool CheckBoxToSphereCollision(BoxCollider* boxCollider, SphereCollider* sphereCollider, CollisionData& collisionData, float direction = 1);

	static bool CheckBoxToCapsuleCollision(BoxCollider* boxCollider, CapsuleCollider* capsuleCollider, CollisionData& collisionData, float direction = 1);

	static bool CheckSphereToSphereCollision(SphereCollider* lCollider, SphereCollider* rCollider, CollisionData& collisionData, float direction = 1);

	static bool CheckSphereToCapsuleCollision(SphereCollider* sphereCollider, CapsuleCollider* capsuleCollider, CollisionData& collisionData, float direction = 1);

	static bool CheckCapsuleToCapsuleCollision(CapsuleCollider* lCollider, CapsuleCollider* rCollider, CollisionData& collisionData, float direction = 1);

	static void ClosestPtSegmentSegment(Vector3 lStartPoint, Vector3 rStartPoint, Vector3 lEndPoint, Vector3 rEndPoint, Vector3& lClosestPoint, Vector3& rClosestPoint);

	static bool CheckRayToCollider(Ray& ray, Collider* collider, RayCastHit& hitInfo);

	static bool CheckRayToSphere(Ray& ray, SphereCollider* sphereCollider, RayCastHit& hitInfo);

	static bool CheckRayToBox(Ray& ray, BoxCollider* boxCollider, RayCastHit& hitInfo);

	static bool CheckRayToCapsule(Ray& ray, CapsuleCollider* capsuleCollider, RayCastHit& hitInfo);

	static bool CheckOverlapSphereToCollider(const Vector3& origin, const float radius, Collider* collider, RayCastHit& hitInfo);

	static bool CheckOverlapSphereToBox(const Vector3& origin, const float radius, BoxCollider* collider, RayCastHit& hitInfo);

	static bool CheckOverlapSphereToSphere(const Vector3& origin, const float radius, SphereCollider* collider, RayCastHit& hitInfo);

	static bool CheckOverlapSphereToCapsule(const Vector3& origin, const float radius, CapsuleCollider* collider, RayCastHit& hitInfo);

	// IN: unsigned int, unsigned int - layers to check
	// OUT: void
	//
	// Checks if the two layers can collide
	static bool CheckLayer(unsigned int layerX, unsigned int layerY);


	// Perform a sphere-to-plane test. 
	// Returns 1 if the sphere is in front of the plane.
	// Returns 2 if the sphere is behind the plane.
	// Returns 3 if the sphere straddles the plane.
	static int CheckSphereToPlane(const Plane& plane, const Sphere& sphere);

	// Perform a sphere-to-plane test. 
	// Returns 1 if the box is in front of the plane.
	// Returns 2 if the box is behind the plane.
	// Returns 3 if the box straddles the plane.
	static int CheckAABBToPlane(const Plane& plane, const AABB& box);

	// Perform a sphere-to-plane test. 
	// Returns 1 if the point is in front of the plane.
	// Returns 2 if the point is behind the plane.
	// Returns 3 if the point straddles the plane.
	static int CheckPointToPlane(const Plane& plane, const Vector3& point);

	// Calculate the plane normal and plane offset from the input points
	static void ComputePlane(Plane &plane, const Vector3& pointA, const Vector3& pointB, const Vector3 &pointC);

	static bool CheckAABBToAABB(const AABB& lhs, const AABB& rhs);

	static bool SphereToSphere(const Sphere& lhs, const Sphere& rhs);

	static bool SphereToAABB(const Sphere& lhs, const AABB& rhs);

	static bool AABBContainsSphere(const AABB& box, const Sphere& sphere);

	static void BuildFrustum(Frustum& frustum, float fov, float nearDist, float farDist, float ratio, const Matrix4& cameraMatrix);

	static bool FrustumToSphere(const Frustum& frustum, const Sphere& sphere);

	static bool FrustumToAABB(const Frustum& frustum, const AABB& box);

};