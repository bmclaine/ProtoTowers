#include "stdafx.h"
#include "TowerDescription.h"

#include "InfernoUI.h"

// === Constructor / Destructor === //
TowerDescription::TowerDescription(UIElement* _owner) : UIComponent(_owner)
{
	m_pBackgroundImg = new Texture();
	m_pBackgroundImg->SetImage(Inferno::GetImage(L"UI_TowerInfo.dds"));

	m_pText = nullptr;
}

TowerDescription::~TowerDescription()
{
	delete m_pBackgroundImg;
}
// ================================ //

// === Interface === //
void TowerDescription::OnEnable()
{
	if (m_pText == nullptr) {
		// === Add on the Background Img
		UIRenderer* image = m_pUIElement->AddComponent<UIRenderer>();
		image->SetTexture(m_pBackgroundImg);
		image->SetLayer(10);
		image->SetColor(Color(1.0f, 1.0f, 1.0f, 0.8f));

		// === Add the Text Child Element
		UIElement* textElement = new UIElement();
		textElement->SetName("Tower Description Text");
		RectTransform* transform = textElement->GetTransform();
		transform->SetParent(m_pUIElement->GetTransform());
		transform->SetBounds(Vector2(268.0f, 92.0f));
		transform->SetPosition(14.0f, 0.0f);
		//// Components
			// === UIText
			m_pText = textElement->AddComponent<UIText>();
			m_pText->SetParagraphAlignment(ParagraphAlignment::PARAGRAPH_CENTER);
			m_pText->SetFontSize(20.0f);
			m_pText->SetText("{ Description Here }");
			m_pText->SetLayer(11);
			m_pText->SetColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	}
}

void TowerDescription::DisplayDescription(std::string& _message, Vector2 _position)
{
	m_pText->SetText(_message);

	_position.x += m_pRectTransform->GetBounds().x;
	m_pRectTransform->SetPosition(_position);
}
// ================= //