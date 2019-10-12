#include "UrhoHeaders.h"
#include "Character.h"
#include "CharacterDemo.h"
#include "Touch.h"

URHO3D_DEFINE_APPLICATION_MAIN(CharacterDemo)

CharacterDemo::CharacterDemo(Context* context) :
    Sample(context),
    firstPerson_(false)
{
	Character::RegisterObject(context);
}

CharacterDemo::~CharacterDemo()
{

}

void CharacterDemo::Start()
{
    Sample::Start();

	CreateScene();
	CreateCharacter();
	CreatePauseMenu();
	CreateInstructions();
	SubscribeToEvents();

	Sample::InitMouseMode(MM_RELATIVE);
}

void CharacterDemo::CreateScene()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	scene_ = factory.CreateScene(context_);

	factory.SetScene(scene_);
	factory.SetCache(GetSubsystem<ResourceCache>());
	factory.SetRoot(GetSubsystem<UI>()->GetRoot());

	cameraNode_ = new Node(context_);

	Camera* camera = factory.SetupCamera(cameraNode_, context_);

	GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, scene_, camera));

	factory.CreateZone();
	factory.CreateLight(LIGHT_DIRECTIONAL);
	factory.CreateFloor();

	for (unsigned i = 0; i < 60; ++i)
	{
		Vector3 pos = Vector3(Random(180.0f) - 90.0f, 0.0f, Random(180.0f) - 90.0f);
		Quaternion rotation = Quaternion(0.0f, Random(360.0f), 0.0f);
		float scale = 2.0f + Random(5.0f);

		factory.CreateMushroom(pos, rotation, scale);
	}

	for (unsigned i = 0; i < 100; ++i)
	{
		Vector3 pos = Vector3(Random(180.0f) - 90.0f, Random(10.0f) + 10.0f, Random(180.0f) - 90.0f);
		Quaternion rotation = Quaternion(Random(360.0f), Random(360.0f), Random(360.0f));
		float scale = Random(2.0f) + 0.5f;

		factory.CreateBox(pos, rotation, scale);
	}
}

void CharacterDemo::CreateCharacter()
{
	Input* input = GetSubsystem<Input>();

	character_ = factory.CreateCharacter("Jack", { 0.0f, 1.0f, 0.0f });

	character_->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT | CTRL_JUMP, false);
}

void CharacterDemo::CreateInstructions()
{
	factory.CreateText(controlText, HA_LEFT, "ControlText", "Fonts/Anonymous Pro.ttf", 15, { 5, 0 }, true);

	controlText->SetText(controlsString);
}

void CharacterDemo::SubscribeToEvents()
{
	SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(CharacterDemo, HandleKeyUp));

	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(CharacterDemo, HandleUpdate));
	SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(CharacterDemo, HandlePostUpdate));

	UnsubscribeFromEvent(E_SCENEUPDATE);
}

void CharacterDemo::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	Input* input = GetSubsystem<Input>();
	
	character_->controls_.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
	character_->controls_.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
	character_->controls_.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
	character_->controls_.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));
	character_->controls_.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));
		
	character_->controls_.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
	character_->controls_.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;
	character_->controls_.pitch_ = Clamp(character_->controls_.pitch_, -80.0f, 80.0f);
	character_->GetNode()->SetRotation(Quaternion(character_->controls_.yaw_, Vector3::UP));
	
	UpdateFPS(eventData[Update::P_TIMESTEP].GetFloat());
}

void CharacterDemo::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
	UpdateCamera();
}

void CharacterDemo::CreatePauseMenu()
{
	factory.CreateText(FPSText, HA_RIGHT, "FPSText", "Fonts/Anonymous Pro.ttf", 15, { 0, 0 }, false);
}

void CharacterDemo::UpdateFPS(float delta)
{
	fpsUpdateTimer -= delta;

	if (fpsUpdateTimer >= 0.0f)
		return;

	fpsUpdateTimer = 1.0f;

	int fps = 1.0f / delta;

	FPSText->SetText(String(fps));
}

void CharacterDemo::TogglePauseMenu()
{
	isPaused = !isPaused;

	FPSText->SetVisible(isPaused);
}

void CharacterDemo::UpdateCamera()
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

void CharacterDemo::SaveScene()
{
	File saveFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/CharacterDemo.xml", FILE_WRITE);
	scene_->SaveXML(saveFile);
}

void CharacterDemo::LoadScene()
{
	File loadFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/CharacterDemo.xml", FILE_READ);
	scene_->LoadXML(loadFile);

	Node* characterNode = scene_->GetChild("Jack", true);

	if (characterNode)
		character_ = characterNode->GetComponent<Character>();
}

void CharacterDemo::HandleKeyUp(StringHash eventType, VariantMap& eventData)
{
	int key = eventData[KeyUp::P_KEY].GetInt();

	switch (key)
	{
	case KEY_F1:
		firstPerson_ = !firstPerson_;
		break;

	case KEY_F3:
		TogglePauseMenu();
		break;

	case KEY_F5:
		SaveScene();
		break;

	case KEY_F7:
		LoadScene();
		break;

	case KEY_F11:
		GetSubsystem<Graphics>()->ToggleFullscreen();
		break;

	case KEY_ESCAPE:
		GetSubsystem<Console>()->SetVisible(false);
		engine_->Exit();
		break;
	}
}
