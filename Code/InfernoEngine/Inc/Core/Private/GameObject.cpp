#include "GameObject.h"
#include "Components\Transform.h"

#include "..\..\Interface\Public\ModuleInterface.h"

// Initialize our Components vector to initally have a transform and then store a reference in a public constant
GameObject::GameObject() : m_vComponents(1, new Transform(this)), transform((Transform*)m_vComponents[0]), m_fcFlags(TOTAL_FLAGS)
{
	m_vComponents.reserve(12);

	m_uiLayer = 0;
	m_uiPrefabID = 0;
	m_fcFlags.SetFlag(IS_ENABLED, true);
	m_fcFlags.SetFlag((unsigned int)GAMEOBJECT_FLAGS::ADDEDFORONENABLE, false);

	InfernoEngine_ObjectManagement::RegisterGameObject(this);
}

// Initialize our Components vector to initally have a transform and then store a reference in a public constant
GameObject::GameObject(const Vector3& Position, const Vector3& Rotation, const Vector3& Scale)
	: m_vComponents(1, new Transform(this, Position, Rotation, Scale)), transform((Transform*)m_vComponents[0]), m_fcFlags(TOTAL_FLAGS)
{
	m_vComponents.reserve(12);

	m_uiLayer = 0;
	m_uiPrefabID = 0;
	m_fcFlags.SetFlag(IS_ENABLED, true);
	m_fcFlags.SetFlag((unsigned int)GAMEOBJECT_FLAGS::ADDEDFORONENABLE, false);

	InfernoEngine_ObjectManagement::RegisterGameObject(this);
}

// Initialize our Components vector to initally have a transform and then store a reference in a public constant
GameObject::GameObject(const Vector3& Position, const Quaternion& Rotation, const Vector3& Scale)
	: m_vComponents(1, new Transform(this, Position, Rotation, Scale)), transform((Transform*)m_vComponents[0]), m_fcFlags(TOTAL_FLAGS)
{
	m_vComponents.reserve(12);

	m_uiLayer = 0;
	m_uiPrefabID = 0;
	m_fcFlags.SetFlag(IS_ENABLED, true);
	m_fcFlags.SetFlag((unsigned int)GAMEOBJECT_FLAGS::ADDEDFORONENABLE, false);

	InfernoEngine_ObjectManagement::RegisterGameObject(this);
}

// Initialize our Components vector to initally have a transform and then store a reference in a public constant
GameObject::GameObject(Transform& _transform)
	: m_vComponents(1, new Transform(this, _transform.GetPosition(), _transform.GetRotation(), _transform.GetScale())), transform((Transform*)m_vComponents[0]), m_fcFlags(TOTAL_FLAGS)
{
	m_vComponents.reserve(12);

	m_uiLayer = 0;
	m_uiPrefabID = 0;
	m_fcFlags.SetFlag(IS_ENABLED, true);
	m_fcFlags.SetFlag((unsigned int)GAMEOBJECT_FLAGS::ADDEDFORONENABLE, false);

	InfernoEngine_ObjectManagement::RegisterGameObject(this);
}

GameObject::GameObject(const GameObject& _copy)
	: m_vComponents(1, new Transform(this)), transform((Transform*)m_vComponents[0]), m_fcFlags(TOTAL_FLAGS)
{
	m_vComponents.reserve(12);

	// === Copy over all Variables
	m_sName = _copy.m_sName;
	m_sTag = _copy.m_sTag;
	m_uiLayer = _copy.m_uiLayer;
	m_uiPrefabID = _copy.m_uiPrefabID;
	m_fcFlags = _copy.m_fcFlags;

	// === Copy over the Transform
	*((Transform*)m_vComponents[0]) = *_copy.transform;

	// === Copy over all Components
	size_t count = _copy.m_vComponents.size();
	for (size_t i = 1; i < count; ++i) {
		AddComponent(*(_copy.m_vComponents[i]));
	}

	// === Copy all the Children
	GameObject* child;
	count = _copy.transform->m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		child = new GameObject(*(_copy.transform->m_vChildren[i]->gameObject));
		child->transform->SetParent(transform);
	}

	InfernoEngine_ObjectManagement::RegisterGameObject(this);
}

// Cleans up all components and children who are attached to the Game Object
GameObject::~GameObject()
{
	std::swap(m_vComponents[0], m_vComponents[m_vComponents.size() - 1]);

	for (size_t component = 0; component < m_vComponents.size(); ++component)
		delete m_vComponents[component];

	m_vComponents.clear();
}

