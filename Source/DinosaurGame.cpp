
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

	SetupAndShowMainMenu();

	AddConsole();

	GetSubsystem<Input>()->SetMouseVisible(true);

	Sample::InitMouseMode(MM_ABSOLUTE);
}

void DinosaurGame::SubscribeToGameEvents()
{
	SubscribeToEvent(E_KEYUP,		URHO3D_HANDLER(DinosaurGame, HandleKeyUp));
	SubscribeToEvent(E_POSTUPDATE,	URHO3D_HANDLER(DinosaurGame, HandlePostUpdate));

	UnsubscribeFromEvent(E_SCENEUPDATE);
}

void DinosaurGame::SubscribeToNetworkEvents_Client()
{
	SubscribeToEvent(E_CLIENTOBJECTAUTHORITY, URHO3D_HANDLER(DinosaurGame, HandleCharacterAllocation));
	SubscribeToEvent(E_UPDATEINTERFACE, URHO3D_HANDLER(DinosaurGame, HandleInterfaceUpdate_Client));

	GetSubsystem<Network>()->RegisterRemoteEvent(E_CLIENTOBJECTAUTHORITY);
	GetSubsystem<Network>()->RegisterRemoteEvent(E_UPDATEINTERFACE);
}

void DinosaurGame::UpdateControls(Controls& controls)
{
	Input* input = GetSubsystem<Input>();

	controls.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
	controls.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
	controls.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
	controls.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));
	controls.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));

	controls.Set(CTRL_SHOOT, input->GetMouseButtonDown(MOUSEB_LEFT));

	controls.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
	controls.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;

	//controls.pitch_ = Clamp(clientControls.pitch_, -80.0f, 80.0f);
}

void DinosaurGame::HandleUpdate_Client(StringHash, VariantMap& eventData)
{
	float delta = eventData[Update::P_TIMESTEP].GetFloat();

	UpdateInterface_Client(delta);

	UpdateControls_Client();
}

void DinosaurGame::HandleUpdate_Server(StringHash, VariantMap& eventData)
{
	float delta = eventData[Update::P_TIMESTEP].GetFloat();

	UpdateInterface_Server(delta);

	if (player.AssignedCharacter())
	{
		UpdateControls_Server();

		CheckCharacterCollisions(player.GetCharacter());

		CheckProjectileCollisions(player);

		player.UpdateMissile(delta);
	}

	for (Connection* con : clients.Keys())
	{
		clients[con].UpdateMissile(delta);

		CheckCharacterCollisions(clients[con].GetCharacter());

		CheckProjectileCollisions(clients[con]);
	}

	boids->Update(delta);
}

void DinosaurGame::HandlePhysicsPreStep_Client(StringHash, VariantMap&)
{
	Network* network = GetSubsystem<Network>();

	Connection* serverConnection = network->GetServerConnection();
	
	serverConnection->SetPosition(cameraNode_->GetDirection());
	serverConnection->SetControls(player.GetControls());
}

void DinosaurGame::HandlePhysicsPreStep_Server(StringHash, VariantMap&)
{
	ProcessClientControls();
}

void DinosaurGame::StartGame_Client()
{
	networkRole = NetworkRole::CLIENT;

	player = Player();

	SetupGame();

	boids->Initialise();

	CreateScene_Client();

	SubscribeToNetworkEvents_Client();

	String address = mainMenu->ipAddress->GetText().Trimmed();

	ConnectToServer(address.Empty() ? "localhost" : address);
}

void DinosaurGame::StartGame_Server()
{
	networkRole = NetworkRole::SERVER;

	SetupGame();

	CreateScene_Server();

	player = Player(CreateCharacter()->GetNode(), true);

	StartServer();

	boids->Initialise(GetSubsystem<ResourceCache>(), scene_);

	SubscribeToGameEvents_Server();
}

void DinosaurGame::CreateScene_Client()
{
	if (scene_ != nullptr)
		scene_->Clear();

	CreateScene(LOCAL);
	CreateCamera();
	CreateLighting();
	CreateFloor();
}

void DinosaurGame::CreateScene_Server()
{
	if (scene_ != nullptr)
		scene_->Clear();

	CreateScene(REPLICATED);
	CreateCamera();
	CreateZone(REPLICATED);
	CreateLighting();
	CreateFloor();
}

