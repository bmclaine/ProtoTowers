// Author : Jack Gillette
//
// Transform class for objects that are in 2D space, such as UIElements. Can be anchored to specific location of the canvas that it
// is attached to.

#pragma once

#include "UIComponent.h"

#include "MathLib.h"

#include <vector>

using std::vector;

class UICanvas;

struct Anchors
{
	float AnchorLeft, AnchorTop;
	enum ANCHORS { ANCHOR_TOPLEFT, ANCHOR_CENTERLEFT, ANCHOR_BOTTOMLEFT, ANCHOR_TOPCENTER, ANCHOR_CENTER, ANCHOR_BOTTOMCENTER, ANCHOR_TOPRIGHT, ANCHOR_CENTERRIGHT, ANCHOR_BOTTOMRIGHT };

	// === Anchors (Constructor)
	//  IN : float - The Left Anchor.
	//       float - The Top Anchor.
	//  OUT : void
	//
	//  Sets the Anchors of this RectTransform. Values are the following; 
	//  Complete Left = -1.0, Complete Right = 1.0, Complete Top = 1.0, and Complete Bottom = -1.0.
	Anchors(float _anchorLeft = 0.0f, float _anchorTop = 0.0f) {
		AnchorLeft = Clamp(-1.0f, 1.0f, _anchorLeft);
		AnchorTop = Clamp(-1.0f, 1.0f, _anchorTop);
	}

	// === Anchors (Constructor)
	//  IN : ANCHORS - Predefined type of Anchor to use
	//  OUT : void
	//
	//  Sets the Anchors of this RectTransform, based off a common predefined positions
	Anchors(ANCHORS _anchorType) {
		switch (_anchorType) {
		case ANCHOR_BOTTOMCENTER:
			AnchorLeft = 0.0f; AnchorTop = 1.0f;
			break;
		case ANCHOR_BOTTOMLEFT:
			AnchorLeft = -1.0; AnchorTop = 1.0f;
			break;
		case ANCHOR_BOTTOMRIGHT:
			AnchorLeft = 1.0f; AnchorTop = 1.0f;
			break;
		case ANCHOR_CENTER:
			AnchorLeft = 0.0f; AnchorTop = 0.0f;
			break;
		case ANCHOR_CENTERLEFT:
			AnchorLeft = -1.0f; AnchorTop = 0.0f;
			break;
		case ANCHOR_CENTERRIGHT:
			AnchorLeft = 1.0f; AnchorTop = 0.0f;
			break;
		case ANCHOR_TOPCENTER:
			AnchorLeft = 0.0f; AnchorTop = -1.0f;
			break;
		case ANCHOR_TOPLEFT:
			AnchorLeft = -1.0f; AnchorTop = -1.0f;
			break;
		case ANCHOR_TOPRIGHT:
			AnchorLeft = 1.0f; AnchorTop = -1.0f;
			break;
		default:
			AnchorLeft = 0.0f; AnchorTop = 0.0f;
			break;
		}
	}
};

class RectTransform : public UIComponent
{
private:
	// === Enumerations
	enum RECTTRANSFORM_FLAGS { RT_FLAG_WORLDDIRTY = 0, RT_FLAG_LOCKED, RT_FLAG_POSTINIT_CALLED, RT_MAX_FLAGS };

	// === Variables
	Matrix4 m_m4LocalMatrix;
	Matrix4 m_m4WorldMatrix;
	Anchors m_Anchors;
	Vector2 m_v2CanvasScale;
	Flags<char> m_fcTransformFlags;
	float m_fWidth, m_fHeight;
	float m_fZRotation;

	// === Private Interface === //
	void SetDirty();
	// ========================= //

protected:
	// === Shared Variables
	RectTransform* m_pParent;
	vector<RectTransform*> m_vChildren;

public:
	// === Constructor / Destructor === //
	RectTransform(UIElement* _owner);
	virtual ~RectTransform() override;
	// ================================ //

	// === Interface === //
	virtual void PostInit() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;

	// === Translate
	//  IN : Vector2 - The units to translate, in their appropiate axis'
	//  OUT : void
	//
	//  Translates the RectTransform the specified units in its Right and Up directions.
	void Translate(Vector2 _translation);
	// === Rotate 
	//  IN : float - The angle to rotate the RectTransform.
	//  OUT : void
	//
	//  Rotates the RectTransform by a given angle, ontop of its current rotation.
	void Rotate(float _angle);
	// === ContainsPoint
	//  IN : Vector2 - 2D position to check against.
	//  OUT : bool - Whether or not the given point is within this RectTransform's bounds.
	//
	//  Checks to see if the given point lies within the bounds of this RectTransform and returns true if it does. False otherwise.
	bool ContainsPoint(Vector2 _point);
	// === SetParent
	//  IN : RectTransform* - Pointer to the desired Parent Transform
	//  OUT : void
	//
	//  Sets the Parent of this transform, properly detaching it from any previous existing parent.
	void SetParent(RectTransform* _parent);
	// === DetachChild
	//  IN : RectTransform* - Pointer to the child Transform
	//  OUT : void
	//
	//  Properly detaches the given child, if it was found. Resets any settings that may have been inherited from this parent.
	void DetachChild(RectTransform* _child);
	// ================= //

	// === Accessors === //

