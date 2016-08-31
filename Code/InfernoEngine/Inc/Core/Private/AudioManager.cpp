//--------------------------------------------------------------------------------
// Written by Justin Murphy
//--------------------------------------------------------------------------------

#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/Tools/Common/AkMonitorError.h>
#include <AK/Tools/Win32/AkPlatformFuncs.h>
#include <AkDefaultIOHookBlocking.h>
#include <string>
#include <sstream>

#include "AudioManager.h"
#include "GameObject.h"
#include "Components\Transform.h"
//--------------------------------------------------------------------------------
#pragma comment(lib, "AkSoundEngineDLL.lib")
//--------------------------------------------------------------------------------
using namespace AK;
//--------------------------------------------------------------------------------
AudioManager* AudioManager::m_spAudioSystem = nullptr;
//--------------------------------------------------------------------------------
AudioManager::AudioManager() :
m_fWorldScale(1.f),
m_bInitialized(false) {
if (m_spAudioSystem == nullptr)
m_spAudioSystem = this;

}
//--------------------------------------------------------------------------------
AudioManager::~AudioManager() { }
//--------------------------------------------------------------------------------
//DESCRIPTION: Access to the audiosystem.  There should only be a single instance of the audiosystem at any given time.
//IN: void
//OUT: Static reference to the audiomanager
AudioManager* AudioManager::Instance() {
return m_spAudioSystem;
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Starts the sound engine. MUST be called before the audio manager can be used
//IN: void
//OUT: bool- true if it was initialized correctly
bool AudioManager::Initialize() {
// Initialize audio engine
// Memory.
AkMemSettings memSettings;
memSettings.uMaxNumPools = 40;

// Streaming.
AkStreamMgrSettings stmSettings;
// 	stmSettings.uMemorySize = 32 * 1024*1024; // 32 MB
StreamMgr::GetDefaultSettings(stmSettings);

AkDeviceSettings deviceSettings;
StreamMgr::GetDefaultDeviceSettings(deviceSettings);

AkInitSettings l_InitSettings;
AkPlatformInitSettings l_platInitSetings;
SoundEngine::GetDefaultInitSettings(l_InitSettings);
SoundEngine::GetDefaultPlatformInitSettings(l_platInitSetings);

// Setting pool sizes for this game.
// These sizes are tuned for this game, every game should determine its own optimal values.
//	l_InitSettings.uDefaultPoolSize           = 4 * 1024 * 1024;  // 4 MB
//	l_InitSettings.uMaxNumPaths               = 16;
//	l_InitSettings.uMaxNumTransitions         = 128;
//	l_platInitSetings.uLEngineDefaultPoolSize    = 4 * 1024 * 1024;  // 4 MB
//l_platInitSetings.hWnd = hwnd;

AkMusicSettings musicInit;
MusicEngine::GetDefaultInitSettings(musicInit);
musicInit.fStreamingLookAheadRatio = 100;

AKRESULT eResult = SOUNDENGINE_DLL::Init(&memSettings,
&stmSettings,
&deviceSettings,
&l_InitSettings,
&l_platInitSetings,
&musicInit);

if (eResult != AK_Success) {
// Then, we will run the game without sound
SOUNDENGINE_DLL::Term();
return false;
}

// This is a default base path. The game programmer should override this when the game is initialized.
SOUNDENGINE_DLL::SetBasePath(L"../Resources/soundbanks/");
//SOUNDENGINE_DLL::SetLangSpecificDirName( L"English(US)/" );

// register 64 dummy game objects for one-shot positioning. See PostEvent(ID, location) function for details
for (int i = 64; i < 128; ++i) {
char name[256];
sprintf_s(name, 256, "Dummy #%i", i - 63);
SoundEngine::RegisterGameObj(i, name);
}

m_bInitialized = true;
//Log("AudioManager has been Initialized");
return true;
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Terminates the audio manager and the sound engine
//IN: void
//OUT: void
void AudioManager::Shutdown() {
SoundEngine::UnregisterAllGameObj();
// Terminate audio engine
SOUNDENGINE_DLL::Term();
//Log("AudioManager has been Shutdown");
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Updates the positions and orientations of all registered entities and renders the audio
//IN: void
//OUT: void
void AudioManager::Update() {
AkListenerPosition listenerPosition;
AkSoundPosition soundPosition;

// update listener positions and orientations
for (unsigned int i = 0; i < m_RegisteredListeners.size(); ++i) {
	const Vector3 pos = m_RegisteredListeners[i]->transform->GetPosition(); 

listenerPosition.Position.X = pos.x;
listenerPosition.Position.Y = pos.y;
listenerPosition.Position.Z = pos.z;

Vector3 forward = m_RegisteredListeners[i]->transform->GetRotation();
listenerPosition.OrientationFront.X = forward.x;
listenerPosition.OrientationFront.Y = forward.y;
listenerPosition.OrientationFront.Z = forward.z;

Vector3 top = m_RegisteredListeners[i]->transform->GetRotation();
listenerPosition.OrientationTop.X = top.x;
listenerPosition.OrientationTop.Y = top.y;
listenerPosition.OrientationTop.Z = top.z;

SoundEngine::SetListenerPosition(listenerPosition, i);
}

// set entities positions and orientations
for (UINT i = 0; i < m_RegisteredEntities.size(); ++i) {
const Vector3 pos = m_RegisteredEntities[i]->transform->GetPosition();

soundPosition.Position.X = pos.x;
soundPosition.Position.Y = pos.y;
soundPosition.Position.Z = pos.z;
Vector3 orientation = m_RegisteredEntities[i]->transform->GetRotation();
soundPosition.Orientation.X = 0.0f;
soundPosition.Orientation.Y = 0.0f;
soundPosition.Orientation.Z = 1.0f;

SoundEngine::SetPosition((AkGameObjectID)m_RegisteredEntities[i], soundPosition);
}
// render the audio
SOUNDENGINE_DLL::Tick();
}
//DESCRIPTION: Registers an entity in the sound engine. Necessary to post events
//
//IN: const GameObject* - entity to be registered
//    const char* - name of the GameObject
//
//OUT: bool - true if the entity is rigistered correctly 
//--------------------------------------------------------------------------------
bool AudioManager::RegisterEntity(const GameObject* entity, const char* name) {
	if (!IsEntityRegistered(entity))
	{
		if (nullptr == name) {
			static int entityNum = 0;
			std::stringstream ssNum;
			ssNum << ++entityNum;
			std::string strTemp = "entity" + ssNum.str();

			if (SoundEngine::RegisterGameObj((AkGameObjectID)entity, strTemp.c_str()) != AK_Success) {
				return false;
			}
		}
		else {
			if (SoundEngine::RegisterGameObj((AkGameObjectID)entity, name) != AK_Success) {
				return false;
			}
		}

		m_RegisteredEntities.push_back(entity);
		return true;
	}
	return false;
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Removes a GameObject from the registry
//
//IN: const GameObject* - entity to be removed
//
//OUT: true if the entity is removed correctly
bool AudioManager::UnRegisterEntity(const GameObject* entity) {
for (unsigned int i = 0; i < m_RegisteredEntities.size(); ++i) {
if (m_RegisteredEntities[i] == entity) {
m_RegisteredEntities.erase(m_RegisteredEntities.begin() + i);
if (SoundEngine::UnregisterGameObj((AkGameObjectID)entity) != AK_Success) {
return false;
}
break;
}
}
return true;
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Registers an entity for listening. Necessary to hear sounds
//
//IN: const GameObject* - Entity to be registered
//    const char* - name of the listener
//
//OUT: bool - true if the entity is registered correctly
bool AudioManager::RegisterListener(const GameObject* entity, const char* name) {
if (nullptr == name) {
std::stringstream ssNum;
ssNum << (m_RegisteredListeners.size() + 1);
std::string strTemp = "listener" + ssNum.str();

if (SoundEngine::RegisterGameObj((AkGameObjectID)entity, strTemp.c_str()) != AK_Success) {
return false;
}
}
else {
if (SoundEngine::RegisterGameObj((AkGameObjectID)entity, name) != AK_Success) {
return false;
}
}

// ensure to update listeners scaling factor to the worlds
AK::SoundEngine::SetListenerScalingFactor((AkUInt32)m_RegisteredListeners.size(), m_fWorldScale);
m_RegisteredListeners.push_back(entity);
m_RegisteredEntities.push_back(entity);
return true;
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Removes a GameObject from the listener registry
//
//IN: const GameObject* - entity to be removed from listener registry
//
//OUT: bool - true if the entity is removed correctly
bool AudioManager::UnRegisterListener(const GameObject* entity) {
for (unsigned int i = 0; i < m_RegisteredListeners.size(); ++i) {
if (m_RegisteredListeners[i] == entity) {
m_RegisteredListeners.erase(m_RegisteredListeners.begin() + i);

if (SoundEngine::UnregisterGameObj((AkGameObjectID)entity) != AK_Success) {
return false;
}
break;
}
}
return UnRegisterEntity(entity);
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Posts an audio event at the caller's position
//
//IN: unsigned int - Id of the event you want played
//    const GameObject* - GameObject that will play the sound
//
//OUT: bool - true if the sound plays correctly, false otherwise
bool AudioManager::PostEvent(unsigned int eventId, const GameObject* eventCaller) {
#ifdef _DEBUG
bool found = false;
for each(auto entity in m_RegisteredEntities) {

if (entity == eventCaller) {
found = true;
break;
}
}
if (!found) {
return false;
}
#endif

if (SoundEngine::PostEvent(eventId, (AkGameObjectID)eventCaller) == AK_INVALID_PLAYING_ID) {
return false;
}
return true;
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Posts an audio event at the specified position
//
//IN: unsigned int - ID of the event you want played
//    XMFLOAT3& - position you want the sound played from
//
//OUT: bool - true if the sound plays correctly, false otherwise
bool AudioManager::PostEvent(unsigned int eventId, const XMFLOAT3& pos)
{
// cycle through the dummy game objects
// number of dummies needs to be big enough to avoid setting a different position
// on the same object twice in the same game frame (64 is probably overkill in this game)
static int iGameObj = 63;
if (127 < ++iGameObj)
iGameObj = 64;

AkSoundPosition soundPosition;
Vector3 orientation(0, 0, 1.0f);
soundPosition.Orientation.X = orientation.Norm().x;
soundPosition.Orientation.Y = orientation.Norm().y;
soundPosition.Orientation.Z = orientation.Norm().z;
Vector3 position(pos.x, pos.y, pos.z);
soundPosition.Position.X = position.x;
soundPosition.Position.Y = position.y;
soundPosition.Position.Z = position.z;

SoundEngine::SetPosition(iGameObj, soundPosition);
if (SoundEngine::PostEvent(eventId, iGameObj) == AK_INVALID_PLAYING_ID) {
return false;
}
return true;
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Posts an audio event at the listeners position (no attenuation/panning)
//
//IN: unsigned int - ID of the event you want played
//    unsigned int - ID of the listener you want the event played at, defaulted to 0
//
//OUT: bool - true if the sound plays correctly, false otherwise
bool AudioManager::PostEvent(unsigned int eventId, unsigned int listenerId) {
if ((m_RegisteredListeners.size() - listenerId) <= 0) {
return false;
}

if (SoundEngine::PostEvent(eventId, (AkGameObjectID)m_RegisteredListeners[listenerId]) == AK_INVALID_PLAYING_ID) {
}
return true;
}
//--------------------------------------------------------------------------------
//void AudioManager::SetMaterialID(const GameObject* pEntity, AkUniqueID akMaterialSwitchDefine, const unsigned int nMaterialID) {}

//DESCRIPTION: Sets the GLOBAL Real Time Parameter Control value, affects the sound volume
//
//IN: const char* - name of the RTPC
//    const AkRtpcValue - a 32 bit float value to set
//
//OUT: void
void AudioManager::SetRTCPValue(const char* szRTCPName, const AkRtpcValue akRtcpVal) {
if (SoundEngine::SetRTPCValue(szRTCPName, akRtcpVal) != AK_Success) {
}
}

//DESCRIPTION: Sets the RTCP value of the gameobject, affects the sound volume
//
//IN: const char* - name of the RTPC
//    const AkRtpcValue - a 32 bit float value to set
//
//OUT: void
void AudioManager::SetRTCPValue(const char* szRTCPName, const AkRtpcValue akRtcpVal, const GameObject* pEntity) {
if (SoundEngine::SetRTPCValue(szRTCPName, akRtcpVal, (AkGameObjectID)pEntity) != AK_Success) {
}
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Sets the RTCP value based on passed in ID
//
//IN: const AkRtpcID - ID of rtcp to affect
//    const AkRtpcValue - value to set
//
//OUT: void
void AudioManager::SetRTCPValue(const AkRtpcID akRtcpID, const AkRtpcValue akRtcpVal) {
if (SoundEngine::SetRTPCValue(akRtcpID, akRtcpVal) != AK_Success) {
}
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Sets the RTCP value based on passed in ID
//
//IN: const AkRtpcID - ID of rtcp to affect
//    const AkRtpcValue - value to set
//    const GameObject* - gameobject that the rtpc affetcs
//
//OUT: void
void AudioManager::SetRTCPValue(const AkRtpcID akRtcpID, const AkRtpcValue akRtcpVal, const GameObject* pEntity) {
if (SoundEngine::SetRTPCValue(akRtcpID, akRtcpVal, (AkGameObjectID)pEntity)) {
}
}

void AudioManager::SetMasterVolume(float volume)
{
	if (SoundEngine::SetRTPCValue("MasterVolume", volume)== AK_Success)
	{
		m_fMasterAudio = volume;
	}
}

void AudioManager::SetSFXVolume(float volume)
{
	if (SoundEngine::SetRTPCValue("SoundEffects", volume) == AK_Success)
	{
		m_fSFXAudio = volume;
	}
}

void AudioManager::SetBackgroundMusicVolume(float volume)
{
	if (SoundEngine::SetRTPCValue("BackgroundMusic", volume) == AK_Success)
	{
		m_fBackgroundAudio = volume;
	}
}

//DESCRIPTION: Gets the master value
//
//IN:void
//
//OUT: float - rtcp value
float AudioManager::GetMasterVolume()
{
	return m_fMasterAudio;
}

//DESCRIPTION: Gets the background value
//
//IN:void
//
//OUT: float - rtcp value
float AudioManager::GetBackgroundVolume()
{
	return m_fBackgroundAudio;
}

//DESCRIPTION: Gets the SFX value
//
//IN:void
//
//OUT: float - rtcp value
float AudioManager::GetSFXVolume()
{
	return m_fSFXAudio;
}


//--------------------------------------------------------------------------------
//DESCRIPTION: Sets the current state of the sound engine
//
//IN: const char*: the name of the state group
//    const char*: the name of the state
//
//OUT:void
void AudioManager::SetState(const char* szStateGroupName, const char* szStateName) {
if (AK::SoundEngine::SetState(szStateGroupName, szStateName) != AK_Success) {

}
}

//--------------------------------------------------------------------------------
//DESCRIPTION: Sets the current state of the sound engine
//
//IN: const AkStateGroupID : the ID of the state group
//     const AkStateID: the ID of the state
//
//OUT:void
void AudioManager::SetState(const AkStateGroupID akStateGroupID, const AkStateID akStateID) {
if (AK::SoundEngine::SetState(akStateGroupID, akStateID) != AK_Success) {
}
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Sets the current switch of the sound engine
//
//IN: const AkSwitchGroupID - the ID of the group of the switch  
//    const AkSwitchStateID - the ID of the switch
//    const GameObject* - the entity that the switch affects, defaulted to nullptr
//
//OUT: void
void AudioManager::SetSwitch(const AkSwitchGroupID akSwitchGroupID, const AkSwitchStateID akStateID, const GameObject* pEntity) {
if (AK::SoundEngine::SetSwitch(akSwitchGroupID, akStateID, (AkGameObjectID)pEntity) != AK_Success) {
}
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Posts a trigger onto the sound engine
//
//IN: const char* - name of the trigger
//
//OUT: void
void AudioManager::PostTrigger(const char* szTriggerName) {
AK::SoundEngine::PostTrigger(szTriggerName, AK_INVALID_GAME_OBJECT);
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Posts a trigger onto the sound engine
//
//IN: AkTriggerID - ID of the trigger
//
//OUT: void
void AudioManager::PostTrigger(const AkTriggerID akTriggerID) {
AK::SoundEngine::PostTrigger(akTriggerID, AK_INVALID_GAME_OBJECT);
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Posts a trigger onto the sound engine
//
//IN: const char* - name of the trigger
//    const GameObject* - entity for the trigger to affect
//
//OUT: void
void AudioManager::PostTrigger(const char* szTriggerName, const GameObject* pEntity) {
AK::SoundEngine::PostTrigger(szTriggerName, (AkGameObjectID)pEntity);
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Posts a trigger onto the sound engine
//
//IN: AkTriggerID - ID of the trigger
//    const GameObject* - entity for the trigger to affect
//
//OUT: void
void AudioManager::PostTrigger(const AkTriggerID akTriggerID, const GameObject* pEntity) {
AK::SoundEngine::PostTrigger(akTriggerID, (AkGameObjectID)pEntity);
}
//--------------------------------------------------------------------------------
//DESCRIPTION: set the base Path for sound banks
//
//IN: const wchar_t* - the path to the folder containing all soundbanks
//
//OUT: void
void AudioManager::SetBasePath(const wchar_t* strPath) {
SOUNDENGINE_DLL::SetBasePath(strPath);
}
//--------------------------------------------------------------------------------
//DESCRIPTION: loads in a sound bank
//
//IN: const wchar_t - the name of the soundbank to load in
//
//OUT: bool - returns true if the soundbank was successfully loaded in
bool AudioManager::LoadSoundBank(const wchar_t* szBankName) {
AkBankID bankID;
if (SoundEngine::LoadBank(szBankName, AK_DEFAULT_POOL_ID, bankID) == AK_Success)
m_RegisteredSoundBanks[szBankName] = bankID;
else {
return false;
}

return true;
}
//--------------------------------------------------------------------------------
//DESCRIPTION: Unloads a sound bank
//
//IN: const wchar_t = the name of the soundbank to unload
//
//OUT: bool - returns true if the soundbank was successfully unloaded
bool AudioManager::UnloadSoundBank(const wchar_t* szBankName) {
if (m_RegisteredSoundBanks.find(szBankName) != m_RegisteredSoundBanks.end()) {

if (SoundEngine::UnloadBank(m_RegisteredSoundBanks[szBankName], nullptr, nullptr) != AK_Success) {
return false;
}
else {
m_RegisteredSoundBanks.erase(szBankName);
}
}
else {
return false;
}

return true;
}

void AudioManager::StopPlayingID(AkPlayingID in_playingID, AkTimeMs in_uTransitionDuration, AkCurveInterpolation in_eFadeCurve)
{
	AK::SoundEngine::StopPlayingID(in_playingID);
}

void AudioManager::StopAllAudio(AkGameObjectID in_gameObjectID)
{
	AK::SoundEngine::StopAll(in_gameObjectID);
}

bool AudioManager::IsEntityRegistered(const GameObject* _gameObject)
{
	for (unsigned int i = 0; i < m_RegisteredEntities.size(); i++)
	{
		if (m_RegisteredEntities[i] == _gameObject)
		{
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------
//DESCRIPTION: set world sound scale
//
//IN: float - scale to set
//
//OUT: void
void AudioManager::SetWorldScale(float fScale) {
m_fWorldScale = fScale;
for (unsigned int i = 0; i < m_RegisteredListeners.size(); ++i) {
AK::SoundEngine::SetListenerScalingFactor(i, m_fWorldScale);
}
}
//--------------------------------------------------------------------------------
#ifdef _DEBUG
void AudioManager::SendMessage(const char* message) {
Monitor::PostString(message, Monitor::ErrorLevel_Message);
}
#endif
//--------------------------------------------------------------------------------
