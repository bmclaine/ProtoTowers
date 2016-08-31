// Author : Jack Gillette
//
// Derived Child of UIRenderer, handles drawing a simple image onto a quad.
//
// ** THIS CLASS IS CURRENTLY NOT NEEDED, FOR NOW JUST USE "UIRENDERER" **
#pragma once
#include "UIRenderer.h"

#include "Color.h"
#include "Assets\Material.h"

class Texture;

class UIImage : public UIRenderer
{
private:
	// === Variables
	Color m_Color;
	Material* m_pMaterial;

	virtual void SetTexture(Texture*) override;
	virtual Texture* GetTexture() const;

public:
	// === Constructor / Destructor === //
	UIImage(UIElement* _owner);
	~UIImage();
	// ================================ //

	// === Interface === //
	// === PreDraw
	//  IN : void
	//  OUT : void
	//
	//  Handles setting any custom settings / colors that the Graphics Manager may not know about.
	virtual void PreDraw();
	// ================= //

	// === Accessors === //

	// === GetColor
	//  IN : void
	//  OUT : Color - The color offset of the Image
	//
	//  Returns the color offset of the Image.
	Color GetColor() const;
	// === GetTexture
	//  IN : void
	//  OUT : Texture* - Pointer to the Texture
	//
	//  Returns the pointer to the texture that the Image is using.
	Material* GetMaterial() const;
	// ================= //

	// === Mutators === //

	// === SetColor
	//  IN : Color - The color offset to use
	//  OUT : void
	//
	//  Sets the color offset of the image.
	void SetColor(Color _color);
	// === SetTexture
	//  IN : Texture* - A pointer to the Texture to use
	//  OUT : void
	//
	//  Sets the Texture for the Image to use.
	void SetMaterial(Material* _texture);
	// ================ //
};

