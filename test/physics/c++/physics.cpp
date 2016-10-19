/**
 * \file physics.cpp
 *
 * \date 2016-10-09
 * \author consultit
 */

#include <pandaFramework.h>
#include <load_prc_file.h>
#include <texturePool.h>
#include <auto_bind.h>
#include <geoMipTerrain.h>
#include <texturePool.h>

#include <gamePhysicsManager.h>
#include <btRigidBody.h>

/// global data declaration
extern string dataDir;
PandaFramework framework;
WindowFramework *window = NULL;
CollideMask mask = BitMask32(0x10);
AsyncTask* updateTask = NULL;
static GeoMipTerrain* terrain;
static LPoint3f terrainRootNetPos;
// models and animations
string modelFile[5] =
{ "eve.egg", "ralph.egg", "sparrow.egg", "ball.egg", "red_car.egg" };
string modelAnimFiles[5][2] =
{
{ "eve-walk.egg", "eve-run.egg" },
{ "ralph-walk.egg", "ralph-run.egg" },
{ "sparrow-flying.egg", "sparrow-flying2.egg" },
{ "", "" },
{ "red_car-anim.egg", "red_car-anim2.egg" }};
const float animRateFactor[2] = { 0.6, 0.175 };
// bam file
string bamFileName("physics.boo");
// debug flag
bool toggleDebugFlag = false;

/// specific data/functions declarations/definitions
NodePath sceneNP;
ClockObject* globalClock = NULL;
// player specifics
WPT(BTRigidBody)playerRigidBody;
vector<vector<PT(AnimControl)> > playerAnimCtls;
NodePath playerNP;

// print creation parameters
void printCreationParameters()
{
	GamePhysicsManager* physicsMgr = GamePhysicsManager::get_global_ptr();
	//
	ValueList<string> valueList = physicsMgr->get_parameter_name_list(
			GamePhysicsManager::RIGIDBODY);
	cout << endl << "BTRigidBody creation parameters:" << endl;
	for (int i = 0; i < valueList.get_num_values(); ++i)
	{
		cout << "\t" << valueList[i] << " = "
				<< physicsMgr->get_parameter_value(GamePhysicsManager::RIGIDBODY,
						valueList[i]) << endl;
	}
	//
	valueList = physicsMgr->get_parameter_name_list(GamePhysicsManager::SOFTBODY);
	cout << endl << "BTSoftBody creation parameters:" << endl;
	for (int i = 0; i < valueList.get_num_values(); ++i)
	{
		cout << "\t" << valueList[i] << " = "
				<< physicsMgr->get_parameter_value(GamePhysicsManager::SOFTBODY,
						valueList[i]) << endl;
	}
}

// set parameters as strings before rigid_bodies/soft_bodies creation
void setParametersBeforeCreation()
{
	GamePhysicsManager* physicsMgr = GamePhysicsManager::get_global_ptr();
	// set rigid_body's parameters
	physicsMgr->set_parameter_value(GamePhysicsManager::RIGIDBODY,
			"shape_type", "box");
	physicsMgr->set_parameter_value(GamePhysicsManager::RIGIDBODY,
			"body_mass", "10.0");
	physicsMgr->set_parameter_value(GamePhysicsManager::RIGIDBODY,
			"collide_mask", "0x10");
	physicsMgr->set_parameter_value(GamePhysicsManager::RIGIDBODY,
			"object", "PlayerNP");

	// set soft_body's parameters
	physicsMgr->set_parameter_value(GamePhysicsManager::SOFTBODY, "static",
			"false");
	//
	printCreationParameters();
}

// start base framework
void startFramework(int argc, char *argv[], const string& msg)
{
	// Load your application's configuration
	load_prc_file_data("", "model-path " + dataDir);
	load_prc_file_data("", "win-size 1024 768");
	load_prc_file_data("", "show-frame-rate-meter #t");
	load_prc_file_data("", "sync-video #t");
	// Setup your application
	framework.open_framework(argc, argv);
	framework.set_window_title("p3physics: " + msg);
	window = framework.open_window();
	if (window != (WindowFramework *) nullptr)
	{
		cout << "Opened the window successfully!\n";
		window->enable_keyboard();
		window->setup_trackball();
	}

	/// typed object init; not needed if you build inside panda source tree
	BTRigidBody::init_type();
	GamePhysicsManager::init_type();
	BTRigidBody::register_with_read_factory();
	///

	//common callbacks
}

