
#pragma once

namespace Urho3D
{
	class Node;
	class Scene;
}

class Character;
class PlayerMissile;

#include "Sample.h"
#include "BoidManager.h"

#include <memory>
#include <iostream>

class DinosaurGame : public Sample
{
    URHO3D_OBJECT(DinosaurGame, Sample);

public:
    DinosaurGame(Context* context);
    ~DinosaurGame();

    virtual void Start();

private:
	enum class NetworkRole { CLIENT, SERVER, OFFLINE };

	bool firstPerson_;

	// - - - - - - - - - - - - -
	std::unique_ptr<BoidManager> boids;

	NetworkRole networkRole;
	Controls clientControls;

	Character* character;
	PlayerMissile* missile;	

	int clientScore;
	int clientBoidCount;

	unsigned int nodeID				= 999999999;
	const unsigned int SERVER_PORT	= 2345;
	float menuUpdateTimer			= 1.0f;

	bool usingFreeCamera	= false;
	bool hasCharacter		= false;

	// UI
	std::unique_ptr<PauseMenu> pauseMenu;
	std::unique_ptr<MainMenu> mainMenu;
	std::unique_ptr<DebugWindow> debugWindow;
	std::unique_ptr<ScoreWindow> scoreWindow;
	std::unique_ptr<ControlsWindow> controlsWindow;

	HashMap<Connection*, WeakPtr<Node> > charactersTable;

	// Game Entry Point
	void StartGame();

	// Init
	void InitialiseInterface();

	// Subscriptions
	void SubscribeToGameEvents();
	void SubscribeToServerEvents();
	void SubscribeToClientEvents();

	void UnsubscribeToGameEvents();

	// Collision Checks
	void CheckCharacterCollisions(Character* chara);

	// Network
	void ConnectToServer(String);
	void StartServer();

	// Menus
	void SetupMainMenu();
	void SetupPauseMenu();

	// Misc. States
	void ToggleFullscreen();
	void ToggleGamePause();

	// Scenes
	void CreateOfflineScene();
	void CreateClientScene();
	void CreateServerScene();

	// Scene Objects
	Character* CreateCharacter();
	void CreateZone(CreateMode);
	void CreateScene(CreateMode);
	void CreateCamera();
	void CreateMushroom();
	void CreateLighting();
	void CreateFloor();

	// Updates
	bool UpdateUI(float, int, int, int);
	void UpdateClientsUI();
	void UpdateFreeCamera(float);
	void UpdateShoulderCamera(float);

	void UpdateCharacterControls();
	void UpdateClientCharacterControls();

	//
	void ShootMissile();

	// Main Menu Callbacks
	void MM_OfflinePlayBtnDown(StringHash, VariantMap&);
	void MM_HostGameBtnDown(StringHash, VariantMap&);
	void MM_JoinGameBtnDown(StringHash, VariantMap&);
	void MM_QuitGameBtnDown(StringHash, VariantMap&);

	// Pause Menu Callbacks
	void PM_DisconnectBtnDown(StringHash, VariantMap&);
	void PM_StartSvrBtnDown(StringHash, VariantMap&);
	void PM_JoinSvrBtnDown(StringHash, VariantMap&);
	void PM_ContinueBtnDown(StringHash, VariantMap&);
	void PM_QuitBtnDown(StringHash, VariantMap&);

	// Game Event Callbacks
	void HandleUpdate(StringHash, VariantMap&);
	void HandlePostUpdate(StringHash, VariantMap&);
	void HandleKeyUp(StringHash, VariantMap&);
	void HandlePhysicsPreStep(StringHash, VariantMap&);
	void HandleMouseDown(StringHash, VariantMap&);

	// Network Methods
	void ProcessClientControls();

	// Network Callbacks
	void HandleClientConnected(StringHash, VariantMap&);
	void HandleClientDisconnected(StringHash, VariantMap&);
	void HandleClientSceneLoaded(StringHash, VariantMap&);

	// Client Only Callbacks
	void HandleServerDisconnect(StringHash, VariantMap&);
	void HandleCharacterAllocation(StringHash, VariantMap&);
	void HandleInterfaceUpdate (StringHash, VariantMap&);

	// Misc.
	void AddConsole();
	void Quit();
	// - - - - - - - - - - - - -
};