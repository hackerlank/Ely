/**
 * \file control.cpp
 *
 * \date 2016-10-09
 * \author consultit
 */

#include <pandaFramework.h>
#include <load_prc_file.h>
#include <texturePool.h>
#include <auto_bind.h>

#include <gamePhysicsManager.h>
#include <p3Sound3d.h>
#include <p3Listener.h>
#include <gameControlManager.h>
#include <p3Driver.h>
#include <p3Chaser.h>

/// global data declaration
extern string dataDir;
PandaFramework framework;
WindowFramework *window = NULL;
CollideMask mask = BitMask32(0x10);
AsyncTask* updateTask = NULL;
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
// sound effects
string soundFile[5] =
{ "eve_voice.wav", "", "sparrow_chirp.wav", "", "red_car_engine.wav" };
string soundName[5] =
{ "eve-voice", "", "sparrow-chirp", "", "red-car-engine" };
// bam file
string bamFileName("audio.boo");

/// specific data/functions declarations/definitions
NodePath sceneNP;
ClockObject* globalClock = NULL;
// camera specifics
WPT(P3Listener)cameraListener;
// player specifics
WPT(P3Sound3d)playerSound3d;
vector<vector<PT(AnimControl)> > playerAnimCtls;
NodePath playerNP;
WPT(P3Driver)playerDriver;
LVector3f playerHeightRayCast;
int forwardMove = 1;
int forwardMoveStop = -1;
int leftMove = 2;
int leftMoveStop = -2;
int backwardMove = 3;
int backwardMoveStop = -3;
int rightMove = 4;
int rightMoveStop = -4;
// pursuer specifics
WPT(P3Sound3d)pursuerSound3d;
vector<vector<PT(AnimControl)> > pursuerAnimCtls;
NodePath pursuerNP;
WPT(P3Chaser)pursuerChaser;

// print creation parameters
void printCreationParameters()
{
	GamePhysicsManager* audioMgr = GamePhysicsManager::get_global_ptr();
	//
	ValueList<string> valueList = audioMgr->get_parameter_name_list(
			GamePhysicsManager::SOUND3D);
	cout << endl << "P3Sound3d creation parameters:" << endl;
	for (int i = 0; i < valueList.get_num_values(); ++i)
	{
		cout << "\t" << valueList[i] << " = "
				<< audioMgr->get_parameter_value(GamePhysicsManager::SOUND3D,
						valueList[i]) << endl;
	}
	//
	valueList = audioMgr->get_parameter_name_list(GamePhysicsManager::LISTENER);
	cout << endl << "P3Listener creation parameters:" << endl;
	for (int i = 0; i < valueList.get_num_values(); ++i)
	{
		cout << "\t" << valueList[i] << " = "
				<< audioMgr->get_parameter_value(GamePhysicsManager::LISTENER,
						valueList[i]) << endl;
	}
}

