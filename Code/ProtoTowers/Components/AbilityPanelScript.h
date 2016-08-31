#pragma once

#include "UIComponents\UIComponent.h"
#include "../Core/PlayerController.h"

class UIRenderer;
class UIText;
class UIElement;

typedef unsigned int UINT;

class AbilityPanelScript : public UIComponent
{
private:
	InfernoTimer m_itActiveTimers[MAX_ABILITIES];
	PlayerController* m_pController;
	UIElement* m_pKeyElement;
	UIElement* m_pButtonElement;
	UIRenderer* m_pAbilityRenderers[MAX_ABILITIES];
	UIRenderer* m_pCooldownRenderers[MAX_ABILITIES];
	UIRenderer* m_pActiveTimeRenderers[MAX_ABILITIES];
	Ability* m_pAbilities[MAX_ABILITIES];
	Texture* m_pAbilityButtonTextures[MAX_ABILITIES];
	UIRenderer* m_pAbilityButtons[MAX_ABILITIES];
	UIText* m_pAbilityKeys[MAX_ABILITIES];
	Texture* m_pAbilitySlotTexture;
	Texture* m_pFillTexture;

	void ControlChange(bool usingController);

public:
	AbilityPanelScript(UIElement* _owner);
	virtual ~AbilityPanelScript() override;

	// === Init
	//  IN : void
	//  OUT : void
	//
	//  Function that gets called to initialize the component. Can be overrriden by any derived UIComponents.
	virtual void Init() override;
	// === PostInit
	//  IN : void
	//  OUT : void
	//
	//  Function that gets called after the component has been initialized. Can be overrriden by any derived UIComponents.
	virtual void PostInit() override;

	// === Update
	//  IN : void
	//  OUT : void
	//
	//  Function that gets called every frame. Can be overriden by any derived UIComponents.
	virtual void Update() override;

	void SetPlayerController(PlayerController* controller);
	void SetAbility(Ability* ability, UINT slot);
	void SetAbilityActiveTime(float time, UINT slot);
	void SetAbilityButtonTexture(Texture* texture, UINT slot);
	void SetAbilityButtonTexture(UINT ID, UINT slot);
};