	// === GetAnchors
	//  IN : void
	//  OUT : Anchors - Anchor Points of the RectTransform
	//  
	//  Returns the Anchors that this RectTransform uses compared to it's parent's bounds
	Anchors GetAnchors() const;
	// === GetBounds
	//  IN : void
	//  OUT : Vector2 - Bounds of the RectTransform. Width(x) and Height(y).
	//
	//  Returns the radius width and height of this RectTransform.
	Vector2 GetBounds() const;
	// === GetTotalBounds
	//  IN : void
	//  OUT : Vector2 - Bounds of the RectTransform. Width(x) and Height(y).
	//
	//  Returns the total width and height of this RectTransform.
	Vector2 GetTotalBounds() const;
	// === GetParent 
	//  IN : void
	//  OUT : RectTransform* - Parent of this RectTransform.
	//
	//  Returns the Parent of this RectTransform, if there is one, Nullptr otherwise.
	RectTransform* GetParent() const;
	// === GetPosition
	//  IN : void
	//  OUT : Vector2 - Position of the RectTransform.
	//
	//  Returns the local position of the RectTransform, relative to it's anchors.
	Vector2 GetPosition() const;
	// === GetRotation
	//  IN : void
	//  OUT : float - The rotation of the RectTransform
	//
	//  Returns the Rotation of the RectTransform, in degrees.
	float GetRotation() const;
	// === GetScale
	//  IN : void
	//  OUT : Vector2 - Scale of the RectTransform.
	//
	//  Returns the Scale of the RectTransform.
	Vector2 GetScale();
	// === GetScale
	//  IN : void
	//  OUT : Vector2 - Scale of the RectTransform.
	//
	//  Returns the Scale of the RectTransform.
	Vector2 GetScale() const;
	// === GetCanvasScale
	//  IN : void
	//  OUT : Vector2 - Scale of the Canvas the RectTransform is attached to
	//
	// Returns the Scale of the Canvas the RectTransform is attached to
	Vector2 GetCanvasScale() const;
	// === GetScreenPosition
	//  IN : void
	//  OUT : Vector2 - Screen Position of the RectTransform.
	//
	//  Returns the Screen Position of the RectTransform, taking into account for its parent and anchors.
	Vector2 GetScreenPosition();
	// === GetScreenRectangle
	//  IN : void
	//  OUT : Vector4 - The 4 bounds of the Rectangle in screen space. Format : { Top, Left, Bottom, Right }
	//
	//  Returns the bounds of the UIRenderer's rectangle in screen space.
	Vector4 GetScreenRectangle();
	// === GetWorldMatrix
	//  IN : void
	//  OUT : Matrix4 - WorldMatrix of the RectTransform.
	//
	//  Returns the WorldMatrix of the RectTransform, with the proper screen position calculated.
	Matrix4 GetWorldMatrix();
	// === GetChildByName
	//  IN : string - Name of the desired child
	//       bool - (Optional) Whether or not to search through the children's children as well
	//  OUT : UIElement* - Pointer to the Child with the desired Name
	//
	//  Searches through all the children of this RectTransform, and it's children's children, if specified, and returns the UIElement with the
	//  desired name, if it was found. Returns nullptr if no element was found with the desired name.
	UIElement* GetChildByName(std::string& _elementName, bool _fullSearch = false);
	// ================= //

	// === Mutators === //

	// === Lock
	//  IN : bool - Whether or not to set the Transform as Locked.
	//  OUT : void
	//
	//  Locks or Unlocks the Transform based of the given bool. When a Transform is locked, it will disregard any changes to it's local
	//  matrix, bounds, or anchors.
	void Lock(bool _value);
	// === SetAnchors
	//  IN : Anchors - The Anchor Points for the RectTransform to use.
	//  OUT : void
	//
	//  Sets the new anchor points for the RectTransform to use, along with updating it's local position to be relative to the new anchors.
	void SetAnchors(Anchors _anchors);
	// === SetBounds
	//  IN : Vector2 - The width(x) and height(y) of the RectTransform.
	//  OUT : void
	//
	//  Sets the Bounds of the RectTransform. Takes the total width and height and divides them by two before storing, as the 
	//  RectTransform uses the width and height as a radius.
	void SetBounds(Vector2 _bounds);
	// === SetPosition
	//  IN : Vector2 - The new Position.
	//  OUT : void
	//
	//  Sets the Position of the RectTransform in 2D space, relative to it's anchor points.
	void SetPosition(Vector2 _position);
	// === SetPosition
	//  IN : float - The new X Position.
	//       float - The new Y Position.
	//  OUT : void
	//
	//  Sets the Position of the RectTransform in 2D space, relative to it's anchor points.
	void SetPosition(float _xPos, float _yPos);
	// === SetXPosition
	//  IN : float - The new X Position.
	//  OUT : void
	//
	//  Sets the X Position of the RectTransform in 2D space, relative to it's anchor points.
	void SetXPosition(float _xPos);
	// === SetYPosition
	//  IN : float - The new Y Position.
	//  OUT : void
	//
	//  Sets the Y Position of the RectTransform in 2D space, relative to it's anchor points.
	void SetYPosition(float _yPos);
	// === SetRotation
	//  IN : float - The rotation in degrees.
	//  OUT : void
	// 
	//  Sets the Rotation of the RectTransform.
	void SetRotation(float _rotation);
	// === SetScale
	//  IN : Vector2 - The new Scale of the RectTransform.
	//  OUT : void
	//
	//  Sets the Scale of the RectTransform.
	void SetScale(Vector2 _scale);
	// === SetCanvasScale
	//  IN : Vector2 - The resolution scale from the Canvas that this RectTransform is a part of.
	//  OUT : void
	//
	//  Sets the Canvas Scale of the this RectTransform.
	void SetCanvasScale(Vector2 _canvasScale);
	// === SetCanvasName
	//  IN : string& - Canvas Name
	//  OUT : void
	//
	//  Sets the Canvas Name of this RectTransform, as well setting it's children's canvas' name
	void SetCanvasName(std::string& _canvasName);
	// ================ //

	// === Friend Classes
	friend class UIElement;
};

