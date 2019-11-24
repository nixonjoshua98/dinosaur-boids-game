
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>

#include <Urho3D/Engine/Engine.h>

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/StaticModel.h>

#include <Urho3D/Input/Input.h>

#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsEvents.h>

#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Scene/Scene.h>

#include <Urho3D/Network/Connection.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>

#include <Urho3D/IO/Log.h>

#include <string>

#include "Character.h"
#include "Touch.h"

#include "MainMenu.h"
#include "PauseMenu.h"

#include "ScoreWindow.h"
#include "DebugWindow.h"
#include "ControlsWindow.h"

#include "RealTimer.h"
#include "Constants.h"
#include "Boid.h"

#include "DinosaurGame.h"

/* 
	Removed alternating updates
*/

URHO3D_DEFINE_APPLICATION_MAIN(DinosaurGame)

static const StringHash E_CLIENTOBJECTAUTHORITY("ClientObjectAuthority");
static const StringHash E_UPDATEINTERFACE("UpdateInterface");

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
	
	InitialiseInterface();

	SetupMainMenu();

	AddConsole();

	GetSubsystem<Input>()->SetMouseVisible(true);

	Sample::InitMouseMode(MM_ABSOLUTE);
}

void DinosaurGame::StartGame()
{
	SetupPauseMenu();

	GetSubsystem<Input>()->SetMouseMode(MM_RELATIVE);

	mainMenu->Hide();

	debugWindow->Hide();

	scoreWindow->Show();

	SubscribeToGameEvents();

	boids.reset(new BoidManager());

	if (networkRole == NetworkRole::OFFLINE)
	{
		boids->Initialise(GetSubsystem<ResourceCache>(), scene_);
	}

	else if (networkRole == NetworkRole::SERVER)
	{
		boids->Initialise(GetSubsystem<ResourceCache>(), scene_);
	}
	
	else if (networkRole == NetworkRole::CLIENT)
	{
		boids->Initialise();
	}
}

void DinosaurGame::InitialiseInterface()
{
	ResourceCache* cache	= GetSubsystem<ResourceCache>();
	UI* ui					= GetSubsystem<UI>();
	UIElement* root			= ui->GetRoot();

	SharedPtr<Cursor> cursor(new Cursor(context_));

	root->SetDefaultStyle(cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
	cursor->SetStyleAuto(cache->GetResource<XMLFile>(root->GetAppliedStyle()));

	ui->SetCursor(cursor);

	cursor->SetVisible(false);

	cursor->SetPosition({ 1024 / 2, 768 / 2 });

	pauseMenu		= std::make_unique<PauseMenu>(ui, cache);
	mainMenu		= std::make_unique<MainMenu>(ui, cache);
	debugWindow		= std::make_unique<DebugWindow>(ui, cache);
	scoreWindow		= std::make_unique<ScoreWindow>(ui, cache);
	controlsWindow	= std::make_unique<ControlsWindow>(ui, cache);

	scoreWindow->Hide();
	pauseMenu->Hide();
	debugWindow->Hide();

	controlsWindow->Show();
}

void DinosaurGame::SetupMainMenu()
{
	mainMenu->Show();

	SubscribeToEvent(mainMenu->offlinePlayBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, MM_OfflinePlayBtnDown));
	SubscribeToEvent(mainMenu->hostOnlineBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, MM_HostGameBtnDown));
	SubscribeToEvent(mainMenu->joinOnlineBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, MM_JoinGameBtnDown));
	SubscribeToEvent(mainMenu->quitBtn,			E_RELEASED, URHO3D_HANDLER(DinosaurGame, MM_QuitGameBtnDown));
}

void DinosaurGame::SetupPauseMenu()
{
	pauseMenu->Hide();

	SubscribeToEvent(pauseMenu->continueBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, PM_ContinueBtnDown));
	SubscribeToEvent(pauseMenu->disconnectBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, PM_DisconnectBtnDown));
	SubscribeToEvent(pauseMenu->startSvrBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, PM_StartSvrBtnDown));
	SubscribeToEvent(pauseMenu->connectBtn,		E_RELEASED, URHO3D_HANDLER(DinosaurGame, PM_JoinSvrBtnDown));
	SubscribeToEvent(pauseMenu->quitBtn,		E_RELEASED, URHO3D_HANDLER(DinosaurGame, PM_QuitBtnDown));
}

