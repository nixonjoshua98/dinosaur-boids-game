#include <Urho3D/Graphics/DebugRenderer.h>

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

	uiFactory = std::make_unique<InterfaceFactory>(GetSubsystem<UI>(), GetSubsystem<ResourceCache>());
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

	boidSet.Initialise(GetSubsystem<ResourceCache>(), scene_);

	SubscribeToEvents();

	Sample::InitMouseMode(MM_RELATIVE);
}

void CharacterDemo::CreateScene()
{
	scene_ = factory.CreateScene(context_);

	factory.SetScene(scene_);
	factory.SetCache(GetSubsystem<ResourceCache>());

	cameraNode_ = new Node(context_);

	Camera* camera = factory.SetupCamera(cameraNode_, context_);

	GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, scene_, camera));

	factory.CreateZone();
	factory.CreateLight(LIGHT_DIRECTIONAL);
	factory.CreateFloor();
}

void CharacterDemo::CreateCharacter()
{
	Input* input = GetSubsystem<Input>();

	character_ = factory.CreateCharacter("Jack", { 0.0f, 1.0f, 0.0f });

	character_->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT | CTRL_JUMP, false);
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

	scene_->RemoveComponent<DebugRenderer>();
	scene_->CreateComponent<DebugRenderer>();

	boidSet.Update(eventData[Update::P_TIMESTEP].GetFloat());
	
	UpdateFPS(eventData[Update::P_TIMESTEP].GetFloat());
}

void CharacterDemo::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
	UpdateCamera();
}

void CharacterDemo::CreatePauseMenu()
{
	uiFactory->CreateText(		FPSText,	HA_RIGHT,		"FPSText", "Fonts/Anonymous Pro.ttf", 15, {0, 0 });
	uiFactory->CreateText(	controlText,	HA_LEFT,	"ControlText", "Fonts/Anonymous Pro.ttf", 15, {0, 0 }, controlsString);
}

void CharacterDemo::UpdateFPS(float delta)
{
	TEMP_DELTA_VAR = delta;

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
	controlText->SetVisible(isPaused);
}

void CharacterDemo::UpdateCamera()
{
	Input* input = GetSubsystem<Input>();

	const float MOVE_SPEED = 50.0f;
	const float MOUSE_SENSITIVITY = 0.1f;

	IntVector2 mouseMove = input->GetMouseMove();

	yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
	pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
	pitch_ = Clamp(pitch_, -90.0f, 90.0f);

	cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

	if (input->GetKeyDown(KEY_W)) cameraNode_->Translate(Vector3::FORWARD	* MOVE_SPEED	* TEMP_DELTA_VAR);
	if (input->GetKeyDown(KEY_S)) cameraNode_->Translate(Vector3::BACK		* MOVE_SPEED	* TEMP_DELTA_VAR);
	if (input->GetKeyDown(KEY_A)) cameraNode_->Translate(Vector3::LEFT		* MOVE_SPEED	* TEMP_DELTA_VAR);
	if (input->GetKeyDown(KEY_D)) cameraNode_->Translate(Vector3::RIGHT		* MOVE_SPEED	* TEMP_DELTA_VAR);

	//Node* characterNode = character_->GetNode();

	//Quaternion rot = characterNode->GetRotation();
	//Quaternion dir = rot * Quaternion(character_->controls_.pitch_, Vector3::RIGHT);

	//Node* headNode = characterNode->GetChild("Bip01_Head", true);
	//float limitPitch = Clamp(character_->controls_.pitch_, -45.0f, 45.0f);


	//Quaternion headDir = rot * Quaternion(limitPitch, Vector3(1.0f, 0.0f, 0.0f));

	//// This could be expanded to look at an arbitrary target, now just look at a point in front 
	//Vector3 headWorldTarget = headNode->GetWorldPosition() + headDir * Vector3(0.0f, 0.0f, 1.0f);
	//headNode->LookAt(headWorldTarget, Vector3(0.0f, 1.0f, 0.0f));

	//// Correct head orientation because LookAt assumes Z = forward, but the bone has been authored differently (Y = forward) 
	//headNode->Rotate(Quaternion(0.0f, 90.0f, 90.0f));

	//if (firstPerson_)
	//{
	//	cameraNode_->SetPosition(headNode->GetWorldPosition() + rot * Vector3(0.0f, 0.15f, 0.2f));
	//	cameraNode_->SetRotation(dir);
	//}
	//else
	//{
	//	Vector3 aimPoint = characterNode->GetPosition() + rot * Vector3(0.0f, 1.7f, 0.0f);

	//	Vector3 rayDir = dir * Vector3::BACK;
	//	float rayDistance = touch_ ? touch_->cameraDistance_ : CAMERA_INITIAL_DIST;  PhysicsRaycastResult result;
	//	scene_->GetComponent<PhysicsWorld>()->RaycastSingle(result, Ray(aimPoint, rayDir), rayDistance, 2);

	//	if (result.body_)
	//		rayDistance = Min(rayDistance, result.distance_);

	//	rayDistance = Clamp(rayDistance, CAMERA_MIN_DIST, CAMERA_MAX_DIST);

	//	cameraNode_->SetPosition(aimPoint + rayDir * rayDistance);   cameraNode_->SetRotation(dir);
	//}
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
