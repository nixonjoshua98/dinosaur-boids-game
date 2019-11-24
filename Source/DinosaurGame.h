
#pragma once

#include "Sample.h"

namespace Urho3D
{
	class Node;
	class Scene;
}

class Character;
class Touch;

#include "BoidManager.h"

#include <memory>
#include <iostream>

enum class NetworkRole
{
	CLIENT,
	SERVER,
	OFFLINE
};


class DinosaurGame : public Sample
{
    URHO3D_OBJECT(DinosaurGame, Sample);

public:
    DinosaurGame(Context* context);
    ~DinosaurGame();

    virtual void Start();

private:
	// Default
	SharedPtr<Touch> touch_;
	bool firstPerson_;

	// - - - - - - - - - - - - -
	int clientScore, clientBoidCount;

	bool hasCharacter = false;

	std::unique_ptr<BoidManager> boids;

	void UpdateCharacterControls();

	bool UpdateUI(float, int, int, int);
	void UpdateClientsUI();

	void PM_DisconnectBtnDown(StringHash, VariantMap&);
	void PM_StartSvrBtnDown(StringHash, VariantMap&);
	void PM_JoinSvrBtnDown(StringHash, VariantMap&);

	void HandleServerDisconnect(StringHash, VariantMap&);

	void UnsubscribeToGameEvents();

	// -------------------------


	SharedPtr<Cursor> cursor;

	NetworkRole networkRole;

	Character* character;
	Controls clientControls;

	unsigned int nodeID = 999999999;

	const unsigned int SERVER_PORT = 2345;

	bool usingFreeCamera	= false;
	float menuUpdateTimer	= 1.0f;

	int fps = 0;

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

	// Collision Checks
	void CheckCharacterCollisions(Character* chara);
	void CheckMissileCollisions();

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
	void UpdateUI(float);
	void UpdateFreeCamera(float);
	void UpdateShoulderCamera(float);

	void UpdateServerCharacterControls();
	void UpdateClientCharacterControls();

	// Main Menu Callbacks
	void MM_OfflinePlayBtnDown(StringHash, VariantMap&);
	void MM_HostGameBtnDown(StringHash, VariantMap&);
	void MM_JoinGameBtnDown(StringHash, VariantMap&);
	void MM_QuitGameBtnDown(StringHash, VariantMap&);

	// Pause Menu Callbacks
	void PM_ContinueBtnDown(StringHash, VariantMap&);
	void PM_QuitBtnDown(StringHash, VariantMap&);

	// Game Event Callbacks
	void HandleUpdate(StringHash, VariantMap&);
	void HandlePostUpdate(StringHash, VariantMap&);
	void HandleKeyUp(StringHash, VariantMap&);
	void HandlePhysicsPreStep(StringHash, VariantMap&);
	void HandleMouseDown(StringHash, VariantMap&);
	void HandlePostRenderUpdate(StringHash, VariantMap&);

	// Network Methods
	void ProcessClientControls();

	// Network Callbacks
	void HandleClientConnected(StringHash eventType, VariantMap& eventData);
	void HandleClientDisconnected(StringHash eventType, VariantMap& eventData);

	// Client Only Callbacks
	void HandleCharacterAllocation(StringHash eventType, VariantMap& eventData);
	void HandleInterfaceUpdate (StringHash eventType, VariantMap& eventData);
	void HandleClientSceneLoaded(StringHash eventType, VariantMap& eventData);

	// Misc.
	void AddConsole();
	void Quit();
	// - - - - - - - - - - - - -
};
