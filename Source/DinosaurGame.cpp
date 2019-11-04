
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

#include "MainMenu.h"
#include "GameMenu.h"

#include "RealTimer.h"
#include "Constants.h"

#include "DinosaurGame.h"


URHO3D_DEFINE_APPLICATION_MAIN(DinosaurGame)

#undef SendMessage

DinosaurGame::DinosaurGame(Context* context) :
    Sample(context),
    firstPerson_(false)
{
	Character::RegisterObject(context);

	AllocConsole();

	freopen("CONOUT$", "w", stdout);
}

DinosaurGame::~DinosaurGame()
{

}

void DinosaurGame::Start()
{
	Sample::Start();

	CreateScene();

	CreateInterface();

	SubscribeToMainMenuEvents();

	GetSubsystem<Input>()->SetMouseVisible(true);

	Sample::InitMouseMode(MM_ABSOLUTE);
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

void DinosaurGame::CreateInterface()
{
	auto timer = RealTimer("DinosaurGame::CreateInterface: ");

	ResourceCache* cache	= GetSubsystem<ResourceCache>();
	UI* ui					= GetSubsystem<UI>();
	UIElement* root			= ui->GetRoot();

	cursor = new Cursor(context_);

	root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
	cursor->SetStyleAuto(cache->GetResource<XMLFile>(root->GetAppliedStyle()));

	ui->SetCursor(cursor);

	cursor->SetVisible(false);

	cursor->SetPosition({ 1024 / 2, 768 / 2 });

	gameMenu	= std::make_unique<GameMenu>(ui, cache);
	mainMenu	= std::make_unique<MainMenu>(ui, cache);

	mainMenu->Create();
	gameMenu->Create();
}

void DinosaurGame::CreateCharacter()
{
	Input* input = GetSubsystem<Input>();

	character_ = factory.CreateCharacter("Jack", { 0.0f, 1.0f, 0.0f });

	character_->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT | CTRL_JUMP, false);
}

void DinosaurGame::StartGame()
{
	CreateCharacter();

	GetSubsystem<Input>()->SetMouseMode(MM_RELATIVE);

	SubscribeToEvents();

	boidManager.Initialise(GetSubsystem<ResourceCache>(), scene_);
}

void DinosaurGame::SubscribeToMainMenuEvents()
{
	SubscribeToEvent(mainMenu->GetPlayBtn(), E_RELEASED, URHO3D_HANDLER(DinosaurGame, OnOfflinePlayButtonDown));
	SubscribeToEvent(mainMenu->GetHostBtn(), E_RELEASED, URHO3D_HANDLER(DinosaurGame, OnHostGameDown));
	SubscribeToEvent(mainMenu->GetJoinBtn(), E_RELEASED, URHO3D_HANDLER(DinosaurGame, OnJoinGameDown));
	SubscribeToEvent(mainMenu->GetQuitBtn(), E_RELEASED, URHO3D_HANDLER(DinosaurGame, OnQuitButtonDown));
}

void DinosaurGame::SubscribeToEvents()
{
	SubscribeToEvent(E_KEYUP,				URHO3D_HANDLER(DinosaurGame, HandleKeyUp));
	SubscribeToEvent(E_UPDATE,				URHO3D_HANDLER(DinosaurGame, HandleUpdate));
	SubscribeToEvent(E_POSTUPDATE,			URHO3D_HANDLER(DinosaurGame, HandlePostUpdate));

	SubscribeToEvent(gameMenu->GetContinueBtn(),	E_RELEASED, URHO3D_HANDLER(DinosaurGame, OnContinueButtonDown));
	SubscribeToEvent(gameMenu->GetQuitBtn(),		E_RELEASED, URHO3D_HANDLER(DinosaurGame, OnQuitButtonDown));

	UnsubscribeFromEvent(E_SCENEUPDATE);
}

void DinosaurGame::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	if (!cursor->IsVisible())
	{
		UpdatePauseMenuText(eventData[Update::P_TIMESTEP].GetFloat());

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
		UpdateCamera();
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
		GetSubsystem<Graphics>()->ToggleFullscreen();
		break;

	case KEY_ESCAPE:
		TogglePauseMenu();
		break;
	}
}

void DinosaurGame::OnContinueButtonDown(StringHash eventType, VariantMap& eventData)
{
	TogglePauseMenu();
}

void DinosaurGame::OnOfflinePlayButtonDown(StringHash eventType, VariantMap& eventData)
{
	mainMenu->Toggle();

	playMode = PlayMode::OFFLINE;

	StartGame();
}

void DinosaurGame::OnHostGameDown(StringHash eventType, VariantMap& eventData)
{
	mainMenu->Toggle();

	playMode = PlayMode::HOST;

	StartGame();
}

void DinosaurGame::OnQuitButtonDown(StringHash eventType, VariantMap& eventData)
{
	GetSubsystem<Console>()->SetVisible(false);

	engine_->Exit();
}

void DinosaurGame::OnJoinGameDown(StringHash eventType, VariantMap& eventData)
{
	mainMenu->Toggle();

	playMode = PlayMode::CLIENT;

	StartGame();
}

void DinosaurGame::TogglePauseMenu()
{
	gameMenu->Toggle();

	cursor->SetPosition({ 1024 / 2, 768 / 2 });

	cursor->SetVisible(gameMenu->IsShown());

	scene_->SetUpdateEnabled(!cursor->IsVisible());

	GetSubsystem<Input>()->SetMouseMode(gameMenu->IsShown() ? MM_ABSOLUTE : MM_RELATIVE);
}

void DinosaurGame::UpdatePauseMenuText(float delta)
{
	prevDelta = delta;

	pauseMenuUpdateTimer -= delta;

	if (pauseMenuUpdateTimer >= 0.0f) return;

	pauseMenuUpdateTimer = 1.0f;

	int fps = 1.0f / delta;

	gameMenu->SetText(fps, boidManager.GetNumBoids(), NUM_BOID_THREADS);
}

void DinosaurGame::UpdateCamera()
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

		if (input->GetKeyDown(KEY_W)) cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * prevDelta);
		if (input->GetKeyDown(KEY_S)) cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * prevDelta);
		if (input->GetKeyDown(KEY_A)) cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * prevDelta);
		if (input->GetKeyDown(KEY_D)) cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * prevDelta);
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