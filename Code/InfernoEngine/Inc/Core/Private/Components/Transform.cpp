#include "Components\Transform.h"
#include "GameObject.h"
#include "../../Interface/Public/Engine.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "GlobalIncludes.h"

Transform::Transform(GameObject* _gameObject, Transform* _transform, const Vector3& Position, const Quaternion& Rotation, const Vector3& Scale) : IComponent(gameObject, this), m_fcFlags(TOTAL_FLAGS)
{
	m_f3Position = Position;
	m_f3Rotation = Rotation.GetEulerAngles();
	m_f3Scale = Scale;
	m_m4LocalMatrix = Matrix4::Scale(Scale) * Rotation.ToMatrix4() * Matrix4::Translate(Position);

	m_fcFlags.SetFlag(WORLD_DIRTY, true);
	m_fcFlags.SetFlag(WORLD_UPDATED, true);
	m_fcFlags.SetFlag(INV_WORLD_DIRTY, true);
	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
	m_pParent = nullptr;
}

Transform::Transform(GameObject* const gameObject, const Vector3& Position, const Quaternion& Rotation, const Vector3& Scale) : IComponent(gameObject, this), m_fcFlags(TOTAL_FLAGS)
{
	m_f3Position = Position;
	m_f3Rotation = Rotation.GetEulerAngles();
	m_f3Scale = Scale;
	m_m4LocalMatrix = Matrix4::Scale(Scale) * Rotation.ToMatrix4() * Matrix4::Translate(Position);

	m_fcFlags.SetFlag(WORLD_DIRTY, true);
	m_fcFlags.SetFlag(WORLD_UPDATED, true);
	m_fcFlags.SetFlag(INV_WORLD_DIRTY, true);
	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
	m_pParent = nullptr;
}

Transform::Transform(GameObject* const gameObject, const Vector3& Position, const Vector3& Rotation, const Vector3& Scale) : IComponent(gameObject, this), m_fcFlags(TOTAL_FLAGS)
{
	m_f3Position = Position;
	m_f3Rotation = Rotation;
	m_f3Scale = Scale;

	Quaternion quat = Quaternion::FromEulerAngles(Rotation);
	m_m4LocalMatrix = Matrix4::Scale(Scale) * quat.ToMatrix4() *Matrix4::Translate(Position);

	m_fcFlags.SetFlag(WORLD_DIRTY, true);
	m_fcFlags.SetFlag(INV_WORLD_DIRTY, true);
	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
	m_pParent = nullptr;
}

Transform::~Transform()
{
	while (m_vChildren.size() > 0) {
		InfernoEngine_ObjectManagement::TrueDestroy(m_vChildren[0]->gameObject);
	}

//	for (size_t child = 0; child < m_vChildren.size(); child++) {
//		if (m_vChildren[child] != nullptr)
//			InfernoEngine_ObjectManagement::TrueDestroy(m_vChildren[child]->gameObject);
//	}

	m_vChildren.clear();
}

IComponent& Transform::operator=(IComponent& _rhs)
{
	Transform* rhsTransform = dynamic_cast<Transform*>(&_rhs);
	if (rhsTransform == nullptr) {
		return *this;
	}

	return operator=(*rhsTransform);
}

Transform& Transform::operator=(const Transform& _rhs)
{
	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
		m_f3Position = _rhs.m_f3Position;
		m_f3Rotation = _rhs.m_f3Rotation;
		m_f3Scale = _rhs.m_f3Scale;

		if (memcmp(&m_m4LocalMatrix, &_rhs.m_m4LocalMatrix, sizeof(Matrix4))) {
			m_m4LocalMatrix = _rhs.m_m4LocalMatrix;
		}

		while (true) {
			size_t count = m_vChildren.size();
			if (count == _rhs.m_vChildren.size()) {
				// === Correct Amount of children, set them equal to each other
				for (size_t i = 0; i < count; ++i) {
					*(m_vChildren[i]->gameObject) = *(_rhs.m_vChildren[i]->gameObject);
				}
				break;
			}
			else if (count > _rhs.m_vChildren.size()) {
				// === Too many children, remove any added children
				// ** NOTE ** Since True Destroy sets the child's parent to null, we need to decrease the count, so that we don't go out of range.
				for (size_t i = _rhs.m_vChildren.size(); i < count; --count) {
					InfernoEngine_ObjectManagement::TrueDestroy(m_vChildren[i]->gameObject);
				}
			}
			else {
				// === Not enough, just exit
				break;
			}
		}

		NotifyDirty();
	}

	return *this;
}