void DinosaurGame::MM_OfflinePlayBtnDown(StringHash, VariantMap&)
{
	networkRole = NetworkRole::OFFLINE;

	CreateOfflineScene();

	StartGame();
}

void DinosaurGame::MM_HostGameBtnDown(StringHash, VariantMap&)
{
	networkRole = NetworkRole::SERVER;

	CreateServerScene();
	StartServer();
	SubscribeToServerEvents();
	StartGame();
}

void DinosaurGame::MM_JoinGameBtnDown(StringHash, VariantMap&)
{
	networkRole = NetworkRole::CLIENT;

	CreateClientScene();

	String address = mainMenu->ipAddress->GetText().Trimmed();

	ConnectToServer(address.Empty() ? "localhost" : address);

	SubscribeToClientEvents();

	StartGame();
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

void DinosaurGame::PM_DisconnectBtnDown(StringHash, VariantMap&)
{
	Network* network = GetSubsystem<Network>();

	hasCharacter = false;

	UnsubscribeToGameEvents();

	if (networkRole == NetworkRole::SERVER)
	{
		network->StopServer();
	}

	else
	{
		if (networkRole == NetworkRole::CLIENT)
		{
			network->Disconnect();
		}
	}

	scene_->Clear();

	scoreWindow->Hide();
	pauseMenu->Hide();
	debugWindow->Hide();

	mainMenu->Show();
}

void DinosaurGame::PM_StartSvrBtnDown(StringHash s, VariantMap& v)
{
	Network* network = GetSubsystem<Network>();

	hasCharacter = false;

	UnsubscribeToGameEvents();

	if (networkRole == NetworkRole::SERVER)
	{
		network->StopServer();
	}

	else
	{
		if (networkRole == NetworkRole::CLIENT)
		{
			network->Disconnect();
		}
	}

	scene_->Clear();

	networkRole = NetworkRole::SERVER;

	CreateServerScene();
	StartServer();
	SubscribeToServerEvents();
	StartGame();
}

void DinosaurGame::PM_JoinSvrBtnDown(StringHash a, VariantMap& b)
{
	UnsubscribeFromEvent(E_SERVERDISCONNECTED);

	UnsubscribeToGameEvents();

	hasCharacter = false;

	ToggleGamePause();

	MM_JoinGameBtnDown(a, b);

	SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(DinosaurGame, HandleServerDisconnect));
}

void DinosaurGame::HandleServerDisconnect(StringHash s, VariantMap& v)
{
	PM_DisconnectBtnDown(s, v);
}

void DinosaurGame::UnsubscribeToGameEvents()
{
	UnsubscribeFromEvent(E_KEYUP);
	UnsubscribeFromEvent(E_UPDATE);
	UnsubscribeFromEvent(E_POSTUPDATE);
}

void DinosaurGame::CreateOfflineScene()
{
	CreateScene(LOCAL);
	CreateCamera();
	CreateZone(LOCAL);
	CreateLighting();
	CreateFloor();

	character = CreateCharacter();

	hasCharacter = true;
}

void DinosaurGame::CreateClientScene()
{
	CreateScene(LOCAL);
	CreateCamera();
	CreateLighting();
	CreateFloor();
}

void DinosaurGame::CreateServerScene()
{
	CreateScene(REPLICATED);
	CreateCamera();
	CreateZone(REPLICATED);
	CreateLighting();
	CreateFloor();
	CreateMushroom();

	character = CreateCharacter();

	hasCharacter = true;
}

void DinosaurGame::UpdateFreeCamera(float deltaTime)
{
	Input* input = GetSubsystem<Input>();

	const float MOVE_SPEED = 50.0f;
	const float MOUSE_SENSITIVITY = 0.1f;

	IntVector2 mouseMove = input->GetMouseMove();

	yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
	pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
	pitch_ = Clamp(pitch_, -90.0f, 90.0f);

	cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

	if (input->GetKeyDown(KEY_W))
		cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * deltaTime);

	if (input->GetKeyDown(KEY_S))
		cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * deltaTime);

	if (input->GetKeyDown(KEY_A))
		cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * deltaTime);

	if (input->GetKeyDown(KEY_D))
		cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * deltaTime);
}

