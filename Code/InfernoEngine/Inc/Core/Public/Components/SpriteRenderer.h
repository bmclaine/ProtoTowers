#pragma once

#include "Renderer.h"
#include "MathLib.h"
#include "GameObject.h"

class Texture;

typedef enum SpriteEffects { FILL_EFFECT, FILL_RIGHT_EFFECT, FILL_DUAL_IMAGE_EFFECT, RADIAL_EFFECT, RADIAL_REVERSE_EFFECT, UNLIT_EFFECT };
typedef enum SpriteRadialOrigins { RADIAL_ORIGIN_TOP, RADIAL_ORIGIN_LEFT, RADIAL_ORIGIN_BOTTOM, RADIAL_ORIGIN_RIGHT };

class SpriteRenderer : public Renderer
{
private:
	struct QuadInfo
	{
		Vector2 Dimensions;
		Vector2 Pad;
	};
	struct RadialOriginInfo
	{
		Vector2 TopLeft;
		Vector2 TopRight;
		Vector2 BottomLeft;
		Vector2 BottomRight;
	};

private:
	RadialOriginInfo m_RadialOriginInfo;
	QuadInfo m_QuadInfo;
	Color m_cColor2;
	std::vector<Material*> m_vMaterials;
	Texture* m_pTexture;
	Texture* m_pTexture2;
	SpriteEffects m_eEffect;
	int m_iLayer;
	int m_iEffect;
	int m_iPositionBufferID;
	int m_iQuadDimensionsBufferID;
	int m_iFillRatioBufferID;
	int m_iSecondColorBufferID;
	int m_iRadialInfoBufferID;
	float m_fFill;
	bool m_bDualImage;
	bool m_bBillboarded;

	virtual ~SpriteRenderer() override;

	virtual IComponent& operator=(IComponent&) override;
	SpriteRenderer& operator=(SpriteRenderer& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

	virtual Mesh* GetMesh() const override;

	virtual int GetNumVerts() override;
	virtual int GetStartVertex() override;

	// IN: void
	// OUT: void
	//
	// Sets up anything needed for drawling from the renderer side
	virtual void PreDraw() override;

	// IN: void
	// OUT: void
	//
	// Calls the draw function without changing the pipeline for doing the depth pre-pass
	virtual void DepthPrePassDraw() override;

	// IN: void
	// OUT: void
	//
	// Calls the overloaded draw call for the current renderer
	virtual void Draw() override;


	// IN: void
	// OUT: Material* - first material pointer
	//
	// Returns the first attached material
	virtual Material* GetMaterial() const override;
	// IN: void
	// OUT: std::vector<Material*>& - refrence to the list of materials
	//
	// Returns the list of materials attached to the renderer
	virtual std::vector<Material*> const & GetMaterials() const override;

public:
	SpriteRenderer(GameObject* const gameObject, Transform* const transform);

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init() override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
	virtual void OnEnable();
	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
	virtual void OnDisable();

	// IN: void
	// OUT: int - ID of the effect being used
	//
	// Returns the ID of the effect being used
	virtual int GetEffectID() const override;

	void SetLayer(int layer);
	void SetFillAmount(float fill);
	void SetEffect(SpriteEffects effect);
	// === SetRadialOrigin
	// IN : RadialOrigin - ID of the origin to use
	// OUT: void
	//
	// Sets the origin for radial fill
	void SetRadialOrigin(SpriteRadialOrigins origin);
	// IN: Vector2 - Dimensions of the sprite (In World Space Units)
	// OUT: void
	//
	// Sets the dimensions to the pass in ones.
	void SetDimensions(Vector2 dimensions);
	void SetImage(int ImageID);
	void SetSecondImageID(int ImageID);
	void SetSecondColor(Color color);
	void SetUseBillboard(bool billboard);

	virtual RendererType GetType() const override;

	int GetLayer() const;
	float GetFillAmount() const;
	Vector2 GetDimensions() const;
	int GetImageID() const;
	int GetSecondImageID() const;

	virtual Sphere GetBoundingSphere() const;

	friend class GraphicsManager;
	friend SpriteRenderer* GameObject::AddComponent();
};