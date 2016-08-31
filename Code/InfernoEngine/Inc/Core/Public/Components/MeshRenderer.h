#pragma once

#include "Renderer.h"

#include "GameObject.h"

class MeshRenderer : public Renderer
{
protected:
	Mesh* m_pMesh;
	std::vector<Material*> m_vMaterials;

	virtual ~MeshRenderer() override;

	virtual IComponent& operator=(IComponent& _rhs) override;
	MeshRenderer& operator=(const MeshRenderer& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

	// IN: void
	// OUT: void
	//
	// Sets the defualt pipeline information this renderer will need
	virtual void SetDefaults();

	virtual Mesh* GetMesh() const;
	virtual RendererType GetType() const;

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

public:
	MeshRenderer(GameObject* const gameObject, Transform* const transform, bool skinned = false);

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
	virtual void OnEnable() override;
	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
	virtual void OnDisable() override;

	// IN: void
	// OUT: Material* - first material pointer
	//
	// Returns the first attached material
	virtual Material* GetMaterial() const;
	// IN: void
	// OUT: std::vector<Material*>& - refrence to the list of materials
	//
	// Returns the list of materials attached to the renderer
	virtual std::vector<Material*> const& GetMaterials() const override;

	// IN: void
	// OUT: int - ID of the effect being used
	//
	// Returns the ID of the effect being used
	virtual int GetEffectID() const override;

	// IN: Mesh* - pointer to the mesh to render
	// OUT: void
	//
	// Sets the mesh to use for rendering
	virtual void SetMesh(Mesh* mesh);

	virtual Sphere GetBoundingSphere() const;

	friend class GraphicsManager;
	friend MeshRenderer* GameObject::AddComponent();
};