///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author:      Doug Berg
//
// Description: Abstract Base Class component for all AI based components.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// Engine includes
#include "Components\AIComponent.h"
#include "AI\AIModule.h"





/*virtual*/ AIComponent::AIComponent(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{
	
}



/*virtual*/ AIComponent::~AIComponent()
{
	
}

