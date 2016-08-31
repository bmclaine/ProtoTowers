#include "Components\SpriteRenderer.h"
#include "Components\Transform.h"
#include "../../Interface/Public/Engine.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "Assets\Vertex.h"
#include "Assets\Texture.h"
#include "Assets\Material.h"
#include "Physics\Collision.h"

SpriteRenderer::SpriteRenderer(GameObject* const gameObject, Transform* const transform) : Renderer(gameObject, transform)
{
	m_eEffect = SpriteEffects::UNLIT_EFFECT;
	m_iInputLayout = InfernoEngine_Graphics::GetInputLayout(L"UILayout");
	m_ttTopology = TopologyType::POINT_TOPOLOGY;
	m_iVertexBuffer = InfernoEngine_Graphics::GetBuffer(VERTEX_BUFFER, L"VertexBuffer");
	m_uiStride = sizeof(Vertex);
	m_iIndexBuffer = InfernoEngine_Graphics::GetBuffer(INDEX_BUFFER, L"IndexBuffer");
	m_iVertexShader = InfernoEngine_Graphics::GetShader(VERTEX_SHADER, L"UIVertexShader");
	m_iGeometryShader = InfernoEngine_Graphics::GetShader(GEOMETRY_SHADER, L"QuadBillboardedGeometryShader");
	m_iDepthStencilState = InfernoEngine_Graphics::GetDepthStencilState(L"ObjectStencilState");
	m_iPositionBufferID = InfernoEngine_Graphics::GetConstantBuffer(L"Position");
	m_iQuadDimensionsBufferID = InfernoEngine_Graphics::GetConstantBuffer(L"QuadInfo");
	m_iFillRatioBufferID = InfernoEngine_Graphics::GetConstantBuffer(L"FillRatio");
	m_iSecondColorBufferID = InfernoEngine_Graphics::GetConstantBuffer(L"Color2");
	m_iRadialInfoBufferID = InfernoEngine_Graphics::GetConstantBuffer(L"RadialOriginInfo");
	m_cColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_cColor2 = Color(1.0f, 1.0f, 1.0f, 1.0f);

	m_pTexture = new Texture();
	m_pTexture->SetImage(-1);
	m_pTexture2 = new Texture();
	m_pTexture2->SetImage(-1);

	m_QuadInfo.Dimensions = Vector2(1, 1);
	m_bDualImage = false;
	m_bBillboarded = true;
}

SpriteRenderer::~SpriteRenderer()
{
	SAFE_DELETE(m_pTexture);
	SAFE_DELETE(m_pTexture2);
}

IComponent& SpriteRenderer::operator=(IComponent& _rhs)
{
	SpriteRenderer* rhsSR = dynamic_cast<SpriteRenderer*>(&_rhs);
	if (rhsSR == nullptr) {
		return *this;
	}

	return operator=(*rhsSR);
}
SpriteRenderer& SpriteRenderer::operator=(SpriteRenderer& _rhs)
{
	if (this != &_rhs) {
		m_bBillboarded = _rhs.m_bBillboarded;
		m_bDualImage = _rhs.m_bDualImage;
		m_cColor = _rhs.m_cColor;
		m_cColor2 = _rhs.m_cColor2;
		m_fFill = _rhs.m_fFill;
		m_iLayer = _rhs.m_iLayer;
		m_iEffect = _rhs.m_iEffect;
		m_iPositionBufferID = _rhs.m_iPositionBufferID;
		m_iQuadDimensionsBufferID = _rhs.m_iQuadDimensionsBufferID;
		m_iFillRatioBufferID = _rhs.m_iFillRatioBufferID;
		m_iSecondColorBufferID = _rhs.m_iSecondColorBufferID;
		*m_pTexture = *_rhs.m_pTexture;
		*m_pTexture2 = *_rhs.m_pTexture2;
		m_vMaterials = _rhs.m_vMaterials;
		m_QuadInfo = _rhs.m_QuadInfo;

		Renderer::operator=(_rhs);
	}

	return *this;
}
IComponent* SpriteRenderer::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	SpriteRenderer* sRend = new SpriteRenderer(gameObject, transform);
	(*sRend) = *this;

	return sRend;
}

