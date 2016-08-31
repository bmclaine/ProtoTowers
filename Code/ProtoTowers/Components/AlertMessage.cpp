#include "stdafx.h"
#include "AlertMessage.h"

#include "../InfernoUI.h"
#include "InfernoTimer.h"

// === Defines
#define BIGTEXT_SIZE 24.0f
#define NORMALTEXT_SIZE 20.0f

// ===== Constructor / Destructor ===== //
AlertMessage::AlertMessage(UIElement* _owner) : UIComponent(_owner), m_fcAlertMessageFlags((unsigned int)AlertMessageFlags::AM_MAX_FLAGS)
{
	m_pFlashTimer = new InfernoTimer();
	m_pFlashTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&AlertMessage::OnFlash, this));
	m_pMessageTimer = new InfernoTimer();
	m_pMessageTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&AlertMessage::OnDisplayMessageEnd, this));

	m_pPrimaryColor = new Color();
	m_pSecondaryColor = new Color();

	m_pText = nullptr;

	m_fFlashTime = 0.25f;
}

AlertMessage::~AlertMessage()
{
	delete m_pFlashTimer;
	delete m_pMessageTimer;
	delete m_pPrimaryColor;
	delete m_pSecondaryColor;
}
// ==================================== //

// ===== Interface ===== //
void AlertMessage::OnEnable()
{
	if (m_pText == nullptr) {
		CreateUIText();
	}
}

// === DisplayMessage
//  IN : string& - Message to display
//       float - Display Time
//       bool - Whether or not to flash the Message
//       bool - Whether or not to override what ever Message is currently being displayed, if there is one
//  OUT : void
//
//  Displayes a message for a certain amount of time. Has the option to Flash between the two colors and to override what ever message is currently being displayed, if there is one.
void AlertMessage::DisplayMessage(std::string& _message, float _displayTime, bool _flash, bool _overrideActiveMsg, bool _bigText)
{
	// === Is there already a message being displayed?
	if (m_fcAlertMessageFlags.GetFlag((unsigned int)AlertMessageFlags::AM_IsDisplaying_Flag) && !_overrideActiveMsg) {
		return;
	}

	m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_IsDisplaying_Flag, (unsigned int)true);

	m_pText->SetText(_message);
	m_pText->SetEnabled(true);
	
	m_pMessageTimer->SetTime(_displayTime);
	m_pMessageTimer->Reset();

	if (_flash) {
		m_pFlashTimer->SetTime(m_fFlashTime);
		m_pFlashTimer->Reset();
	}

	if (_bigText && !m_fcAlertMessageFlags.GetFlag((unsigned int)AlertMessageFlags::AM_BigText_Flag)) {
		m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_BigText_Flag, (unsigned int)true);
		m_pText->SetFontSize(BIGTEXT_SIZE);
	}
	else if (!_bigText && m_fcAlertMessageFlags.GetFlag((unsigned int)AlertMessageFlags::AM_BigText_Flag)) {
		m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_BigText_Flag, (unsigned int)false);
		m_pText->SetFontSize(NORMALTEXT_SIZE);
	}
}

// === QueueMessage
//  IN : string - Message to Display
//       float - Display time
//       Color& - Primary Color
//       Color& - Secondary Color
//       bool - Whether or not to flash the message
//  OUT : void
//
//  Queues up a message with all of it's settings to be displayed when available. 
void AlertMessage::QueueMessage(std::string _message, float _displayTime, Color& _primaryColor, Color& _secondaryColor, bool _flash, bool _bigText)
{
	// === Is there currently a Message being displayed?
	if (m_fcAlertMessageFlags.GetFlag((unsigned int)AlertMessageFlags::AM_IsDisplaying_Flag)) {
		// == Queue the Message
		QueuedMessage newMessage;
		newMessage.BigText = _bigText;
		newMessage.DisplayTime = _displayTime;
		newMessage.Flash = _flash;
		newMessage.Message = _message;
		newMessage.PrimaryColor = _primaryColor;
		newMessage.SecondaryColor = _secondaryColor;
		m_qQueuedMessages.push(newMessage);
	}
	else {
		// === Just Display it
		m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_IsDisplaying_Flag, (unsigned int)true);
		m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_PrimaryColor_Flag, (unsigned int)true);

		m_pText->SetText(_message);
		m_pText->SetColor(_primaryColor);
		m_pText->SetEnabled(true);

		m_pMessageTimer->SetTime(_displayTime);
		m_pMessageTimer->Reset();

		*m_pPrimaryColor = _primaryColor;
		*m_pSecondaryColor = _secondaryColor;

		if (_flash) {
			m_pFlashTimer->SetTime(m_fFlashTime);
			m_pFlashTimer->Reset();
		}

		if (_bigText && !m_fcAlertMessageFlags.GetFlag((unsigned int)AlertMessageFlags::AM_BigText_Flag)) {
			m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_BigText_Flag, (unsigned int)true);
			m_pText->SetFontSize(BIGTEXT_SIZE);
		}
		else if (!_bigText && m_fcAlertMessageFlags.GetFlag((unsigned int)AlertMessageFlags::AM_BigText_Flag)) {
			m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_BigText_Flag, (unsigned int)false);
			m_pText->SetFontSize(NORMALTEXT_SIZE);
		}
	}
}
// ===================== //