void DinosaurGame::SetupGame()
{
	SetupPauseMenu();

	GetSubsystem<Input>()->SetMouseMode(MM_RELATIVE);

	mainMenu->Hide();

	debugWindow->Hide();

	scoreWindow->Show();

	boids.reset(new BoidManager());

	cameraMode = CameraMode::SHOULDER;
}

void DinosaurGame::SubscribeToGameEvents_Client()
{
	SubscribeToGameEvents();

	SubscribeToEvent(E_UPDATE,			URHO3D_HANDLER(DinosaurGame, HandleUpdate_Client));
	SubscribeToEvent(E_PHYSICSPRESTEP,	URHO3D_HANDLER(DinosaurGame, HandlePhysicsPreStep_Client));
}

void DinosaurGame::SubscribeToGameEvents_Server()
{
	SubscribeToGameEvents();

	SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(DinosaurGame, HandleClientConnected));
	SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(DinosaurGame, HandleClientDisconnected));
	SubscribeToEvent(E_CLIENTSCENELOADED, URHO3D_HANDLER(DinosaurGame, HandleClientSceneLoaded));

	SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(DinosaurGame, HandleMouseDown_Server));
	SubscribeToEvent(E_UPDATE,			URHO3D_HANDLER(DinosaurGame, HandleUpdate_Server));
	SubscribeToEvent(E_PHYSICSPRESTEP,	URHO3D_HANDLER(DinosaurGame, HandlePhysicsPreStep_Server));

	GetSubsystem<Network>()->RegisterRemoteEvent(E_CLIENTOBJECTAUTHORITY);
	GetSubsystem<Network>()->RegisterRemoteEvent(E_UPDATEINTERFACE);
}

void DinosaurGame::UpdateControls_Client()
{
	UpdateControls(player.GetControls());
}

void DinosaurGame::UpdateControls_Server()
{
	Character* c = player.GetCharacter();

	Controls& controls = player.GetControls();

	UpdateControls(controls);

	c->GetNode()->SetRotation(Quaternion(c->controls_.yaw_, Vector3::UP));
}

void DinosaurGame::UpdateInterface_Client(float delta)
{
	if (clientUI.isUpdated)
	{
		float fps = 1.0f / delta;

		scoreWindow->SetText(clientUI.score);
		debugWindow->SetText(fps, clientUI.numBoids, 0);

		clientUI.isUpdated = false;
	}
}

void DinosaurGame::UpdateInterface_Server(float delta)
{
	menuTimer -= delta;

	if (menuTimer <= 0.0f)
	{
		menuTimer = 1.0f;

		float fps = 1.0f / delta;

		scoreWindow->SetText(player.GetScore());
		debugWindow->SetText(fps, boids->GetNumEnabledBoids(), NUM_BOID_THREADS);

		UpdateClientInterface_Server();
	}
}

void DinosaurGame::UpdateClientInterface_Server()
{
	VariantMap remoteData;

	remoteData["boidNum"] = boids->GetNumEnabledBoids();

	Network* network = GetSubsystem<Network>();

	const Vector< SharedPtr<Connection> >& connections = network->GetClientConnections();

	for (unsigned i = 0; i < connections.Size(); ++i)
	{
		Connection* con = connections[i];

		remoteData["score"] = clients[con].GetCharacter()->score;

		con->SendRemoteEvent(E_UPDATEINTERFACE, true, remoteData);
	}
}

void DinosaurGame::HandleMouseDown_Server(StringHash, VariantMap& eventData)
{
	int key = eventData[MouseButtonDown::P_BUTTON].GetInt();

	switch (key)
	{
	case MOUSEB_LEFT:
		ShootMissile();
		break;
	}
}

void DinosaurGame::HandlePostUpdate(StringHash, VariantMap& eventData)
{
	float deltaTime = eventData[PostUpdate::P_TIMESTEP].GetFloat();

	if (cameraMode == CameraMode::FREE)
	{
		UpdateFreeCamera(deltaTime);
	}

	else if (player.AssignedCharacter() && cameraMode == CameraMode::SHOULDER)
	{
		UpdateShoulderCamera(deltaTime);
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

void DinosaurGame::SetupAndShowMainMenu()
{
	mainMenu->Show();

	SubscribeToEvent(mainMenu->offlinePlayBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, MM_OfflinePlayBtnDown));
	SubscribeToEvent(mainMenu->hostOnlineBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, MM_HostGameBtnDown));
	SubscribeToEvent(mainMenu->joinOnlineBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, JoinGameFromMainMenu));
	SubscribeToEvent(mainMenu->quitBtn,			E_RELEASED, URHO3D_HANDLER(DinosaurGame, MM_QuitGameBtnDown));
}

