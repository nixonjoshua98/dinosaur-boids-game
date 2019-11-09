
#pragma once

#include "Sample.h"

namespace Urho3D
{
	class Node;
	class Scene;
}

class Character;
class Touch;
class _ObjectFactory;

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

	SharedPtr<Touch> touch_;
	WeakPtr<Character> character_;
	SharedPtr<Cursor> cursor;

	bool firstPerson_;


	// - - - - - - - - - - - - -
	BoidManager boidManager;

	_ObjectFactory factory;

	float pauseMenuUpdateTimer = 1.0f;

	// UI
	std::unique_ptr<GameMenu> pauseMenu;
	std::unique_ptr<MainMenu> mainMenu;
	std::unique_ptr<DebugWindow> debugWindow;
	std::unique_ptr<ScoreWindow> scoreWindow;
	std::unique_ptr<ControlsWindow> controlsWindow;

	void CreateScene();
	void StartGame();
	void InitialiseInterface();
	void CreateCharacter();
	void SubscribeToGameEvents();

	void UpdateUI(float);
	void UpdateCamera(float);

	// UI
	void SetupMainMenu();
	void SetupPauseMenu();
	void ToggleGamePause();

	// Main Menu Callbacks
	void MM_OfflinePlayBtnDown(StringHash, VariantMap&);
	void MM_HostGameBtnDown(StringHash, VariantMap&);
	void MM_JoinGameBtnDown(StringHash, VariantMap&);
	void MM_QuitGameBtnDown(StringHash, VariantMap&);

	// Pause Menu Callbacks
	void PM_ContinueBtnDown(StringHash, VariantMap&);
	void PM_QuitBtnDown(StringHash, VariantMap&);

	// Event Callbacks
	void HandleUpdate(StringHash, VariantMap&);
	void HandlePostUpdate(StringHash, VariantMap&);
	void HandleKeyUp(StringHash, VariantMap&);

	void ToggleFullscreen();
	void AddConsole();
	void Quit();
	// - - - - - - - - - - - - -
};