// read scene from a file
bool readFromBamFile(string fileName)
{
	return GamePhysicsManager::get_global_ptr()->read_from_bam_file(fileName);
}

// write scene to a file (and exit)
void writeToBamFileAndExit(const Event*, void* data)
{
	string fileName = *reinterpret_cast<string*>(data);
	GamePhysicsManager::get_global_ptr()->write_to_bam_file(fileName);
	/// second option: remove custom update updateTask
//	framework.get_task_mgr().remove(updateTask);

	/// this is for testing explicit removal and destruction of all elements
	WPT(GamePhysicsManager)physicsMgr = GamePhysicsManager::get_global_ptr();
	// destroy rigid_bodies
	while (physicsMgr->get_num_rigid_bodies() > 0)
	{
		// destroy the first one on every cycle
		physicsMgr->destroy_rigid_body(NodePath::any_path(physicsMgr->get_rigid_body(0)));
///		delete DCAST(BTRigidBody, physicsMgr->get_rigid_body(0).node()); //ERROR
	}
//	// destroy soft_bodies xxx
//	while (physicsMgr->get_num_soft_bodies() > 0)
//	{
//		// destroy the first one on every cycle
//		physicsMgr->destroy_soft_body(
//				NodePath::any_path(physicsMgr->get_soft_body(0)));
/////		delete DCAST(BTSoftBody, physicsMgr->get_soft_body(0).node()); //ERROR
//	}
	// delete managers
	delete GamePhysicsManager::get_global_ptr();
	// close the window framework
	framework.close_framework();
	//
	exit(0);
}

// load terrain low poly stuff
NodePath loadTerrainLowPoly(const string& name, float widthScale = 128,
		float heightScale = 64.0, const string& texture = "dry-grass.png")
{
	NodePath terrainNP = window->load_model(framework.get_models(),
			"terrain-low-poly.egg");
	terrainNP.set_name(name);
	terrainNP.set_transform(TransformState::make_identity());
	terrainNP.set_scale(widthScale, widthScale, heightScale);
	PT(Texture)tex =
	TexturePool::load_texture(Filename(texture));
	terrainNP.set_texture(tex);
	return terrainNP;
}

// terrain update
static AsyncTask::DoneStatus terrainUpdate(GenericAsyncTask* task, void* data)
{
	//set focal point
	//see https://www.panda3d.org/forums/viewtopic.php?t=5384
	LPoint3f focalPointNetPos =
			window->get_camera_group().get_net_transform()->get_pos();
	terrain->set_focal_point(focalPointNetPos - terrainRootNetPos);
	//update every frame
	terrain->update();
	//
	return AsyncTask::DS_cont;
}

// load terrain stuff
NodePath loadTerrain(const string& name, float widthScale = 0.5,
		float heightScale = 10.0)
{
	GeoMipTerrain *terrain = new GeoMipTerrain("terrain");
	PNMImage heightField(Filename(dataDir + string("/heightfield.png")));
	terrain->set_heightfield(heightField);
	terrain->get_root().set_transform(TransformState::make_identity());
	//sizing
	float environmentWidthX = (heightField.get_x_size() - 1) * widthScale;
	float environmentWidthY = (heightField.get_y_size() - 1) * widthScale;
	float environmentWidth = (environmentWidthX + environmentWidthY) / 2.0;
	terrain->get_root().set_sx(widthScale);
	terrain->get_root().set_sy(widthScale);
	terrain->get_root().set_sz(heightScale);
	//set other terrain's properties
	unsigned short blockSize = 64, minimumLevel = 0;
	float nearPercent = 0.1, farPercent = 0.7;
	float terrainLODmin = min<float>(minimumLevel, terrain->get_max_level());
	GeoMipTerrain::AutoFlattenMode flattenMode = GeoMipTerrain::AFM_off;
	terrain->set_block_size(blockSize);
	terrain->set_near(nearPercent * environmentWidth);
	terrain->set_far(farPercent * environmentWidth);
	terrain->set_min_level(terrainLODmin);
	terrain->set_auto_flatten(flattenMode);
	//terrain texturing
	PT(TextureStage)textureStage0 = new TextureStage("TextureStage0");
	PT(Texture)textureImage = TexturePool::load_texture(
			Filename(string("terrain.png")));
	terrain->get_root().set_tex_scale(textureStage0, 1.0, 1.0);
	terrain->get_root().set_texture(textureStage0, textureImage, 1);
	terrain->get_root().set_collide_mask(mask);
	terrain->get_root().set_name(name);
	//brute force generation
	bool bruteForce = true;
	terrain->set_bruteforce(bruteForce);
	//Generate the terrain
	terrain->generate();
	//check if terrain needs update or not
	if (not bruteForce)
	{
		//save the net pos of terrain root
		terrainRootNetPos = terrain->get_root().get_net_transform()->get_pos();
		// Add a task to keep updating the terrain
		framework.get_task_mgr().add(
				new GenericAsyncTask("terrainUpdate", &terrainUpdate,
						(void*) nullptr));
	}
	//
	return terrain->get_root();
}