void DinosaurGame::UpdateShoulderCamera(float deltaTime)
{
	Controls controls;

	Node* characterNode = character->GetNode();

	// Client
	if (networkRole == NetworkRole::CLIENT)
		controls = clientControls;

	// Server / Offline
	else
		controls = character->controls_;

	Quaternion rot = characterNode->GetRotation();
	Quaternion dir = rot * Quaternion(controls.pitch_, Vector3::RIGHT);

	Node* headNode		= characterNode->GetChild("Mutant:Head", true);
	float limitPitch			= Clamp(controls.pitch_, -45.0f, 45.0f);
	Quaternion headDir	= rot * Quaternion(limitPitch, Vector3(1.0f, 0.0f, 0.0f));

	Vector3 headWorldTarget = headNode->GetWorldPosition() + headDir * Vector3(0.0f, 0.0f, -1.0f);

	headNode->LookAt(headWorldTarget, Vector3(0.0f, 1.0f, 0.0f));

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
		float rayDistance = CAMERA_INITIAL_DIST;

		PhysicsRaycastResult result;

		scene_->GetComponent<PhysicsWorld>()->RaycastSingle(result, Ray(aimPoint, rayDir), rayDistance, 2);

		if (result.body_)
			rayDistance = Min(rayDistance, result.distance_);

		rayDistance = Clamp(rayDistance, CAMERA_MIN_DIST, CAMERA_MAX_DIST);

		cameraNode_->SetPosition(aimPoint + rayDir * rayDistance);
		cameraNode_->SetRotation(dir);
	}
}

void DinosaurGame::UpdateClientCharacterControls()
{
	Input* input = GetSubsystem<Input>();

	clientControls.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
	clientControls.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
	clientControls.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
	clientControls.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));
	clientControls.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));

	clientControls.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
	clientControls.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;
	clientControls.pitch_ = Clamp(clientControls.pitch_, -80.0f, 80.0f);
}

void DinosaurGame::SubscribeToGameEvents()
{
	SubscribeToEvent(E_KEYUP,				URHO3D_HANDLER(DinosaurGame, HandleKeyUp));
	SubscribeToEvent(E_UPDATE,				URHO3D_HANDLER(DinosaurGame, HandleUpdate));
	SubscribeToEvent(E_POSTUPDATE,			URHO3D_HANDLER(DinosaurGame, HandlePostUpdate));

	UnsubscribeFromEvent(E_SCENEUPDATE);
}

void DinosaurGame::SubscribeToServerEvents()
{
	SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(DinosaurGame, HandleClientConnected));
	SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(DinosaurGame, HandleClientDisconnected));
	SubscribeToEvent(E_CLIENTSCENELOADED, URHO3D_HANDLER(DinosaurGame, HandleClientSceneLoaded));

	SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(DinosaurGame, HandlePhysicsPreStep));

	GetSubsystem<Network>()->RegisterRemoteEvent(E_CLIENTOBJECTAUTHORITY);
	GetSubsystem<Network>()->RegisterRemoteEvent(E_UPDATEINTERFACE);
}

void DinosaurGame::SubscribeToClientEvents()
{
	SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(DinosaurGame, HandlePhysicsPreStep));

	SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(DinosaurGame, HandleServerDisconnect));

	SubscribeToEvent(E_CLIENTOBJECTAUTHORITY, URHO3D_HANDLER(DinosaurGame, HandleCharacterAllocation));

	SubscribeToEvent(E_UPDATEINTERFACE, URHO3D_HANDLER(DinosaurGame, HandleInterfaceUpdate));

	GetSubsystem<Network>()->RegisterRemoteEvent(E_CLIENTOBJECTAUTHORITY);
	GetSubsystem<Network>()->RegisterRemoteEvent(E_UPDATEINTERFACE);

}

