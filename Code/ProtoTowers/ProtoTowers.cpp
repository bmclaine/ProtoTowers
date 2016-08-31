#include "stdafx.h"
#include "ProtoTowers.h"

#define MAX_LOADSTRING 100

// Global Variables:							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

#if _DEBUG
//	#include <vld.h>
#endif

//TempInclude
#include "Assets\Material.h"
#include "Assets\Mesh.h"
#include <ctime>
#include <future>

#include "Assets\Texture.h"
#include "UIComponents\UIButton.h"
#include "UIComponents\UICanvas.h"
#include "UIComponents\UIEventSystem.h"
#include "UIComponents\UIRenderer.h"
#include "UIComponents\UIText.h"
#include "..\ProtoTowers\Components\MainMenuScript.h"
#include "..\ProtoTowers\Components\CreditsMenuScript.h"
#include "..\ProtoTowers\Components\3rdPersonCamera.h"
#include "..\ProtoTowers\Components\MinionSpawner.h"
#include "UIElement.h"
#include "ThreadPool\WorkerTypes\WorkerThreadFuctionPointers.h"

#include "Core\SplashScreen.h"

#include "Components\BaseWeapon.h"
#include "Components\MiniMinion.h"
#include "Components\BossMinion.h"
#include "Components\TankMinion.h"
#include "Components\TurretProjectile.h"
#include "Components\BaseTurret.h"
#include "Components\ResourceCrate.h"
#include "Components\BaseProjectile.h"
#include "Components\MainBase.h"
#include "Components\TowerNode.h"
#include "Components\Resource.h"
#include "Components\FreezeEffect.h"
#include "Components\FreezeTower.h"
#include "Components\PlasmaBlaster.h"
#include "Components\Barrier.h"
#include "Components\RangedMinion.h"
#include "Components\EndGame.h"
#include "Components\PlayerHead.h"
#include "Components\SpringArm.h"
#include "Components\StunGernadeAbility.h"
#include "Components\StunEffect.h"
#include "Components\PlayerShieldScript.h"
#include "Components\ShieldAbility.h"
#include "Components\WeaponBuffAbility.h"
#include "Components\WeaponBuffScript.h"
#include "Components\MinionBuffAbility.h"
#include "Components\MinionBuffEffect.h"
#include "Components\ControlPoint.h"
#include "Components\FrostHUDEffectPrefab.h"
#include "Components\FadeAwayEffectPrefab.h"
#include "Components\HelpScreenPrefab.h"
#include "Components\PauseMenuPrefab.h"
#include "Components\PlayerHUD.h"
#include "Components\TowerBuildMenuPrefab.h"
#include "Components\LoseHUDPrefab.h"
#include "Components\WinHUDPrefab.h"
#include "Components\MainMenuPrefab.h"
#include "Components\CreditsMenuPrefab.h"
#include "Components\BossEvent.h"
#include "Components\PlayerShieldScript.h"
#include "Components\DebugProjectile.h"
#include "Components\Waypoint.h"
#include "Components\BossMinionMace.h"
#include "Components\HealthPickup.h"
#include "Components\AlertMessagePrefab.h"
#include "Components\BossEvent.h"
#include "Components\Waypoint.h"
#include "Components\TowerOptionsPrefab.h"
#include "Components\BarrierButton.h"
#include "Components\CreditsMenuPrefab.h"
#include "Components\DelayCredits.h"
#include "Components\ControlPointProgressBarPrefab.h"
#include "Core\FreeCameraController.h"
#include "Components\BossMinionMace.h"
#include "Components\OptionsMenuPrefab.h"
#include "Components\MortarProjectile.h"
#include "Components\MortarTower.h"
#include "Components\BehaviorTree.h"
#include "Components\MinionSpawnPoint.h"
#include "Components\AOEDamage.h"
#include "Components\PoisonEffect.h"
#include "Components\PoisonSprayArea.h"
#include "Components\PoisonTurret.h"
#include "Components\PoisonSprayProjectile.h"
#include "Components\DebugProjectile.h"
#include "Components\BackgroundMusic.h"
#include "Components\PoisonAOEProjectile.h"
#include "Components\MinionBuffWave.h"
#include "Components\ControllerDisconnectPrefab.h"
#include "Components\PopupAlert.h"
#include "Components\CutScene.h"
#include "Components\CutSceneWaypoint.h"
#include "Components\FirstMinion.h"
#include "Components\FirstTurret.h"
#include "Components\MinionWaveSpawn.h"
#include "Components\ToolTipPrefab.h"
#include "Components\SkipTutorial.h"
#include "Components\RaiseBarrier.h"
#include "Components\FirstTurretUpgrade.h"
#include "Components\TutorialPrompt.h"
#include "Components\FreezeBlastWave.h"
#include "Components\ResourceCrateSpawner.h"
#include "Components\AfterCutscenePrompt.h"
#include "Components\PlayerRecall.h"
#include "Components\MortarClusterProjectile.h"
#include "Components\MinionSpawnerShield.h"
#include "Components\TowerUpgradeEffect.h"
#include "Components\BuffMinionSpawner.h"
#include "Components\AbilityFlyInPrefab.h"

