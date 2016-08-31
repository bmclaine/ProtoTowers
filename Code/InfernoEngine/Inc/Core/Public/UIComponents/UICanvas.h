// === Author : Jack Gillette
//
// UIComponent that represents the Renderable space for any UIElements that are added to it. The Canvas can handle scaling with the
// the screen, as well as what resolution to use a reference. 
#pragma once
#include "UIComponent.h"

#include <vector>

struct Vector2;

// === Defines
#define BindOnCanvasScaleChangeFunc(Func, Class) { std::bind(Func, Class) }

class UICanvas : public UIComponent
{
private:
	// === TypeDefs
	typedef std::function<void(void)> CanvasScaleChangeFunc;

	// === Structures
	struct ScaleChangeEventObject {
		void* CallingObject;
		CanvasScaleChangeFunc Func;
	};

	// === Enumerations
	enum UICANVAS_FLAGS { UCa_SCALEWITHSCREEN_FLAG = 0, UCa_SCREENSPACE_FLAG, UCa_MAX_FLAGS };

	// === Variables
	std::vector<ScaleChangeEventObject> m_vScaleChangeFunctions;
	Flags<char> m_fcCanvasFlags = Flags<char>(UICANVAS_FLAGS::UCa_MAX_FLAGS);
	float m_fTargetResolutionWidth, m_fTargetResolutionHeight;
	float m_fWidthScale, m_fHeightScale;

public:
	// === Constructor / Destructor === //
	UICanvas(UIElement* _owner);
	~UICanvas();
	// ================================ //

	// === Interface === //
	virtual void Init() override;
	virtual void PostInit() override;
	virtual void Update() override;
	void RegisterForCanvasScaleChange(void* _callingObject, CanvasScaleChangeFunc _func);
	void UnregisterFromCanvasScaleChange(void* _callingObject);
	// ================= //

	// === Event Interface === //
	void Event_OnResolutionChange(void* _pData);
	// ======================= //

	// === Accessors === //
	Vector2 GetScreenScaler() const;
	Vector2 GetTargetResolution() const;
	// ================= //

	// === Mutators === //
	void SetTargetResolution(float _width, float _height);
	// ================ // 
};

