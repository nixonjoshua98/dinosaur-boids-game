
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

protected:
    virtual String GetScreenJoystickPatchString() const { return
        "<patch>"
        "    <add sel=\"/element\">"
        "        <element type=\"Button\">"
        "            <attribute name=\"Name\" value=\"Button3\" />"
        "            <attribute name=\"Position\" value=\"-120 -120\" />"
        "            <attribute name=\"Size\" value=\"96 96\" />"
        "            <attribute name=\"Horiz Alignment\" value=\"Right\" />"
        "            <attribute name=\"Vert Alignment\" value=\"Bottom\" />"
        "            <attribute name=\"Texture\" value=\"Texture2D;Textures/TouchInput.png\" />"
        "            <attribute name=\"Image Rect\" value=\"96 0 192 96\" />"
        "            <attribute name=\"Hover Image Offset\" value=\"0 0\" />"
        "            <attribute name=\"Pressed Image Offset\" value=\"0 0\" />"
        "            <element type=\"Text\">"
        "                <attribute name=\"Name\" value=\"Label\" />"
        "                <attribute name=\"Horiz Alignment\" value=\"Center\" />"
        "                <attribute name=\"Vert Alignment\" value=\"Center\" />"
        "                <attribute name=\"Color\" value=\"0 0 0 1\" />"
        "                <attribute name=\"Text\" value=\"Gyroscope\" />"
        "            </element>"
        "            <element type=\"Text\">"
        "                <attribute name=\"Name\" value=\"KeyBinding\" />"
        "                <attribute name=\"Text\" value=\"G\" />"
        "            </element>"
        "        </element>"
        "    </add>"
        "    <remove sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/attribute[@name='Is Visible']\" />"
        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Text']/@value\">1st/3rd</replace>"
        "    <add sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]\">"
        "        <element type=\"Text\">"
        "            <attribute name=\"Name\" value=\"KeyBinding\" />"
        "            <attribute name=\"Text\" value=\"F\" />"
        "        </element>"
        "    </add>"
        "    <remove sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]/attribute[@name='Is Visible']\" />"
        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Text']/@value\">Jump</replace>"
        "    <add sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]\">"
        "        <element type=\"Text\">"
        "            <attribute name=\"Name\" value=\"KeyBinding\" />"
        "            <attribute name=\"Text\" value=\"SPACE\" />"
        "        </element>"
        "    </add>"
        "</patch>";
    }

private:
	// Default
	SharedPtr<Touch> touch_;
	bool firstPerson_;

	// - - - - - - - - - - - - -
	SharedPtr<Cursor> cursor;

	NetworkRole networkRole;
	BoidManager boidManager;

	Character* character;
	Controls clientControls;

	unsigned int nodeID = 0;

	const unsigned int SERVER_PORT = 2345;

	bool usingFreeCamera	= false;
	float menuUpdateTimer	= 1.0f;

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
	void CheckCharacterCollisions();	
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
	void HandlePhysicsPreStep(StringHash eventType, VariantMap& eventData);
	void HandleMouseDown(StringHash, VariantMap&);

	// Network Methods
	void ProcessClientControls();

	// Network Callbacks
	void HandleClientConnected(StringHash eventType, VariantMap& eventData);
	void HandleClientDisconnected(StringHash eventType, VariantMap& eventData);
	void HandleCharacterAllocation(StringHash eventType, VariantMap& eventData);

	// Misc.
	void AddConsole();
	void Quit();
	// - - - - - - - - - - - - -
};