#include "ModuleInterface.h"

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	srand((unsigned int)time(0));

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PROTOTOWERS, szWindowClass, MAX_LOADSTRING);

	Inferno::InitializeEngine(
		hInstance,
		MAKEINTRESOURCE(IDI_PROTOTOWERS),
		MAKEINTRESOURCE(IDI_SMALL),
		szWindowClass
		);

	// === Register All Input Actions (Should be moved later on)
#pragma region Input_Setup
	InputManager::RegisterBinding("Forward", Button::None, Button::None, Key::W, Key::S, Button::None, Button::None, Axis::LeftStick_Y, 0);
	InputManager::RegisterBinding("Right", Button::None, Button::None, Key::D, Key::A, Button::None, Button::None, Axis::LeftStick_X, 0);
	InputManager::RegisterBinding("RotationX", Button::None, Button::None, Key::None, Key::None, Button::None, Button::None, Axis::RightStick_X, 0, true);
	InputManager::RegisterBinding("RotationY", Button::None, Button::None, Key::None, Key::None, Button::None, Button::None, Axis::RightStick_Y, 0);
	InputManager::RegisterBinding("Shoot", Button::LeftClick, Button::None, Key::Space, Key::None, Button::None, Button::None, Axis::RightTrigger, 0);
	InputManager::RegisterBinding("SecondaryShoot", Button::RightClick, Button::None, Key::None, Key::None, Button::None, Button::None, Axis::LeftTrigger, 0);
	InputManager::RegisterBinding("Recall", Button::None, Button::None, Key::R, Key::None, Button::RightStick, Button::None, Axis::None, 0);
	InputManager::RegisterBinding("Dash", Button::None, Button::None, Key::Shift, Key::None, Button::LeftStick, Button::None, Axis::None, 0);
	InputManager::RegisterBinding("AbilityOne", Button::None, Button::None, Key::N1, Key::None, Button::LeftBumper, Button::None, Axis::None, 0);
	InputManager::RegisterBinding("AbilityTwo", Button::None, Button::None, Key::N2, Key::None, Button::RightBumper, Button::None, Axis::None, 0);
	InputManager::RegisterBinding("AbilityThree", Button::None, Button::None, Key::N3, Key::None, Button::X, Button::None, Axis::None, 0);
	InputManager::RegisterBinding("AbilityFour", Button::None, Button::None, Key::N4, Key::None, Button::Y, Button::None, Axis::None, 0);
	InputManager::RegisterBinding("Interact", Button::None, Button::None, Key::E, Key::Enter, Button::A);
	InputManager::RegisterBinding("SecondaryInteract", Button::None, Button::None, Key::Q, Key::None, Button::B);
	InputManager::RegisterBinding("Escape", Button::None, Button::None, Key::Escape);
	InputManager::RegisterBinding("Pause", Button::None, Button::None, Key::P, Key::None, Button::Start);
	InputManager::RegisterBinding("Help", Button::None, Button::None, Key::F1, Key::None, Button::Back);
	InputManager::RegisterBinding("Next", Button::None, Button::None, Key::E, Key::None, Button::None, Button::None, Axis::RightTrigger, 0);
	InputManager::RegisterBinding("Previous", Button::None, Button::None, Key::Q, Key::None, Button::None, Button::None, Axis::LeftTrigger, 0);
	InputManager::RegisterKey(Key::Control); // Used for cheat input
	InputManager::RegisterKey(Key::NumPad0); // Used for cheat input
	InputManager::RegisterKey(Key::NumPad1); // Used for cheat input
	InputManager::RegisterKey(Key::NumPad2); // Used for cheat input
	InputManager::RegisterKey(Key::NumPad3); // Used for cheat input
	InputManager::RegisterBinding("Cheat_GodMode", Button::None, Button::None, Key::None, Key::None, Button::DPad_Up, Button::None, Axis::None, 0);
	InputManager::RegisterBinding("Cheat_SpawnResources", Button::None, Button::None, Key::None, Key::None, Button::DPad_Down, Button::None, Axis::None, 0);
	InputManager::RegisterBinding("Cheat_KillEnemyWave", Button::None, Button::None, Key::None, Key::None, Button::DPad_Left, Button::None, Axis::None, 0);
	InputManager::RegisterBinding("Cheat_SpawnFriendlyWave", Button::None, Button::None, Key::None, Key::None, Button::DPad_Right, Button::None, Axis::None, 0);