// set parameters as strings before sound3ds/listeners creation
void setParametersBeforeCreation()
{
	GamePhysicsManager* audioMgr = GamePhysicsManager::get_global_ptr();
	// set sound3d's parameters
	audioMgr->set_parameter_value(GamePhysicsManager::SOUND3D, "static", "false");
	audioMgr->set_parameter_value(GamePhysicsManager::SOUND3D, "min_distance",
			"0.5");
	// set listener's parameters
	audioMgr->set_parameter_value(GamePhysicsManager::LISTENER, "static",
			"false");
	//
	GameControlManager* controlMgr = GameControlManager::get_global_ptr();
	// set driver's parameters
	controlMgr->set_parameter_value(GameControlManager::DRIVER, "max_angular_speed",
			"50.0");
	controlMgr->set_parameter_value(GameControlManager::DRIVER, "angular_accel",
			"10.0");
	controlMgr->set_parameter_value(GameControlManager::DRIVER, "max_linear_speed",
			"150.0");
	controlMgr->set_parameter_value(GameControlManager::DRIVER, "linear_accel",
			"10.0");
	controlMgr->set_parameter_value(GameControlManager::DRIVER, "linear_friction",
			"0.5");
	controlMgr->set_parameter_value(GameControlManager::DRIVER, "angular_friction",
			"5.0");
	// set chaser's parameters
	controlMgr->set_parameter_value(GameControlManager::CHASER, "fixed_relative_position",
			"false");
	controlMgr->set_parameter_value(GameControlManager::CHASER, "max_distance",
			"25.0");
	controlMgr->set_parameter_value(GameControlManager::CHASER, "min_distance",
			"18.0");
	controlMgr->set_parameter_value(GameControlManager::CHASER, "max_height",
			"18.0");
	controlMgr->set_parameter_value(GameControlManager::CHASER, "min_height",
			"15.0");
	controlMgr->set_parameter_value(GameControlManager::CHASER, "friction",
			"5.0");
	controlMgr->set_parameter_value(GameControlManager::CHASER, "fixed_look_at",
			"true");
	controlMgr->set_parameter_value(GameControlManager::CHASER, "mouse_head",
			"true");
	controlMgr->set_parameter_value(GameControlManager::CHASER, "mouse_pitch",
			"true");
	controlMgr->set_parameter_value(GameControlManager::CHASER, "look_at_distance",
			"5.0");
	controlMgr->set_parameter_value(GameControlManager::CHASER, "look_at_height",
			"12.5");
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
	framework.set_window_title("p3audio: " + msg);
	window = framework.open_window();
	if (window != (WindowFramework *) nullptr)
	{
		cout << "Opened the window successfully!\n";
		window->enable_keyboard();
		window->setup_trackball();
	}

	/// typed object init; not needed if you build inside panda source tree
	P3Sound3d::init_type();
	P3Listener::init_type();
	GamePhysicsManager::init_type();
	P3Sound3d::register_with_read_factory();
	P3Listener::register_with_read_factory();
	P3Driver::init_type();
	P3Chaser::init_type();
	GameControlManager::init_type();
	P3Driver::register_with_read_factory();
	P3Chaser::register_with_read_factory();
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
	// before saving to bam file, reparent listener to reference node
	NodePath::any_path(cameraListener).reparent_to(
			GamePhysicsManager::get_global_ptr()->get_reference_node_path());
	GamePhysicsManager::get_global_ptr()->write_to_bam_file(fileName);
	/// second option: remove custom update updateTask
	framework.get_task_mgr().remove(updateTask);

	/// this is for testing explicit removal and destruction of all elements
	WPT(GamePhysicsManager)audioMgr = GamePhysicsManager::get_global_ptr();
	// destroy sound3ds
	while (audioMgr->get_num_sound3ds() > 0)
	{
		// destroy the first one on every cycle
		audioMgr->destroy_sound3d(NodePath::any_path(audioMgr->get_sound3d(0)));
///		delete DCAST(P3Sound3d, audioMgr->get_sound3d(0).node()); //ERROR
	}
	// destroy listeners
	while (audioMgr->get_num_listeners() > 0)
	{
		// destroy the first one on every cycle
		audioMgr->destroy_listener(
				NodePath::any_path(audioMgr->get_listener(0)));
///		delete DCAST(P3Listener, audioMgr->get_listener(0).node()); //ERROR
	}
	//
	WPT(GameControlManager)controlMgr = GameControlManager::get_global_ptr();
	// destroy drivers
	while (controlMgr->get_num_drivers() > 0)
	{
		// destroy the first one on every cycle
		controlMgr->destroy_driver(
				NodePath::any_path(controlMgr->get_driver(0)));
///		delete DCAST(P3Driver, controlMgr->get_driver(0).node()); //ERROR
	}
	// destroy chasers
	while (controlMgr->get_num_chasers() > 0)
	{
		// destroy the first one on every cycle
		controlMgr->destroy_chaser(
				NodePath::any_path(controlMgr->get_chaser(0)));
///		delete DCAST(P3Chaser, controlMgr->get_chaser(0).node()); //ERROR
	}
	// delete managers
	delete GamePhysicsManager::get_global_ptr();
	delete GameControlManager::get_global_ptr();
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
	terrainNP.set_scale(widthScale, widthScale, heightScale);
	PT(Texture)tex =
	TexturePool::load_texture(Filename(texture));
	terrainNP.set_texture(tex);
	return terrainNP;
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
void handlePlayerUpdate()
{
	// get current forward velocity size
	float currentVelSize =
			abs(playerDriver->get_current_speeds().get_first().get_y());
	NodePath playerDriverNP = NodePath::any_path(playerDriver);
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
	// make playerNP kinematic (ie stand on floor)
	if (currentVelSize > 0.0)
	{
		// get control manager
		WPT(GameControlManager)controlMgr = GameControlManager::get_global_ptr();
		// correct player's Z: set the collision ray origin wrt collision root
		LPoint3f pOrig = controlMgr->get_collision_root().get_relative_point(
				controlMgr->get_reference_node_path(), playerDriverNP.get_pos()) + playerHeightRayCast * 2.0;
		// get the collision height wrt the reference node path
		Pair<bool,float> gotCollisionZ = controlMgr->get_collision_height(pOrig,
				controlMgr->get_reference_node_path());
		if (gotCollisionZ.get_first())
		{
			//updatedPos.z needs correction
			playerDriverNP.set_z(gotCollisionZ.get_second());
		}
	}
}

// handles pursuer on every update
void handlePursuerUpdate()
{
	// get current forward velocity size
	float currentVelSize =
			abs(DCAST(P3Driver, pursuerChaser->get_chased_object().node())->
					get_current_speeds().get_first().get_y());
	// handle pursuer's animation
	for (int i = 0; i < (int) pursuerAnimCtls.size(); ++i)
	{
		int animOnIdx, animOffIdx;
		currentVelSize < 5.0 ? animOnIdx = 0 : animOnIdx = 1;
		animOffIdx = (animOnIdx + 1) % 2;
		// Off anim (0:walk, 1:run)
		if (pursuerAnimCtls[i][animOffIdx]->is_playing())
		{
			pursuerAnimCtls[i][animOffIdx]->stop();
		}
		// On amin (0:walk, 1:run)
		pursuerAnimCtls[i][animOnIdx]->set_play_rate(
				(currentVelSize + 1.0) * 0.5);
		if (!pursuerAnimCtls[i][animOnIdx]->is_playing())
		{
			pursuerAnimCtls[i][animOnIdx]->loop(true);
		}
	}
}

// custom update task for controls
AsyncTask::DoneStatus updateControls(GenericAsyncTask*, void* data)
{
	// call update for controls
	double dt = ClockObject::get_global_clock()->get_dt();
	playerDriver->update(dt);
	pursuerChaser->update(dt);
	// handle player on update
	handlePlayerUpdate();
	// handle player on update
	handlePursuerUpdate();
	//
	return AsyncTask::DS_cont;
}

// player's movement callback
void movePlayer(const Event*, void* data)
{
	if (not playerDriver)
	{
		return;
	}
	int action = *reinterpret_cast<int*>(data);
	bool enable;
	if (action > 0)
	{
		//start movement
		enable = true;
	}
	else
	{
		action = -action;
		//stop movement
		enable = false;
	}
	//
	if (action == forwardMove)
	{
		playerDriver->set_move_forward(enable);
	}
	else if (action == leftMove)
	{
		playerDriver->set_rotate_head_left(enable);
	}
	else if (action == backwardMove)
	{
		playerDriver->set_move_backward(enable);
	}
	else if (action == rightMove)
	{
		playerDriver->set_rotate_head_right(enable);
	}
}

// sound3d update callback function
void sound3dCallback(PT(P3Sound3d)sound3d)
{
	if (sound3d != playerSound3d)
	{
		return;
	}
	float currentVelSize =
			abs(playerDriver->get_current_speeds().get_first().get_y());
	(*sound3d)[0]->set_play_rate(0.1 + currentVelSize * 0.05);
}

// listener update callback function
void listenerCallback(PT(P3Listener)listener)
{
	NodePath refNP =
			GamePhysicsManager::get_global_ptr()->get_reference_node_path();
	float distLS = (NodePath::any_path(playerSound3d).get_pos(refNP) -
			NodePath::any_path(listener).get_pos(refNP)).length();
	cout << *listener << string(" ") + str(globalClock->get_real_time()) +
			string(" - ") + str(distLS) << endl;
}

int main(int argc, char *argv[])
{
	string msg("'P3Sound3d & P3Listener'");
	startFramework(argc, argv, msg);

	/// here is room for your own code
	// print some help to screen
	PT(TextNode)text;
	text = new TextNode("Help");
	text->set_text(
			msg + "\n\n"
			"- press \"up\"/\"left\"/\"down\"/\"right\" arrows to move the player\n");
	NodePath textNodePath = window->get_aspect_2d().attach_new_node(text);
	textNodePath.set_pos(-1.25, 0.0, 0.8);
	textNodePath.set_scale(0.035);

	// create a audio manager
	WPT(GamePhysicsManager)audioMgr = new GamePhysicsManager();
	// create a control manager; set root and mask to manage 'kinematic' players
	WPT(GameControlManager)controlMgr = new GameControlManager(
			framework.get_window(0)->get_graphics_window(), 10,
			window->get_render(), mask);

	// print creation parameters: default values
	cout << endl << "Default creation parameters:";
	printCreationParameters();

	// load or restore all scene stuff: if passed an argument
	// try to read it from bam file
	if ((not (argc > 1)) or (not readFromBamFile(argv[1])))
	{
		// no argument or no valid bamFile
		// set a common reference node and reparent it to render
		controlMgr->set_reference_node_path(audioMgr->get_reference_node_path());
		audioMgr->get_reference_node_path().reparent_to(window->get_render());

		// get a sceneNP, naming it with "SceneNP" to ease restoring from bam file
		sceneNP = loadTerrainLowPoly("SceneNP");
		// and reparent to the reference node
		sceneNP.reparent_to(audioMgr->get_reference_node_path());

		// set sceneNP's collide mask
		sceneNP.set_collide_mask(mask);

		// set various creation parameters as string
		setParametersBeforeCreation();
		// get a player with anims
		playerNP = getModelAnims("PlayerNP", 1.2, 4, playerAnimCtls);
		// get a pursuer with anims
		pursuerNP = getModelAnims("PursuerNP", 0.01, 2, pursuerAnimCtls);
		pursuerNP.set_h(180);

		// create the driver (attached to the reference node)
		NodePath playerDriverNP = controlMgr->create_driver("PlayerDriver");
		// get a reference to the player's driver
		playerDriver = DCAST(P3Driver, playerDriverNP.node());
		// set the position
		playerDriverNP.set_pos(LPoint3f(4.1, -12.0, 1.5));
		// attach some geometry (a model) to player's driver
		playerNP.reparent_to(playerDriverNP);

		// create the pursuer (attached to the reference node)
		NodePath pursuerChaserNP = controlMgr->create_chaser("PursuerChaser");
		// get a reference to the pursuer's chaser
		pursuerChaser = DCAST(P3Chaser, pursuerChaserNP.node());
		// set the chased object: playerDriverNP or playerNP
		pursuerChaser->set_chased_object(playerDriverNP);
		// attach some geometry (a model) to pursuer's chaser
		pursuerNP.reparent_to(pursuerChaserNP);

		// create some sound3ds (attached to the reference node)
		NodePath playerSound3dNP = audioMgr->create_sound3d("PlayerSound3d");
		NodePath pursuerSound3dNP = audioMgr->create_sound3d("PursuerSound3d");
		// get a reference to the sound3ds
		playerSound3d = DCAST(P3Sound3d, playerSound3dNP.node());
		pursuerSound3d = DCAST(P3Sound3d, pursuerSound3dNP.node());
		// reparent the sound3ds
		playerSound3dNP.reparent_to(playerNP);
		pursuerSound3dNP.reparent_to(pursuerNP);
		// attach some sounds to the sound3ds
		playerSound3d->add_sound(soundName[4], soundFile[4]);
		pursuerSound3d->add_sound(soundName[2], soundFile[2]);
		// set sounds looping
		PT(PhysicsSound)sound = playerSound3d->get_sound_by_name(soundName[4]);
		sound->set_loop(true);
		sound->play();
		//
		sound = pursuerSound3d->get_sound_by_name(soundName[2]);
		sound->set_loop(true);
		sound->play();

		// create a listener (attached to the reference node)
		NodePath cameraListenerNP = audioMgr->create_listener("CameraListener");
		// get a reference to the camera's listener
		cameraListener = DCAST(P3Listener, cameraListenerNP.node());
		// reparent listener to the camera
		cameraListenerNP.reparent_to(window->get_camera_group());
	}
	else
	{
		// valid bamFile
		// set a common reference node and reparent it to render
		controlMgr->set_reference_node_path(
				audioMgr->get_reference_node_path());
		audioMgr->get_reference_node_path().reparent_to(window->get_render());

		// restore sceneNP: through panda3d
		sceneNP = audioMgr->get_reference_node_path().find("**/SceneNP");
		// restore the player's reference
		playerNP = audioMgr->get_reference_node_path().find("**/PlayerNP");

		// restore driver: through control manager
		playerDriver = controlMgr->get_driver(0);
		// restore animations
		playerAnimCtls.resize(1);
		AnimControlCollection tmpAnims;
		auto_bind(playerDriver, tmpAnims);
		playerAnimCtls[0] = vector<PT(AnimControl)>(2);
		for (int j = 0; j < tmpAnims.get_num_anims(); ++j)
		{
			playerAnimCtls[0][j] = tmpAnims.get_anim(j);
		}

		// restore chaser: through control manager
		pursuerChaser = controlMgr->get_chaser(0);
		// restore animations
		pursuerAnimCtls.resize(1);
		tmpAnims.clear_anims();
		auto_bind(pursuerChaser, tmpAnims);
		pursuerAnimCtls[0] = vector<PT(AnimControl)>(2);
		for (int j = 0; j < tmpAnims.get_num_anims(); ++j)
		{
			pursuerAnimCtls[0][j] = tmpAnims.get_anim(j);
		}

		// restore sound3ds: through audio manager
		for (int i = 0; i < audioMgr->get_num_sound3ds(); ++i)
		{
			PT(P3Sound3d)sound3d = GamePhysicsManager::get_global_ptr()->get_sound3d(i);
			if (sound3d->get_name() == "PlayerSound3d")
			{
				playerSound3d = sound3d;
			}
			if (sound3d->get_name() == "PursuerSound3d")
			{
				pursuerSound3d = sound3d;
			}
		}
		// set sounds looping
		PT(PhysicsSound)sound = playerSound3d->get_sound_by_name(soundName[4]);
		sound->set_loop(true);
		sound->play();
		//
		sound = pursuerSound3d->get_sound_by_name(soundName[2]);
		sound->set_loop(true);
		sound->play();

		// restore listeners: through audio manager
		cameraListener = audioMgr->get_listener(0);
		// reparent listener to the camera
		NodePath::any_path(cameraListener).reparent_to(
				window->get_camera_group());

		// set creation parameters as strings before other objects creation
		cout << endl << "Current creation parameters:";
		setParametersBeforeCreation();
	}

	/// first option: start the default update task for all drivers
	audioMgr->start_default_update();
    playerSound3d->set_update_callback(sound3dCallback);
    pursuerSound3d->set_update_callback(sound3dCallback);
    cameraListener->set_update_callback(listenerCallback);
    globalClock = ClockObject::get_global_clock();

    /// second option: start the custom update task for the drivers
	updateTask = new GenericAsyncTask("updateControls", &updateControls,
			nullptr);
	framework.get_task_mgr().add(updateTask);
	updateTask->set_sort(10);

	// write to bam file on exit
	window->get_graphics_window()->set_close_request_event(
			"close_request_event");
	framework.define_key("close_request_event", "writeToBamFile",
			&writeToBamFileAndExit, (void*) &bamFileName);

	// get player dims for kinematic ray cast
	LVecBase3f modelDims;
	LVector3f modelDeltaCenter;
	controlMgr->get_bounding_dimensions(playerNP, modelDims, modelDeltaCenter);
	playerHeightRayCast = LVector3f(0.0, 0.0, modelDims.get_z());

	// player will be driven by arrows keys
	framework.define_key("arrow_up", "forwardMove", &movePlayer, &forwardMove);
	framework.define_key("arrow_up-up", "forwardMoveStop", &movePlayer,
			&forwardMoveStop);
	framework.define_key("arrow_left", "leftMove", &movePlayer, &leftMove);
	framework.define_key("arrow_left-up", "leftMoveStop", &movePlayer,
			&leftMoveStop);
	framework.define_key("arrow_down", "backwardMove", &movePlayer,
			&backwardMove);
	framework.define_key("arrow_down-up", "backwardMoveStop", &movePlayer,
			&backwardMoveStop);
	framework.define_key("arrow_right", "rightMove", &movePlayer, &rightMove);
	framework.define_key("arrow_right-up", "rightMoveStop", &movePlayer,
			&rightMoveStop);

	// place camera trackball (local coordinate)
	PT(Trackball)trackball = DCAST(Trackball, window->get_mouse().find("**/+Trackball").node());
	trackball->set_pos(0.0, 120.0, 5.0);
	trackball->set_hpr(0.0, 10.0, 0.0);

	// do the main loop, equals to call app.run() in python
	framework.main_loop();

	return (0);
}
