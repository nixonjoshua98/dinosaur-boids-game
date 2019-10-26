
#pragma once

#include "Sample.h"

namespace Urho3D
{
	class Node;
	class Scene;
}

class Character;
class Touch;

class PauseMenu;
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
	bool firstPerson_;

	BoidManager boidManager;

    void CreateScene();
    void CreateCharacter();

    void SubscribeToEvents();

	void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
	void HandleKeyUp(StringHash eventType, VariantMap& eventData);

	void OnContinueButtonDown(StringHash eventType, VariantMap& eventData);
	void OnQuitButtonDown(StringHash eventType, VariantMap& eventData);

	void TogglePauseMenu();

	std::unique_ptr<PauseMenu> pauseMenu;

	void UpdatePauseMenuText(float);
	void UpdateCamera();

	_ObjectFactory factory;

	float pauseMenuUpdateTimer = 1.0f;

	float prevDelta;
};