void DinosaurGame::UpdateCharacterControls()
{
	Input* input = GetSubsystem<Input>();

	character->controls_.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
	character->controls_.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
	character->controls_.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
	character->controls_.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));
	character->controls_.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));

	character->controls_.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
	character->controls_.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;
	character->controls_.pitch_ = Clamp(character->controls_.pitch_, -80.0f, 80.0f);

	character->GetNode()->SetRotation(Quaternion(character->controls_.yaw_, Vector3::UP));
}

bool DinosaurGame::UpdateUI(float delta, int numBoids, int numThreads, int score)
{
	menuUpdateTimer -= delta;

	if (menuUpdateTimer <= 0.0f)
	{
		menuUpdateTimer = 1.0f;

		float fps = 1.0f / delta;

		scoreWindow->SetText(score);

		debugWindow->SetText(fps, numBoids, numThreads);

		return true;
	}

	return false;
}

void DinosaurGame::UpdateClientsUI()
{
	VariantMap remoteData;

	remoteData["boidNum"] = boids->GetNumEnabledBoids();

	Network* network = GetSubsystem<Network>();

	const Vector< SharedPtr<Connection> >& connections = network->GetClientConnections();

	for (unsigned i = 0; i < connections.Size(); ++i)
	{
		Connection* connection = connections[i];

		remoteData["score"] = charactersTable[connection]->GetComponent<Character>()->score;

		connection->SendRemoteEvent(E_UPDATEINTERFACE, true, remoteData);
	}
}

void DinosaurGame::HandleUpdate(StringHash, VariantMap& eventData)
{
	float deltaTime = eventData[Update::P_TIMESTEP].GetFloat();

	if (networkRole == NetworkRole::OFFLINE || networkRole == NetworkRole::SERVER)
	{
		bool updatedUI = UpdateUI(deltaTime, boids->GetNumEnabledBoids(), NUM_BOID_THREADS, character->score);

		if (hasCharacter)
		{
			UpdateCharacterControls();
			CheckCharacterCollisions(character);
		}

		if (networkRole == NetworkRole::SERVER)
		{
			for (Connection* con : charactersTable.Keys())
				CheckCharacterCollisions(charactersTable[con]->GetComponent<Character>());

			if (updatedUI)
				UpdateClientsUI();
		}

		boids->Update(deltaTime);
	}

	else if (networkRole == NetworkRole::CLIENT)
	{
		UpdateUI(deltaTime, clientBoidCount, 0, clientScore);

		UpdateClientCharacterControls();
	}
}

void DinosaurGame::HandlePostUpdate(StringHash, VariantMap& eventData)
{
	float deltaTime = eventData[PostUpdate::P_TIMESTEP].GetFloat();

	if (usingFreeCamera)
		UpdateFreeCamera(deltaTime);

	else if (hasCharacter && !usingFreeCamera)
		UpdateShoulderCamera(deltaTime);

}

void DinosaurGame::HandleKeyUp(StringHash, VariantMap& eventData)
{
	int key = eventData[KeyUp::P_KEY].GetInt();

	switch (key)
	{
	case KEY_F1:
		firstPerson_ = !firstPerson_;
		break;

	case KEY_F3:
		usingFreeCamera = !usingFreeCamera;
		break;

	case KEY_F4:
		GetSubsystem<Console>()->Toggle();
		break;

	case KEY_F11:
		ToggleFullscreen();
		break;

	case KEY_ESCAPE:
		ToggleGamePause();
		break;
	}
}

void DinosaurGame::HandlePhysicsPreStep(StringHash eventType, VariantMap& eventData)
{
	Network* network = GetSubsystem<Network>();

	Connection* serverConnection = network->GetServerConnection();

	// Client: Collect controls
	if (serverConnection)
	{
		serverConnection->SetPosition(cameraNode_->GetPosition()); // send camera position too
		serverConnection->SetControls(clientControls); // send controls to server
	}

	// Server: Read Controls, Apply them if needed
	else if (network->IsServerRunning())
		ProcessClientControls(); // take data from clients, process it
}

