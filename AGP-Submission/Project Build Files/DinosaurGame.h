
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

	const float GAME_TIMER = 31.f;

	struct ClientUI {
		bool isUpdated	= false;
		int numBoids	= 0;
		int score		= 0;
		String weaponName;
		int timeLeft;
	};

	bool firstPerson_;

	std::unique_ptr<BoidManager> boids;
	HashMap<Connection*, Player> clients;

	ClientUI clientUI;
	Player player;

	std::vector<Node*> borderNodes;

	SoundSource* source;

	NetworkRole networkRole;
	CameraMode cameraMode;

	const unsigned int SERVER_PORT = 2345;

	float splashScreenTimer = 3.0f;

	float menuTimer	= 0.5f;

	float gameTimer = GAME_TIMER;

	bool isSceneCreated = false;

	void CreateGameBorder();

	void SplashScreenUpdate(StringHash, VariantMap& eventData);

	void SubscribeToGameEvents();
	void SubscribeToNetworkEvents_Client();
	void SubscribeToGameEvents_Client();
	void SubscribeToGameEvents_Offline();
	void SubscribeToGameEvents_Server();

	void StartMusic();

	void UpdateControls(Controls&);
	void UpdateControls_Client();
	void UpdateControls_Server_Offline();

	void _SendMessage_Client(StringHash, VariantMap&);
	void _SendMessage_Server(StringHash, VariantMap&);

	void HandleIncomingMessage_Server(StringHash, VariantMap&);
	void HandleIncomingMessage_Client(StringHash, VariantMap&);

	void OnGameEnd_Offline();
	void OnGameEnd_Server();
	void OnGameEnd_Client(StringHash, VariantMap&);

	void UpdateClientInterface_Server();
	void UpdateInterface_Server(float);
	void UpdateInterface_Client(float);
	void UpdateInterface_Offline(float);

	void HandleUpdate_Client(StringHash, VariantMap&);
	void HandleUpdate_Server(StringHash, VariantMap&);
	void HandleUpdate_Offline(StringHash, VariantMap&);

	void HandleClientWeaponChange_Server(StringHash, VariantMap&);

	void HandleServerDisconnect_Client(StringHash, VariantMap&);

	void HandlePhysicsPreStep_Client(StringHash, VariantMap&);
	void HandlePhysicsPreStep_Server(StringHash, VariantMap&);

	void SetupGame();
	void StartGame_Offline();
	void StartGame_Client();
	void StartGame_Server();

	void CreateScene_Client();
	void CreateScene_Server();

	void HandleInterfaceUpdate_Client(StringHash, VariantMap&);

	void HandleMouseDown_Server_Offline(StringHash, VariantMap&);

	void HandlePostUpdate(StringHash, VariantMap&);

	void HandleKeyUp(StringHash, VariantMap&);
	void HandleKeyUp_Server_Offline(StringHash, VariantMap&);
	void HandleKeyUp_Client(StringHash, VariantMap&);

	void DisconnectFromServer();

	void CreateSkyBox();

	// UI
	std::unique_ptr<PauseMenu> pauseMenu;
	std::unique_ptr<MainMenu> mainMenu;
	std::unique_ptr<DebugWindow> debugWindow;
	std::unique_ptr<ScoreWindow> scoreWindow;
	std::unique_ptr<ControlsWindow> controlsWindow;
	std::unique_ptr<WeaponWindow> weaponWindow;
	std::unique_ptr<TimerWindow> timeWindow;
	std::unique_ptr<GameoverWindow> gameoverWindow;
	std::unique_ptr<ChatWindow> chatWindow;

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

	void ClearScene();

	// Updates
	void UpdateFreeCamera(float);
	void UpdateShoulderCamera(float);

	// Main Menu Callbacks
	void MM_OfflinePlayBtnDown(StringHash, VariantMap&);
	void MM_HostGameBtnDown(StringHash, VariantMap&);
	void JoinGameFromMainMenu(StringHash, VariantMap&);
	void MM_QuitGameBtnDown(StringHash, VariantMap&);

	// Pause Menu Callbacks
	void GoToMainMenuFromGame(StringHash, VariantMap&);
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

	Node* CreateMushroom();
	Node* CreateBox();
	Node* CreateCharacterModel();

	void CreateLighting();
	void CreateFloor();
};