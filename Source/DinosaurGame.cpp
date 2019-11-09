
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>

#include <Urho3D/Engine/Engine.h>

#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/DebugRenderer.h>

#include <Urho3D/Input/Input.h>

#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>

#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>

#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/CheckBox.h>

#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/VectorBuffer.h>

#include <string>

#include "Character.h"
#include "Touch.h"

#include "_ObjectFactory.h"


// Interface
#include "MainMenu.h"
#include "PauseMenu.h"

#include "ScoreWindow.h"
#include "DebugWindow.h"
#include "ControlsWindow.h"

#include "RealTimer.h"
#include "Constants.h"

#include "DinosaurGame.h"

URHO3D_DEFINE_APPLICATION_MAIN(DinosaurGame)


DinosaurGame::DinosaurGame(Context* context) :
    Sample(context),
    firstPerson_(false)
{
	Character::RegisterObject(context);
}

DinosaurGame::~DinosaurGame()
{

}

void DinosaurGame::Start()
{
	Sample::Start();
	
	//AddConsole();

	CreateScene();

	InitialiseInterface();

	SetupMainMenu();

	GetSubsystem<Input>()->SetMouseVisible(true);

	Sample::InitMouseMode(MM_ABSOLUTE);
}

void DinosaurGame::StartGame()
{
	SetupPauseMenu();

	debugWindow->Create();
	scoreWindow->Create();

	CreateCharacter();

	GetSubsystem<Input>()->SetMouseMode(MM_RELATIVE);

	SubscribeToGameEvents();

	mainMenu->Hide();
	debugWindow->Hide();

	scoreWindow->Show();

	boidManager.Initialise(GetSubsystem<ResourceCache>(), scene_);
}

void DinosaurGame::InitialiseInterface()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	UI* ui = GetSubsystem<UI>();
	UIElement* root = ui->GetRoot();

	cursor = new Cursor(context_);

	root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
	cursor->SetStyleAuto(cache->GetResource<XMLFile>(root->GetAppliedStyle()));

	ui->SetCursor(cursor);

	cursor->SetVisible(false);

	cursor->SetPosition({ 1024 / 2, 768 / 2 });

	pauseMenu		= std::make_unique<GameMenu>(ui, cache);
	mainMenu		= std::make_unique<MainMenu>(ui, cache);
	debugWindow		= std::make_unique<DebugWindow>(ui, cache);
	scoreWindow		= std::make_unique<ScoreWindow>(ui, cache);
	controlsWindow	= std::make_unique<ControlsWindow>(ui, cache);

	controlsWindow->Create();
	controlsWindow->Show();
}

void DinosaurGame::SetupMainMenu()
{
	mainMenu->Create();

	mainMenu->Show();

	SubscribeToEvent(mainMenu->offlinePlayBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, MM_OfflinePlayBtnDown));
	SubscribeToEvent(mainMenu->hostOnlineBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, MM_HostGameBtnDown));
	SubscribeToEvent(mainMenu->joinOnlineBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, MM_JoinGameBtnDown));
	SubscribeToEvent(mainMenu->quitBtn,			E_RELEASED, URHO3D_HANDLER(DinosaurGame, MM_QuitGameBtnDown));
}

void DinosaurGame::SetupPauseMenu()
{
	pauseMenu->Create();

	pauseMenu->Hide();

	SubscribeToEvent(pauseMenu->continueBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, PM_ContinueBtnDown));
	SubscribeToEvent(pauseMenu->quitBtn,		E_RELEASED, URHO3D_HANDLER(DinosaurGame, PM_QuitBtnDown));
}

void DinosaurGame::MM_OfflinePlayBtnDown(StringHash, VariantMap&)
{
	StartGame();
}

void DinosaurGame::MM_HostGameBtnDown(StringHash, VariantMap&)
{

}

void DinosaurGame::MM_JoinGameBtnDown(StringHash, VariantMap&)
{

}

void DinosaurGame::MM_QuitGameBtnDown(StringHash, VariantMap&)
{
	Quit();
}

void DinosaurGame::PM_ContinueBtnDown(StringHash, VariantMap&)
{
	ToggleGamePause();
}

void DinosaurGame::PM_QuitBtnDown(StringHash, VariantMap&)
{
	Quit();
}

void DinosaurGame::ToggleFullscreen()
{
	GetSubsystem<Graphics>()->ToggleFullscreen();
}

void DinosaurGame::AddConsole()
{
	AllocConsole();

	freopen("CONOUT$", "w", stdout);
}

void DinosaurGame::Quit()
{
	engine_->Exit();
}

void DinosaurGame::CreateScene()
{
	scene_ = factory.CreateScene(context_);

	factory.SetScene(scene_);
	factory.SetCache(GetSubsystem<ResourceCache>());

	cameraNode_ = new Node(context_);

	Camera* camera = factory.SetupCamera(cameraNode_, context_);

	GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, scene_, camera));

	scene_->CreateComponent<DebugRenderer>();

	factory.CreateZone();
	factory.CreateLight(LIGHT_DIRECTIONAL);
	factory.CreateFloor();
}

void DinosaurGame::CreateCharacter()
{
	Input* input = GetSubsystem<Input>();

	character_ = factory.CreateCharacter("Jack", { 0.0f, 1.0f, 0.0f });

	character_->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT | CTRL_JUMP, false);
}

