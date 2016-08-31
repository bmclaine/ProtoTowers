#include "Physics\CollisionLibrary.h"

#include "GameObject.h"
#include "Components\Transform.h"

#define COL_NONE -1
#define COL_BOX 0
#define COL_SPHERE 1
#define COL_CAPSULE 2

bool CollisionLibrary::CheckCollision(Collider* lCollider, Collider* rCollider, CollisionData& collisionData){
	int lType = lCollider->GetColliderType();
	int rType = rCollider->GetColliderType();

	if (lType == COL_BOX){
		if (rType == COL_BOX){
			return CheckBoxToBoxCollision((BoxCollider*)lCollider, (BoxCollider*)rCollider, collisionData);
		}
		else if (rType == COL_SPHERE){
			return CheckBoxToSphereCollision((BoxCollider*)lCollider, (SphereCollider*)rCollider, collisionData);
		}
		else if (rType == COL_CAPSULE){
			return CheckBoxToCapsuleCollision((BoxCollider*)lCollider, (CapsuleCollider*)rCollider, collisionData);
		}
	}
	else if (lType == COL_SPHERE){
		if (rType == COL_BOX){
			return CheckBoxToSphereCollision((BoxCollider*)rCollider, (SphereCollider*)lCollider, collisionData);
		}
		else if (rType == COL_SPHERE){
			return CheckSphereToSphereCollision((SphereCollider*)lCollider, (SphereCollider*)rCollider, collisionData);
		}
		else if (rType == COL_CAPSULE){
			return CheckSphereToCapsuleCollision((SphereCollider*)lCollider, (CapsuleCollider*)rCollider, collisionData);
		}
	}
	else if (lType == COL_CAPSULE){
		if (rType == COL_BOX){
			return CheckBoxToCapsuleCollision((BoxCollider*)rCollider, (CapsuleCollider*)lCollider, collisionData);
		}
		else if (rType == COL_SPHERE){
			return CheckSphereToCapsuleCollision((SphereCollider*)rCollider, (CapsuleCollider*)lCollider, collisionData);
		}
		else if (rType == COL_CAPSULE){
			return CheckCapsuleToCapsuleCollision((CapsuleCollider*)lCollider, (CapsuleCollider*)rCollider, collisionData);
		}
	}

	return false;
}

bool CollisionLibrary::CheckBoxToBoxCollision(BoxCollider* lCollider, BoxCollider* rCollider, CollisionData& collisionData, float direction){

	if (lCollider->transform->WorldUpdated())
		lCollider->CalculateBoundPoints();
	Vector3* const boundPoints = lCollider->GetBoundPoints();

	if (rCollider->transform->WorldUpdated())
		rCollider->CalculateBoundPoints();
	Vector3* const otherPoints = rCollider->GetBoundPoints();

	Vector3 axis[15];

	Vector3 edges[3];
	lCollider->GetEdges(edges);

	Vector3 otherEdges[3];
	rCollider->GetEdges(otherEdges);

	axis[0] = ((boundPoints[2] - boundPoints[0]) ^ (boundPoints[3] - boundPoints[1]));
	axis[1] = ((boundPoints[1] - boundPoints[0]) ^ (boundPoints[4] - boundPoints[0]));
	axis[2] = ((boundPoints[2] - boundPoints[1]) ^ (boundPoints[5] - boundPoints[1]));

	axis[3] = ((otherPoints[2] - otherPoints[0]) ^ (otherPoints[3] - otherPoints[1]));
	axis[4] = ((otherPoints[1] - otherPoints[0]) ^ (otherPoints[4] - otherPoints[0]));
	axis[5] = ((otherPoints[2] - otherPoints[1]) ^ (otherPoints[5] - otherPoints[1]));

	axis[6] = edges[0] ^ otherEdges[0];
	axis[7] = edges[0] ^ otherEdges[1];
	axis[8] = edges[0] ^ otherEdges[2];
	axis[9] = edges[1] ^ otherEdges[0];
	axis[10] = edges[1] ^ otherEdges[1];
	axis[11] = edges[1] ^ otherEdges[2];
	axis[12] = edges[2] ^ otherEdges[0];
	axis[13] = edges[2] ^ otherEdges[1];
	axis[14] = edges[2] ^ otherEdges[2];

	Vector3 lCenter = lCollider->GetCenter();
	Vector3 rCenter = rCollider->GetCenter();
	Vector3 toCenter = rCenter - lCenter;
	Vector3 contactNormal = Vector3::Zero();
	float bestOverlap = FLT_MAX;
	int bestCase = 0;

	for (int index = 0; index < 15; ++index){
		Vector3 _axis = axis[index];

		if (_axis.MagnitudeSq() < 0.001)
			continue;

		_axis.Normalize();

		float overlap = BoxCollider::GetPenetrationOnAxis(lCollider, rCollider, _axis, toCenter);

		if (overlap < 0) return false;

		if (overlap < bestOverlap){
			bestOverlap = overlap;
			bestCase = index;
		}
	}

	contactNormal = axis[bestCase].Norm();
	if (contactNormal * toCenter > 0)
		contactNormal.Negate();

	collisionData.lCollider = lCollider;
	collisionData.rCollider = rCollider;
	collisionData.contactNormal = contactNormal * direction;
	collisionData.penetration = bestOverlap;
	return true;
}

