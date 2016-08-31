//--------------------------------------------------------------------------------
// AudioManager
// Written by Justin Murphy
//
// Wwise audio wrapper keeps all audio engine classes abstracted from the games classes,
// allows entities to register so their positions/orientations will be updated internally,
// provides useful interfaces for commonly used features of Wwise
//--------------------------------------------------------------------------------

#ifndef AudioManager_h
#define AudioManager_h
//--------------------------------------------------------------------------------
#include <DirectXMath.h>
#include <vector>
#include <map>

#include "AkSoundEngineDLL.h"
//--------------------------------------------------------------------------------
using DirectX::XMFLOAT3;
//--------------------------------------------------------------------------------
#if defined(_WIN64)
typedef unsigned __int64 AudioEvent;				///< Integer (unsigned) type for pointers
#else
typedef __w64 unsigned int AudioEvent;			///< Integer (unsigned) type for pointers
#endif
//--------------------------------------------------------------------------------
class GameObject;
//--------------------------------------------------------------------------------
class AudioManager {
private:
	float m_fMasterAudio;
	float m_fSFXAudio;
	float m_fBackgroundAudio;
public:
AudioManager();
virtual ~AudioManager();

//DESCRIPTION: Access to the audiosystem.  There should only be a single instance of the audiosystem at any given time.
//IN: void
//OUT: Static reference to the audiomanager
static AudioManager* Instance();

//DESCRIPTION: Starts the sound engine. MUST be called before the audio manager can be used
//IN: void
//OUT: bool- true if it was initialized correctly
virtual bool Initialize();
//DESCRIPTION: Terminates the audio manager and the sound engine
//IN: void
//OUT: void
virtual void Shutdown();

//DESCRIPTION: Updates the positions and orientations of all registered entities and renders the audio
//IN: void
//OUT: void
virtual void Update();

////////////// listener and entity registration //////////////

//DESCRIPTION: Registers an entity in the sound engine. Necessary to post events
//
//IN: const GameObject* - entity to be registered
//    const char* - name of the GameObject
//
//OUT: bool - true if the entity is rigistered correctly 
virtual bool RegisterEntity(const GameObject* entity, const char* name);

//DESCRIPTION: Removes a GameObject from the registry
//
//IN: const GameObject* - entity to be removed
//
//OUT: true if the entity is removed correctly
virtual bool UnRegisterEntity(const GameObject* entity);

//DESCRIPTION: Registers an entity for listening. Necessary to hear sounds
//
//IN: const GameObject* - Entity to be registered
//    const char* - name of the listener
//
//OUT: bool - true if the entity is registered correctly
virtual bool RegisterListener(const GameObject* entity, const char* name);

//DESCRIPTION: Removes a GameObject from the listener registry
//
//IN: const GameObject* - entity to be removed from listener registry
//
//OUT: bool - true if the entity is removed correctly
virtual bool UnRegisterListener(const GameObject* entity);

//DESCRIPTION: Posts an audio event at the caller's position
//
//IN: unsigned int - Id of the event you want played
//    const GameObject* - GameObject that will play the sound
//
//OUT: bool - true if the sound plays correctly, false otherwise
virtual bool PostEvent(unsigned int eventId, const GameObject* eventCaller);

//DESCRIPTION: Posts an audio event at the specified position
//
//IN: unsigned int - ID of the event you want played
//    XMFLOAT3& - position you want the sound played from
//
//OUT: bool - true if the sound plays correctly, false otherwise
virtual bool PostEvent(unsigned int eventId, const XMFLOAT3& pos);
//DESCRIPTION: Posts an audio event at the listeners position (no attenuation/panning)
//
//IN: unsigned int - ID of the event you want played
//    unsigned int - ID of the listener you want the event played at, defaulted to 0
//
//OUT: bool - true if the sound plays correctly, false otherwise
virtual bool PostEvent(unsigned int eventId, unsigned int listenerId = 0);

// Materials
//void SetMaterialID(const GameObject* pEntity, AkUniqueID akMaterialSwitchDefine, const unsigned int nMaterialID);

// RTCP handling (dynamic sound adjustment based on a value)

//DESCRIPTION: Sets the GLOBAL Real Time Parameter Control value, affects the sound volume
//
//IN: const char* - name of the RTPC
//    const AkRtpcValue - a 32 bit float value to set
//
//OUT: void
void SetRTCPValue(const char* szRTCPName, const AkRtpcValue akRtcpVal);

//DESCRIPTION: Sets the RTCP value of the gameobject, affects the sound volume
//
//IN: const char* - name of the RTPC
//    const AkRtpcValue - a 32 bit float value to set
//    const GameObject* - gameobject that the rtpc affetcs
//
//OUT: void
void SetRTCPValue(const char* szRTCPName, const AkRtpcValue akRtcpVal, const GameObject* pEntity);

//DESCRIPTION: Sets the RTCP value based on passed in ID
//
//IN: const AkRtpcID - ID of rtcp to affect
//    const AkRtpcValue - value to set
//
//OUT: void
void SetRTCPValue(const AkRtpcID akRtcpID, const AkRtpcValue akRtcpVal);

void SetMasterVolume(float volume);

void SetSFXVolume(float volume);

void SetBackgroundMusicVolume(float volume);

//DESCRIPTION: Gets the master value
//
//IN:void
//
//OUT: float - rtcp value
float GetMasterVolume();

//DESCRIPTION: Gets the background value
//
//IN:void
//
//OUT: float - rtcp value
float GetBackgroundVolume();

//DESCRIPTION: Gets the SFX value
//
//IN:void
//
//OUT: float - rtcp value
float GetSFXVolume();

//DESCRIPTION: Sets the RTCP value based on passed in ID
//
//IN: const AkRtpcID - ID of rtcp to affect
//    const AkRtpcValue - value to set
//    const GameObject* - gameobject that the rtpc affetcs
//
//OUT: void
void SetRTCPValue(const AkRtpcID akRtcpID, const AkRtpcValue akRtcpVal, const GameObject* pEntity);

// States

//DESCRIPTION: Sets the current state of the sound engine
//
//IN: const char*: the name of the state group
//    const char*: the name of the state
//
//OUT:void
void SetState(const char* szStateGroupName, const char* szStateName);

//DESCRIPTION: Sets the current state of the sound engine
//
//IN: const AkStateGroupID : the ID of the state group
//     const AkStateID: the ID of the state
//
//OUT:void
void SetState(const AkStateGroupID akStateGroupID, const AkStateID akStateID);

// Switches

//DESCRIPTION: Sets the current switch of the sound engine
//
//IN: const AkSwitchGroupID - the ID of the group of the switch  
//    const AkSwitchStateID - the ID of the switch
//    const GameObject* - the entity that the switch affects, defaulted to nullptr
//
//OUT: void
void SetSwitch(const AkSwitchGroupID akSwitchGroupID, const AkSwitchStateID akStateID, const GameObject* pEntity = nullptr);

// Triggers
//DESCRIPTION: Posts a trigger onto the sound engine
//
//IN: const char* - name of the trigger
//
//OUT: void
void PostTrigger(const char* szTriggerName);

//DESCRIPTION: Posts a trigger onto the sound engine
//
//IN: AkTriggerID - ID of the trigger
//
//OUT: void
void PostTrigger(const AkTriggerID akTriggerID);

//DESCRIPTION: Posts a trigger onto the sound engine
//
//IN: const char* - name of the trigger
//    const GameObject* - entity for the trigger to affect
//
//OUT: void
void PostTrigger(const char* szTriggerName, const GameObject* pEntity);

//DESCRIPTION: Posts a trigger onto the sound engine
//
//IN: AkTriggerID - ID of the trigger
//    const GameObject* - entity for the trigger to affect
//
//OUT: void
void PostTrigger(const AkTriggerID akTriggerID, const GameObject* pEntity);

//DESCRIPTION: set the base Path for sound banks
//
//IN: const wchar_t* - the path to the folder containing all soundbanks
//
//OUT: void
void SetBasePath(const wchar_t* strPath);

//DESCRIPTION: loads in a sound bank
//
//IN: const wchar_t - the name of the soundbank to load in
//
//OUT: bool - returns true if the soundbank was successfully loaded in
bool LoadSoundBank(const wchar_t* szBankName);

//DESCRIPTION: Unloads a sound bank
//
//IN: const wchar_t = the name of the soundbank to unload
//
//OUT: bool - returns true if the soundbank was successfully unloaded
bool UnloadSoundBank(const wchar_t* szBankName);

//DESCRIPTION: set world sound scale
//
//IN: float - scale to set
//
//OUT: void
void SetWorldScale(float fScale);

virtual void StopPlayingID(AkPlayingID in_playingID, AkTimeMs in_uTransitionDuration = 0, AkCurveInterpolation in_eFadeCurve = AkCurveInterpolation_Linear);

virtual void StopAllAudio(AkGameObjectID in_gameObjectID = AK_INVALID_GAME_OBJECT);

bool IsEntityRegistered(const GameObject* _gameObject);

//DESCRIPTION: returns if the audiomanager has been initialized
//
//IN: void
//
//OUT: bool - true if the audio manager has been initialized
bool GetInitialized() const { return m_bInitialized; }

#ifdef _DEBUG
// post a message in the wwise capture list. ERROR and DEBUGGING Purposes only.
void SendMessage(const char* message);
#endif

protected:
AudioManager& operator=(const AudioManager&);

static AudioManager*	m_spAudioSystem;

std::vector<const GameObject*> m_RegisteredListeners;		// entities registered as Listeners
std::vector<const GameObject*> m_RegisteredEntities;			// entities registered as game objects
std::map<std::wstring, AkBankID> m_RegisteredSoundBanks;

float m_fWorldScale; //world scaling factor
bool m_bInitialized;
};
//--------------------------------------------------------------------------------
#endif // AudioManager_h
//--------------------------------------------------------------------------------