void DinosaurGame::ProcessClientControls()
{
	Network* network = GetSubsystem<Network>();

	const Vector< SharedPtr<Connection> >& connections = network->GetClientConnections();

	// Go through every client connected
	for (unsigned i = 0; i < connections.Size(); ++i)
	{
		Connection* connection = connections[i];

		const Controls& controls = connection->GetControls();

		Character* c = charactersTable[connection]->GetComponent<Character>();

		c->controls_ = controls;

		charactersTable[connection]->SetRotation(Quaternion(controls.yaw_, Vector3::UP));
	}
}																	  

void DinosaurGame::HandleClientConnected(StringHash eventType, VariantMap& eventData)
{
	Connection* clientCon = static_cast<Connection*>(eventData[ClientConnected::P_CONNECTION].GetPtr());

	Character* c = CreateCharacter();	 

	charactersTable[clientCon] = c->GetNode();

	clientCon->SetScene(scene_);
}

void DinosaurGame::HandleClientDisconnected(StringHash eventType, VariantMap& eventData)
{
	Connection* clientCon = static_cast<Connection*>(eventData[ClientDisconnected::P_CONNECTION].GetPtr());

	charactersTable[clientCon]->Remove();

	charactersTable.Erase(clientCon);
}

void DinosaurGame::HandleCharacterAllocation(StringHash eventType, VariantMap& eventData)
{
	nodeID = eventData["nodeID"].GetUInt();

	Node* n = scene_->GetNode(nodeID);
	
	character = n->GetComponent<Character>();

	hasCharacter = true;
}

void DinosaurGame::HandleInterfaceUpdate(StringHash eventType, VariantMap& eventData)
{
	clientScore		= eventData["score"].GetInt();
	clientBoidCount = eventData["boidNum"].GetUInt();
}

void DinosaurGame::HandleClientSceneLoaded(StringHash eventType, VariantMap& eventData)
{
	Connection* clientCon = static_cast<Connection*>(eventData[ClientSceneLoaded::P_CONNECTION].GetPtr());

	VariantMap remoteData;

	remoteData["nodeID"] = charactersTable[clientCon]->GetID();

	clientCon->SendRemoteEvent(E_CLIENTOBJECTAUTHORITY, true, remoteData);
}

void DinosaurGame::ToggleGamePause()
{
	pauseMenu->Toggle();
	debugWindow->Toggle();
	scoreWindow->Toggle();

	GetSubsystem<UI>()->GetCursor()->SetPosition({ 1024 / 2, 768 / 2 });
	GetSubsystem<UI>()->GetCursor()->SetVisible(pauseMenu->IsShown());

	GetSubsystem<Input>()->SetMouseMode(pauseMenu->IsShown() ? MM_ABSOLUTE : MM_RELATIVE);
}

void DinosaurGame::CheckCharacterCollisions(Character* chara)
{
	Vector3 playerPos = chara->GetPosition();

	std::vector<Boid*> boids = this->boids->GetBoidsInCell(playerPos);

	const int COLLIDER_DIST = 1.0f;

	for (int i = 0; i < boids.size(); i++)
	{
		if ((boids[i]->GetPosition() - playerPos).Length() < COLLIDER_DIST && boids[i]->IsEnabled())
		{
			boids[i]->Destroy();

			chara->score--;

			break;
		}
	}
}

void DinosaurGame::ConnectToServer(String addr)
{
	Network* network = GetSubsystem<Network>();

	network->Connect(addr, SERVER_PORT, scene_);
}

void DinosaurGame::StartServer()
{
	Network* network = GetSubsystem<Network>();

	network->StartServer(SERVER_PORT);
}

void DinosaurGame::CreateCamera()
{
	cameraNode_ = new Node(context_);
	Camera* camera = cameraNode_->CreateComponent<Camera>(LOCAL);

	cameraNode_->SetPosition(Vector3(0.0f, 5.0f, 0.0f));
	camera->SetFarClip(300.0f);

	GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, scene_, camera));
}