bool CollisionLibrary::CheckBoxToSphereCollision(BoxCollider* boxCollider, SphereCollider* sphereCollider, CollisionData& collisionData, float direction){

	Vector3 contactNormal = Vector3::Zero();
	float penetration = -FLT_MAX;

	Vector3 sphereCenter = sphereCollider->GetCenter();

	Vector3 center = boxCollider->transform->InverseTransformPoint(sphereCenter);
	Vector3 boxHalfSize = boxCollider->GetSize() * 0.5f;
	float radius = sphereCollider->GetRadius();
	if (fabsf(center.x) - radius > boxHalfSize.x ||
		fabsf(center.y) - radius > boxHalfSize.y ||
		fabsf(center.z) - radius > boxHalfSize.z){
		return false;
	}

	Vector3 closestPoint(0, 0, 0);
	float distance = 0;

	distance = center.x;
	if (distance > boxHalfSize.x) distance = boxHalfSize.x;
	if (distance < -boxHalfSize.x) distance = -boxHalfSize.x;
	closestPoint.x = distance;

	distance = center.y;
	if (distance > boxHalfSize.y) distance = boxHalfSize.y;
	if (distance < -boxHalfSize.y) distance = -boxHalfSize.y;
	closestPoint.y = distance;

	distance = center.z;
	if (distance > boxHalfSize.z) distance = boxHalfSize.z;
	if (distance < -boxHalfSize.z) distance = -boxHalfSize.z;
	closestPoint.z = distance;

	//Check to see if there is a collision
	distance = (closestPoint - center).MagnitudeSq();
	if (distance > radius * radius) return false;

	Vector3 closestPointWorld = boxCollider->transform->TransformPoint(closestPoint);
	contactNormal = (closestPointWorld - sphereCollider->GetCenter()).Norm();

	penetration = radius - sqrtf(distance);

	collisionData.lCollider = boxCollider;
	collisionData.rCollider = sphereCollider;
	collisionData.contactNormal = contactNormal;
	collisionData.penetration = penetration;
	return true;
}

bool CollisionLibrary::CheckBoxToCapsuleCollision(BoxCollider* boxCollider, CapsuleCollider* capsuleCollider, CollisionData& collisionData, float direction){
	Vector3 contactNormal = Vector3::Zero();
	float penetration = FLT_MAX;

	Vector3 capsuleCenter = capsuleCollider->GetCenter();
	Vector3 capsuleStart = capsuleCollider->GetStart();
	Vector3 capsuleEnd = capsuleCollider->GetEnd();

	Vector3 colDirection = (capsuleEnd - capsuleStart).Norm();
	Vector3 toCenter = boxCollider->GetCenter() - capsuleCenter;
	float dotValue = colDirection * toCenter;
	dotValue = Clamp(0.0f, 1.0f, dotValue);

	Vector3 capsulePoint = capsuleStart + (colDirection * dotValue);
	capsulePoint = boxCollider->transform->InverseTransformPoint(capsulePoint);

	Vector3 boxHalfSize = boxCollider->GetSize() * 0.5f;
	float radius = capsuleCollider->GetRadius();
	if (fabsf(capsulePoint.x) - radius > boxHalfSize.x ||
		fabsf(capsulePoint.y) - radius > boxHalfSize.y ||
		fabsf(capsulePoint.z) - radius > boxHalfSize.z) {
		return false;
	}

	Vector3 closestPoint = Vector3(0.0f, 0.0f, 0.0f);
	float distance = 0;

	distance = capsulePoint.x;
	if (distance > boxHalfSize.x) distance = boxHalfSize.x;
	if (distance < -boxHalfSize.x) distance = -boxHalfSize.x;
	closestPoint.x = distance;

	distance = capsulePoint.y;
	if (distance > boxHalfSize.y) distance = boxHalfSize.y;
	if (distance < -boxHalfSize.y) distance = -boxHalfSize.y;
	closestPoint.y = distance;

	distance = capsulePoint.z;
	if (distance > boxHalfSize.z) distance = boxHalfSize.z;
	if (distance < -boxHalfSize.z) distance = -boxHalfSize.z;
	closestPoint.z = distance;

	//Check to see if there is a collision
	distance = (closestPoint - capsulePoint).MagnitudeSq();
	if (distance > radius * radius) return false;

	Vector3 closestPointWorld = boxCollider->transform->TransformPoint(closestPoint);
	Vector3 capsulePointWorld = boxCollider->transform->TransformPoint(capsulePoint);
	contactNormal = (closestPointWorld - capsulePointWorld).Norm();

	penetration = radius - sqrtf(distance);

	collisionData.lCollider = boxCollider;
	collisionData.rCollider = capsuleCollider;
	collisionData.contactNormal = contactNormal;
	collisionData.penetration = penetration;
	return true;
}