Mesh* SpriteRenderer::GetMesh() const { return nullptr; }
RendererType SpriteRenderer::GetType() const { return SPRITE_RENDERER; }

int SpriteRenderer::GetNumVerts() { return 1; }
int SpriteRenderer::GetStartVertex() { return 0; }

// IN: void
// OUT: void
//
// Sets up anything needed for drawling from the renderer side
void SpriteRenderer::PreDraw()
{
	if (m_fFill >= 0.0f) {
		InfernoEngine_Graphics::UpdateSubResource(m_iFillRatioBufferID, &m_fFill, sizeof(float));
	}
	if (m_bDualImage) {
		InfernoEngine_Graphics::UpdateSubResource(m_iSecondColorBufferID, &m_cColor2, sizeof(Color));
	}
	if (m_eEffect == RADIAL_EFFECT || m_eEffect == RADIAL_REVERSE_EFFECT) {
		InfernoEngine_Graphics::UpdateSubResource(m_iRadialInfoBufferID, &m_RadialOriginInfo, sizeof(m_RadialOriginInfo));
	}
	if (m_bBillboarded) {
		InfernoEngine_Graphics::UpdateSubResource(m_iPositionBufferID, &transform->WorldMatrix().Translation(), sizeof(Vector3));
	}
	else {
		InfernoEngine_Graphics::SetWorldMatrix(transform->WorldMatrix());
	}

	InfernoEngine_Graphics::UpdateSubResource(m_iQuadDimensionsBufferID, &m_QuadInfo, sizeof(QuadInfo));
	InfernoEngine_Graphics::SetColor(m_cColor);
}

// IN: void
// OUT: void
//
// Calls the draw function without changing the pipeline for doing the depth pre-pass
void SpriteRenderer::DepthPrePassDraw()
{
#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::StartEvent(Inferno::ToWideString(gameObject->GetName()).c_str());
#endif

	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_pTexture->GetImage(), 0);
	InfernoEngine_Graphics::SetUVOffsets(m_pTexture->GetOffsets(), m_pTexture->GetTiling());
	InfernoEngine_Graphics::ApplyPipelineState();
	InfernoEngine_Graphics::Draw(1u, 0u);

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::EndEvent();
#endif
}