void Transform::Init()
{
	size_t count = m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		m_vChildren[i]->Init();
	}
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void Transform::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propertyName = "Position";
	Property* prop = data->GetProperty(propertyName);
	std::vector<std::string> values = Inferno::StringParse(prop->value, ',');
	SetPosition((float)atof(values[0].c_str()), (float)atof(values[1].c_str()), (float)atof(values[2].c_str()));

	propertyName = "Rotation";
	prop = data->GetProperty(propertyName);
	values = Inferno::StringParse(prop->value, ',');
	SetRotation(ToRadians((float)atof(values[0].c_str())), ToRadians((float)atof(values[1].c_str())), ToRadians((float)atof(values[2].c_str())));

	propertyName = "Scale";
	prop = data->GetProperty(propertyName);
	values = Inferno::StringParse(prop->value, ',');
	SetScale((float)atof(values[0].c_str()), (float)atof(values[1].c_str()), (float)atof(values[2].c_str()));

	if (m_pParent == nullptr){
		propertyName = "Parent";
		prop = data->GetProperty(propertyName);
		if (prop){
			unsigned int parentID = (unsigned int)atoi(prop->value.c_str());
			Transform* parent = static_cast<Transform*>(objectMap.find(parentID)->second);
			SetParent(parent);
		}
	}
}

void Transform::OnEnable()
{
	size_t count = m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		m_vChildren[i]->gameObject->OnEnable();
	}

	IComponent::OnEnable();
}

void Transform::OnDisable()
{
	size_t count = m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		m_vChildren[i]->gameObject->OnDisable();
	}

	IComponent::OnDisable();
}

void Transform::OnDestroy()
{
	size_t count = m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		m_vChildren[i]->gameObject->OnDestroy();
	}
}

IComponent* Transform::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	Transform* trans = new Transform(gameObject);
	(*trans) = *this;

	return trans;
}

// IN: void
// OUT: void
//
// Sets our dirty flags to true
void Transform::NotifyDirty()
{
	// Check if we are already considered dirty
	if (m_fcFlags.GetFlag(WORLD_DIRTY))
		return;

	m_fcFlags.SetFlag(WORLD_DIRTY, true);
	m_fcFlags.SetFlag(INV_WORLD_DIRTY, true);

	// Work down the chain of children to let them know that they are now dirty as well
	for (size_t child = 0; child < m_vChildren.size(); child++)
		m_vChildren[child]->NotifyDirty();
}

// IN: Vector3 - the position of the object to look at
// OUT: void
//
// Sets the world of the object's forward to look at the look position
void Transform::LookAt(const Vector3& lookPosition)
{
	// Grab the world matrix of the game object and have it look at the passed in position
	Matrix4 world = WorldMatrix();
	world.LookAt(world.Translation(), lookPosition);

	// If we are not the root game object, grab Inverse World matrix so we can update our local matrix
	if (m_pParent != nullptr)
		m_m4LocalMatrix = m_pParent->InvWorldMatrix() * world;
	else
		m_m4LocalMatrix = world;

	m_f3Rotation = m_m4LocalMatrix.GetRotation();

	// Notify all children we are now dirty
	NotifyDirty();
}

// IN: void
// OUT: Transform& - A reference to the top mose transform
//
// Returns the topmost transform
const Transform& Transform::Root() const
{
	if (m_pParent == nullptr)
		return *this;
	else
		return m_pParent->Root();
}

