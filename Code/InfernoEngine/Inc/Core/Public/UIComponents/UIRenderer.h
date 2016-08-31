// Author : Jack Gillette
//
// Rendering component for UIElements. Friends with GraphicsManager so that when it's time to draw, the GraphicsManager can just
// grab what it needs.
#pragma once
#include"UIComponent.h"
#include "MathLib.h"

#include "Color.h"

class Texture;

typedef enum UIEffects {
	UI_FILL_EFFECT = 1,
	UI_FILL_RIGHT_EFFECT = 2,
	UI_FILL_TOP_EFFECT = 4,
	UI_FILL_RADIAL = 8,
	UI_FILL_RADIAL_CLOCKWISE = 16,
	UI_UNLIT_EFFECT = 32,
};
typedef enum RadialOrigins { ORIGIN_TOP, ORIGIN_LEFT, ORIGIN_BOTTOM, ORIGIN_RIGHT };

class UIRenderer : public UIComponent
{
private:
	struct RadialOriginInfo
	{
		Vector2 TopLeft;
		Vector2 TopRight;
		Vector2 BottomLeft;
		Vector2 BottomRight;
	};

protected:
	// === Variables
	RadialOriginInfo m_RadialOriginInfo;
	Texture* m_pTexture;
	Color m_Color;
	int m_iCanvasDetailsBufferID;
	int m_iFillRatioBufferID;
	int m_iRadialInfoBufferID;
	int m_iRotationInfoBufferID;
	unsigned int m_uiLayer;
	unsigned int m_uiEffectID;
	float m_fImageFill;
	UIEffects m_eEffect;
	bool m_bRegistered;

	friend class GraphicsManager;

public:
	// === Constructor / Destructor === //
	UIRenderer(UIElement* _owner);
	virtual ~UIRenderer() override;
	// ================================ //

	// === Interface === //
	virtual void Init() override;
	virtual void PostInit() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;

	// === PreDraw
	//  IN : void
	//  OUT : void
	//
	//  Handles setting any custom settings / colors that the Graphics Manager may not know about.
	virtual void PreDraw();
	// ================= //

	// === Accessors === //

	// === GetLayer
	//  IN : void
	//  OUT : unsigned int - The layer of the UI. Higher number is in front.
	//
	//  Returns the Layer of the UIRenderer, higher numbers will render in front of lower numbers.
	unsigned int GetLayer() const;
	// === GetColor
	//  IN : void
	//  OUT : Color - The color offset of the Image
	//
	//  Returns the color offset of the Image.
	virtual Color GetColor() const;
	// === GetTexture
	//  IN : void
	//  OUT : Texture* - Pointer to the Texture
	//
	//  Returns the pointer to the texture that the Image is using.
	virtual Texture* GetTexture() const;
	// ================= //

	// === Mutators === //

	// === SetLayer
	//  IN : unsigned int - The layer to put the UI on
	//  OUT : void
	//
	//  Sets the Layer of the UIRenderer. Higher numbers will be drawn in front of lower numbers.
	void SetLayer(unsigned int _layer);
	// === SetImageFill
	//  IN : float - The Fill Ratio of the Image. Clamps between 0 and 1.
	//  OUT : void
	//
	//  Sets the Fill Ratio of this image. If it's the first time that the Fill is being set, it'll set up the UIRenderer to have the correct 
	//  effect on it.
	void SetImageFill(float _fillRatio);
	// === SetEffect
	// IN : UIEffect - ID of the effect to use
	// OUT : void
	//
	// Sets the effect for the UIRenderer to use
	void SetEffect(UIEffects effect);
	// === SetRadialOrigin
	// IN : RadialOrigin - ID of the origin to use
	// OUT: void
	//
	// Sets the origin for radial fill
	void SetRadialOrigin(RadialOrigins origin);
	// === SetFillFromLeft
	//  IN : bool - Whether or not to fill from the left side.
	//  OUT : void
	//
	//  Sets the correct variables for Filling from Left to Right or Right to Left
	//void SetFillFromLeft(bool _true);
	// === SetColor
	//  IN : Color - The color offset to use
	//  OUT : void
	//
	//  Sets the color offset of the image.
	virtual void SetColor(Color _color);
	// === SetTexture
	//  IN : Texture* - A pointer to the Texture to use
	//  OUT : void
	//
	//  Sets the Texture for the Image to use.
	virtual void SetTexture(Texture* _texture);
	// ================ //
};