// ===== Private Interface ===== //
void AlertMessage::CreateUIText()
{
	if (m_pText != nullptr) {
		return;
	}

	m_pText = m_pUIElement->AddComponent<UIText>();
	m_pText->SetAlignment(TextAlignment::CENTER);
	m_pText->SetFontSize(NORMALTEXT_SIZE);
	m_pText->SetBorderSize(1);
	m_pText->SetEnabled(false);
}

void AlertMessage::OnDisplayMessageEnd()
{
	// === Is there another message to display?
	if (m_qQueuedMessages.size() > 0) {
		QueuedMessage nextMsg = m_qQueuedMessages.front();
		m_qQueuedMessages.pop();

		*m_pPrimaryColor = nextMsg.PrimaryColor;
		*m_pSecondaryColor = nextMsg.SecondaryColor;

		m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_PrimaryColor_Flag, (unsigned int)true);

		m_pText->SetText(nextMsg.Message);
		m_pText->SetColor(*m_pPrimaryColor);

		m_pMessageTimer->SetTime(nextMsg.DisplayTime);
		m_pMessageTimer->Reset();

		if (nextMsg.Flash) {
			m_pFlashTimer->SetTime(m_fFlashTime);
			m_pFlashTimer->Reset();
		}

		if (nextMsg.BigText && !m_fcAlertMessageFlags.GetFlag((unsigned int)AlertMessageFlags::AM_BigText_Flag)) {
			m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_BigText_Flag, (unsigned int)true);
			m_pText->SetFontSize(BIGTEXT_SIZE);
		}
		else if (!nextMsg.BigText && m_fcAlertMessageFlags.GetFlag((unsigned int)AlertMessageFlags::AM_BigText_Flag)) {
			m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_BigText_Flag, (unsigned int)false);
			m_pText->SetFontSize(NORMALTEXT_SIZE);
		}
	}
	else {
		// === Stop Displaying
		m_pMessageTimer->SetTime(0.0f);
		m_pMessageTimer->Reset();
		m_pFlashTimer->SetTime(0.0f);
		m_pFlashTimer->Reset();

		m_pText->SetEnabled(false);

		m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_IsDisplaying_Flag, (unsigned int)false);
	}
}

void AlertMessage::OnFlash()
{
	// === Switch Colors
	if (m_fcAlertMessageFlags.GetFlag((unsigned int)AlertMessageFlags::AM_PrimaryColor_Flag)) {
		m_pText->SetColor(*m_pSecondaryColor);
		m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_PrimaryColor_Flag, (unsigned int)false);
	}
	else {
		m_pText->SetColor(*m_pPrimaryColor);
		m_fcAlertMessageFlags.SetFlag((unsigned int)AlertMessageFlags::AM_PrimaryColor_Flag, (unsigned int)true);
	}

	m_pFlashTimer->Reset();
}
// ============================= //

// ===== Mutators ===== //

// === SetFlashColors
//  IN : Color& - Primary Color
//       Color& - Secondary Color
//       bool - Whether or not to override the current colors, if a Message is currently displaying
//  OUT : void
//
//  Sets the two flash colors if there isn't a message being displayed currently. Has the option to override the colors no matter what.
void AlertMessage::SetFlashColors(Color& _primaryColor, Color& _secondaryColor, bool _overrideCurrent)
{
	if (!_overrideCurrent && m_fcAlertMessageFlags.GetFlag((unsigned int)AlertMessageFlags::AM_IsDisplaying_Flag)) {
		return;
	}

	*m_pPrimaryColor = _primaryColor;
	*m_pSecondaryColor = _secondaryColor;

	if (m_pText == nullptr) {
		CreateUIText();
	}

	m_pText->SetColor(_primaryColor);
}

void AlertMessage::SetFlashTime(float _seconds)
{
	m_fFlashTime = _seconds;
}
// ==================== //

// ===== Accessors ===== //
Color AlertMessage::GetPrimaryColor() const
{
	return *m_pPrimaryColor;
}

Color AlertMessage::GetSecondaryColor() const
{
	return *m_pSecondaryColor;
}

bool AlertMessage::IsDisplayingAMessage() const
{
	return m_fcAlertMessageFlags.GetFlag((unsigned int)AlertMessageFlags::AM_IsDisplaying_Flag);
}
// ===================== //
