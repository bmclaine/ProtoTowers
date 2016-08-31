// Author: Jonathan Gyurkovics
//
// Base Renderer that all other renderers will derive from.

#pragma once

#include "IComponent.h"
#include "ShaderInfo.h"
#include "Color.h"

#include <vector>

class Mesh;
struct Material;
struct Sphere;

enum RendererType
{
	SPRITE_RENDERER,
	MESH_RENDERER,
	SKINNED_MESH_RENDERER,
	PARTICLE_RENDERER,
};

class Renderer : public IComponent
{
protected:
	int m_iInputLayout;
	TopologyType m_ttTopology;
	int m_iVertexBuffer;
	unsigned int m_uiStride;
	int m_iIndexBuffer;
	int m_iVertexShader;
	int m_iGeometryShader;
	int m_iDepthStencilState;
	float m_fDistFromCamera;
	Color m_cColor;
	bool m_bAlphaBlending;

	Renderer(GameObject* const gameObject, Transform* const transform);
	virtual ~Renderer() override = 0;

	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override = 0;

	virtual Mesh* GetMesh() const = 0;

	virtual int GetNumVerts() = 0;
	virtual int GetStartVertex() = 0;

	// IN: void
	// OUT: void
	//
	// Sets up anything needed for drawling from the renderer side
	virtual void PreDraw() = 0;

	// IN: void
	// OUT: void
	//
	// Calls the draw function without changing the pipeline for doing the depth pre-pass
	virtual void DepthPrePassDraw() = 0;

	// IN: void
	// OUT: void
	//
	// Calls the overloaded draw call for the current renderer
	virtual void Draw() = 0;

public:
	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init();

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	// IN: Color - color to use
	// OUT: void
	//
	// Sets the objects color to the incoming color
	void SetColor(Color newColor);

	// IN: void
	// OUT: Material* - first material pointer
	//
	// Returns the first attached material
	virtual Material* GetMaterial() const = 0;
	// IN: void
	// OUT: std::vector<Material*>& - refrence to the list of materials
	//
	// Returns the list of materials attached to the renderer
	virtual std::vector<Material*> const& GetMaterials() const = 0;

	// IN: void
	// OUT: int - ID of the effect being used
	//
	// Returns the ID of the effect being used
	virtual int GetEffectID() const = 0;
	bool IsBlendingRenderer() const;

	virtual Sphere GetBoundingSphere() const = 0;

	virtual RendererType GetType() const = 0;

	friend class GraphicsManager;
	friend bool MergeCompare(const Renderer*, const Renderer*);

	bool IsStatic() const;
};