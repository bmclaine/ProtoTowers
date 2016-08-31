#include "UIComponents\RectTransform.h"

#include "UIElement.h"

// ===== Constructor / Destructor ===== //
RectTransform::RectTransform(UIElement* _owner) : UIComponent(_owner), m_fcTransformFlags((unsigned int)RT_MAX_FLAGS)
{
	m_m4WorldMatrix = m_m4LocalMatrix = Matrix4::Identity();
	m_v2CanvasScale = Vector2(1.0f, 1.0f);
	m_fWidth = m_fHeight = 100.0f;
	m_pParent = nullptr;
}

RectTransform::~RectTransform()
{
	//size_t count = m_vChildren.size();
	for (size_t i = 0; i < m_vChildren.size(); ++i) {
		delete m_vChildren[i]->m_pUIElement;
	}
}
// =================================== //

// === Interface === //
void RectTransform::PostInit()
{
	size_t count = m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		m_vChildren[i]->m_pUIElement->PostInit();
	}

	m_fcTransformFlags.SetFlag((unsigned int)RT_FLAG_POSTINIT_CALLED, (unsigned int)true);
}

void RectTransform::OnEnable()
{
	size_t count = m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		m_vChildren[i]->m_pUIElement->OnEnable();
	}
}

void RectTransform::OnDisable()
{
	size_t count = m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		m_vChildren[i]->m_pUIElement->OnDisable();
	}
}

// === Translate
//  IN : Vector2 - The units to translate, in their appropiate axis'
//  OUT : void
//
//  Translates the RectTransform the specified units in its Right and Up directions.
void RectTransform::Translate(Vector2 _translation)
{
	if (m_fcTransformFlags.GetFlag((unsigned int)RT_FLAG_LOCKED)) {
		return;
	}

	Vector3 translation = (m_m4LocalMatrix.Right() * _translation.x) + (m_m4LocalMatrix.Up() * _translation.y);
	m_m4LocalMatrix._41 += translation.x;
	m_m4LocalMatrix._42 += translation.y;
	SetDirty();
}

// === Rotate 
//  IN : float - The angle to rotate the RectTransform.
//  OUT : void
//
//  Rotates the RectTransform by a given angle, ontop of its current rotation.
void RectTransform::Rotate(float _angle)
{
	if (m_fcTransformFlags.GetFlag((unsigned int)RT_FLAG_LOCKED)) {
		return;
	}

	m_m4LocalMatrix.RotateAround(GetPosition(), Vector3(0.0f, 0.0f, 0.0f), _angle);
}

// === ContainsPoint
//  IN : Vector2 - 2D position to check against.
//  OUT : bool - Whether or not the given point is within this RectTransform's bounds.
//
//  Checks to see if the given point lies within the bounds of this RectTransform and returns true if it does. False otherwise. 
//  Takes into account for the scale of the Transform.
bool RectTransform::ContainsPoint(Vector2 _point)
{
	Vector2 scale = GetScale(); scale.x *= m_v2CanvasScale.x; scale.y *= m_v2CanvasScale.y;
	Vector2 topLeft, bottomRight;
	topLeft = GetScreenPosition(); topLeft.x *= scale.x; topLeft.y *= scale.y;
	bottomRight = topLeft;
	topLeft.x -= m_fWidth * scale.x; topLeft.y -= m_fHeight * scale.y;
	bottomRight.x += m_fWidth * scale.x; bottomRight.y += m_fHeight * scale.y;

	if (_point.x > topLeft.x && _point.x < bottomRight.x
		&& _point.y > topLeft.y && _point.y < bottomRight.y) {
		return true;
	}

	return false;
}

// === SetParent
//  IN : RectTransform* - Pointer to the desired Parent Transform
//  OUT : void
//
//  Sets the Parent of this transform, properly detaching it from any previous existing parent.
void RectTransform::SetParent(RectTransform* _parent)
{
	if (m_pParent != nullptr) {
		m_pParent->DetachChild(this);
	}

	SetDirty();

	m_pParent = _parent;
	if (_parent == nullptr) {
		m_pUIElement->m_sCanvasName = "";
		return;
	}

	m_pParent->m_vChildren.push_back(this);
	m_v2CanvasScale = _parent->m_v2CanvasScale;

	if (m_pParent->m_pParent == nullptr) {
		SetCanvasName(m_pParent->m_pUIElement->GetName());
	}
	else {
		SetCanvasName(m_pParent->m_pUIElement->GetCanvasName());
	}

	size_t count = m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		// === Set the Values of the Children
		m_vChildren[i]->SetCanvasScale(m_v2CanvasScale);
	}

	if (_parent->m_fcTransformFlags.GetFlag(RT_FLAG_POSTINIT_CALLED)) {
		m_pUIElement->PostInit();
	}
}