void DinosaurGame::SubscribeToGameEvents()
{
	SubscribeToEvent(E_KEYUP,				URHO3D_HANDLER(DinosaurGame, HandleKeyUp));
	SubscribeToEvent(E_UPDATE,				URHO3D_HANDLER(DinosaurGame, HandleUpdate));
	SubscribeToEvent(E_POSTUPDATE,			URHO3D_HANDLER(DinosaurGame, HandlePostUpdate));

	UnsubscribeFromEvent(E_SCENEUPDATE);
}

void DinosaurGame::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	if (!cursor->IsVisible())
	{
		UpdateUI(eventData[Update::P_TIMESTEP].GetFloat());

		Input* input = GetSubsystem<Input>();

		boidManager.Update(eventData[Update::P_TIMESTEP].GetFloat());

		character_->controls_.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
		character_->controls_.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
		character_->controls_.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
		character_->controls_.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));
		character_->controls_.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));

		character_->controls_.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
		character_->controls_.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;
		character_->controls_.pitch_ = Clamp(character_->controls_.pitch_, -80.0f, 80.0f);

		character_->GetNode()->SetRotation(Quaternion(character_->controls_.yaw_, Vector3::UP));
	}
}

void DinosaurGame::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
	if (!cursor->IsVisible())
		UpdateCamera(eventData[PostUpdate::P_TIMESTEP].GetFloat());
}

void DinosaurGame::HandleKeyUp(StringHash eventType, VariantMap& eventData)
{
	int key = eventData[KeyUp::P_KEY].GetInt();

	switch (key)
	{
	case KEY_F1:
		firstPerson_ = !firstPerson_;
		break;

	case KEY_F11:
		ToggleFullscreen();
		break;

	case KEY_ESCAPE:
		ToggleGamePause();
		break;
	}
}

void DinosaurGame::ToggleGamePause()
{
	pauseMenu->Toggle();
	debugWindow->Toggle();
	scoreWindow->Toggle();

	bool isPaused = pauseMenu->IsShown();

	cursor->SetPosition({ 1024 / 2, 768 / 2 });

	cursor->SetVisible(isPaused);

	scene_->SetUpdateEnabled(!isPaused);

	GetSubsystem<Input>()->SetMouseMode(isPaused ? MM_ABSOLUTE : MM_RELATIVE);
}

void DinosaurGame::UpdateUI(float delta)
{
	pauseMenuUpdateTimer -= delta;

	if (pauseMenuUpdateTimer <= 0.0f)
	{
		pauseMenuUpdateTimer = 1.0f;

		int fps = 1.0f / delta;

		scoreWindow->SetText(character_->GetScore());

		debugWindow->SetText(fps, boidManager.GetNumBoids(), NUM_BOID_THREADS);
	}
}

void DinosaurGame::UpdateCamera(float delta)
{
	bool freeCam = false;

	if (freeCam)
	{
		Input* input = GetSubsystem<Input>();

		const float MOVE_SPEED = 50.0f;
		const float MOUSE_SENSITIVITY = 0.1f;

		IntVector2 mouseMove = input->GetMouseMove();

		yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
		pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
		pitch_ = Clamp(pitch_, -90.0f, 90.0f);

		cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

		if (input->GetKeyDown(KEY_W)) cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * delta);
		if (input->GetKeyDown(KEY_S)) cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * delta);
		if (input->GetKeyDown(KEY_A)) cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * delta);
		if (input->GetKeyDown(KEY_D)) cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * delta);
	}
	else
	{
		Node* characterNode = character_->GetNode();

		Quaternion rot = characterNode->GetRotation();
		Quaternion dir = rot * Quaternion(character_->controls_.pitch_, Vector3::RIGHT);

		Node* headNode		= characterNode->GetChild("Mutant:Head", true);
		float limitPitch	= Clamp(character_->controls_.pitch_, -45.0f, 45.0f);
		Quaternion headDir	= rot * Quaternion(limitPitch, Vector3(1.0f, 0.0f, 0.0f));

		Vector3 headWorldTarget = headNode->GetWorldPosition() + headDir * Vector3(0.0f, 0.0f, -1.0f);
		headNode->LookAt(headWorldTarget, Vector3(0.0f, 1.0f, 0.0f));

		// Correct head orientation because LookAt assumes Z = forward, but the bone has been authored differently (Y = forward) 
		headNode->Rotate(Quaternion(0.0f, 90.0f, 90.0f));

		if (firstPerson_)
		{
			cameraNode_->SetPosition(headNode->GetWorldPosition() + rot * Vector3(0.0f, 0.15f, 0.2f));
			cameraNode_->SetRotation(dir);
		}
		else
		{
			Vector3 aimPoint	= characterNode->GetPosition() + rot * Vector3(0.0f, 1.7f, 0.0f);
			Vector3 rayDir		= dir * Vector3::BACK;
			float rayDistance	= touch_ ? touch_->cameraDistance_ : CAMERA_INITIAL_DIST;
			PhysicsRaycastResult result;
			scene_->GetComponent<PhysicsWorld>()->RaycastSingle(result, Ray(aimPoint, rayDir), rayDistance, 2);

			if (result.body_)
				rayDistance = Min(rayDistance, result.distance_);

			rayDistance = Clamp(rayDistance, CAMERA_MIN_DIST, CAMERA_MAX_DIST);

			cameraNode_->SetPosition(aimPoint + rayDir * rayDistance); 
			cameraNode_->SetRotation(dir);
		}
	}
}