// IN: void
// OUT: void
//
// Update the component
void Transform::Update()
{
	if (m_fcFlags.GetFlag(LOCAL_DIRTY))
	{
		m_m4LocalMatrix = Matrix4::Affine(m_f3Position, Quaternion::FromEulerAngles(m_f3Rotation), m_f3Scale);//Matrix4::Scale(m_f3Scale) * Quaternion::FromEulerAngles(m_f3Rotation).ToMatrix4()/*Matrix4::RotationZ(m_f3Rotation.z) * Matrix4::RotationX(m_f3Rotation.x) * Matrix4::RotationY(m_f3Rotation.y)*/ * Matrix4::Translate(m_f3Position);

		m_fcFlags.SetFlag(LOCAL_DIRTY, false);
		m_fcFlags.SetFlag(WORLD_UPDATED, true);
		NotifyDirty();
	}
	else if (m_fcFlags.GetFlag(WORLD_DIRTY))
	{
		m_fcFlags.SetFlag(WORLD_UPDATED, true);
	}
	else
		m_fcFlags.SetFlag(WORLD_UPDATED, false);
}

// IN: void
// OUT: void
//
// Detaches all attached children
void Transform::DetachChildren()
{
	for (size_t child = 0; child < m_vChildren.size(); child++)
		m_vChildren[child]->SetParent(nullptr);

	m_vChildren.clear();
}

// IN: void
// OUT: void
//
// Detaches all children WITHOUT alerting the children and setting their parent to nullptr. The is an unsafe function, that should only ever be called
// when the engine is shutting down, for cases like cleaning up all created GameObjects with no order.
void Transform::LoseChildren()
{
	m_vChildren.clear();
}

// IN: string - Name of the desired child
// OUT: void
//
// Finds a Child by the given name. First looks through the first layer of children, then looks in the children's children.
GameObject* Transform::GetChildByName(std::string& _name)
{
	size_t count = m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vChildren[i]->gameObject->GetName() == _name) {
			return m_vChildren[i]->gameObject;
		}
	}

	GameObject* foundChild = nullptr;
	for (size_t i = 0; i < count; ++i) {
		foundChild = m_vChildren[i]->GetChildByName(_name);
		if (foundChild != nullptr) {
			return foundChild;
		}
	}

	return foundChild;
}

// IN: string - the name of the child
// OUT: Transform* - pointer to the found child
//
// Finds a child by name and returns it
Transform* Transform::Find(std::string name) const { return Find(name.c_str()); }
// IN: string - the name of the child
// OUT: Transform* - pointer to the found child
//
// Finds a child by name and returns it
Transform* Transform::Find(const char* name) const
{
	for (size_t child = 0; child < m_vChildren.size(); child++)
	{
		if (strcmp(m_vChildren[child]->gameObject->GetName().c_str(), name) == 0)
			return m_vChildren[child];
	}

	return nullptr;
}

// IN: unsigned int - index of the child to grab
// OUT: Transform* - pointer to the child transform
//
// Returns the child of the passed in index
Transform* Transform::GetChild(unsigned int index) const
{
	if (index < m_vChildren.size())
		return m_vChildren[index];

	return nullptr;
}

// IN: void
// OUT: int - index into the parents child array. -1 = not found or null parent
//
// Returns the sibling index of the transform
int Transform::GetSiblingIndex() const
{
	if (m_pParent == nullptr)
		return -1;

	for (size_t child = 0; child < m_pParent->m_vChildren.size(); child++)
	{
		if (m_pParent->m_vChildren[child] == this)
			return (int)child;
	}

	return -1;
}

// IN: Vector3 - the direction to transform
// OUT: Vector3 - transformed direction
//
// Transforms a direction from world space to local space
Vector3 Transform::InverseTransformDirection(const Vector3& direction) { return InvWorldMatrix() * direction; }
// IN: Vector3 - the position to transform
// OUT: Vector3 - transformed position
//
// Transforms a point from world space to local space
Vector3 Transform::InverseTransformPoint(const Vector3& point) { return InvWorldMatrix() * point; }
// IN: Vector3 - the vector to transform
// OUT: Vector3 - transformed vector
//
// Transforms a vector from world space to local space
Vector3 Transform::InverseTransformVector(const Vector3& vector) { return InvWorldMatrix() * vector; }

