#include "Components\IComponent.h"
#include "GameObject.h"
#include "Components\Transform.h"

IComponent::IComponent(GameObject* const gameObject, Transform* const transform) : gameObject(gameObject), transform(transform), m_fcFlags(TOTAL_FLAGS)
{
}

IComponent::~IComponent()
{
}

IComponent& IComponent::operator=(IComponent& anim)
{
	return *this;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void IComponent::Init()
{

}

// IN: void
// OUT: void
//
// Update that is called every frame
void IComponent::Update() {}
// IN: void
// OUT: void
//
// Update only called every 1/60th of second
void IComponent::FixedUpdate() {}
// IN: void
// OUT: void
//
// Update called only after every other update has been called
void IComponent::LateUpdate() {}

// IN: void
// OUT: void
//
// Draws Gizmos for the component
void IComponent::OnDrawGizmos() {}

// IN: void
// OUT: void
//
// Called on the first frame that collision has been detected
void IComponent::OnCollisionEnter(Collider* col) {}
// IN: void
// OUT: void
//
// Called on each sequential frame that collision is detected
void IComponent::OnCollisionStay(Collider* col) {}
// IN: void
// OUT: void
//
// Called on first frame that collision is no longer detected
void IComponent::OnCollisionExit(Collider* col) {}

// IN: void
// OUT: void
//
// Called on the first frame that collision with a trigger is detected
void IComponent::OnTriggerEnter(Collider* col) {}
// IN: void
// OUT: void
//
// Called each sequential frame after collision with a trigger is detected
void IComponent::OnTriggerStay(Collider* col) {}
// IN: void
// OUT: void
//
// Called when collision is no longer detected with a trigger
void IComponent::OnTriggerExit(Collider* col) {}

// IN: void
// OUT: void
//
// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
void IComponent::OnEnable() { m_fcFlags.SetFlag((unsigned int)COMPONENT_FLAGS::IS_ENABLED, (unsigned int)true); }
// IN: void
// OUT: void
//
// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
void IComponent::OnDisable() { m_fcFlags.SetFlag((unsigned int)COMPONENT_FLAGS::IS_ENABLED, (unsigned int)false); }

void IComponent::OnDestroy() { }

// IN: Sets if the component is enabled or not
// OUT: void
//
// Set the enabled of the component
void IComponent::SetEnabled(bool enabled) 
{ 
	// === Handle OnEnable / OnDisable
	if (m_fcFlags.GetFlag(IS_ENABLED) != enabled) {
		if (enabled) {
			OnEnable();
		}
		else {
			OnDisable();
		}
	}

	m_fcFlags.SetFlag(IS_ENABLED, enabled); 
}

// IN: void
// OUT: bool - enabled of the component
//
// Returns the current status of the enabled of the component
bool IComponent::isEnabled() const { return m_fcFlags.GetFlag(IS_ENABLED); }