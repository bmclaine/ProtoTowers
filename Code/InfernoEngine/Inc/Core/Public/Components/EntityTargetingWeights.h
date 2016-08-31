///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Target weights for each entity.
//              These are used by the AI agents to compute their best active target.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#define USE_WEIGHTS		1

namespace AI_Weights
{

	// Max Weights
	/// Structures
#define MAX_WEIGHT_MAIN_BASE		50.0f
#define MAX_WEIGHT_SPAWNER			12.0f
	/// Turrets
#define MAX_WEIGHT_AUTO_TURRET		10.0f
#define MAX_WEIGHT_POISON_TURRET	10.0f
#define MAX_WEIGHT_FREEZE_TURRET	10.0f
#define MAX_WEIGHT_MORTAR_TURRET	10.0f
#define MAX_WEIGHT_LASER_TURRET		10.0f
	/// AIEntities
#define MAX_WEIGHT_MINI_MINION		2.0f
#define MAX_WEIGHT_RANGED_MINION	3.0f
#define MAX_WEIGHT_TANK_MINION		4.0f
#define MAX_WEIGHT_BOSS_MINION		7.0f

	// Influnce
#if USE_WEIGHTS

	/// Structures
#define INFLUENCE_MAIN_BASE			0.0f
#define INFLUENCE_SPAWNER			0.0f
	/// Turrets
#define INFLUENCE_AUTO_TURRET		0.0f	//10.0f
#define INFLUENCE_POISON_TURRET		0.0f	//10.0f
#define INFLUENCE_FREEZE_TURRET		0.0f	//5.0f
#define INFLUENCE_MORTAR_TURRET		0.0f	//10.0f
#define INFLUENCE_LASER_TURRET		0.0f	//10.0f
	/// AIEntities
#define INFLUENCE_MINI_MINION		1.0f
#define INFLUENCE_RANGED_MINION		3.0f
#define INFLUENCE_TANK_MINION		3.0f
#define INFLUENCE_BOSS_MINION		5.0f

#else

	/// Structures
#define INFLUENCE_MAIN_BASE			0.0f
#define INFLUENCE_SPAWNER			0.0f
	/// Turrets
#define INFLUENCE_AUTO_TURRET		0.0f
#define INFLUENCE_POISON_TURRET		0.0f
#define INFLUENCE_FREEZE_TURRET		0.0f
#define INFLUENCE_MORTAR_TURRET		0.0f
#define INFLUENCE_LASER_TURRET		0.0f
	/// AIEntities
#define INFLUENCE_MINI_MINION		0.0f
#define INFLUENCE_RANGED_MINION		0.0f
#define INFLUENCE_TANK_MINION		0.0f
#define INFLUENCE_BOSS_MINION		0.0f


#endif

}