// === DetachChild
//  IN : RectTransform* - Pointer to the child Transform
//  OUT : void
//
//  Properly detaches the given child, if it was found. Resets any settings that may have been inherited from this parent.
void RectTransform::DetachChild(RectTransform* _child)
{
	std::vector<RectTransform*>::iterator iter = m_vChildren.begin();
	while (iter != m_vChildren.end()) {
		if (*iter == _child) {
			m_vChildren.erase(iter);
			break;
		}
		++iter;
	}

	_child->m_pParent = nullptr;
	_child->m_v2CanvasScale = Vector2(1.0f, 1.0f);
}
// ================= //

// === Accessors === //

// === GetAnchors
//  IN : void
//  OUT : Anchors - Anchor Points of the RectTransform
//  
//  Returns the Anchors that this RectTransform uses compared to it's parent's bounds
Anchors RectTransform::GetAnchors() const
{
	return m_Anchors;
}

// === GetBounds
//  IN : void
//  OUT : Vector2 - Bounds of the RectTransform. Width(x) and Height(y).
//
//  Returns the radius width and height of this RectTransform and takes into account for the scale of the Transform.
Vector2 RectTransform::GetBounds() const
{
	return Vector2(m_fWidth, m_fHeight);
}

// === GetTotalBounds
//  IN : void
//  OUT : Vector2 - Bounds of the RectTransform. Width(x) and Height(y).
//
//  Returns the total width and height of this RectTransform and takes into account for the scale of the Transform.
Vector2 RectTransform::GetTotalBounds() const
{
	return Vector2(m_fWidth * 2, m_fHeight * 2);
}

// === GetParent 
//  IN : void
//  OUT : RectTransform* - Parent of this RectTransform.
//
//  Returns the Parent of this RectTransform, if there is one, Nullptr otherwise.
RectTransform* RectTransform::GetParent() const
{
	return m_pParent;
}

// === GetPosition
//  IN : void
//  OUT : Vector2 - Position of the RectTransform.
//
//  Returns the local position of the RectTransform, relative to it's anchors.
Vector2 RectTransform::GetPosition() const
{
	return Vector2(m_m4LocalMatrix._41, m_m4LocalMatrix._42);
}

// === GetRotation
//  IN : void
//  OUT : float - The rotation of the RectTransform
//
//  Returns the Rotation of the RectTransform, in degrees.
float RectTransform::GetRotation() const
{
	//Quaternion rotation;
	//m_m4LocalMatrix.DecomposeRotation(rotation);
	return m_fZRotation;// rotation.GetEulerAngles().z;
}

// === GetScale
//  IN : void
//  OUT : Vector2 - Scale of the RectTransform.
//
//  Returns the Scale of the RectTransform.
Vector2 RectTransform::GetScale()
{
	return Vector2(m_m4LocalMatrix._11, m_m4LocalMatrix._22);
}

// === GetScale
//  IN : void
//  OUT : Vector2 - Scale of the RectTransform.
//
//  Returns the Scale of the RectTransform.
Vector2 RectTransform::GetScale() const
{
	return Vector2(m_m4LocalMatrix._11, m_m4LocalMatrix._22);
}

// === GetCanvasScale
//  IN : void
//  OUT : Vector2 - Scale of the Canvas the RectTransform is attached to
//
// Returns the Scale of the Canvas the RectTransform is attached to
Vector2 RectTransform::GetCanvasScale() const
{
	return m_v2CanvasScale;
}

// === GetScreenPosition
//  IN : void
//  OUT : Vector2 - Screen Position of the RectTransform.
//
//  Returns the Screen Position of the RectTransform, taking into account for its parent and anchors.
Vector2 RectTransform::GetScreenPosition()
{
	return Vector2(GetWorldMatrix()._41, GetWorldMatrix()._42);
}

