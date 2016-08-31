#include "stdafx.h"
#include "ControlPointProgressBar.h"
#include "ControlPoint.h"
#include "UIComponents\UIRenderer.h"
#include "UIElement.h"
#include "EventSystem.h"

ControlPointProgressBar::ControlPointProgressBar(UIElement* _owner) : UIComponent( _owner)
{
	m_pControlPoint = nullptr;
	m_pProgressBar = nullptr;
}

ControlPointProgressBar::~ControlPointProgressBar()
{
	OnDisable();
}

void ControlPointProgressBar::OnEnable()
{
	if (m_pProgressBar == nullptr)
		m_pProgressBar = GetUIElement()->GetComponent<UIRenderer>();

	EventSystem::GetInstance()->RegisterForEvent("SetControlPoint", this, BindEventFunction(&ControlPointProgressBar::SetControlPoint, this));
}

void ControlPointProgressBar::OnDisable()
{
	EventSystem::GetInstance()->UnregisterFromEvent("SetControlPoint", this, BindEventFunction(&ControlPointProgressBar::SetControlPoint, this));
}
 
void ControlPointProgressBar::Update()
{
	if (m_pControlPoint != nullptr)
	{
		float captureProgress = m_pControlPoint->GetCaptureProgress();
		m_pProgressBar->SetImageFill(abs(captureProgress));
		if (captureProgress >= 1.0f)
		{
			m_pProgressBar->SetEnabled(false);
			m_pProgressBarBackground->SetEnabled(false);

		}
		else
		{
			m_pProgressBar->SetEnabled(true);
			m_pProgressBarBackground->SetEnabled(true);
		}
		if (captureProgress <= 0)
		{
			m_pProgressBar->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));

		}
		else
		{
			m_pProgressBar->SetColor(Color(0.0f, 0.0f, 1.0f, 1.0f));
		}
	}
}

void ControlPointProgressBar::SetControlPoint(void* _controlPoint)
{
	m_pControlPoint = (ControlPoint *)_controlPoint;
	if (m_pProgressBar->IsEnabled()==false && m_pControlPoint != nullptr)
	{
		m_pProgressBar->SetEnabled(true);
		m_pProgressBarBackground->SetEnabled(true);
	}
	else if (m_pControlPoint == nullptr && m_pProgressBar->IsEnabled())
	{
		m_pProgressBar->SetEnabled(false);
		m_pProgressBarBackground->SetEnabled(false);
	}
}

void ControlPointProgressBar::SetProgressBar(UIRenderer * _progressBar)
{
	m_pProgressBar = _progressBar;
}

void ControlPointProgressBar::SetProgressBarBackground(UIRenderer * _progressBarBackground)
{
	m_pProgressBarBackground = _progressBarBackground;
}