bool CollisionLibrary::CheckSphereToSphereCollision(SphereCollider* lCollider, SphereCollider* rCollider, CollisionData& collisionData, float direction){

	Vector3 contactNormal = Vector3::Zero();
	float penetration = 0;

	Vector3 lCenter = lCollider->GetCenter();
	Vector3 rCenter = rCollider->GetCenter();

	Vector3 diff = lCenter - rCenter;
	float distance = diff.Magnitude();

	if (distance > lCollider->GetRadius() + rCollider->GetRadius())
		return false;

	contactNormal = diff * (1.0f / distance);
	penetration = lCollider->GetRadius() + rCollider->GetRadius() - distance;

	collisionData.lCollider = lCollider;
	collisionData.rCollider = rCollider;
	collisionData.contactNormal = contactNormal * direction;
	collisionData.penetration = penetration;
	return true;
}

bool CollisionLibrary::CheckSphereToCapsuleCollision(SphereCollider* sphereCollider, CapsuleCollider* capsuleCollider, CollisionData& collisionData, float direction){
	Vector3 contactNorml = Vector3::Zero();
	float penetration = 0;

	Vector3 sphereCenter = sphereCollider->GetCenter();
	Vector3 capsuleStart = capsuleCollider->GetStart();

	Vector3 colDirection = capsuleCollider->GetDirection().Norm();
	Vector3 toCenter = sphereCenter - capsuleStart;
	float dotValue = colDirection * toCenter;

	dotValue = Clamp(0.0f, 1.0f, dotValue);

	Vector3 closestPoint = capsuleStart + (colDirection * dotValue);

	Vector3 diff = sphereCenter - closestPoint;
	float distance = diff.Magnitude();

	if (distance > sphereCollider->GetRadius() + capsuleCollider->GetRadius())
		return false;

	contactNorml = diff * (1.0f / distance);
	penetration = sphereCollider->GetRadius() + capsuleCollider->GetRadius() - distance;

	collisionData.lCollider = sphereCollider;
	collisionData.rCollider = capsuleCollider;
	collisionData.contactNormal = contactNorml * direction;
	collisionData.penetration = penetration;
	return true;
}

bool CollisionLibrary::CheckCapsuleToCapsuleCollision(CapsuleCollider* lCollider, CapsuleCollider* rCollider, CollisionData& collisionData, float direction){

	Vector3 contactNorml = Vector3::Zero();
	float penetration = 0;

	Vector3 lClosestPoint = lCollider->GetStart();
	Vector3 rClosestPoint = rCollider->GetStart();

	ClosestPtSegmentSegment(lCollider->GetStart(), rCollider->GetStart(), lCollider->GetDirection(), rCollider->GetDirection(), lClosestPoint, rClosestPoint);

	Vector3 diff = lClosestPoint - rClosestPoint;
	float distance = diff.Magnitude();

	if (distance > lCollider->GetRadius() + rCollider->GetRadius())
		return false;

	contactNorml = diff * (1.0f / distance);
	penetration = lCollider->GetRadius() + rCollider->GetRadius() - distance;

	collisionData.lCollider = lCollider;
	collisionData.rCollider = rCollider;
	collisionData.contactNormal = contactNorml * direction;
	collisionData.penetration = penetration;
	return true;
}