Character* DinosaurGame::CreateCharacter()
{
	ResourceCache* cache	= GetSubsystem<ResourceCache>();
	Input* input			= GetSubsystem<Input>();

	Node* charNode		= scene_->CreateChild("Player", REPLICATED);
	Node* adjustNode	= charNode->CreateChild("AdjNode", REPLICATED);

	AnimatedModel* object				= adjustNode->CreateComponent<AnimatedModel>(REPLICATED);
	RigidBody* body						= charNode->CreateComponent<RigidBody>(REPLICATED);
	CollisionShape* shape				= charNode->CreateComponent<CollisionShape>(REPLICATED);
	AnimationController* animController = adjustNode->CreateComponent<AnimationController>(REPLICATED);

	Character* chara = charNode->CreateComponent<Character>(REPLICATED);

	charNode->SetPosition({ 0.0f, 1.0f, 0.0f });

	adjustNode->SetRotation(Quaternion(180, Vector3(0, 1, 0)));

	object->SetModel(cache->GetResource<Model>("Models/Mutant/Mutant.mdl"));
	object->SetMaterial(cache->GetResource<Material>("Models/Mutant/Materials/mutant_M.xml"));
	object->SetCastShadows(true);

	object->GetSkeleton().GetBone("Mutant:Head")->animated_ = false;

	body->SetCollisionLayer(1);
	body->SetMass(1.0f);
	body->SetAngularFactor(Vector3::ZERO);
	body->SetCollisionEventMode(COLLISION_ALWAYS);

	shape->SetCapsule(0.7f, 1.8f, Vector3(0.0f, 0.9f, 0.0f));

	chara->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT | CTRL_JUMP, false);

	return chara;
}

void DinosaurGame::CreateZone(CreateMode scope)
{
	Node* zoneNode	= scene_->CreateChild("Zone", scope);
	Zone* zone		= zoneNode->CreateComponent<Zone>(scope);

	zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
	zone->SetFogColor(Color(0.5f, 0.5f, 0.7f));
	zone->SetBoundingBox(BoundingBox(-FLOOR_SIZE, FLOOR_SIZE));
}

void DinosaurGame::CreateLighting()
{
	Node* lightNode = scene_->CreateChild("DirectionalLight", LOCAL);
	Light* light	= lightNode->CreateComponent<Light>(LOCAL);

	lightNode->SetDirection(Vector3(0.3f, -0.5f, 0.425f));
	light->SetLightType(LIGHT_DIRECTIONAL);
	light->SetCastShadows(true);
	light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
	light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));
	light->SetSpecularIntensity(0.5f);
}

void DinosaurGame::CreateScene(CreateMode scope)
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	scene_ = new Scene(context_);

	scene_->CreateComponent<Octree>(scope);
	scene_->CreateComponent<PhysicsWorld>(scope);
	scene_->CreateComponent<DebugRenderer>();
}

void DinosaurGame::CreateMushroom()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	Node* node = scene_->CreateChild("Mushroom", REPLICATED);

	StaticModel* object	 = node->CreateComponent<StaticModel>(REPLICATED);
	RigidBody* body			 = node->CreateComponent<RigidBody>(REPLICATED);
	CollisionShape* shape = node->CreateComponent<CollisionShape>(REPLICATED);

	node->SetPosition({ 5.0f, 0.0f, 5.0f });
	node->SetRotation({ 0, 0, 0 });
	node->SetScale(5.0f);

	object->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
	object->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));

	object->SetCastShadows(true);

	body->SetCollisionLayer(2);
	body->SetMass(0.0f);

	shape->SetTriangleMesh(object->GetModel(), 0);
}

void DinosaurGame::CreateFloor()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();

	Node* floorNode = scene_->CreateChild("Floor", LOCAL);
	StaticModel* model = floorNode->CreateComponent<StaticModel>(LOCAL);
	RigidBody* body = floorNode->CreateComponent<RigidBody>(LOCAL);
	CollisionShape* shape = floorNode->CreateComponent<CollisionShape>(LOCAL);

	floorNode->SetPosition(Vector3(0.0f, -0.5f, 0.0f));
	floorNode->SetScale(Vector3(FLOOR_SIZE, 1.0f, FLOOR_SIZE));

	model->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
	model->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

	body->SetCollisionLayer(2);

	shape->SetBox(Vector3::ONE);
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