// get model and animations
NodePath getModelAnims(const string& name, float scale,
		int modelFileIdx, vector<vector<PT(AnimControl)> >& modelAnimCtls)
{
	// get some models, with animations
	// get the model
	NodePath modelNP = window->load_model(framework.get_models(), modelFile[modelFileIdx]);
	// set the name
	modelNP.set_name(name);
	// set scale
	modelNP.set_scale(scale);
	// associate an anim with a given anim control
	AnimControlCollection tmpAnims;
	NodePath modelAnimNP[2];
	modelAnimCtls.push_back(vector<PT(AnimControl)>(2));
	if((!modelAnimFiles[modelFileIdx][0].empty()) &&
	(!modelAnimFiles[modelFileIdx][1].empty()))
	{
		// first anim -> modelAnimCtls[i][0]
		modelAnimNP[0] = window->load_model(modelNP, modelAnimFiles[modelFileIdx][0]);
		auto_bind(modelNP.node(), tmpAnims,
		PartGroup::HMF_ok_part_extra |
		PartGroup::HMF_ok_anim_extra |
		PartGroup::HMF_ok_wrong_root_name);
		modelAnimCtls.back()[0] = tmpAnims.get_anim(0);
		tmpAnims.clear_anims();
		modelAnimNP[0].detach_node();
		// second anim -> modelAnimCtls[i][1]
		modelAnimNP[1] = window->load_model(modelNP, modelAnimFiles[modelFileIdx][1]);
		auto_bind(modelNP.node(), tmpAnims,
		PartGroup::HMF_ok_part_extra |
		PartGroup::HMF_ok_anim_extra |
		PartGroup::HMF_ok_wrong_root_name);
		modelAnimCtls.back()[1] = tmpAnims.get_anim(0);
		tmpAnims.clear_anims();
		modelAnimNP[1].detach_node();
		// reparent all node paths
		modelAnimNP[0].reparent_to(modelNP);
		modelAnimNP[1].reparent_to(modelNP);
	}
	//
	return modelNP;
}

// handles player on every update
void handlePlayerUpdate(float currentVelSize = 0.0)
{
	// handle player's animation
	for (int i = 0; i < (int) playerAnimCtls.size(); ++i)
	{
		if (currentVelSize > 0.0)
		{
			int animOnIdx, animOffIdx;
			currentVelSize < 5.0 ? animOnIdx = 0 : animOnIdx = 1;
			animOffIdx = (animOnIdx + 1) % 2;
			// Off anim (0:walk, 1:run)
			if (playerAnimCtls[i][animOffIdx]->is_playing())
			{
				playerAnimCtls[i][animOffIdx]->stop();
			}
			// On amin (0:walk, 1:run)
			playerAnimCtls[i][animOnIdx]->set_play_rate(
					currentVelSize * animRateFactor[animOnIdx]);
			if (!playerAnimCtls[i][animOnIdx]->is_playing())
			{
				playerAnimCtls[i][animOnIdx]->loop(true);
			}
		}
		else
		{
			// stop any animation
			playerAnimCtls[i][0]->stop();
			playerAnimCtls[i][1]->stop();
		}
	}
//	NodePath playerRigidBodyNP = NodePath::any_path(playerRigidBody);
//	// make playerNP kinematic (ie stand on floor)
//	if (currentVelSize > 0.0)
//	{
//		// get control manager
//		WPT(GameControlManager)controlMgr = GameControlManager::get_global_ptr();
//		// correct player's Z: set the collision ray origin wrt collision root
//		LPoint3f pOrig = controlMgr->get_collision_root().get_relative_point(
//				controlMgr->get_reference_node_path(), playerRigidBodyNP.get_pos()) + playerHeightRayCast * 2.0;
//		// get the collision height wrt the reference node path
//		Pair<bool,float> gotCollisionZ = controlMgr->get_collision_height(pOrig,
//				controlMgr->get_reference_node_path());
//		if (gotCollisionZ.get_first())
//		{
//			//updatedPos.z needs correction
//			playerRigidBodyNP.set_z(gotCollisionZ.get_second());
//		}
//	}
}