void CollisionLibrary::ClosestPtSegmentSegment(Vector3 lStartPoint, Vector3 rStartPoint, Vector3 lDirection, Vector3 rDirection, Vector3& lClosestPoint, Vector3& rClosestPoint){

	Vector3 startToStart = lStartPoint - rStartPoint;
	float a = (lDirection * lDirection);
	float e = (rDirection * rDirection);
	float f = (rDirection * startToStart);

	float s = 0;
	float t = 0;

	if (a <= FLT_EPSILON && e <= FLT_EPSILON) {
		lClosestPoint = lStartPoint;
		rClosestPoint = rStartPoint;
		return;
	}

	if (a <= FLT_EPSILON) {
		s = 0.0f;
		t = f / e;
		t = Clamp(0.0f, 1.0f, t);
	}
	else {
		float c = (lDirection* startToStart);
		if (e <= FLT_EPSILON) {
			t = 0.0f;
			s = Clamp(0.0f, 1.0f, -c / a);
		}
		else {
			float b = (lDirection * rDirection);
			float denom = a * e - b * b;

			if (denom != 0.0f) {
				float temp = b * f - c * e;
				s = Clamp(0.0f, 1.0f, temp / denom);
			}
			else
				s = 0;

			t = (b * s + f) / e;

			if (t < 0.0f) {
				t = 0.0f;
				s = Clamp(0.0f, 1.0f, -c / a);
			}
			else if (t > 1.0f) {
				t = 1.0f;
				s = Clamp(0.0f, 1.0f, (b - c) / a);
			}
		}
	}

	lClosestPoint = lStartPoint + lDirection * s;
	rClosestPoint = rStartPoint + rDirection * t;
}

bool CollisionLibrary::CheckRayToCollider(Ray& ray, Collider* collider, RayCastHit& hitInfo){
	int colliderType = collider->GetColliderType();
	if (colliderType == COL_BOX)
		return CheckRayToBox(ray, (BoxCollider*)collider, hitInfo);
	else if (colliderType == COL_SPHERE)
		return CheckRayToSphere(ray, (SphereCollider*)collider, hitInfo);
	else if (colliderType == COL_CAPSULE)
		return CheckRayToCapsule(ray, (CapsuleCollider*)collider, hitInfo);

	return false;
}

bool CollisionLibrary::CheckRayToSphere(Ray& ray, SphereCollider* sphereCollider, RayCastHit& hitInfo){

	float radius = sphereCollider->GetRadius();
	Vector3 toCenter = sphereCollider->GetCenter() - ray.origin;
	float dotResult = ray.direction * toCenter;
	float c = (toCenter * toCenter) - (radius * radius);
	if (dotResult > 0.0f && c > 0.0f)
		return false;

	float discr = (dotResult * dotResult) - c;
	if (discr < 0.0f){
		return false;
	}

	float time = -dotResult - sqrtf(discr);
	if (time > hitInfo.time)
		return false;

	if (time < 0.0f)
		time = 0.0f;

	Vector3 hitPoint = ray.origin + (ray.direction * time);

	hitInfo.collider = sphereCollider;
	hitInfo.point = hitPoint;
	hitInfo.time = time;
	hitInfo.distance = (ray.origin - hitPoint).Magnitude();

	return true;
}

