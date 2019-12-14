
#pragma once

namespace Urho3D
{
	class Node;
	class Scene;
}

#include "Sample.h"
#include "BoidManager.h"

#include <memory>
#include <iostream>

#include "Player.h"

class DinosaurGame : public Sample
{
    URHO3D_OBJECT(DinosaurGame, Sample);

public:
    DinosaurGame(Context* context);
    ~DinosaurGame();

    virtual void Start();

private:
	enum class NetworkRole { CLIENT, SERVER, OFFLINE };
	enum class CameraMode { FREE, SHOULDER };

	struct ClientUI {
		bool isUpdated	= false;
		int numBoids	= 0;
		int score		= 0;
	};

	bool firstPerson_;

	// - - - - - - - - - - - - -
	std::unique_ptr<BoidManager> boids;
	HashMap<Connection*, Player> clients;

	ClientUI clientUI;
	Player player;

	NetworkRole networkRole;
	CameraMode cameraMode;

	const unsigned int SERVER_PORT	= 2345;

	float menuTimer	= 1.0f;

	// - - -
	void SubscribeToGameEvents();
	void SubscribeToNetworkEvents_Client();
	void SubscribeToGameEvents_Client();
	void SubscribeToGameEvents_Server();

	void UpdateControls(Controls&);
	void UpdateControls_Client();
	void UpdateControls_Server();

	void UpdateClientInterface_Server();
	void UpdateInterface_Server(float);
	void UpdateInterface_Client(float);

	void HandleUpdate_Client(StringHash, VariantMap&);
	void HandleUpdate_Server(StringHash, VariantMap&);

	void HandlePhysicsPreStep_Client(StringHash, VariantMap&);
	void HandlePhysicsPreStep_Server(StringHash, VariantMap&);

	void SetupGame();
	void StartGame_Client();
	void StartGame_Server();

	void CreateScene_Client();
	void CreateScene_Server();

	void HandleInterfaceUpdate_Client(StringHash, VariantMap&);

	void HandleMouseDown_Server(StringHash, VariantMap&);

	void HandlePostUpdate(StringHash, VariantMap&);
	void HandleKeyUp(StringHash, VariantMap&);
	// - - -

	// UI
	std::unique_ptr<PauseMenu> pauseMenu;
	std::unique_ptr<MainMenu> mainMenu;
	std::unique_ptr<DebugWindow> debugWindow;
	std::unique_ptr<ScoreWindow> scoreWindow;
	std::unique_ptr<ControlsWindow> controlsWindow;

	// Init
	void InitialiseInterface();

	void UnsubscribeToGameEvents();

	void CheckCharacterCollisions(Character* chara);
	void CheckProjectileCollisions(Player player);

	// Network
	void ConnectToServer(String);
	void StartServer();

	// Menus
	void SetupAndShowMainMenu();
	void SetupPauseMenu();

	// Misc. States
	void ToggleFullscreen();
	void ToggleGamePause();

	// Scenes
	void CreateOfflineScene();

	// Updates
	void UpdateFreeCamera(float);
	void UpdateShoulderCamera(float);

	//
	void ShootMissile();

	// Main Menu Callbacks
	void MM_OfflinePlayBtnDown(StringHash, VariantMap&);
	void MM_HostGameBtnDown(StringHash, VariantMap&);
	void JoinGameFromMainMenu(StringHash, VariantMap&);
	void MM_QuitGameBtnDown(StringHash, VariantMap&);

	// Pause Menu Callbacks
	void PM_DisconnectBtnDown(StringHash, VariantMap&);
	void PM_JoinSvrBtnDown(StringHash, VariantMap&);
	void PM_ContinueBtnDown(StringHash, VariantMap&);
	void PM_QuitBtnDown(StringHash, VariantMap&);

	// Network Methods
	void ProcessClientControls();

	// Network Callbacks
	void HandleClientConnected(StringHash, VariantMap&);
	void HandleClientDisconnected(StringHash, VariantMap&);
	void HandleClientSceneLoaded(StringHash, VariantMap&);

	void HandleCharacterAllocation(StringHash, VariantMap&);

	// Misc.
	void AddConsole();
	void Quit();

	// Scene Objects
	Character* CreateCharacter();
	void CreateZone(CreateMode);
	void CreateScene(CreateMode);
	void CreateCamera();
	void CreateMushroom();
	void CreateLighting();
	void CreateFloor();
};