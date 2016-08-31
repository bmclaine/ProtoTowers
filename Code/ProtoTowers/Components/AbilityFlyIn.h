#pragma once
#include "UIComponents\UIComponent.h"


class AbilityFlyIn : public UIComponent
{
private:
	Vector2 m_vStartPosition;
	Vector2 m_vEndPosition;
	float m_fTime;
	bool m_bMove;
	bool m_bWaiting;

	InfernoTimer m_itDisplayTimer;
	InfernoTimer m_itWaitTimer;

	Texture *m_pAbilityTexture;
	UIRenderer * m_pAbilityRenderer;
protected:

public:
	AbilityFlyIn(UIElement* _owner);
	virtual ~AbilityFlyIn();

	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;

	void SetStartPosition(Vector2 _start);
	void SetEndPostion(Vector2 _end);
	void Reset();

	void UpdateFlyIn();

	void SetTexture(Texture * _image);

	void SetTextureImage(int);

	void SetRenderer(UIRenderer * _renderer);

	void SetMoving(bool moving);

	void DisplayAbilityIcon(int _ability);
};