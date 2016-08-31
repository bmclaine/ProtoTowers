// Author : Jack Gillette
//
// Handles recieving Input Events from the InputManager, and what to do with that info. As a UIController it will keep track of the
// current UISelectable, and use that along with the Input Events to navigate the UI on screen. This is meant to just be the base 
// class with basic functionality as a controller. If more functionality is needed for navigating UI, you should derive off this 
// class and create / register what functions you need.
#pragma once

#include <string>

class UISelectable;
enum class ACTION_STATUS;

class UIController
{
protected:
	// === Variables
	std::string m_sBindingUp;
	std::string m_sBindingRight;
	std::string m_sBindingInteract;
	UISelectable* m_pCurrentSelectable;
	float m_fCooldown;

public:
	// === Constructor / Destructor === //
	UIController();
	~UIController();
	// ================================ //

	// === Interface === //
	virtual void Init();
	virtual void PostInit();
	void Update();

	// === UnregisterEvents
	//  IN : void
	//  OUT : void
	//
	//  Unregisters all Events from the Input Manager.
	virtual void UnregisterEvents();
	// === RegisterEvents
	//  IN : void
	//  OUT : void
	//
	//  Registers all needed events with Input Manager. Gets called in the PostInit, but can be called later, if and only if the 
	//  events were previously unregistered.
	virtual void RegisterEvents();
	// ================= //

	// === Event Functions === //
	void AxisEvent_OnNavigateUp(float _axisValue);
	void AxisEvent_OnNavigateRight(float _axisValue);
	void Event_OnNavigateUp(ACTION_STATUS _status);
	void Event_OnNavigateDown(ACTION_STATUS _status);
	void Event_OnNavigateRight(ACTION_STATUS _status);
	void Event_OnNavigateLeft(ACTION_STATUS _status);
	void Event_OnSelect(ACTION_STATUS _status);
	// ======================= //

	// === Accessors === //
	UISelectable* GetCurrentSelectable() const;
	// ================= //
	
	// === Mutators === //
	void SetCurrentSelectable(UISelectable* _selectable);
	// ================ //
};