GameObject& GameObject::operator=(const GameObject& _rhs)
{
	// === Set the Variables
	m_sName = _rhs.m_sName;
	m_sTag = _rhs.m_sTag;
	m_uiLayer = _rhs.m_uiLayer;
	m_uiPrefabID = _rhs.m_uiPrefabID;
	for (unsigned int i = 0; i < TOTAL_FLAGS; ++i) {
		m_fcFlags.SetFlag(i, (unsigned int)_rhs.m_fcFlags.GetFlag(i));
	}

	m_fcFlags.SetFlag((unsigned int)GAMEOBJECT_FLAGS::ADDEDFORONENABLE, false);

	// === Set all the Components
	size_t count = m_vComponents.size();

	if (count < 12)
		m_vComponents.reserve(12);
	else
		m_vComponents.reserve(count);

	for (size_t i = 0; i < count; ++i) {
		(*m_vComponents[i]) = (*_rhs.m_vComponents[i]);
	}

	return *this;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void GameObject::Init()
{
	// === Initialize all Components
	size_t count = m_vComponents.size();
	for (size_t i = 0; i < count; ++i) {
		m_vComponents[i]->Init();
	}

	// TEMP (Move to Post Init after PostInit and Init functions have been swapped)
	m_fcFlags.SetFlag((unsigned int)GAMEOBJECT_FLAGS::POSTINIT_CALLED, true);
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void GameObject::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;
	Property* nameProperty = data->GetProperty(std::string("Name"));
	SetName(nameProperty->value.c_str());
	Property* layerProperty = data->GetProperty(std::string("Layer"));
	SetLayer(atoi(layerProperty->value.c_str()));
	Property* tagProperty = data->GetProperty(std::string("Tag"));
	SetTag(tagProperty->value.c_str());
	Property* staticProperty = data->GetProperty(std::string("IsStatic"));
	SetIsStatic(staticProperty ? atoi(staticProperty->value.c_str()) != 0 : false);
}

void GameObject::OnEnable()
{
	// === Enable all Components
	size_t count = m_vComponents.size();
	for (size_t i = 1; i < count; ++i) {
		m_vComponents[i]->OnEnable();
	}

	// === Enable the Transform last
	transform->OnEnable();

	m_fcFlags.SetFlag((unsigned int)GAMEOBJECT_FLAGS::ACTIVE, (unsigned int)true);
	m_fcFlags.SetFlag((unsigned int)GAMEOBJECT_FLAGS::ADDEDFORONENABLE, (unsigned int)false);
}

void GameObject::OnDisable()
{
	size_t count = m_vComponents.size();
	for (size_t i = 0; i < count; ++i) {
		m_vComponents[i]->OnDisable();
	}
	m_fcFlags.SetFlag((int)GAMEOBJECT_FLAGS::ACTIVE, false);
}


// IN: void
// OUT: void
//
// Updates all of the game object's components
void GameObject::Update()
{
	if (!m_fcFlags.GetFlag((unsigned int)GAMEOBJECT_FLAGS::POSTINIT_CALLED)) {
		Init();
	}

	for (size_t i = 0; i < m_vComponents.size(); i++)
		m_vComponents[i]->Update();

	for (unsigned int object = 0; object < transform->GetChildCount(); object++)
		transform->GetChild(object)->gameObject->Update();
}

// IN: void
// OUT: void
//
// Updates all of the game object's components
void GameObject::FixedUpdate()
{
	for (size_t i = 0; i < m_vComponents.size(); i++)
		m_vComponents[i]->FixedUpdate();

	for (unsigned int object = 0; object < transform->GetChildCount(); object++)
		transform->GetChild(object)->gameObject->FixedUpdate();
}

// IN: void
// OUT: void
//
// Updates all of the game object's components
void GameObject::LateUpdate()
{
	for (size_t i = 0; i < m_vComponents.size(); i++)
		m_vComponents[i]->LateUpdate();

	for (unsigned int object = 0; object < transform->GetChildCount(); object++)
		transform->GetChild(object)->gameObject->LateUpdate();
}

// IN: void
// OUT: void
//
// Calls the components OnDrawGizmos function
void GameObject::OnDrawGizmos()
{
	for (size_t i = 0; i < m_vComponents.size(); i++)
		m_vComponents[i]->OnDrawGizmos();

	for (unsigned int object = 0; object < transform->GetChildCount(); object++)
		transform->GetChild(object)->gameObject->OnDrawGizmos();
}

// IN: Collider - pointer to the collider that collided with this object
// OUT: void
//
// Called on the first frame that collision has been detected
void GameObject::OnCollisionEnter(Collider* collider){
	for (unsigned int i = 0; i < m_vComponents.size(); ++i){
		m_vComponents[i]->OnCollisionEnter(collider);
	}
}

// IN: Collider - pointer to the collider to the collider that is still colliding with this object
// OUT: void
//
// Called on each sequential frame that collision is detected
void GameObject::OnCollisionStay(Collider* collider){
	for (unsigned int i = 0; i < m_vComponents.size(); ++i){
		m_vComponents[i]->OnCollisionStay(collider);
	}
}

// IN: Collider - pointer to collider that is no longer colliding with this object
// OUT: void
//
// Called on first frame that collision is no longer detected
void GameObject::OnCollisionExit(Collider* collider){
	for (unsigned int i = 0; i < m_vComponents.size(); ++i){
		m_vComponents[i]->OnCollisionExit(collider);
	}
}

// IN: Collider - pointer to collider that entered the trigger
// OUT: void
//
// Called on the first frame that collision with a trigger is detected
void GameObject::OnTriggerEnter(Collider* collider){
	for (unsigned int i = 0; i < m_vComponents.size(); ++i){
		m_vComponents[i]->OnTriggerEnter(collider);
	}
}

// IN: Collider - pointer to collider that still inside of the trigger
// OUT: void
//
// Called each sequential frame after collision with a trigger is detected
void GameObject::OnTriggerStay(Collider* collider){
	for (unsigned int i = 0; i < m_vComponents.size(); ++i){
		m_vComponents[i]->OnTriggerStay(collider);
	}
}

// IN: Collider - pointer to collider that exited trigger
// OUT: void
//
// Called when collision is no longer detected with a trigger
void GameObject::OnTriggerExit(Collider* collider){
	for (unsigned int i = 0; i < m_vComponents.size(); ++i){
		m_vComponents[i]->OnTriggerExit(collider);
	}
}

void GameObject::OnDestroy()
{
	for (unsigned int i = 0; i < m_vComponents.size(); ++i){
		m_vComponents[i]->OnDestroy();
	}
}

// IN: string - String to compare
// OUT: bool - true if the tags match
//
// Compares the tag to the incoming string
bool GameObject::CompareTag(const char* tag) const
{
	return strcmp(tag, m_sTag.c_str()) == 0;
}
// IN: string - String to compare
// OUT: bool - true if the tags match
//
// Compares the tag to the incoming string
bool GameObject::CompareTag(const std::string tag) const
{
	return tag == m_sTag;
}

// IN: void
// OUT: bool - true if all parents are active as well as this
//
// Returns if the object is considered active in the hierarchy
bool GameObject::ActiveInHierarchy() const
{
	if (transform->m_pParent == nullptr)
		return IsActive();
	else
		return (IsActive() && transform->m_pParent->gameObject->ActiveInHierarchy());
}

// IN: void
// OUT: bool - true if object is active
//
// Returns if the object is active
bool GameObject::IsActive() const { return m_fcFlags.GetFlag((int)GAMEOBJECT_FLAGS::ACTIVE); }
// IN: void
// OUT: unsigned int - Physics layer the object is on
//
// Returns the objects layer
unsigned int GameObject::GetLayer() const { return m_uiLayer; }
// IN: void
// OUT: string - name of the object
//
// Returns the objects name
std::string GameObject::GetName() const { return m_sName; }
// IN: void
// OUT: string - tag of the object
//
// Returns the objects tag
std::string GameObject::GetTag() const { return m_sTag; }

unsigned int GameObject::GetPrefabID() const { return m_uiPrefabID; }

bool GameObject::IsStatic() const{ return m_fcFlags.GetFlag((unsigned int)GAMEOBJECT_FLAGS::IsStatic); }

// IN: bool - Whether or not to make the GameObject Active
//     bool - Whether or not to have the call happen instantly, or next frame
// OUT: void
//
// Sets the GameObject to active or not, with the choice of having it happen next frame, only for Disabling the gameobject. Enabling the GameObject
// always happens next frame.
void GameObject::SetActive(bool _active, bool _nextFrame)
{ 
	// === Was it already added to be enabled?
	if (_active && m_fcFlags.GetFlag((unsigned int)GAMEOBJECT_FLAGS::ADDEDFORONENABLE)) {
		return;
	}

	// === Handle OnEnable / OnDisable
	if (m_fcFlags.GetFlag((unsigned int)GAMEOBJECT_FLAGS::ACTIVE) != _active || _nextFrame) {
		if (_active) {
			InfernoEngine_ObjectManagement::AddForOnEnable(this);
			m_fcFlags.SetFlag((unsigned int)GAMEOBJECT_FLAGS::ADDEDFORONENABLE, (unsigned int)true);
		}
		else {
			if (_nextFrame) {
				InfernoEngine_ObjectManagement::AddForOnDisable(this);
			}
			else {
				OnDisable();
			}
		}
	} 
}
void GameObject::SetLayer(unsigned int layer) { m_uiLayer = layer; }
void GameObject::SetName(std::string name) { m_sName = name; }
void GameObject::SetName(const char* name) { m_sName = name; }
void GameObject::SetTag(std::string tag) { m_sTag = tag; }
void GameObject::SetTag(const char* tag) { m_sTag = tag; }
void GameObject::SetPrefabID(unsigned int _id) { m_uiPrefabID = _id; }
void GameObject::SetIsStatic(bool isStatic){ m_fcFlags.SetFlag((unsigned int)GAMEOBJECT_FLAGS::IsStatic, isStatic ? 1 : 0); }


// IN: string - name of the component to add
// OUT: void
//
// Adds a default version of the component based on incoming string
IComponent* GameObject::AddComponent(std::string& component)
{
	IComponent* comp = InfernoEngine_ObjectManagement::CreateComponent(component, this, transform);
	m_vComponents.push_back(comp);
	return comp;
}