// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

/***********************************
 *      Inferno Engine Includes    *
 ***********************************/
// Anything the Game should know about should be included here
#include "Engine.h"
#include "InfernoTime.h"
#include "InfernoTimer.h"
#include "CameraManager.h"
#include "GameObject.h"
#include "AssertHelper.h"
#include "Assets\Texture.h"
#include "Assets\Mesh.h"
#include "Components\Transform.h"
#include "Components\Camera.h"
#include "Components\MeshRenderer.h"
#include "Components\SkinnedMeshRenderer.h"
#include "Components\SpriteRenderer.h"
#include "Components\Animator.h"
#include "Components\NavAgent.h"
#include "Components\Light.h"
#include "Components\ParticleSystem.h"
#include "Physics\BoxCollider.h"
#include "Physics\CapsuleCollider.h"
#include "Physics\SphereCollider.h"
#include "Physics\Rigidbody.h"

// TODO: This needs to be removed when we get a proper way to close the engine game-side
#include "InputManager.h"

/***********************************
 *        Custom Components        *
 ***********************************/
// Any components which are not in the engine
#include "..\ProtoTowers\Core\PlayerController.h"
#include "..\ProtoTowers\Components\Player.h"
#include "..\ProtoTowers\Core\WWise_IDs.h"
//#include "..\ProtoTowers\Components\3rdPersonCamera.h"