void DinosaurGame::SetupPauseMenu()
{
	pauseMenu->Hide();

	SubscribeToEvent(pauseMenu->continueBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, PM_ContinueBtnDown));
	SubscribeToEvent(pauseMenu->disconnectBtn,	E_RELEASED, URHO3D_HANDLER(DinosaurGame, PM_DisconnectBtnDown));
	SubscribeToEvent(pauseMenu->connectBtn,		E_RELEASED, URHO3D_HANDLER(DinosaurGame, PM_JoinSvrBtnDown));
	SubscribeToEvent(pauseMenu->quitBtn,		E_RELEASED, URHO3D_HANDLER(DinosaurGame, PM_QuitBtnDown));
}

void DinosaurGame::MM_OfflinePlayBtnDown(StringHash, VariantMap&)
{
	networkRole = NetworkRole::OFFLINE;

	CreateOfflineScene();

	SetupGame();
}

void DinosaurGame::MM_HostGameBtnDown(StringHash, VariantMap&)	{ StartGame_Server(); }
void DinosaurGame::MM_QuitGameBtnDown(StringHash, VariantMap&)	{ Quit(); }
void DinosaurGame::PM_ContinueBtnDown(StringHash, VariantMap&)	{ ToggleGamePause(); }
void DinosaurGame::PM_QuitBtnDown(StringHash, VariantMap&)		{ Quit(); }

void DinosaurGame::JoinGameFromMainMenu(StringHash, VariantMap&) 
{ 
	player = Player();

	StartGame_Client();
}

void DinosaurGame::PM_DisconnectBtnDown(StringHash, VariantMap&)
{
	UnsubscribeToGameEvents();

	Network* network = GetSubsystem<Network>();

	switch (networkRole)
	{
	case NetworkRole::CLIENT:
		network->Disconnect();
		break;

	case NetworkRole::SERVER:
		network->StopServer();
		break;
	}

	ToggleGamePause();

	Cursor* cur = GetSubsystem<UI>()->GetCursor();
	Input* inp = GetSubsystem<Input>();

	cur->SetVisible(true);
	inp->SetMouseMode(MM_ABSOLUTE);

	scene_->Clear();

	scoreWindow->Hide();

	mainMenu->Show();

	player = Player();
}

void DinosaurGame::PM_JoinSvrBtnDown(StringHash a, VariantMap& b)
{
	UnsubscribeToGameEvents();

	ToggleGamePause();

	Network* network = GetSubsystem<Network>();

	switch (networkRole)
	{
	case NetworkRole::CLIENT:
		network->Disconnect();
		break;

	case NetworkRole::SERVER:
		network->StopServer();
		break;
	}

	GetSubsystem<UI>()->GetCursor()->SetVisible(true);

	StartGame_Client();
}

void DinosaurGame::UnsubscribeToGameEvents()
{
	UnsubscribeFromEvent(E_KEYUP);
	UnsubscribeFromEvent(E_UPDATE);
	UnsubscribeFromEvent(E_POSTUPDATE);
	UnsubscribeFromEvent(E_PHYSICSPRESTEP);
	UnsubscribeFromEvent(E_MOUSEBUTTONDOWN);
}

