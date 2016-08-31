// Author : Jack Gillette
//
// Handles passing input events to registered UIEventSystems that are being interacted with. Uses Mouse input by default
// along with an optional UIController to handle advanced input (Keys / Controllers).
#pragma once
#include "../../Utilities/Public/Registry.h"

#include "../../Utilities/Public/InputEvents.h"
#include "../../Utilities/Public/Flags.h"

#include <vector>

class UICanvas;
class UIController;
class UIElement;
class UIEventSystem;
class UISelectable;

class UIManager : public Registry<UIEventSystem>
{
private:
	// === Enumerations
	enum UIMANAGER_FLAGS { UM_POSTINITCALLED_FLAG = 0, UM_DESTROYPENDINGUI_NEXTFRAME_FLAG = 1, UM_MAX_FLAGS };

	// === Vairables
	std::vector<UIElement*> m_vCanvasList;
	std::vector<UIElement*> m_vPendingDestroyList;
	std::vector<std::string> m_vCanvasNames;
	UIController* m_pUIController;
	UIEventSystem* m_pFocusedSystem;
	Flags<char> m_fcFlags;
	int m_iScaleBufferID;
	float m_fTargetResolutionWidth, m_fTargetResolutionHeight;
	float m_fWidthScale, m_fHeightScale;
	bool m_bIgnoreMouseInput;

	// === Private Interface === //
	void SetFocusedSystem(UIEventSystem* _system);
	// ========================= //

public:
	// === Constructor / Destructor === //
	UIManager();
	~UIManager();
	// ================================ //
	
	// === Interface === //
	void Init();
	void PostInit();

	void UnregisterEventSystem(UIEventSystem* _eventSystem);

	// === RegisterForCanvasScaleChange
	//  IN : string& - Name of the Canvas to register with
	//       void* - The owning object of the function to be called.
	//       CanvasScaleChangeFunc - Func to be registered. Format of void(void)
	//  OUT : void
	//
	//  Registers a Function to be called when a specific canvas changes it's scale size. 
	void RegisterForCanvasScaleChange(std::string& _canvasName, void* _callingObject, std::function<void(void)> _func);
	// === UnregisterFromCanvasScaleChange
	//  IN : string& - Name of the Canvas to unregister from
	//       void* - The owning object of the function that was registered.
	//  OUT : void
	// 
	//  Unregisters a CanvasScaleChange Function from a specific canvas.
	void UnregisterFromCanvasScaleChange(std::string& _canvasName, void* _callingObject);

	// === Update
	//  IN : void
	//  Out : void
	//
	//  Checks for input interactions with any registered UIEventSystems, passing the type of event that was trigger to the affected
	//  UIEventSystem. Checks for Mouse Input by default, and then calls update on its UIController, which can handle advanced input.
	void Update();
	// === CreateNewCanvas
	//  IN : UICanvas* - { Out Parameter } Pointer to the created canvas
	//  OUT : unsigned int - The slot that the canvas ws created in
	//
	//  Creates a new Canvas, at the first available slot, and returns that slot ID. Returns the created Canvas through parameters.
	unsigned int CreateNewCanvas(std::string _canvasName, UICanvas** _canvasCanvas);
	// === DestroyCanvas
	//  IN : string - Name of the Canvas
	//  OUT : void
	//
	//  Destroys a Canvas and all UIElements that are attached to the Canvas.
	void DestroyCanvas(std::string _canvasName);
	// === AddToView
	//  IN : UIElement* - The Pointer to the UIElement to add
	//       unsigned int - The slot of the canvas that the element should be added to
	//  OUT : bool - Whether or not the element was added to the desired canvas
	//
	//  Adds the given UIElement to a desired UICanvas. Returns true if the canvas was found and the element was added, false otherwise.
	bool AddToView(UIElement* _element, unsigned int _canvasSlot);
	// === AddToView
	//  IN : UIElement* - The Pointer to the UIElement to add
	//       std::string - The name of the Canvas to add the element to
	//  OUT : bool - Whether or not the element was added to the desired canvas
	//
	//  Adds the given UIElement to a desired UICanvas. Returns true if the canvas was found and the element was added, false otherwise.
	bool AddToView(UIElement* _element, std::string _canvasName);

	void ClearView();
	// ================= //

	// === Input Events === //
	void InputEvent_MouseMovement(tagPOINT _cursorPosition, tagPOINT _displacement);
	void InputEvent_MouseButton(ACTION_STATUS _status);
	// ==================== //

	// === Events === //
	void OnResolutionChange(void* _pData);
	// ============== //

	// === Accessors === //

	// === IsIgnoringMouseInput
	//  IN : void
	//  OUT : bool - Whether or not the Mouse Input is being ignored.
	//
	//  Returns whether or not the UIManager is currently ignoring mouse input when checking for interaction with UIEventSystems.
	bool IsIgnoringMouseInput() const;
	// === IsFocusedElementLocked
	//  IN : void
	//  OUT : bool - Whether or not the current focused element is locked
	//
	//  Checks to see if the current focused element is locked or not.
	bool IsFocusedElementLocked() const;
	// === GetUIController
	//  IN : void
	//  OUT : UIController* - Pointer to the UIController that the UIManager is currently using
	// 
	//  Returns the UIController that the UIManager is currently using.
	UIController* GetUIController() const;
	// === GetCanvas
	//  IN : unsigned int - The slot of the Canvas to retrieve
	//  OUT : UICanvas* - The Canvas at the specific slot
	//
	//  Returns the Canvas at a given slot. If there is no canvas at that slot, it returns a nullptr.
	UICanvas* GetCanvas(unsigned int _canvasSlot) const;
	// === GetCanvasByName
	//  IN : string - The name of the Canvas
	//  OUT : UICanvas* - The desired Canvas, if it exists
	//
	//  Returns the Canvas found under the given name, if it exists.
	UICanvas* GetCanvasByName(std::string _canvasName) const;
	// === GetUIElementByName
	//  IN : string - Name of the Element to look for
	//       string - (Optional) Name of the Canvas to look in
	//  OUT : UIElement* - The found UIElement
	//
	//  Looks for a UIElement with the given name, on the specified canvas, if one is specified. Returns the found element, or nullptr if no element
	//  was found with that name
	UIElement* GetUIElementByName(std::string& _elementName, std::string& _canvasName = std::string());
	// === GetCurrentSelectable
	//  IN : void
	//  OUT : UISelectable* - Pointer to the Selectable that is selected, nullptr if none
	//
	//  Returns the currently selected UISelectable
	UISelectable* GetCurrentSelectable();
	// ================= //

	// === Mutators === //

	// === IgnoreMouseInput
	//  IN : bool - Whether or not the input should be ignored
	//  OUT : void
	//
	//  Sets whether or not the UIManager will check mouse input for interactions with the registered UIEventSystems
	void IgnoreMouseInput(bool _value);
	// === SetUIController
	//  IN : UIController - The desired controller for the UIManager to use
	//  OUT : void
	// 
	//  Sets the UIController that the UIManager will use for handling advanced input / events. Handles cleaning up the old 
	//  UIController, if there was one.
	void SetUIController(UIController* _controller);
	// === SetTargetResolution
	//  IN : float - The target width
	//       float - The target height
	//  OUT : void 
	//
	//  Sets the Target Resolution that UI is designed for.
	void SetTargetResolution(float _width, float _height);
	// === SetCurrentSelectable
	//  IN : UISelectable* - Pointer to the Selectable that should be focused on
	//  OUT : void
	//
	//  Sets the UISelectable that the UIController should be focusing on
	void SetCurrentSelectable(UISelectable* _selectable);
	// ================ //
};