bool CollisionLibrary::CheckRayToBox(Ray& ray, BoxCollider* boxCollider, RayCastHit& hitInfo){

	float timeMin = 0.0f;
	float timeMax = FLT_MAX;
	Vector3 boxMin = boxCollider->GetCenter() - (boxCollider->GetSize() * 0.5f);
	Vector3 boxMax = boxCollider->GetCenter() + (boxCollider->GetSize() * 0.5f);
	Vector3 rayOrigin = boxCollider->transform->InverseTransformPoint(ray.origin);
	Vector3 rayDir = boxCollider->transform->InverseTransformPoint(ray.direction);
	if (fabs(rayDir.x) < FLT_EPSILON){
		if (rayOrigin.x < boxMin.x || rayOrigin.x > boxMax.x) return false;
	}
	else{
		float ood = 1.0f / rayDir.x;
		float t1 = (boxMin.x - rayOrigin.x) * ood;
		float t2 = (boxMax.x - rayOrigin.x) * ood;

		if (t1 > t2) std::swap(t1, t2);
		if (t1 > timeMin) timeMin = t1;
		if (t2 > timeMax) timeMax = t2;
		if (timeMin > timeMax) return false;
	}

	if (fabs(rayDir.y) < FLT_EPSILON){
		if (rayOrigin.y < boxMin.y || rayOrigin.y > boxMax.y) return false;
	}
	else{
		float ood = 1.0f / rayDir.y;
		float t1 = (boxMin.y - rayOrigin.y) * ood;
		float t2 = (boxMax.y - rayOrigin.y) * ood;

		if (t1 > t2) std::swap(t1, t2);
		if (t1 > timeMin) timeMin = t1;
		if (t2 > timeMax) timeMax = t2;
		if (timeMin > timeMax) return false;
	}

	if (fabs(rayDir.z) < FLT_EPSILON){
		if (rayOrigin.z < boxMin.z || rayOrigin.z > boxMax.z) return false;
	}
	else{
		float ood = 1.0f / rayDir.z;
		float t1 = (boxMin.z - rayOrigin.z) * ood;
		float t2 = (boxMax.z - rayOrigin.z) * ood;

		if (t1 > t2) std::swap(t1, t2);
		if (t1 > timeMin) timeMin = t1;
		if (t2 > timeMax) timeMax = t2;
		if (timeMin > timeMax) return false;
	}

	if (timeMin > hitInfo.time) return false;

	Vector3 hitPoint = ray.origin + (ray.direction * timeMin);

	hitInfo.collider = boxCollider;
	hitInfo.point = hitPoint;
	hitInfo.time = timeMin;
	hitInfo.distance = (hitPoint - ray.origin).Magnitude();

	return true;
}

bool CollisionLibrary::CheckRayToCapsule(Ray& ray, CapsuleCollider* capsuleCollider, RayCastHit& hitInfo){

	Vector3 closestPointRay(0.0f, 0.0f, 0.0f);
	Vector3 closestPointCapsule(0.0f, 0.0f, 0.0f);

	ClosestPtSegmentSegment(ray.origin, capsuleCollider->GetStart(), ray.direction, capsuleCollider->GetDirection(), closestPointRay, closestPointCapsule);

	float radius = capsuleCollider->GetRadius();
	Vector3 toCenter = closestPointCapsule - ray.origin;
	float dotResult = ray.direction * toCenter;
	float c = (toCenter * toCenter) - (radius * radius);
	if (dotResult > 0.0f && c > 0.0f)
		return false;

	float discr = (dotResult * dotResult) - c;
	if (discr < 0.0f){
		return false;
	}

	float time = -dotResult - sqrtf(discr);
	if (time > hitInfo.time)
		return false;

	if (time < 0.0f)
		time = 0.0f;

	Vector3 hitPoint = ray.origin + (ray.direction * time);

	hitInfo.collider = capsuleCollider;
	hitInfo.point = hitPoint;
	hitInfo.time = time;
	hitInfo.distance = (ray.origin - hitPoint).Magnitude();

	return true;
}

bool CollisionLibrary::CheckOverlapSphereToCollider(const Vector3& origin, const float radius, Collider* collider, RayCastHit& hitInfo){
	int colliderType = collider->GetColliderType();

	if (colliderType == COL_BOX)
		return CheckOverlapSphereToBox(origin, radius, (BoxCollider*)collider, hitInfo);
	else if (colliderType == COL_SPHERE)
		return CheckOverlapSphereToSphere(origin, radius, (SphereCollider*)collider, hitInfo);
	else if (colliderType == COL_CAPSULE)
		return CheckOverlapSphereToCapsule(origin, radius, (CapsuleCollider*)collider, hitInfo);

	return false;
}