// custom update task for controls
AsyncTask::DoneStatus updateControls(GenericAsyncTask*, void* data)
{
	// call update for controls
	double dt = ClockObject::get_global_clock()->get_dt();
	playerRigidBody->update(dt);
	// handle player on update
	handlePlayerUpdate();
	//
	return AsyncTask::DS_cont;
}

// rigid body update callback function
void rigidBodyCallback(PT(BTRigidBody)rigidBody)
{
	if (rigidBody != playerRigidBody)
	{
		return;
	}
	float currentVelSize =
			abs(playerRigidBody->get_linear_velocity().length());
	// handle player on callback
	handlePlayerUpdate(currentVelSize);
}

//// soft body update callback function xxx
//void softBodyCallback(PT(BTSoftBody)soft_body)
//{
//	NodePath refNP =
//			GamePhysicsManager::get_global_ptr()->get_reference_node_path();
//	float distLS = (NodePath::any_path(playerRigidBody).get_pos(refNP) -
//			NodePath::any_path(soft_body).get_pos(refNP)).length();
//	cout << *soft_body << string(" ") + str(globalClock->get_real_time()) +
//			string(" - ") + str(distLS) << endl;
//}

// toggle debug draw
void toggleDebugDraw(const Event* e, void* data)
{
	toggleDebugFlag = not toggleDebugFlag;
	GamePhysicsManager::get_global_ptr()->debug(toggleDebugFlag);
}

// collision notify
void collisionNotify(const Event* e, void* data)
{
	PT(PandaNode)object0 = DCAST(PandaNode, e->get_parameter(0).get_ptr());
	PT(PandaNode)object1 = DCAST(PandaNode, e->get_parameter(1).get_ptr());

	cout << string("got '") + e->get_name() + string("' between '") +
			object0->get_name() + string("' and '") + object1->get_name() +
			string("'") << endl;
}