void DinosaurGame::CreateOfflineScene()
{
	CreateScene(LOCAL);
	CreateCamera();
	CreateZone(LOCAL);
	CreateLighting();
	CreateFloor();
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
	Node* characterNode = player.GetCharacterNode();

	Controls controls = player.GetControls();

	Quaternion rot = characterNode->GetRotation();
	Quaternion dir = rot * Quaternion(controls.pitch_, Vector3::RIGHT);

	Node* headNode		= characterNode->GetChild("Mutant:Head", true);
	float limitPitch	= Clamp(controls.pitch_, -45.0f, 45.0f);
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

void DinosaurGame::ShootMissile()
{
	player.Shoot(player.GetCharacterPosition(), cameraNode_->GetDirection());
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
		cameraMode = cameraMode == CameraMode::FREE ? CameraMode::SHOULDER : CameraMode::FREE;
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

void DinosaurGame::ProcessClientControls()
{
	Network* network = GetSubsystem<Network>();

	const Vector< SharedPtr<Connection> >& connections = network->GetClientConnections();

	for (unsigned i = 0; i < connections.Size(); ++i)
	{
		Connection* con = connections[i];

		const Controls& controls = con->GetControls();

		Character* c = clients[con].GetCharacter();

		c->controls_ = controls;

		c->GetNode()->SetRotation(Quaternion(controls.yaw_, Vector3::UP));

		if (c->controls_.IsDown(CTRL_SHOOT))
		{
			clients[con].Shoot(c->GetPosition(), con->GetPosition());
		}
	}
}																	  

void DinosaurGame::HandleClientConnected(StringHash eventType, VariantMap& eventData)
{
	Connection* con = static_cast<Connection*>(eventData[ClientConnected::P_CONNECTION].GetPtr());

	Character* c = CreateCharacter();	 

	clients[con] = Player(c->GetNode());

	con->SetScene(scene_);
}

void DinosaurGame::HandleClientDisconnected(StringHash eventType, VariantMap& eventData)
{
	Connection* con = static_cast<Connection*>(eventData[ClientDisconnected::P_CONNECTION].GetPtr());

	clients[con].DestroyCharacter();

	clients.Erase(con);
}

void DinosaurGame::HandleCharacterAllocation(StringHash eventType, VariantMap& eventData)
{
	//unsinged int nodeID = eventData["nodeID"].GetUInt();

	std::cout << "a,";

	//Node* n = scene_->GetNode(nodeID);

	std::cout << "b,";

	//player = Player(n);

	std::cout << "c,";

	SubscribeToGameEvents_Client();

	std::cout << "d\n";
}

void DinosaurGame::HandleInterfaceUpdate_Client(StringHash eventType, VariantMap& eventData)
{
	clientUI = ClientUI({true, eventData["boidNum"].GetInt(), eventData["score"].GetInt()});
}

void DinosaurGame::HandleClientSceneLoaded(StringHash eventType, VariantMap& eventData)
{
	Connection* con = static_cast<Connection*>(eventData[ClientSceneLoaded::P_CONNECTION].GetPtr());

	VariantMap remoteData;

	remoteData["nodeID"] = clients[con].GetCharacterNode()->GetID();

	con->SendRemoteEvent(E_CLIENTOBJECTAUTHORITY, true, remoteData);
}

void DinosaurGame::ToggleGamePause()
{
	pauseMenu->Toggle();
	debugWindow->Toggle();
	scoreWindow->Toggle();

	//GetSubsystem<UI>()->GetCursor()->SetPosition({ 1024 / 2, 768 / 2 });
	GetSubsystem<UI>()->GetCursor()->SetVisible(pauseMenu->IsShown());

	GetSubsystem<Input>()->SetMouseMode(pauseMenu->IsShown() ? MM_ABSOLUTE : MM_RELATIVE);
}

void DinosaurGame::CheckCharacterCollisions(Character* chara)
{
	Vector3 playerPos = chara->GetPosition();

	std::vector<Boid*> boidsInCell = boids->GetBoidsInCell(playerPos);

	const int COLLIDER_DIST = 1.0f;

	for (int i = 0; i < boidsInCell.size(); i++)
	{
		if ((boidsInCell[i]->GetPosition() - playerPos).Length() < COLLIDER_DIST && boidsInCell[i]->IsEnabled())
		{
			boidsInCell[i]->Destroy();

			chara->score--;
		}
	}
}

void DinosaurGame::CheckProjectileCollisions(Player player)
{
	std::vector<Vector3> positions = player.GetProjectilePositions();

	const int COLLIDER_DIST = 1.5f;

	int hits = 0;

	for (int i = 0; i < positions.size(); ++i)
	{
		Vector3 p = positions[i];

		std::vector<Boid*> boidsInCell = boids->GetBoidsInCell(p);

		for (int j = 0; j < boidsInCell.size(); j++)
		{
			if ((boidsInCell[j]->GetPosition() - p).Length() < COLLIDER_DIST && boidsInCell[j]->IsEnabled())
			{
				boidsInCell[j]->Destroy();

				hits++;
			}
		}
	}

	player.SetScore(player.GetScore() - hits);
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
	CollisionShape* shape				= charNode->CreateComponent<CollisionShape>(LOCAL);
	AnimationController* animController = adjustNode->CreateComponent<AnimationController>(REPLICATED);

	Character* chara = charNode->CreateComponent<Character>(LOCAL);

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

	chara->controls_.Reset();

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