// IN: Transform* - pointer to a transform to check if it's a parent
// OUT: bool - true if sent in transform is a parent, false otherwise
//
// Returns if this transform is a child of the passed in transform
bool Transform::IsChildOf(const Transform* parent) const
{
	if (m_pParent == nullptr)
		return false;

	if (m_pParent == parent)
		return true;

	return m_pParent->IsChildOf(parent);
}

// IN: void
// OUT: void
//
// Sets to first element of the local transform list
void Transform::SetAsFirstSibling()
{
	if (m_pParent != nullptr)
	{
		auto iter = m_pParent->m_vChildren.begin();
		for (; iter != m_pParent->m_vChildren.end(); ++iter)
		{
			if ((*iter) == this)
			{
				m_pParent->m_vChildren.erase(iter);
				m_pParent->m_vChildren.insert(m_pParent->m_vChildren.begin(), this);
				break;
			}
		}
	}
}

// IN: void
// OUT: void
//
// Sets the last element of the local transform list
void Transform::SetAsLastSibling()
{
	if (m_pParent != nullptr)
	{
		auto iter = m_pParent->m_vChildren.begin();
		for (; iter != m_pParent->m_vChildren.end(); ++iter)
		{
			if ((*iter) == this)
			{
				m_pParent->m_vChildren.erase(iter);
				m_pParent->m_vChildren.insert(m_pParent->m_vChildren.end(), this);
				break;
			}
		}
	}
}

// IN: unsigned int - index to change to
// OUT: void
//
// Sets the index of the local transform list
void Transform::SetSiblingIndex(unsigned int index)
{
	if (m_pParent != nullptr && index < m_pParent->m_vChildren.size())
	{
		auto iter = m_pParent->m_vChildren.begin();
		for (; iter != m_pParent->m_vChildren.end(); ++iter)
		{
			if ((*iter) == this)
			{
				m_pParent->m_vChildren.erase(iter);
				m_pParent->m_vChildren.insert(m_pParent->m_vChildren.begin() + index, this);
				break;
			}
		}
	}
}

// IN: Transform* - pointer to the new parent
// OUT: void
//
// Sets this transforms parent to the passed in transform
void Transform::SetParent(Transform* parent)
{
	if (parent == m_pParent)
		return;

	// If we have a parent, remove use from the parent children list
	if (m_pParent != nullptr)
	{
		auto iter = m_pParent->m_vChildren.begin();
		for (; iter != m_pParent->m_vChildren.end(); ++iter)
		{
			if ((*iter) == this)
			{
				m_pParent->m_vChildren.erase(iter);
				break;
			}
		}
	}

	m_pParent = parent;

	// If our incoming parent is not null, add ourselves to the child list
	if (parent != nullptr) {
		parent->m_vChildren.push_back(this);
		InfernoEngine_ObjectManagement::RemoveFromSceneList(gameObject);

		// === Has our parent been enabled yet?
		if (!parent->isEnabled()) {
			InfernoEngine_ObjectManagement::RemoveFromOnEnable(this->gameObject);
		}
	} 
	else {
		// Add us back to the SceneList

	}

	m_fcFlags.SetFlag((unsigned int)WORLD_DIRTY, (unsigned int)true);
}

Transform * Transform::GetParent() const
{
	return m_pParent;
}

// IN: Vector3 - the direction to transform
// OUT: Vector3 - transformed direction
//
// Transforms a direction from local space to world space
Vector3 Transform::TransformDirection(const Vector3& direction) { return WorldMatrix() * direction; }
// IN: Vector3 - the point to transform
// OUT: Vector3 - transformed point
//
// Transforms a point from local space to world space
Vector3 Transform::TransformPoint(const Vector3& point) { return WorldMatrix() * point; }
// IN: Vector3 - the vector to transform
// OUT: Vector3 - transformed vector
//
// Transforms a vector from local space to world space
Vector3 Transform::TransformVector(const Vector3& vector) { return WorldMatrix() * vector; }