bool CollisionLibrary::CheckOverlapSphereToBox(const Vector3& origin, const float radius, BoxCollider* collider, RayCastHit& hitInfo){

	Vector3 center = collider->transform->InverseTransformPoint(origin);
	Vector3 boxHalfSize = collider->GetSize() * 0.5f;

	if (fabsf(center.x) - radius > boxHalfSize.x ||
		fabsf(center.y) - radius > boxHalfSize.y ||
		fabsf(center.z) - radius > boxHalfSize.z){
		return false;
	}

	Vector3 closestPoint(0, 0, 0);
	float distance = 0;

	distance = center.x;
	if (distance > boxHalfSize.x) distance = boxHalfSize.x;
	if (distance < -boxHalfSize.x) distance = -boxHalfSize.x;
	closestPoint.x = distance;

	distance = center.y;
	if (distance > boxHalfSize.y) distance = boxHalfSize.y;
	if (distance < -boxHalfSize.y) distance = -boxHalfSize.y;
	closestPoint.y = distance;

	distance = center.z;
	if (distance > boxHalfSize.z) distance = boxHalfSize.z;
	if (distance < -boxHalfSize.z) distance = -boxHalfSize.z;
	closestPoint.z = distance;

	//Check to see if there is a collision
	distance = (closestPoint - center).MagnitudeSq();
	if (distance > radius * radius) return false;

	distance = sqrtf(distance);

	closestPoint = collider->transform->TransformPoint(closestPoint);

	float penetration = radius - distance;
	Vector3 hitPoint = (closestPoint - center).Norm() * penetration;

	hitInfo.collider = collider;
	hitInfo.point = hitPoint;
	hitInfo.distance = distance;
	hitInfo.time = 0.0f;

	return true;
}

bool CollisionLibrary::CheckOverlapSphereToSphere(const Vector3& origin, const float radius, SphereCollider* collider, RayCastHit& hitInfo){

	Vector3 diff = origin - collider->GetCenter();
	float distance = diff.Magnitude();

	if (distance > radius + collider->GetRadius())
		return false;

	float penetration = radius + collider->GetRadius() - distance;
	Vector3 hitPoint = -diff * (1.0f / distance) * penetration;

	hitInfo.collider = collider;
	hitInfo.point = hitPoint;
	hitInfo.distance = (hitPoint - origin).Magnitude();
	hitInfo.time = 0.0f;

	return true;
}

bool CollisionLibrary::CheckOverlapSphereToCapsule(const Vector3& origin, const float radius, CapsuleCollider* collider, RayCastHit& hitInfo){

	Vector3 colDirection = collider->GetDirection().Norm();
	Vector3 toCenter = origin - collider->GetStart();
	float dotValue = colDirection * toCenter;

	dotValue = Clamp(0.0f, 1.0f, dotValue);

	Vector3 closestPoint = collider->GetStart() + (colDirection * dotValue);

	Vector3 diff = origin - closestPoint;
	float distance = diff.Magnitude();

	if (distance > radius + collider->GetRadius())
		return false;

	float penetration = radius + collider->GetRadius() - distance;
	Vector3 hitPoint = -diff * (1.0f / distance) * penetration;

	hitInfo.collider = collider;
	hitInfo.point = hitPoint;
	hitInfo.distance = (hitPoint - origin).Magnitude();
	hitInfo.time = 0.0f;

	return true;
}

// Perform a sphere-to-plane test. 
// Returns 1 if the sphere is in front of the plane.
// Returns 2 if the sphere is behind the plane.
// Returns 3 if the sphere straddles the plane.
int CollisionLibrary::CheckSphereToPlane(const Plane& plane, const Sphere& sphere){

	float sphereDistance = (plane.normal * sphere.center) - plane.offset;

	if (sphereDistance > sphere.radius)
		return 1;
	else if (sphereDistance < -sphere.radius)
		return 2;

	return 3;
}

// Perform a sphere-to-plane test. 
// Returns 1 if the box is in front of the plane.
// Returns 2 if the box is behind the plane.
// Returns 3 if the box straddles the plane.
int CollisionLibrary::CheckAABBToPlane(const Plane& plane, const AABB& box){

	int value = 0;

	Vector3 boxMin = box.GetMin();
	Vector3 boxMax = box.GetMax();

	Vector3 points[8];
	points[0] = Vector3(boxMax.x, boxMin.y, boxMin.z);
	points[1] = Vector3(boxMax.x, boxMax.y, boxMin.z);
	points[2] = Vector3(boxMax.x, boxMin.y, boxMax.z);
	points[3] = Vector3(boxMax.x, boxMax.y, boxMax.z);

	points[4] = Vector3(boxMin.x, boxMin.y, boxMin.z);
	points[5] = Vector3(boxMin.x, boxMax.y, boxMin.z);
	points[6] = Vector3(boxMin.x, boxMin.y, boxMax.z);
	points[7] = Vector3(boxMin.x, boxMax.y, boxMax.z);

	for (int i = 0; i < 8; ++i)
	{
		int result = CheckPointToPlane(plane, points[i]);

		if (result == 1)
			value++;
		else
			value--;
	}

	if (value == 8)
		return 1;
	else if (value == -8)
		return 2;

	return 3;
}