// === GetScreenRectangle
//  IN : void
//  OUT : Vector4 - The 4 bounds of the Rectangle in screen space. Format : { Top, Left, Bottom, Right }
//
//  Returns the bounds of the UIRenderer's rectangle in screen space.
Vector4 RectTransform::GetScreenRectangle()
{
	Vector2 position = GetScreenPosition();
	Vector2 bounds = GetBounds();
	Vector2 scale = GetScale();
	Vector4 rectangle = { (position.y - (bounds.y * scale.y)), (position.x - (bounds.x * scale.x)), (position.y + (bounds.y * scale.y)), (position.x + (bounds.x * scale.x))};
	return rectangle;
}

// === GetWorldMatrix
//  IN : void
//  OUT : Matrix4 - WorldMatrix of the RectTransform.
//
//  Returns the WorldMatrix of the RectTransform, with the proper screen position calculated.
Matrix4 RectTransform::GetWorldMatrix()
{
	if (m_fcTransformFlags.GetFlag(RT_FLAG_WORLDDIRTY)) {
		m_fcTransformFlags.SetFlag(RT_FLAG_WORLDDIRTY, false);

		if (m_pParent != nullptr) {
			Matrix4 parentWorldMatrix = m_pParent->GetWorldMatrix();
			m_m4WorldMatrix = m_m4LocalMatrix * parentWorldMatrix;
			m_m4WorldMatrix._41 = m_m4LocalMatrix._41 + (parentWorldMatrix._41 + (m_Anchors.AnchorLeft * m_pParent->m_fWidth));
			m_m4WorldMatrix._42 = m_m4LocalMatrix._42 + (parentWorldMatrix._42 + (m_Anchors.AnchorTop * m_pParent->m_fHeight));
		}
		else {
			m_m4WorldMatrix = m_m4LocalMatrix;
		}
	}

	return m_m4WorldMatrix;
}

// === GetChildByName
//  IN : string - Name of the desired child
//       bool - (Optional) Whether or not to search through the children's children as well
//  OUT : UIElement* - Pointer to the Child with the desired Name
//
//  Searches through all the children of this RectTransform, and it's children's children, if specified, and returns the UIElement with the
//  desired name, if it was found. Returns nullptr if no element was found with the desired name.
UIElement* RectTransform::GetChildByName(std::string& _elementName, bool _fullSearch)
{
	// === Search through it's children first
	size_t count = m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vChildren[i]->m_pUIElement->GetName() == _elementName) {
			return m_vChildren[i]->m_pUIElement;
		}
	}

	if (_fullSearch) {
		// === Search through it's children children
		UIElement* foundChild = nullptr;
		for (size_t i = 0; i < count; ++i) {
			foundChild = m_vChildren[i]->GetChildByName(_elementName, true);
			if (foundChild != nullptr) {
				return foundChild;
			}
		}
	}
	
	return nullptr;
}
// ================= //

// === Mutators === //

// === Lock
//  IN : bool - Whether or not to set the Transform as Locked.
//  OUT : void
//
//  Locks or Unlocks the Transform based of the given bool. When a Transform is locked, it will disregard any changes to it's local
//  matrix.
void RectTransform::Lock(bool _value)
{
	m_fcTransformFlags.SetFlag((unsigned int)RT_FLAG_LOCKED, (unsigned int)_value);
}

// === SetAnchors
//  IN : Anchors - The Anchor Points for the RectTransform to use.
//  OUT : void
//
//  Sets the new anchor points for the RectTransform to use, along with updating it's local position to be relative to the new anchors.
void RectTransform::SetAnchors(Anchors _anchors)
{
	if (m_fcTransformFlags.GetFlag((unsigned int)RT_FLAG_LOCKED)) {
		return;
	}

	m_Anchors = _anchors;
	if (m_pParent != nullptr) {
		m_m4LocalMatrix._41 = GetScreenPosition().x - (m_pParent->GetScreenPosition().x + (m_Anchors.AnchorLeft * m_pParent->m_fWidth));
		m_m4LocalMatrix._42 = GetScreenPosition().y - (m_pParent->GetScreenPosition().y + (m_Anchors.AnchorTop * m_pParent->m_fHeight));
		SetDirty();
	}
}

// === SetBounds
//  IN : Vector2 - The width(x) and height(y) of the RectTransform.
//  OUT : void
//
//  Sets the Bounds of the RectTransform. Takes the total width and height and divides them by two before storing, as the 
//  RectTransform uses the width and height as a radius.
void RectTransform::SetBounds(Vector2 _bounds)
{
	if (m_fcTransformFlags.GetFlag((unsigned int)RT_FLAG_LOCKED)) {
		return;
	}

	m_fWidth = _bounds.x / 2.0f;
	m_fHeight = _bounds.y / 2.0f;
	SetDirty();
}