#if defined (_DEBUG)
	InputManager::RegisterBinding("Debug", Button::None, Button::None, Key::Question);
#endif

#pragma endregion
	// ===

	// === Component Registry
#pragma region Register_Components
	using namespace Inferno_Components;
	RegisterComponent<FreezeEffect>("FreezeEffect");
	RegisterComponent<BossMinion>("BossMinion");
	RegisterComponent<MiniMinion>("MiniMinion");
	RegisterComponent<RangedMinion>("RangedMinion");
	RegisterComponent<TankMinion>("TankMinion");
	RegisterComponent<ThirdPersonCamera>("ThirdPersonCamera");
	RegisterComponent<Player>("Player");
	RegisterComponent<PlayerController>("PlayerController");
	RegisterComponent<PlayerHead>("PlayerHead");
	RegisterComponent<SpringArm>("SpringArm");
	RegisterComponent<StunGrenadeAbility>("StunGrenadeAbility");
	RegisterComponent<StunEffect>("StunEffect");
	RegisterComponent<PlayerShieldScript>("PlayerShield");
	RegisterComponent<ShieldAbility>("ShieldAbility");
	RegisterComponent<WeaponBuffAbility>("WeaponBuffAbility");
	RegisterComponent<WeaponBuffScript>("WeaponBuff");
	RegisterComponent<MinionBuffAbility>("MinionBuffAbility");
	RegisterComponent<MinionBuffEffect>("MinionBuff");
	RegisterComponent<BaseTurret>("BaseTurret");
	RegisterComponent<FreezeTower>("FreezeTower");
	RegisterComponent<MainBase>("MainBase");
	RegisterComponent<MinionSpawner>("MinionSpawner");
	RegisterComponent<Barrier>("Barrier");
	RegisterComponent<ControlPoint>("ControlPoint");
	RegisterComponent<EndGame>("EndGame");
	RegisterComponent<Resource>("Resource");
	RegisterComponent<ResourceCrate>("ResourceCrate");
	RegisterComponent<TowerNode>("TowerNode");
	RegisterComponent<BaseProjectile>("BaseProjectile");
	RegisterComponent<TurretProjectile>("TurretProjectile");
	RegisterComponent<BaseWeapon>("BaseWeapon");
	RegisterComponent<PlasmaBlaster>("PlasmaBlaster");
	RegisterComponent<FadeAwayEffectPrefab>("FadeAwayEffectPrefab");
	RegisterComponent<HelpScreenPrefab>("HelpScreenPrefab");
	RegisterComponent<PauseMenuPrefab>("PauseMenuPrefab");
	RegisterComponent<PlayerHUD>("PlayerHUD");
	RegisterComponent<TowerBuildMenuPrefab>("TowerBuildPrefab");
	RegisterComponent<FrostHUDEffectPrefab>("FrostHUDEffectPrefab");
	RegisterComponent<LoseHUDPrefab>("LoseHUDPrefab");
	RegisterComponent<WinHUDPrefab>("WinHUDPrefab");
	RegisterComponent<MainMenuPrefab>("MainMenuPrefab");
	RegisterComponent<CreditsMenuPrefab>("CreditsMenuPrefab");
	RegisterComponent<HealthPickup>("HealthPickup");
	RegisterComponent<FreeCameraController>("FreeCameraController");
	RegisterComponent<AlertMessagePrefab>("AlertMessagePrefab");
	RegisterComponent<BossEvent>("BossEvent");
	RegisterComponent<Waypoint>("Waypoint");
	RegisterComponent<TowerOptionsPrefab>("TowerOptionsPrefab");
	RegisterComponent<BarrierButton>("BarrierButton");
	RegisterComponent<CreditsMenuPrefab>("CreditsMenu");
	RegisterComponent<DelayCredits>("DelayedCredits");
	RegisterComponent<ControlPointProgressBarPrefab>("ControlPointProgressbar");
	RegisterComponent<BossMinionMace>("BossMinionMace");
	RegisterComponent<OptionsMenuPrefab>("OptionsMenu");
	RegisterComponent<MortarProjectile>("MortarProjectile");
	RegisterComponent<MortarTower>("MortarTower");
	RegisterComponent<MinionSpawnPoint>("Minion SpawnPoint");
	RegisterComponent<AOEDamage>("AOEDamage");
	RegisterComponent<PoisonEffect>("PoisonEffect");
	RegisterComponent<PoisonSprayArea>("PoisonSprayArea");
	RegisterComponent<PoisonTurret>("PoisonTurret");
	RegisterComponent<PoisonSprayProjectile>("PoisonSprayProjectile");
	RegisterComponent<PoisonAOEProjectile>("PoisonAOEProjectile");
	RegisterComponent<DebugProjectile>("DebugProjectile");
	RegisterComponent<BackgroundMusic>("BackgroundMusic");
	RegisterComponent<BehaviorTree>("BehaviorTree");
	RegisterComponent<MinionBuffWave>("MinionBuffWave");
	RegisterComponent<ControllerDisconnectPrefab>("DisconnectedController");
	RegisterComponent<PopupAlert>("PopupAlert");
	RegisterComponent<CutScene>("CutScene");
	RegisterComponent<CutSceneWaypoint>("CutSceneWaypoint");
	RegisterComponent<FirstMinion>("FirstMinion");
	RegisterComponent<FirstTurret>("FirstTurret");
	RegisterComponent<MinionWaveSpawn>("MinionWaveSpawn");
	RegisterComponent<ToolTipPrefab>("ToolTipPrefab");
	RegisterComponent<SkipTutorial>("SkipTutorial");
	RegisterComponent<RaiseBarrier>("RaiseBarrier");
	RegisterComponent<FirstTurretUpgrade>("FirstTurretUpgrade");
	RegisterComponent<TutorialPrompt>("TutorialPrompt");
	RegisterComponent<FreezeBlastWave>("FreezeBlastWave");
	RegisterComponent<ResourceCrateSpawner>("ResourceCrateSpawner");
	RegisterComponent<AfterCutscenePrompt>("AfterCutscenePrompt");
	RegisterComponent<PlayerRecall>("PlayerRecall");
	RegisterComponent<MortarClusterProjectile>("MortarClusterProjectile");
	RegisterComponent<MinionSpawnerShield>("MinionSpawnerShield");
	RegisterComponent<TowerUpgradeEffect>("TowerUpgradeEffect");
	RegisterComponent<BuffMinionSpawner>("BuffMinionSpawner");
	RegisterComponent<AbilityFlyInPrefab>("AbilityFlyIn");

