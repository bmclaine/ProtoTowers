#pragma once

#include "MathLib.h"

class Collider;

enum CollisionState
{
	COL_NONE = -1,
	COL_ENTER = 0,
	COL_STAY = 1,
	COL_EXIT = 2
};

struct AABB{
	Vector3 center;
	Vector3 size;

	AABB() : center(0, 0, 0), size(0, 0, 0){
	}

	AABB(Vector3 _center, Vector3 _size){
		center = _center;
		size = _size;
	}

	Vector3 GetMax() const {
		return center + (size * 0.5f);
	}

	Vector3 GetMin() const {
		return center - (size * 0.5f);
	}

	bool Contains(const AABB& rhs) const{
		Vector3 lhsMax = GetMax();
		Vector3 lhsMin = GetMin();
		Vector3 rhsMax = rhs.GetMax();
		Vector3 rhsMin = rhs.GetMin();

		return (rhsMin.x > lhsMin.x && rhsMax.x < lhsMax.x && rhsMin.y > lhsMin.y && rhsMax.y < lhsMax.y && rhsMin.z > lhsMin.z && rhsMax.z < lhsMax.z);
	}
};

struct Box2D{
	Vector2 center;
	Vector2 size;
	Vector2 min;
	Vector2 max;

	Box2D() : center(0, 0), size(1, 1){
		
	}

	Box2D(Vector2 _center, Vector2 _size){
		center = _center;
		size = _size;
		max = center + (size * 0.5f);
		min = center - (size * 0.5f);
	}

	Vector2 GetMax()const{
		return max;
	}

	Vector2 GetMin()const{
		return min;
	}

	bool Contains(const Box2D& rhs) const{
		Vector2 lhsMax = GetMax();
		Vector2 lhsMin = GetMin();
		Vector2 rhsMax = rhs.GetMax();
		Vector2 rhsMin = rhs.GetMin();

		return (rhsMin.x > lhsMin.x && rhsMax.x < lhsMax.x && rhsMin.y > lhsMin.y && rhsMax.y < lhsMax.y);
	}

	bool Intersects(const Box2D& rhs) const{
		Vector2 lhsMax = GetMax();
		Vector2 lhsMin = GetMin();
		Vector2 rhsMax = rhs.GetMax();
		Vector2 rhsMin = rhs.GetMin();

		if (lhsMax.x < rhsMin.x || lhsMin.x > rhsMax.x) return false;
		if (lhsMax.y < rhsMin.y || lhsMin.y > rhsMax.y) return false;

		return true;
	}
};

struct Circle2D{
	Vector2 center;
	float radius;

	Circle2D() : center(0, 0), radius(0.5f){

	}

	Circle2D(Vector2 _center, float _radius){
		center = _center;
		radius = _radius;
	}
};

struct Sphere{
	Sphere(){

	}
	Sphere(Vector3 _center, float _radius = 0.5f) : center(_center), radius(_radius){

	}

	Vector3 center;
	float radius;
};

struct Plane{
	Vector3 normal;
	float offset;
};

struct Frustum{
	Plane planes[6];
	Vector3 corners[8];
};

struct CollisionData{
	Collider* lCollider;
	Collider* rCollider;
	Vector3 contactNormal;
	float penetration;
};

struct Ray{

	Ray(Vector3 _origin, Vector3 _direction){
		origin = _origin;
		direction = _direction;
	}

	Vector3 origin;
	Vector3 direction;
};

struct RayCastHit{
	RayCastHit(){
		distance = -FLT_MAX;
		collider = nullptr;
		point = Vector3(0, 0, 0);
		time = FLT_MAX;
	}

	Collider* collider;
	float distance;
	float time;
	Vector3 point;
};