// Perform a sphere-to-plane test. 
// Returns 1 if the point is in front of the plane.
// Returns 2 if the point is behind the plane.
// Returns 3 if the point straddles the plane.
int CollisionLibrary::CheckPointToPlane(const Plane& plane, const Vector3& point){

	float result = (plane.normal * point);
	result -= plane.offset;

	if (result > FLT_EPSILON)
		return 1;
	else
		return 2;
}

// Calculate the plane normal and plane offset from the input points
void CollisionLibrary::ComputePlane(Plane &plane, const Vector3& pointA, const Vector3& pointB, const Vector3 &pointC){

	plane.normal = ((pointB - pointA) ^ (pointC - pointA)).Normalize();

	plane.offset = (plane.normal * pointA);
}

bool CollisionLibrary::CheckAABBToAABB(const AABB& lhs, const AABB& rhs){
	Vector3 lhsMax = lhs.GetMax();
	Vector3 lhsMin = lhs.GetMin();
	Vector3 rhsMax = rhs.GetMax();
	Vector3 rhsMin = rhs.GetMin();

	if (lhsMax.x < rhsMin.x || lhsMin.x > rhsMax.x) return false;
	if (lhsMax.y < rhsMin.y || lhsMin.y > rhsMax.y) return false;
	if (lhsMax.z < rhsMin.z || lhsMin.z > rhsMax.z) return false;
	return true;
}

bool CollisionLibrary::SphereToSphere(const Sphere& lhs, const Sphere& rhs){
	Vector3 diff = lhs.center - rhs.center;
	return diff.MagnitudeSq() < (lhs.radius + rhs.radius) * (lhs.radius + rhs.radius);
}

bool CollisionLibrary::SphereToAABB(const Sphere& lhs, const AABB& rhs){

	Vector3 center = lhs.center;
	Vector3 halfSize = rhs.size * 0.5f;
	Vector3 closestPoint(0, 0, 0);
	float distance = 0;

	distance = center.x;
	if (distance > halfSize.x) distance = halfSize.x;
	if (distance < -halfSize.x) distance = -halfSize.x;
	closestPoint.x = distance;

	distance = center.y;
	if (distance > halfSize.y) distance = halfSize.y;
	if (distance < -halfSize.y) distance = -halfSize.y;
	closestPoint.y = distance;

	distance = center.z;
	if (distance > halfSize.z) distance = halfSize.z;
	if (distance < -halfSize.z) distance = -halfSize.z;
	closestPoint.z = distance;

	distance = (closestPoint - center).MagnitudeSq();

	return distance < (lhs.radius * lhs.radius);
}

bool CollisionLibrary::AABBContainsSphere(const AABB& box, const Sphere& sphere){
	Vector3 halfSize = box.size * 0.5f;
	Vector3 toCenter = box.center - sphere.center;

	return (fabsf(toCenter.x) + sphere.radius < halfSize.x && fabsf(toCenter.y) + sphere.radius < halfSize.y && fabsf(toCenter.z) + sphere.radius < halfSize.z);
}


