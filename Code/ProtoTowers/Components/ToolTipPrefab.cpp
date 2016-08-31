#include "stdafx.h"
#include "ToolTipPrefab.h"

#include "InfernoUI.h"
#include "ToolTipManager.h"

// ===== Constructor / Destructor ===== //
ToolTipPrefab::ToolTipPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pBackgroundTexture = new Texture();
	m_pBackgroundTexture->SetImage(Inferno::GetImage(L"UI_ToolTip_Bkgd.dds"));
}

ToolTipPrefab::~ToolTipPrefab()
{
	delete m_pBackgroundTexture;
}
// ==================================== //

// ===== Interface ===== //
void ToolTipPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	// === Nothing to load from the Prefab
}

void ToolTipPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	RectTransform* transform;
	UIRenderer* image;
	UIText* text;

	hudCanvas = GetCanvasByName("ExtendedHUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("ExtendedHUD", &hudCanvas, &hudCanvasSlot);
	}

	// === Tool Tip 
	UIElement* toolTipElement = new UIElement();
	toolTipElement->SetName("ToolTip Main Element");
	transform = toolTipElement->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_TOPRIGHT);
	transform->SetBounds(Vector2(256.0f, 112.0f));
	transform->SetPosition(-128.0f - 32.0f, 128.0f + 128.0f);
	//// Components
		// == ToolTip Manager
		toolTipElement->AddComponent<ToolTipManager>();
	//// Children
		// === ToolTip Visuals
		UIElement* toolTipVisuals = new UIElement();
		toolTipVisuals->SetName("ToolTip Visuals");
		transform = toolTipVisuals->GetTransform();
		transform->SetParent(toolTipElement->GetTransform());
		//// Children
			// === ToolTip Background
			UIElement* toolTipBackground = new UIElement();
			toolTipBackground->SetName("ToolTip Background");
			transform = toolTipBackground->GetTransform();
			transform->SetParent(toolTipVisuals->GetTransform());
			transform->SetBounds(Vector2(256.0f, 112.0f));
			transform->SetPosition(0.0f, 0.0f);
			//// Components
				// == UIRenderer
				image = toolTipBackground->AddComponent<UIRenderer>();
				image->SetTexture(m_pBackgroundTexture);
				image->SetColor(Color(0.55f, 0.55f, 0.55f, 0.55f));
			// === Tip Label
			UIElement* tipLabel = new UIElement();
			tipLabel->SetName("ToolTip Label");
			transform = tipLabel->GetTransform();
			transform->SetParent(toolTipVisuals->GetTransform());
			transform->SetAnchors(Anchors::ANCHOR_CENTERLEFT);
			transform->SetBounds(Vector2(224.0f, 20.0f));
			transform->SetPosition(104.0f, -32.0f);
			//// Components
				// == UIText
				text = tipLabel->AddComponent<UIText>();
				text->SetColor(Color(255.0f, 215.0f / 255.0f, 0.0f, 1.0f));
				text->SetText("Tip:");
				text->SetFontSize(16.0f);
			// === Tip Message
			UIElement* tipMsgElement = new UIElement();
			tipMsgElement->SetName("ToolTip Message");
			transform = tipMsgElement->GetTransform();
			transform->SetParent(toolTipVisuals->GetTransform());
			transform->SetBounds(Vector2(224.0f, 80.0f));
			transform->SetPosition(0.0f, 8.0f);
			//// Components
				// == UIText
				text = tipMsgElement->AddComponent<UIText>();
				text->SetColor(Color(255.0f, 215.0f / 255.0f, 0.0f, 1.0f));
				text->SetText("Tool Tip Message Test. You has gun, you shoot gun, you no die. Much good, great wow.");
				text->SetFontSize(16.0f);
				text->SetParagraphAlignment(ParagraphAlignment::PARAGRAPH_CENTER);

	AddToView(toolTipElement, "ExtendedHUD");
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& ToolTipPrefab::operator=(IComponent& _rhs)
{
	return *this;
}

ToolTipPrefab& ToolTipPrefab::operator=(const ToolTipPrefab& _rhs)
{
	return *this;
}

IComponent* ToolTipPrefab::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	return new ToolTipPrefab(_gameObject, _transform);
}
// =============================== //
