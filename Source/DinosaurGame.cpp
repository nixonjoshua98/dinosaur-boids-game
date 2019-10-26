
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
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

#include <string>

#include "Character.h"
#include "Touch.h"

#include "_ObjectFactory.h"
#include "PauseMenu.h"

#include "Constants.h"

#include "DinosaurGame.h"

#define PRINT(x, y) std::cout << x << ", " << y <<"\n";

URHO3D_DEFINE_APPLICATION_MAIN(DinosaurGame)

DinosaurGame::DinosaurGame(Context* context) :
    Sample(context),
    firstPerson_(false)
{
	Character::RegisterObject(context);

	pauseMenu = std::make_unique<PauseMenu>(GetSubsystem<UI>(), GetSubsystem<ResourceCache>());

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
	CreateCharacter();

	SubscribeToEvents();

	GetSubsystem<Input>()->SetMouseVisible(true);

	Sample::InitMouseMode(MM_RELATIVE);
}

void DinosaurGame::CreateScene()
{
	pauseMenu->Create();

	scene_ = factory.CreateScene(context_);

	factory.SetScene(scene_);
	factory.SetCache(GetSubsystem<ResourceCache>());

	cameraNode_ = new Node(context_);

	Camera* camera = factory.SetupCamera(cameraNode_, context_);

	GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, scene_, camera));

	factory.CreateZone();
	factory.CreateLight(LIGHT_DIRECTIONAL);
	factory.CreateFloor(FLOOR_SIZE, FLOOR_SIZE);

	boidManager.Initialise(GetSubsystem<ResourceCache>(), scene_);
}

void DinosaurGame::CreateCharacter()
{
	Input* input = GetSubsystem<Input>();

	character_ = factory.CreateCharacter("Jack", { 0.0f, 1.0f, 0.0f });

	character_->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT | CTRL_JUMP, false);
}

void DinosaurGame::SubscribeToEvents()
{
	SubscribeToEvent(E_KEYUP,		URHO3D_HANDLER(DinosaurGame, HandleKeyUp));
	SubscribeToEvent(E_UPDATE,		URHO3D_HANDLER(DinosaurGame, HandleUpdate));
	SubscribeToEvent(E_POSTUPDATE,	URHO3D_HANDLER(DinosaurGame, HandlePostUpdate));

	SubscribeToEvent(pauseMenu->GetContinueButton(),	E_RELEASED, URHO3D_HANDLER(DinosaurGame, OnContinueButtonDown));
	SubscribeToEvent(pauseMenu->GetQuitButton(),		E_RELEASED, URHO3D_HANDLER(DinosaurGame, OnQuitButtonDown));

	UnsubscribeFromEvent(E_SCENEUPDATE);
}

// Event Callbacks

void DinosaurGame::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	Input* input = GetSubsystem<Input>();

	scene_->RemoveComponent<DebugRenderer>();
	scene_->CreateComponent<DebugRenderer>();

	boidManager.SetDeltaTime(eventData[Update::P_TIMESTEP].GetFloat());
	
	character_->controls_.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
	character_->controls_.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
	character_->controls_.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
	character_->controls_.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));
	character_->controls_.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));
		
	character_->controls_.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
	character_->controls_.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;
	character_->controls_.pitch_ = Clamp(character_->controls_.pitch_, -80.0f, 80.0f);
	character_->GetNode()->SetRotation(Quaternion(character_->controls_.yaw_, Vector3::UP));
	
	UpdatePauseMenuText(eventData[Update::P_TIMESTEP].GetFloat());
}

void DinosaurGame::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
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

// Button callbacks

void DinosaurGame::OnContinueButtonDown(StringHash eventType, VariantMap& eventData)
{
	TogglePauseMenu();
}

void DinosaurGame::OnQuitButtonDown(StringHash eventType, VariantMap& eventData)
{
	GetSubsystem<Console>()->SetVisible(false);

	engine_->Exit();
}

void DinosaurGame::TogglePauseMenu()
{
	Input* input = GetSubsystem<Input>();

	pauseMenu->Toggle();

	input->SetMouseMode(pauseMenu->IsShown() ? MM_ABSOLUTE : MM_RELATIVE);

	input->SetMousePosition({ 1024 / 2, 768 / 2 });
}

//

void DinosaurGame::UpdatePauseMenuText(float delta)
{
	prevDelta = delta;

	pauseMenuUpdateTimer -= delta;

	if (pauseMenuUpdateTimer >= 0.0f) return;

	pauseMenuUpdateTimer = 1.0f;

	int fps = 1.0f / delta;

	pauseMenu->SetFPS(fps);
	pauseMenu->SetBoidCount(NUM_BOIDS);
}

void DinosaurGame::UpdateCamera()
{
	bool freeCam = true;

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

		Node* headNode = characterNode->GetChild("Bip01_Head", true);
		float limitPitch = Clamp(character_->controls_.pitch_, -45.0f, 45.0f);


		Quaternion headDir = rot * Quaternion(limitPitch, Vector3(1.0f, 0.0f, 0.0f));

		// This could be expanded to look at an arbitrary target, now just look at a point in front 
		Vector3 headWorldTarget = headNode->GetWorldPosition() + headDir * Vector3(0.0f, 0.0f, 1.0f);
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
			Vector3 aimPoint = characterNode->GetPosition() + rot * Vector3(0.0f, 1.7f, 0.0f);

			Vector3 rayDir = dir * Vector3::BACK;
			float rayDistance = touch_ ? touch_->cameraDistance_ : CAMERA_INITIAL_DIST;  PhysicsRaycastResult result;
			scene_->GetComponent<PhysicsWorld>()->RaycastSingle(result, Ray(aimPoint, rayDir), rayDistance, 2);

			if (result.body_)
				rayDistance = Min(rayDistance, result.distance_);

			rayDistance = Clamp(rayDistance, CAMERA_MIN_DIST, CAMERA_MAX_DIST);

			cameraNode_->SetPosition(aimPoint + rayDir * rayDistance);   cameraNode_->SetRotation(dir);
		}
	}
}