void CollisionLibrary::BuildFrustum(Frustum& frustum, float fov, float nearDist, float farDist, float ratio, const Matrix4& cameraMatrix){
	enum Corners { FAR_TOP_LEFT, FAR_BOTTOM_LEFT, FAR_BOTTOM_RIGHT, FAR_TOP_RIGHT, NEAR_TOP_LEFT, NEAR_TOP_RIGHT, NEAR_BOTTOM_RIGHT, NEAR_BOTTOM_LEFT};
	enum Planes { FAR, NEAR, TOP, BOTTOM, LEFT, RIGHT };

	Vector3 nearC = cameraMatrix.Translation() + cameraMatrix.Forward() * nearDist;
	Vector3 farC = cameraMatrix.Translation() + cameraMatrix.Forward() * farDist;

	float hNear, hFar, wNear, wFar;
	hNear = 2 * tanf(fov / 2) * nearDist;
	hFar = 2 * tanf(fov / 2) * farDist;
	//hNear = 2 * tanf(fov * 0.5f) * nearDist;
	//hFar = 2 * tanf(fov * 0.5f) * farDist;
	wNear = hNear * ratio;
	wFar = hFar * ratio;

	float mult = 0.5f;

	frustum.corners[FAR_TOP_LEFT]     = farC + cameraMatrix.Up() * (hFar * mult) - cameraMatrix.Right() * (wFar * mult);
	frustum.corners[FAR_BOTTOM_LEFT]  = farC - cameraMatrix.Up() * (hFar * mult) - cameraMatrix.Right() * (wFar * mult);
	frustum.corners[FAR_BOTTOM_RIGHT] = farC - cameraMatrix.Up() * (hFar * mult) + cameraMatrix.Right() * (wFar * mult);
	frustum.corners[FAR_TOP_RIGHT]    = farC + cameraMatrix.Up() * (hFar * mult) + cameraMatrix.Right() * (wFar * mult);

	frustum.corners[NEAR_TOP_LEFT]     = nearC + cameraMatrix.Up() * (hNear * mult) - cameraMatrix.Right() * (wNear * mult);
	frustum.corners[NEAR_TOP_RIGHT]    = nearC + cameraMatrix.Up() * (hNear * mult) + cameraMatrix.Right() * (wNear * mult);
	frustum.corners[NEAR_BOTTOM_RIGHT] = nearC - cameraMatrix.Up() * (hNear * mult) + cameraMatrix.Right() * (wNear * mult);
	frustum.corners[NEAR_BOTTOM_LEFT]  = nearC - cameraMatrix.Up() * (hNear * mult) - cameraMatrix.Right() * (wNear * mult);

	/*frustum.corners[0] = farC + cameraMatrix.Up() * (hFar * 0.5f) - cameraMatrix.Right() * (wFar * 0.5f);
	frustum.corners[1] = frustum.corners[0] - (cameraMatrix.Up() * hFar);
	frustum.corners[2] = frustum.corners[1] + (cameraMatrix.Right() * wFar);
	frustum.corners[3] = frustum.corners[0] + (cameraMatrix.Right() * wFar);

	frustum.corners[4] = nearC + cameraMatrix.Up() * (hNear * 0.5f) - cameraMatrix.Right() * (wNear * 0.5f);
	frustum.corners[5] = frustum.corners[4] - (cameraMatrix.Up() * hNear);
	frustum.corners[6] = frustum.corners[5] + (cameraMatrix.Right() * wNear);
	frustum.corners[7] = frustum.corners[4] + (cameraMatrix.Right() * wNear);*/

	ComputePlane(frustum.planes[FAR], frustum.corners[FAR_TOP_LEFT], frustum.corners[FAR_TOP_RIGHT], frustum.corners[FAR_BOTTOM_RIGHT]);
	ComputePlane(frustum.planes[NEAR], frustum.corners[NEAR_TOP_RIGHT], frustum.corners[NEAR_TOP_LEFT], frustum.corners[NEAR_BOTTOM_RIGHT]);
	ComputePlane(frustum.planes[TOP], frustum.corners[NEAR_TOP_LEFT], frustum.corners[NEAR_TOP_RIGHT], frustum.corners[FAR_TOP_LEFT]);
	ComputePlane(frustum.planes[BOTTOM], frustum.corners[NEAR_BOTTOM_RIGHT], frustum.corners[NEAR_BOTTOM_LEFT], frustum.corners[FAR_BOTTOM_LEFT]);
	ComputePlane(frustum.planes[LEFT], frustum.corners[NEAR_BOTTOM_LEFT], frustum.corners[NEAR_TOP_LEFT], frustum.corners[FAR_TOP_LEFT]);
	ComputePlane(frustum.planes[RIGHT], frustum.corners[NEAR_TOP_RIGHT], frustum.corners[NEAR_BOTTOM_RIGHT], frustum.corners[FAR_BOTTOM_RIGHT]);
}

bool CollisionLibrary::FrustumToSphere(const Frustum& frustum, const Sphere& sphere){
	for (int i = 0; i < 6; ++i){
		if (CheckSphereToPlane(frustum.planes[i], sphere) == 2)
			return false;
	}

	return true;
}

bool CollisionLibrary::FrustumToAABB(const Frustum& frustum, const AABB& box){

	for (int i = 0; i < 6; ++i){
		if (CheckAABBToPlane(frustum.planes[i], box) == 2)
			return false;
	}

	return true;
}



