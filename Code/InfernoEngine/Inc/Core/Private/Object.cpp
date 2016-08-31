#include "Object.h"

#include <rpc.h>

// IN: void
// OUT: unsigned int - unique instance id of the object
//
// Returns the unique instance id of the object
unsigned int Object::GetInstanceID() const{
	return m_uiInstanceID;
}

void Object::SetInstanceID(unsigned int id){
	m_uiInstanceID = id;
}