// IN: void
// OUT: void
//
// Calls the overloaded draw call for the current renderer
void SpriteRenderer::Draw()
{
#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::StartEvent(Inferno::ToWideString(gameObject->GetName()).c_str());
#endif

	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_pTexture->GetImage(), 0);
	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_pTexture2->GetImage(), 1);
	InfernoEngine_Graphics::SetUVOffsets(m_pTexture->GetOffsets(), m_pTexture->GetTiling());
	InfernoEngine_Graphics::Draw(m_iEffect, 0, 1);

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::EndEvent();
#endif
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void SpriteRenderer::Init()
{

}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void SpriteRenderer::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Dimensions";
	Property* prop = data->GetProperty(propName);
	std::vector<std::string> values = Inferno::StringParse(prop->value, ',');
	m_QuadInfo.Dimensions.x = (float)atof(values[0].c_str());	m_QuadInfo.Dimensions.y = (float)atof(values[1].c_str());

	propName = "Pad";
	prop = data->GetProperty(propName);
	values = Inferno::StringParse(prop->value, ',');
	m_QuadInfo.Pad.x = (float)atof(values[0].c_str());	m_QuadInfo.Pad.y = (float)atof(values[1].c_str());

	propName = "Color";
	prop = data->GetProperty(propName);
	values = Inferno::StringParse(prop->value, ',');
	m_cColor.Red = (float)atof(values[0].c_str()); m_cColor.Green = (float)atof(values[1].c_str());
	m_cColor.Blue = (float)atof(values[2].c_str()); m_cColor.Alpha = 1.0f;

	propName = "Color2";
	prop = data->GetProperty(propName);
	values = Inferno::StringParse(prop->value, ',');
	m_cColor2.Red = (float)atof(values[0].c_str()); m_cColor2.Green = (float)atof(values[1].c_str());
	m_cColor2.Blue = (float)atof(values[2].c_str()); m_cColor2.Alpha = 1.0f;

	propName = "Layer";
	prop = data->GetProperty(propName);
	m_iLayer = atoi(prop->value.c_str());

	propName = "Fill";
	prop = data->GetProperty(propName);
	m_fFill = (float)atof(prop->value.c_str());

	propName = "Texture1";
	prop = data->GetProperty(propName);
	SetImage(Inferno::GetImage(Inferno::ToWideString(prop->value).c_str()));

	propName = "Tiling1";
	prop = data->GetProperty(propName);
	std::vector<std::string> tilingInfo = Inferno::StringParse(prop->value, ',');
	m_pTexture->SetTiling(Vector2((float)atof(tilingInfo[0].c_str()), (float)atof(tilingInfo[1].c_str())));

	propName = "Texture2";
	prop = data->GetProperty(propName);
	SetSecondImageID(Inferno::GetImage(Inferno::ToWideString(prop->value).c_str()));

	propName = "Tiling2";
	prop = data->GetProperty(propName);
	tilingInfo = Inferno::StringParse(prop->value, ',');
	m_pTexture2->SetTiling(Vector2((float)atof(tilingInfo[0].c_str()), (float)atof(tilingInfo[1].c_str())));

	propName = "Billboarded";
	prop = data->GetProperty(propName);
	SetUseBillboard(Inferno::to_bool(atoi(prop->value.c_str())));

	if (!m_bBillboarded)
		m_iGeometryShader = InfernoEngine_Graphics::GetShader(GEOMETRY_SHADER, L"QuadGeometryShader");

	propName = "Effect";
	prop = data->GetProperty(propName);
	SetEffect((SpriteEffects)atoi(prop->value.c_str()));

	for (unsigned int i = 0; i < (unsigned int)m_vMaterials.size(); ++i){
		delete m_vMaterials[i];
	}
	m_vMaterials.clear();

	propName = "MaterialCount";
	prop = data->GetProperty(propName);
	int materialCount = atoi(prop->value.c_str());
	for (int i = 0; i < materialCount; ++i){
		propName = "Material" + std::to_string(i);
		prop = data->GetProperty(propName);
		if (prop == nullptr) continue;
		Material* mat = Inferno::GetMaterial(prop->value);
		if (mat){
			Material* cMat = new Material;
			*cMat = *mat;
			m_vMaterials.push_back(cMat);
		}
	}
}

// IN: void
// OUT: void
//
// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
void SpriteRenderer::OnEnable()
{
	IComponent::OnEnable();
	InfernoEngine_Graphics::RegisterRenderComponent(this);
	IComponent::OnEnable();
}
// IN: void
// OUT: void
//
// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
void SpriteRenderer::OnDisable()
{
	IComponent::OnDisable();
	InfernoEngine_Graphics::UnregisterRenderComponent(this);
	IComponent::OnDisable();
}

// IN: void
// OUT: int - ID of the effect being used
//
// Returns the ID of the effect being used
int SpriteRenderer::GetEffectID() const
{
	return m_iEffect;
}

// IN: void
// OUT: Material* - first material pointer
//
// Returns the first attached material
Material* SpriteRenderer::GetMaterial() const
{
	return nullptr;
}
// IN: void
// OUT: std::vector<Material*>& - refrence to the list of materials
//
// Returns the list of materials attached to the renderer
std::vector<Material*> const & SpriteRenderer::GetMaterials() const
{
	return m_vMaterials;
}

