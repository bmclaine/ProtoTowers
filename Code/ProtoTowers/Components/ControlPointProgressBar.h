//Author: William Lewis
//
//Control point progress bar that attaches to the HUD
#pragma once
#include "UIComponents\UIComponent.h"

class ControlPoint;

class ControlPointProgressBar : public UIComponent
{
private:
	ControlPoint * m_pControlPoint;
	UIRenderer * m_pProgressBar;
	UIRenderer * m_pProgressBarBackground;
protected:

public:
	ControlPointProgressBar(UIElement* _owner);
	virtual ~ControlPointProgressBar();

	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;

	void SetControlPoint(void * _controlPoint);
	void SetProgressBar(UIRenderer * _progressBar);
	void SetProgressBarBackground(UIRenderer *_progressBarBackground);
};