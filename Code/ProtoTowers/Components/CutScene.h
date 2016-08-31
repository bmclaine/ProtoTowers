//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#pragma once

#include "Components\IComponent.h"
#include "InfernoTimer.h"

#include <functional>

#define BindOnCutSceneEndFunction(Func, Class) {std::bind(Func, Class)}

class Camera;
class CameraManager;
class CutSceneWaypoint;
class Texture;
class UIRenderer;
class AIModule;

class CutScene : public IComponent
{
private:
	typedef std::function<void(void)> OnCutSceneEndFunc;

	OnCutSceneEndFunc m_fOnCutSceneEndFunc;

	struct MoveFromWaypoint
	{
		Vector3 Position;
		Quaternion Rotation;
	};

	struct MoveToWaypoint
	{
		Vector3 Position;
		Quaternion Rotation;
	};

public:
	CutScene(GameObject* const gameObject, Transform* const transform);
	~CutScene();
	CutScene& operator=(const CutScene& _rhs);

	IComponent& operator=(IComponent& _rhs);
	IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

	void Init() override;
	void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	void Update() override;

	void OnTriggerEnter(Collider* _col) override;

	virtual void OnEnable();
	virtual void OnDisable();

	// Called when intially starting the cutscene.
	void StartCutScene();

	// Called when cutscene ends. Sets main camera back to game camera.
	void EndCutScene();

	// Sets run bool to true (camera updates)
	void PlayCutScene();

	// Sets run bool to false (camera stops updating)
	void PauseCutScene();

	// Called when the player provides input to skip cutscene
	void SkipCutScene(ACTION_STATUS _status);

	// Returns true when the cutscene is finished playing. false if it is not.
	bool IsFinished();

	// Sets the looping bool
	void SetIsLooping(bool _isLooping);

	// Set the camera used by this cutscene.
	void SetCutSceneCamera(Camera* _camera);

	// Sets the start waypoing of this cutscene.
	void SetStartWaypoint(CutSceneWaypoint* _waypoint);

	// Sets the function to be called when the cutscene ends.
	void SetOnCutSceneEndFunction(OnCutSceneEndFunc _func);

	// Sets if the cutscene can be skipped or not
	void SetIsSkipable(bool skipable);

	// Returns the start waypoing of the cutscene
	CutSceneWaypoint* GetStartWaypoint() const;

private:
	// The main game scene camera.
	Camera* m_pGameCamera;
	// The camera used during this cutscene.
	Camera* m_pCutSceneCamera;

	// Where our cutscene camera starts from.
	CutSceneWaypoint* m_pStartWaypoint;
	CutSceneWaypoint* m_pTangentWaypoint;

	// The waypoint our cutscene camera is moving from.
	//CutSceneWaypoint* m_pMoveFromWaypoint;

	// The structures our waypoint data will be stored into using Decompose.
	MoveFromWaypoint m_sMoveFromWaypoint;
	MoveToWaypoint m_sMoveToWaypoint;

	// The waypoint our cutscene camera is moving to.
	CutSceneWaypoint* m_pMoveToWaypoint;

	// The timer object that keeps track of how long we should be moving between waypoints.
	InfernoTimer m_itTravelTimer;

	// The timer object the keeps track of when this cutscene can be skipped.
	InfernoTimer m_itSkipTimer;

	// The textures used for displaying cutscene skip messege
	Texture* m_pSkipKeyboardImage;
	Texture* m_pSkipControllerImage;

	// The elements that will be used to activate and deactivate the respective skip messeges.
	UIElement* m_pSkipKeyboard;
	UIElement* m_pSkipController;

	// A pointer to the AIModule class
	AIModule* m_pAIModule;

	bool m_bIsLooping;
	bool m_bIsRunning;
	bool m_bIsFinished;
	bool m_bFuncCalled;
	bool m_bIsSkipable;

	// Interpolates between MoveFrom and MoveTo Waypoints based on time ratio.
	void UpdateCameraLocation();

	// Called at the end of travel time. Updates MoveFrom and MoveTo Waypoints.
	void UpdateWaypoints();

	// Called at the end of this cutscenes skip timer.
	void SetCutSceneSkippable();

	// Called when keyboard switches to controller or vice-versa
	void OnControllerChange(bool _usingController);

protected:


};