void SpriteRenderer::SetLayer(int layer)
{
	m_iLayer = layer;
}
void SpriteRenderer::SetFillAmount(float fill)
{
	m_fFill = Clamp(0.0f, 1.0f, fill);
}
void SpriteRenderer::SetEffect(SpriteEffects effect)
{
	m_eEffect = effect;

	switch (effect)
	{
	case FILL_EFFECT:
		m_iEffect = Inferno::GetEffectByName("UI_Fill_Unlit");
		m_bDualImage = false;
		break;
	case FILL_RIGHT_EFFECT:
		m_iEffect = Inferno::GetEffectByName("UI_FillRight_Unlit");
		m_bDualImage = false;
		break;
	case FILL_DUAL_IMAGE_EFFECT:
		m_iEffect = Inferno::GetEffectByName("UI_FillDualImage_Unlit");
		m_bDualImage = true;
		break;
	case RADIAL_EFFECT:
		m_iEffect = Inferno::GetEffectByName("UI_RadialFillClockwise_Unlit");
		m_bDualImage = false;
		break;
	case RADIAL_REVERSE_EFFECT:
		m_iEffect = Inferno::GetEffectByName("UI_RadialFill_Unlit");
		m_bDualImage = false;
		break;
	case UNLIT_EFFECT:
		m_iEffect = Inferno::GetEffectByName("UI_Unlit");
		m_bDualImage = false;
		break;
	}
}
// === SetRadialOrigin
// IN : RadialOrigin - ID of the origin to use
// OUT: void
//
// Sets the origin for radial fill
void SpriteRenderer::SetRadialOrigin(SpriteRadialOrigins origin)
{
	switch (origin)
	{
	case RADIAL_ORIGIN_LEFT:
		m_RadialOriginInfo.TopLeft = Vector2(-1, 1);
		m_RadialOriginInfo.TopRight = Vector2(1, 1);
		m_RadialOriginInfo.BottomLeft = Vector2(-1, -1);
		m_RadialOriginInfo.BottomRight = Vector2(1, -1);
		break;
	case RADIAL_ORIGIN_BOTTOM:
		m_RadialOriginInfo.TopLeft = Vector2(1, 1);
		m_RadialOriginInfo.TopRight = Vector2(1, -1);
		m_RadialOriginInfo.BottomLeft = Vector2(-1, 1);
		m_RadialOriginInfo.BottomRight = Vector2(-1, -1);
		break;
	case RADIAL_ORIGIN_RIGHT:
		m_RadialOriginInfo.TopLeft = Vector2(1, -1);
		m_RadialOriginInfo.TopRight = Vector2(-1, -1);
		m_RadialOriginInfo.BottomLeft = Vector2(1, 1);
		m_RadialOriginInfo.BottomRight = Vector2(-1, 1);
		break;
	case RADIAL_ORIGIN_TOP:
		m_RadialOriginInfo.TopLeft = Vector2(-1, -1);
		m_RadialOriginInfo.TopRight = Vector2(-1, 1);
		m_RadialOriginInfo.BottomLeft = Vector2(1, -1);
		m_RadialOriginInfo.BottomRight = Vector2(1, 1);
		break;
	}
}
void SpriteRenderer::SetDimensions(Vector2 dimensions)
{
	m_QuadInfo.Dimensions = dimensions;
}
void SpriteRenderer::SetImage(int ImageID)
{
	m_pTexture->SetImage(ImageID);
}
void SpriteRenderer::SetSecondImageID(int ImageID)
{
	m_pTexture2->SetImage(ImageID);
}
void SpriteRenderer::SetSecondColor(Color color)
{
	m_cColor2 = color;

	m_cColor2.Alpha = Clamp(0.0f, 1.0f, m_cColor2.Alpha);
	m_cColor2.Red = Clamp(0.0f, 1.0f, m_cColor2.Red);
	m_cColor2.Green = Clamp(0.0f, 1.0f, m_cColor2.Green);
	m_cColor2.Blue = Clamp(0.0f, 1.0f, m_cColor2.Blue);
}
void SpriteRenderer::SetUseBillboard(bool billboard)
{
	if (billboard == m_bBillboarded)
		return;

	m_bBillboarded = billboard;

	if (billboard)
		m_iGeometryShader = InfernoEngine_Graphics::GetShader(GEOMETRY_SHADER, L"QuadBillboardedGeometryShader");
	else
		m_iGeometryShader = InfernoEngine_Graphics::GetShader(GEOMETRY_SHADER, L"QuadGeometryShader");
}

int SpriteRenderer::GetLayer() const
{
	return m_iLayer;
}
float SpriteRenderer::GetFillAmount() const
{
	return m_fFill;
}
Vector2 SpriteRenderer::GetDimensions() const
{
	return m_QuadInfo.Dimensions;
}
int SpriteRenderer::GetImageID() const
{
	return m_pTexture->GetImage();
}
int SpriteRenderer::GetSecondImageID() const
{
	return m_pTexture2->GetImage();
}

Sphere SpriteRenderer::GetBoundingSphere() const{
	return Sphere(transform->GetPosition(), fmaxf(m_QuadInfo.Dimensions.x, m_QuadInfo.Dimensions.y));
}