#pragma endregion
	// ===

	Inferno::ResetTime();

#pragma region Splash and Initial Asset Loading
	SplashScreen splash;
	{
		std::future<bool> assestsLoaded;

		WorkerThreadFuctionPointers* work = static_cast<WorkerThreadFuctionPointers*>(Inferno::LoadAssets(&assestsLoaded));

		auto status = assestsLoaded.wait_for(std::chrono::milliseconds(0));
		while (!(splash.IsFinished() && status == std::future_status::ready))
		{
			status = assestsLoaded.wait_for(std::chrono::milliseconds(0));

			Inferno::UpdateEnginePreAssets();
			splash.Update();
		}

		SAFE_DELETE(work);
	}
#pragma endregion

	InfernoEngine_SceneManagement::LoadScene("MainMenuScene");

	// ===

	while (Inferno::UpdateEngine());

	// === Unregister All Input ( Should Be moved later )
	InputManager::UnregisterKey(Key::Escape);
	InputManager::UnregisterBinding("Forward");
	InputManager::UnregisterBinding("Right");
	InputManager::UnregisterBinding("RotationX");
	InputManager::UnregisterBinding("RotationY");
	InputManager::UnregisterBinding("Shoot");
	// ===
	
	Inferno::DestroyEngine();

	return 0/*(int) msg.wParam*/;
}