// IN: Vector3 - vector to translate by
// OUT: void
//
// Translates the transfrom by the passed in vector
void Transform::Translate(const Vector3& translation)
{
	m_f3Position += m_m4LocalMatrix.Right() * translation.x + m_m4LocalMatrix.Up() * translation.y + m_m4LocalMatrix.Forward() * translation.z;

	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
	NotifyDirty();
}

// IN: Vector3 - Vector to rotate by (In Radians)
// OUT: void
//
// Applies a rotation of EulerAngles.z around the Z-Axis, EulerAngles.y around the Y-Axis,
// and EulerAngles.x around the X-Axis in that order
void Transform::Rotate(const Vector3& rotation)
{
	m_f3Rotation += rotation;

	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
	NotifyDirty();
}

// IN: Vector3 - Point to rotate around
//     Vector3 - Axis to rotate about
//     float   - Angle to rotate by
// OUT: void
//
// Rotates about a point by the axis that passes through it by the angle passed in
void Transform::RotateAround(const Vector3& point, const Vector3& axis, float angle)
{
	m_m4LocalMatrix.RotateAround(point, axis, angle);

	// Notify our children that we are now dirty
	NotifyDirty();
}

// Accessors and Mutators

// IN: void
// OUT: bool - true if the world matrix was updated
//
// States if the local matrix was updated this frame
bool Transform::WorldUpdated() const { return m_fcFlags.GetFlag(WORLD_UPDATED); }
// IN: void
// OUT: Vector3 - World Position
//
// Returns the World Position of the transform
Vector3 Transform::GetPosition() { return WorldMatrix().Translation(); }
// IN: void
// OUT: Quaternion - World Rotation
//
// Returns the World Rotation of the transform
Vector3 Transform::GetRotation()
{
	//	return WorldMatrix().GetRotation();
	return m_f3Rotation;
}
// IN: void
// OUT: World Scale
//
// Returns the World Scale of the transform
Vector3 Transform::GetScale()
{
	return WorldMatrix().GetScale();
}

// IN: void
// OUT: unsigned int - number of children
//
// Returns the number of children the transform has
unsigned int Transform::GetChildCount() const { return (unsigned int)m_vChildren.size(); }

// IN: void
// OUT: Vector3 - Local Forward
//
// Returns the local foward of the transform
Vector3 Transform::Forward() { return LocalMatrix().Forward(); }

Vector3 Transform::WorldForward() { return WorldMatrix().Forward(); }

// IN: void
// OUT: Vector3 - Local Right
//
// Returns the local right of the transform
Vector3 Transform::Right() { return LocalMatrix().Right(); }

Vector3 Transform::WorldRight() { return WorldMatrix().Right(); }

// IN: void
// OUT: Vector3 - Local Up
//
// Returns the local up of the transform
Vector3 Transform::Up() { return LocalMatrix().Up(); }

Vector3 Transform::WorldUp() { return WorldMatrix().Up(); }

// IN: void
// OUT: Vecor3 - Local Position
// Returns the Local Position of the transform
Vector3 Transform::LocalPosition() { return LocalMatrix().Translation(); }
// IN: Void
// OUT: Quaternion - Local Rotation
//
// Returns the Local Rotation of the transform
Vector3 Transform::LocalRotation() { return LocalMatrix().GetRotation(); }
// IN: void
// OUT: Local Scale
//
// Returns the Local Scale of the transform
Vector3 Transform::LocalScale() { return LocalMatrix().GetScale(); }