int main(int argc, char *argv[])
{
	string msg("'BTRigidBody & BTSoftBody & BTGhost'");
	startFramework(argc, argv, msg);

	/// here is room for your own code
	// print some help to screen
	PT(TextNode)text;
	text = new TextNode("Help");
	text->set_text(
			msg + "\n\n"
			"- press \"d\" to toggle debug drawing\n"
			"- press \"up\"/\"left\"/\"down\"/\"right\" arrows to move the player\n");
	NodePath textNodePath = window->get_aspect_2d().attach_new_node(text);
	textNodePath.set_pos(-1.25, 0.0, 0.8);
	textNodePath.set_scale(0.035);

	// create a physics manager
	WPT(GamePhysicsManager)physicsMgr = new GamePhysicsManager(10,
			window->get_render(), mask);

	// print creation parameters: default values
	cout << endl << "Default creation parameters:";
	printCreationParameters();

	// load or restore all scene stuff: if passed an argument
	// try to read it from bam file
	if ((not (argc > 1)) or (not readFromBamFile(argv[1])))
	{
		// no argument or no valid bamFile
		// reparent reference node to render
		physicsMgr->get_reference_node_path().reparent_to(window->get_render());

		// get a sceneNP, naming it with "SceneNP" to ease restoring from bam file
//		sceneNP = loadTerrainLowPoly("SceneNP");
		sceneNP = loadTerrain("SceneNP", 1.0, 60.0);
		// create scene's rigid_body (attached to the reference node)
		NodePath sceneRigidBodyNP =
				physicsMgr->create_rigid_body("SceneRigidBody");
		// get a reference to the scene's rigid_body
		PT(BTRigidBody) sceneRigidBody =
				DCAST(BTRigidBody, sceneRigidBodyNP.node());
		// set some parameters: trimesh shape, static, collide mask etc...
//		sceneRigidBody->set_shape_type(GamePhysicsManager::TRIANGLEMESH);
		sceneRigidBody->set_shape_type(GamePhysicsManager::HEIGHTFIELD);
		sceneRigidBody->set_shape_heightfield_file(dataDir + string("/heightfield.png"));
		// other parameters
		sceneRigidBody->switch_body_type(BTRigidBody::STATIC);
		sceneRigidBodyNP.set_collide_mask(mask);
		sceneRigidBodyNP.set_pos(LPoint3f(0.0, 0.0, 0.0));
		// setup the player's rigid body
		sceneRigidBody->setup(sceneNP);

		// set various creation parameters as string for other rigid bodies
		setParametersBeforeCreation();

		// get a player with anims, reparent to reference node, set transform
		playerNP = getModelAnims("PlayerNP", 1.2, 4, playerAnimCtls);
		playerNP.reparent_to(physicsMgr->get_reference_node_path());
		playerNP.set_pos(LPoint3f(4.1, -12.0, 100.1));
		playerNP.set_hpr(LVecBase3f(0.0, 90.0, 0.0));
		// create player's rigid_body (attached to the reference node)
		NodePath playerRigidBodyNP =
				physicsMgr->create_rigid_body("PlayerRigidBody");
 		// get a reference to the player's rigid_body
 		playerRigidBody = DCAST(BTRigidBody, playerRigidBodyNP.node());
	}
	else
	{
		// valid bamFile
		// reparent reference node to render
		physicsMgr->get_reference_node_path().reparent_to(window->get_render());

		// restore sceneNP: through panda3d
		sceneNP = physicsMgr->get_reference_node_path().find("**/SceneNP");
		// restore the player's reference
		playerNP = physicsMgr->get_reference_node_path().find("**/PlayerNP");

		// restore rigid_bodies: through physics manager
		for (int i = 0; i < physicsMgr->get_num_rigid_bodies(); ++i)
		{
			PT(BTRigidBody)rigid_body = GamePhysicsManager::get_global_ptr()->get_rigid_body(i);
			if (rigid_body->get_name() == "PlayerRigidBody")
			{
				playerRigidBody = rigid_body;
			}
		}

		// set creation parameters as strings before other objects creation
		cout << endl << "Current creation parameters:";
		setParametersBeforeCreation();
	}

	// setup DEBUG DRAWING
	physicsMgr->init_debug();
	framework.define_key("d", "toggleDebugDraw", &toggleDebugDraw,
			nullptr);

	// enable collision notify event: BTRigidBody_BTRigidBody_Collision
	physicsMgr->enable_collision_notify(GamePhysicsManager::COLLISIONNOTIFY, 10.0);
	framework.define_key("BTRigidBody_BTRigidBody_Collision", "collisionNotify",
			&collisionNotify, nullptr);

	/// first option: start the default update task for all drivers
	physicsMgr->start_default_update();
	playerRigidBody->set_update_callback(rigidBodyCallback);
	globalClock = ClockObject::get_global_clock();

    /// second option: start the custom update task for the drivers
//	updateTask = new GenericAsyncTask("updateControls", &updateControls,
//			nullptr);
//	framework.get_task_mgr().add(updateTask);
//	updateTask->set_sort(10);

	// write to bam file on exit
	window->get_graphics_window()->set_close_request_event(
			"close_request_event");
	framework.define_key("close_request_event", "writeToBamFile",
			&writeToBamFileAndExit, (void*) &bamFileName);

	// place camera trackball (local coordinate)
	PT(Trackball)trackball = DCAST(Trackball, window->get_mouse().find("**/+Trackball").node());
	trackball->set_pos(0.0, 120.0, 5.0);
	trackball->set_hpr(0.0, 10.0, 0.0);

	// do the main loop, equals to call app.run() in python
	framework.main_loop();

	return (0);
}
