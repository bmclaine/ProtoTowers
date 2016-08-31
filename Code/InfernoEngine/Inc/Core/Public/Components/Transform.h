// Author: Jonathan Gyurkovics
//
// Handles the position, rotation, and scalar data of an object, as well as handling the local and world matrices

#pragma once

#include "IComponent.h"
#include "MathLib.h"

#include <vector>

class Transform : public IComponent
{
protected:
	enum TRANSFORM_FLAGS {
		WORLD_DIRTY,		// States if the world matrix is currently dirty
		WORLD_UPDATED,		// States if the world matrix was updated this frame
		INV_WORLD_DIRTY,	// States if the inverse world is currently dirty
		LOCAL_DIRTY,		// States if the local matrix is currently dirty
		ENABLED,			// States if the OnEnable function has been called yet
		TOTAL_FLAGS,
	};

	std::vector<Transform*> m_vChildren;		// Holds the list of children the current transform has
	Transform*				m_pParent;			// Holds the pointer to the transforms partent, if it has one

	// Temp for now
	Vector3 m_f3Position;
	Vector3 m_f3Rotation;
	Vector3 m_f3Scale;

	Matrix4 m_m4LocalMatrix;					// Holds the local matrix
	Matrix4 m_m4WorldMatrix;					// Holds the world matrix
	Matrix4 m_m4InvWorldMatrix;					// Holds the inverse world matrix

	Flags<char> m_fcFlags;						// Holds the transform flags

	// IN: void
	// OUT: void
	//
	// Sets our dirty flags to true
	void NotifyDirty();

	Transform(GameObject* const, const Vector3& Position = Vector3(), const Quaternion& Rotation = Quaternion::Identity(), const Vector3& Scale = Vector3(1.0f, 1.0f, 1.0f));
	Transform(GameObject* const, const Vector3& Position, const Vector3& Rotation, const Vector3& Scale = Vector3(1.0f, 1.0f, 1.0f));
	virtual ~Transform();

	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	Transform(GameObject* _gameObject, Transform* _transform, const Vector3& Position = Vector3(), const Quaternion& Rotation = Quaternion::Identity(), const Vector3& Scale = Vector3(1.0f, 1.0f, 1.0f));

	virtual IComponent& operator=(IComponent& _rhs) override;
	Transform& operator=(const Transform& component);

	virtual void Init() override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	virtual void OnEnable() override;
	virtual void OnDisable() override;

	virtual void OnDestroy() override;

	// IN: Vector3 - the position of the object to look at
	// OUT: void
	//
	// Sets the world of the object's forward to look at the look position
	void LookAt(const Vector3& lookPosition);

	// IN: void
	// OUT: Transform& - A reference to the top mose transform
	//
	// Returns the topmost transform
	const Transform& Root() const;

	// IN: void
	// OUT: void
	//
	// Update the component
	virtual void Update() override;

	// IN: void
	// OUT: void
	//
	// Detaches all attached children
	void DetachChildren();

	// IN: void
	// OUT: void
	//
	// Detaches all children WITHOUT alerting the children and setting their parent to nullptr. The is an unsafe function, that should only ever be called
	// when the engine is shutting down, for cases like cleaning up all created GameObjects with no order.
	void LoseChildren();

	// IN: string - Name of the desired child
	// OUT: void
	//
	// Finds a Child by the given name. First looks through the first layer of children, then looks in the children's children.
	GameObject* GetChildByName(std::string& _name);

	// IN: string - the name of the child
	// OUT: Transform* - pointer to the found child
	//
	// Finds a child by name and returns it
	Transform* Find(std::string name) const;
	// IN: string - the name of the child
	// OUT: Transform* - pointer to the found child
	//
	// Finds a child by name and returns it
	Transform* Find(const char* name) const;
	
	// IN: unsigned int - index of the child to grab
	// OUT: Transform* - pointer to the child transform
	//
	// Returns the child of the passed in index
	Transform* GetChild(unsigned int index) const;

	// IN: void
	// OUT: int - index into the parents child array. -1 = not found or null parent
	//
	// Returns the sibling index of the transform
	int GetSiblingIndex() const;

	// IN: Vector3 - the direction to transform
	// OUT: Vector3 - transformed direction
	//
	// Transforms a direction from world space to local space
	Vector3 InverseTransformDirection(const Vector3& direction);
	// IN: Vector3 - the position to transform
	// OUT: Vector3 - transformed position
	//
	// Transforms a point from world space to local space
	Vector3 InverseTransformPoint(const Vector3& point);
	// IN: Vector3 - the vector to transform
	// OUT: Vector3 - transformed vector
	//
	// Transforms a vector from world space to local space
	Vector3 InverseTransformVector(const Vector3& vector);

	// IN: Transform* - pointer to a transform to check if it's a parent
	// OUT: bool - true if sent in transform is a parent, false otherwise
	//
	// Returns if this transform is a child of the passed in transform
	bool IsChildOf(const Transform* parent) const;