// IN: void
// OUT: Matrix4 - Local Matrix of the transform
//
// Returns the Local Matrix of the transform
const Matrix4& Transform::LocalMatrix()
{
	if (m_fcFlags.GetFlag(LOCAL_DIRTY)){
		m_m4LocalMatrix = Matrix4::Affine(m_f3Position, Quaternion::FromEulerAngles(m_f3Rotation), m_f3Scale); //Matrix4::Scale(m_f3Scale) * Quaternion::FromEulerAngles(m_f3Rotation).ToMatrix4()/*Matrix4::RotationZ(m_f3Rotation.z) * Matrix4::RotationX(m_f3Rotation.x) * Matrix4::RotationY(m_f3Rotation.y)*/ *Matrix4::Translate(m_f3Position);
		m_fcFlags.SetFlag(LOCAL_DIRTY, false);
		NotifyDirty();
	}

	return m_m4LocalMatrix;
}
// IN: void
// OUT: Matrix4 - World Matrix of the transfrom
//
// Returns the World Matrix of the transform
const Matrix4& Transform::WorldMatrix()
{
	if (m_fcFlags.GetFlag(WORLD_DIRTY))
	{
		m_fcFlags.SetFlag(WORLD_DIRTY, false);

		if (m_pParent != nullptr)
			m_m4WorldMatrix = LocalMatrix() * m_pParent->WorldMatrix();
		else
			m_m4WorldMatrix = LocalMatrix();
	}

	return m_m4WorldMatrix;
}
// IN: void
// OUT: Matrix4 - Inverse World Matrix of the transform
//
// Returns the Inverse World Matrix of the transform
const Matrix4& Transform::InvWorldMatrix()
{
	if (m_fcFlags.GetFlag(INV_WORLD_DIRTY))
	{
		m_fcFlags.SetFlag(INV_WORLD_DIRTY, false);

		m_m4InvWorldMatrix = WorldMatrix().Inverse();
	}

	return m_m4InvWorldMatrix;
}

void Transform::SetLocalMatrix(const Matrix4& mat)
{
	m_m4LocalMatrix = mat;

	// Needed for Decompose
	Quaternion rot;
	m_m4LocalMatrix.Decompose(m_f3Position, rot, m_f3Scale);
	m_f3Rotation = m_m4LocalMatrix.GetRotation();

	NotifyDirty();
}
void Transform::SetWorldMatrix(const Matrix4& mat)
{
	Matrix4 newMat = mat;

	if (m_pParent)
		newMat = m_pParent->InvWorldMatrix() * newMat;

	SetLocalMatrix(newMat);
}
void Transform::SetPosition(const Vector3& pos)
{
	if (!pos.IsValid())
		return;

	m_f3Position = pos;

	NotifyDirty();
	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
}
void Transform::SetPosition(const Vector4& pos)
{
	if (!pos.IsValid())
		return;

	m_f3Position = pos;

	NotifyDirty();
	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
}
void Transform::SetPosition(const float x, const float y, const float z)
{
	if (!Vector3(x, y, z).IsValid())
		return;

	m_f3Position = Vector3(x, y, z);

	NotifyDirty();
	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
}
void Transform::SetXPosition(const float x)
{
	if (!IsFloatValid(x))
		return;

	m_f3Position.x = x;

	NotifyDirty();
	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
}
void Transform::SetYPosition(const float y)
{
	if (!IsFloatValid(y))
		return;

	m_f3Position.y = y;

	NotifyDirty();
	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
}
void Transform::SetZPosition(const float z)
{
	if (!IsFloatValid(z))
		return;

	m_f3Position.z = z;

	NotifyDirty();
	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
}

//void Transform::SetRotation(const Quaternion& rot) { m_m4LocalMatrix = rot.ToMatrix3(); }

void Transform::SetRotation(const Vector3& rot) { 

	m_f3Rotation = rot;

	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
}

void Transform::SetRotation(const Vector4& rot)
{
	m_f3Rotation = rot;

	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
}

void Transform::SetRotation(const float x, const float y, const float z)
{ 
	m_f3Rotation.x = x;
	m_f3Rotation.y = y;
	m_f3Rotation.z = z;
	
	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
}

void Transform::SetScale(const Vector3& scale) { 
	m_f3Scale = scale; 

	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
}

void Transform::SetScale(const Vector4& scale) { m_f3Scale = scale; }
void Transform::SetScale(const float x, const float y, const float z)
{
	m_f3Scale = Vector3(x, y, z);

	NotifyDirty();
	m_fcFlags.SetFlag(LOCAL_DIRTY, true);
}
//void Transform::SetXScale(const float x) { m_f3Scale.x = x; }
//void Transform::SetYScale(const float y) { m_f3Scale.y = y; }
//void Transform::SetZScale(const float z) { m_f3Scale.z = z; }