#pragma once
#include "UIComponents\UIComponent.h"

#include <vector>
#include <queue>

class InfernoTimer;
class UIText;
struct Color;

class AlertMessage : public UIComponent
{
private:
	// === Enumerations
	enum class AlertMessageFlags { AM_PrimaryColor_Flag, AM_IsDisplaying_Flag, AM_BigText_Flag, AM_MAX_FLAGS };

	// === Structures
	struct QueuedMessage {
		std::string Message;
		Color PrimaryColor, SecondaryColor;
		float DisplayTime;
		bool Flash;
		bool BigText;
	};

	// === Variables
	std::queue<QueuedMessage> m_qQueuedMessages;
	Color* m_pPrimaryColor;
	Color* m_pSecondaryColor;
	InfernoTimer* m_pFlashTimer;
	InfernoTimer* m_pMessageTimer;
	UIText* m_pText;
	Flags<char> m_fcAlertMessageFlags;
	float m_fFlashTime;

	// === Private Interface === //
	void CreateUIText();
	void OnDisplayMessageEnd();
	void OnFlash();
	// ========================= //

public:
	// === Constructor / Destructor === //
	AlertMessage(UIElement* _owner);
	virtual ~AlertMessage();
	// ================================ //

	// === Interface === //
	virtual void OnEnable() override;
	// === DisplayMessage
	//  IN : string& - Message to display
	//       float - Display Time
	//       bool - Whether or not to flash the Message
	//       bool - Whether or not to override what ever Message is currently being displayed, if there is one
	//  OUT : void
	//
	//  Displayes a message for a certain amount of time. Has the option to Flash between the two colors and to override what ever message is currently being displayed, if there is one.
	void DisplayMessage(std::string& _message, float _displayTime, bool _flash = false, bool _overrideActiveMsg = false, bool _bigText = false);
	// === QueueMessage
	//  IN : string - Message to Display
	//       float - Display time
	//       Color& - Primary Color
	//       Color& - Secondary Color
	//       bool - Whether or not to flash the message
	//  OUT : void
	//
	//  Queues up a message with all of it's settings to be displayed when available. 
	void QueueMessage(std::string _message, float _displayTime, Color& _primaryColor, Color& _secondaryColor, bool _flash = false, bool _bigText = false);
	// ================= //

	// === Mutators === //

	// === SetFlashColors
	//  IN : Color& - Primary Color
	//       Color& - Secondary Color
	//       bool - Whether or not to override the current colors, if a Message is currently displaying
	//  OUT : void
	//
	//  Sets the two flash colors if there isn't a message being displayed currently. Has the option to override the colors no matter what.
	void SetFlashColors(Color& _primaryColor, Color& _secondaryColor, bool _overrideCurrent = false);
	void SetFlashTime(float _seconds);
	// ================ //

	// === Accessors === //
	Color GetPrimaryColor() const;
	Color GetSecondaryColor() const;
	bool IsDisplayingAMessage() const;
	// ================= //
};