// === SetPosition
//  IN : Vector2 - The new Position.
//  OUT : void
//
//  Sets the Position of the RectTransform in 2D space, relative to it's anchor points.
void RectTransform::SetPosition(Vector2 _position)
{
	if (m_fcTransformFlags.GetFlag((unsigned int)RT_FLAG_LOCKED)) {
		return;
	}

	m_m4LocalMatrix._41 = _position.x;
	m_m4LocalMatrix._42 = _position.y;
	SetDirty();
}

// === SetPosition
//  IN : float - The new X Position.
//       float - The new Y Position.
//  OUT : void
//
//  Sets the Position of the RectTransform in 2D space, relative to it's anchor points.
void RectTransform::SetPosition(float _xPos, float _yPos)
{
	if (m_fcTransformFlags.GetFlag((unsigned int)RT_FLAG_LOCKED)) {
		return;
	}

	m_m4LocalMatrix._41 = _xPos;
	m_m4LocalMatrix._42 = _yPos;
	SetDirty();
}

// === SetXPosition
//  IN : float - The new X Position.
//  OUT : void
//
//  Sets the X Position of the RectTransform in 2D space, relative to it's anchor points.
void RectTransform::SetXPosition(float _xPos)
{
	if (m_fcTransformFlags.GetFlag((unsigned int)RT_FLAG_LOCKED)) {
		return;
	}

	m_m4LocalMatrix._41 = _xPos;
	SetDirty();
}

// === SetYPosition
//  IN : float - The new Y Position.
//  OUT : void
//
//  Sets the Y Position of the RectTransform in 2D space, relative to it's anchor points.
void RectTransform::SetYPosition(float _yPos)
{
	if (m_fcTransformFlags.GetFlag((unsigned int)RT_FLAG_LOCKED)) {
		return;
	}

	m_m4LocalMatrix._42 = _yPos;
	SetDirty();
}

// === SetRotation
//  IN : float - The rotation in degrees.
//  OUT : void
// 
//  Sets the Rotation of the RectTransform.
void RectTransform::SetRotation(float _rotation)
{
	if (m_fcTransformFlags.GetFlag((unsigned int)RT_FLAG_LOCKED)) {
		return;
	}

	m_fZRotation =  _rotation;
	//m_m4LocalMatrix.RotateAround(GetPosition(), Vector3(0.0f, 0.0f, 0.0f), _rotation - GetRotation());
}

// === SetScale
//  IN : Vector2 - The new Scale of the RectTransform.
//  OUT : void
//
//  Sets the Scale of the RectTransform.
void RectTransform::SetScale(Vector2 _scale)
{
	if (m_fcTransformFlags.GetFlag((unsigned int)RT_FLAG_LOCKED)) {
		return;
	}

	m_m4LocalMatrix._11 = _scale.x;
	m_m4LocalMatrix._22 = _scale.y;
}

// === SetCanvasScale
//  IN : Vector2 - The resolution scale from the Canvas that this RectTransform is a part of.
//  OUT : void
//
//  Sets the Canvas Scale of the this RectTransform.
void RectTransform::SetCanvasScale(Vector2 _canvasScale)
{
	if (m_fcTransformFlags.GetFlag((unsigned int)RT_FLAG_LOCKED)) {
		return;
	}

	m_v2CanvasScale = _canvasScale;

	size_t count = m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		m_vChildren[i]->SetCanvasScale(_canvasScale);
	}
}

// === SetCanvasName
//  IN : string& - Canvas Name
//  OUT : void
//
//  Sets the Canvas Name of this RectTransform, as well setting it's children's canvas' name
void RectTransform::SetCanvasName(std::string& _canvasName)
{
	m_pUIElement->m_sCanvasName = _canvasName;

	for (size_t i = 0; i < m_vChildren.size(); ++i) {
		m_vChildren[i]->SetCanvasName(_canvasName);
	}
}
// ================ //

// === Private Interface === //
void RectTransform::SetDirty()
{
	m_fcTransformFlags.SetFlag(RT_FLAG_WORLDDIRTY, (unsigned int)true);

	size_t count = m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		m_vChildren[i]->SetDirty();
	}
}
// ========================= //