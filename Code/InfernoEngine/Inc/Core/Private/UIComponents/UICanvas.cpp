#include "../Public/UIComponents/UICanvas.h"

#include "EventSystem.h"
#include "../Public/UIComponents/RectTransform.h"

// ===== Constructor / Destructor ===== //
UICanvas::UICanvas(UIElement* _owner) : UIComponent(_owner)
{
	m_fTargetResolutionWidth = 800;
	m_fTargetResolutionHeight = 600;
	m_fWidthScale = m_fHeightScale = 1.0f;
	m_fcCanvasFlags.SetFlag((unsigned int)UICANVAS_FLAGS::UCa_SCALEWITHSCREEN_FLAG, (unsigned int)true);
	m_fcCanvasFlags.SetFlag((unsigned int)UICANVAS_FLAGS::UCa_SCREENSPACE_FLAG, (unsigned int)true);
}

UICanvas::~UICanvas()
{
	EventSystem::GetInstance()->UnregisterFromEvent("ResolutionChange", this, BindEventFunction(&UICanvas::Event_OnResolutionChange, this));
}
// ==================================== //

// ===== Interface ===== //
void UICanvas::Init()
{
	m_pRectTransform->SetBounds(Vector2(m_fTargetResolutionWidth, m_fTargetResolutionHeight));
	m_pRectTransform->SetAnchors(Anchors::ANCHORS::ANCHOR_CENTER);
	m_pRectTransform->SetPosition(Vector2(m_fTargetResolutionWidth / 2.0f, m_fTargetResolutionHeight / 2.0f));
	m_pRectTransform->SetCanvasScale(Vector2(m_fWidthScale, m_fHeightScale));
	m_pRectTransform->Lock(true);

	EventSystem::GetInstance()->RegisterForEvent("ResolutionChange", this, BindEventFunction(&UICanvas::Event_OnResolutionChange, this));
}

void UICanvas::PostInit()
{

}

void UICanvas::Update()
{

}

void UICanvas::RegisterForCanvasScaleChange(void* _callingObject, CanvasScaleChangeFunc _func)
{
	ScaleChangeEventObject newEventObj = { _callingObject, _func };
	m_vScaleChangeFunctions.push_back(newEventObj);
}

void UICanvas::UnregisterFromCanvasScaleChange(void* _callingObject)
{
	size_t count = m_vScaleChangeFunctions.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vScaleChangeFunctions[i].CallingObject == _callingObject) {
			std::iter_swap(m_vScaleChangeFunctions.begin() + i, m_vScaleChangeFunctions.end() - 1);
			m_vScaleChangeFunctions.erase(m_vScaleChangeFunctions.end() - 1);
			return;
		}
	}
}
// ===================== //

// ===== Event Interface ===== //
void UICanvas::Event_OnResolutionChange(void* _pData)
{
	if (!m_fcCanvasFlags.GetFlag((unsigned int)UCa_SCREENSPACE_FLAG)) {
		float width = *(static_cast<float*>(_pData));
		float height = *((static_cast<float*>(_pData)+1));
		m_fWidthScale = m_fTargetResolutionWidth / width;
		m_fHeightScale = m_fTargetResolutionHeight / height;

		RectTransform* transform = GetTransform();
		transform->Lock(false);
		transform->SetCanvasScale(Vector2(m_fWidthScale, m_fHeightScale));
		transform->Lock(true);
	}

	size_t count = m_vScaleChangeFunctions.size();
	for (size_t i = 0; i < count; ++i) {
		m_vScaleChangeFunctions[i].Func();
	}
}
// =========================== //

// ===== Accessors ===== //
Vector2 UICanvas::GetScreenScaler() const
{
	return Vector2(m_fWidthScale, m_fHeightScale);
}

Vector2 UICanvas::GetTargetResolution() const
{
	return Vector2(m_fTargetResolutionWidth, m_fTargetResolutionHeight);
}
// ===================== //

// === Mutators === //
void UICanvas::SetTargetResolution(float _width, float _height)
{
	m_fTargetResolutionWidth = _width;
	m_fTargetResolutionHeight = _height;

	m_pRectTransform->Lock(false);
	m_pRectTransform->SetBounds(Vector2(m_fTargetResolutionWidth, m_fTargetResolutionHeight));
	m_pRectTransform->SetPosition(Vector2(m_fTargetResolutionWidth / 2.0f, m_fTargetResolutionHeight / 2.0f));
	m_pRectTransform->Lock(true);
}
// ================ //