	// IN: void
	// OUT: void
	//
	// Sets to first element of the local transform list
	void SetAsFirstSibling();
	// IN: void
	// OUT: void
	//
	// Sets the last element of the local transform list
	void SetAsLastSibling();
	// IN: unsigned int - index to change to
	// OUT: void
	//
	// Sets the index of the local transform list
	void SetSiblingIndex(unsigned int index);

	// IN: Transform* - pointer to the new parent
	// OUT: void
	//
	// Sets this transforms parent to the passed in transform
	void SetParent(Transform* parent);

	Transform * GetParent() const;

	// IN: Vector3 - the direction to transform
	// OUT: Vector3 - transformed direction
	//
	// Transforms a direction from local space to world space
	Vector3 TransformDirection(const Vector3& direction);
	// IN: Vector3 - the point to transform
	// OUT: Vector3 - transformed point
	//
	// Transforms a point from local space to world space
	Vector3 TransformPoint(const Vector3& point);
	// IN: Vector3 - the vector to transform
	// OUT: Vector3 - transformed vector
	//
	// Transforms a vector from local space to world space
	Vector3 TransformVector(const Vector3& vector);

	// IN: Vector3 - vector to translate by
	// OUT: void
	//
	// Translates the transfrom by the passed in vector
	void Translate(const Vector3& translation);

	// IN: Vector3 - Vector to rotate by (In Radians)
	// OUT: void
	//
	// Applies a rotation of EulerAngles.z around the Z-Axis, EulerAngles.y around the Y-Axis,
	// and EulerAngles.x around the X-Axis in that order
	void Rotate(const Vector3& rotation);
	// IN: Vector3 - Point to rotate around
	//     Vector3 - Axis to rotate about
	//     float   - Angle to rotate by
	// OUT: void
	//
	// Rotates about a point by the axis that passes through it by the angle passed in
	void RotateAround(const Vector3& point, const Vector3& axis, float angle);

	// Accessors and Mutators

	// IN: void
	// OUT: bool - true if the world matrix was updated
	//
	// States if the local matrix was updated this frame
	bool WorldUpdated() const;
	// IN: void
	// OUT: Vector3 - World Position
	//
	// Returns the World Position of the transform
	Vector3 GetPosition();
	// IN: void
	// OUT: Quaternion - World Rotation
	//
	// Returns the World Rotation of the transform
	Vector3 GetRotation();
	// IN: void
	// OUT: World Scale
	//
	// Returns the World Scale of the transform
	Vector3 GetScale();

	// IN: void
	// OUT: unsigned int - number of children
	//
	// Returns the number of children the transform has
	unsigned int GetChildCount() const;

	// IN: void
	// OUT: Vector3 - Local Forward
	//
	// Returns the local foward of the transform
	Vector3 Forward();

	Vector3 WorldForward();

	// IN: void
	// OUT: Vector3 - Local Right
	//
	// Returns the local right of the transform
	Vector3 Right();

	Vector3 WorldRight();

	// IN: void
	// OUT: Vector3 - Local Up
	//
	// Returns the local up of the transform
	Vector3 Up();

	Vector3 WorldUp();
	
	// IN: void
	// OUT: Vecor3 - Local Position
	// Returns the Local Position of the transform
	Vector3 LocalPosition();
	// IN: Void
	// OUT: Quaternion - Local Rotation
	//
	// Returns the Local Rotation of the transform
	Vector3 LocalRotation();
	// IN: void
	// OUT: Local Scale
	//
	// Returns the Local Scale of the transform
	Vector3 LocalScale();

	// IN: void
	// OUT: Matrix4 - Local Matrix of the transform
	//
	// Returns the Local Matrix of the transform
	const Matrix4& LocalMatrix();
	// IN: void
	// OUT: Matrix4 - World Matrix of the transfrom
	//
	// Returns the World Matrix of the transform
	const Matrix4& WorldMatrix();
	// IN: void
	// OUT: Matrix4 - Inverse World Matrix of the transform
	//
	// Returns the Inverse World Matrix of the transform
	const Matrix4& InvWorldMatrix();

	void SetLocalMatrix(const Matrix4& mat);
	void SetWorldMatrix(const Matrix4& mat);
	void SetPosition(const Vector3& pos);
	void SetPosition(const Vector4& pos);
	void SetPosition(const float x, const float y, const float z);
	void SetXPosition(const float x);
	void SetYPosition(const float y);
	void SetZPosition(const float z);
	void SetRotation(const Quaternion& rot);
	void SetRotation(const Vector3& rot);
	void SetRotation(const Vector4& rot);
	void SetRotation(const float x, const float y, const float z);
	void SetScale(const Vector3& scale);
	void SetScale(const Vector4& scale);
	void SetScale(const float x, const float y, const float z);
	void SetXScale(const float x);
	void SetYScale(const float y);
	void SetZScale(const float z);

	friend class GameObject;
};

