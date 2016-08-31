// Author : Jack Gillette
//
// Interface class for anything that is interactable within the game.
#pragma once

class iInteractable
{
public:
	// === Interface === //
	virtual void Interact() = 0;
	// ================= //
};