/**
 * \file control.cpp
 *
 * \date 2016-09-20
 * \author consultit
 */

#include <pandaFramework.h>
#include <load_prc_file.h>
#include <texturePool.h>
#include <auto_bind.h>

#include <controlManager.h>
#include <p3Driver.h>

/// global data declaration
extern string dataDir;
PandaFramework framework;
WindowFramework *window = NULL;
CollideMask mask = BitMask32(0x10);
AsyncTask* updateTask = NULL;
// models and animations
string playerFile[5] =
{ "eve.egg", "ralph.egg", "sparrow.egg", "ball.egg", "red_car.egg" };
string playerAnimFiles[5][2] =
{
{ "eve-walk.egg", "eve-run.egg" },
{ "ralph-walk.egg", "ralph-run.egg" },
{ "sparrow-flying.egg", "sparrow-flying2.egg" },
{ "", "" },
{ "red_car-anim.egg", "red_car-anim2.egg" }};
const float animRateFactor[2] = { 0.6, 0.175 };
// bam file
string bamFileName("control.boo");

/// specific data/functions declarations/definitions
NodePath sceneNP;
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
ClockObject* globalClock = NULL;

// print creation parameters
void printCreationParameters()
{
	ControlManager* controlMgr = ControlManager::get_global_ptr();
	//
	ValueList<string> valueList = controlMgr->get_parameter_name_list(
			ControlManager::DRIVER);
	cout << endl << "P3Driver creation parameters:" << endl;
	for (int i = 0; i < valueList.get_num_values(); ++i)
	{
		cout << "\t" << valueList[i] << " = "
				<< controlMgr->get_parameter_value(ControlManager::DRIVER,
						valueList[i]) << endl;
	}
	//
	valueList = controlMgr->get_parameter_name_list(ControlManager::CHASER);
	cout << endl << "P3Chaser creation parameters:" << endl;
	for (int i = 0; i < valueList.get_num_values(); ++i)
	{
		cout << "\t" << valueList[i] << " = "
				<< controlMgr->get_parameter_value(ControlManager::CHASER,
						valueList[i]) << endl;
	}
}

// set parameters as strings before drivers/chasers creation
void setParametersBeforeCreation()
{
	ControlManager* controlMgr = ControlManager::get_global_ptr();
	// set driver's parameters
	controlMgr->set_parameter_value(ControlManager::DRIVER, "max_angular_speed",
			"100.0");
	controlMgr->set_parameter_value(ControlManager::DRIVER, "angular_accel",
			"50.0");
	controlMgr->set_parameter_value(ControlManager::DRIVER, "max_linear_speed",
			"8.0");
	controlMgr->set_parameter_value(ControlManager::DRIVER, "linear_accel",
			"1.0");
	controlMgr->set_parameter_value(ControlManager::DRIVER, "linear_friction",
			"0.5");
	controlMgr->set_parameter_value(ControlManager::DRIVER, "angular_friction",
			"5.0");
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
	framework.set_window_title("p3opensteer: " + msg);
	window = framework.open_window();
	if (window != (WindowFramework *) nullptr)
	{
		cout << "Opened the window successfully!\n";
		window->enable_keyboard();
		window->setup_trackball();
	}

	/// typed object init; not needed if you build inside panda source tree
	P3Driver::init_type();
	ControlManager::init_type();
	P3Driver::register_with_read_factory();
	///

	//common callbacks
}

// read scene from a file
bool readFromBamFile(string fileName)
{
	return ControlManager::get_global_ptr()->read_from_bam_file(fileName);
}

// write scene to a file (and exit)
void writeToBamFileAndExit(const Event*, void* data)
{
	string fileName = *reinterpret_cast<string*>(data);
	ControlManager::get_global_ptr()->write_to_bam_file(fileName);
	/// second option: remove custom update updateTask
//	framework.get_task_mgr().remove(updateTask);

	/// this is for testing explicit removal and destruction of all elements
	WPT(ControlManager)steerMgr = ControlManager::get_global_ptr();
	// destroy drivers
	while (steerMgr->get_num_drivers() > 0)
	{
		// destroy the first one on every cycle
		steerMgr->destroy_driver(
				NodePath::any_path(steerMgr->get_driver(0)));
///		delete DCAST(OSSteerVehicle, steerMgr->get_driver(0).node()); //ERROR
	}
//	// destroy chasers
//	while (steerMgr->get_num_chasers() > 0)
//	{
//		// destroy the first one on every cycle
//		steerMgr->destroy_chaser(
//				NodePath::any_path(steerMgr->get_chaser(0)));
/////		delete DCAST(OSSteerPlugIn, steerMgr->get_chaser(0).node()); //ERROR
//	}
	// delete control manager
	delete ControlManager::get_global_ptr();
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
	NodePath modelNP = window->load_model(framework.get_models(), playerFile[modelFileIdx]);
	// set the name
	modelNP.set_name(name);
	// set scale
	modelNP.set_scale(scale);
	// associate an anim with a given anim control
	AnimControlCollection tmpAnims;
	NodePath modelAnimNP[2];
	modelAnimCtls.push_back(vector<PT(AnimControl)>(2));
	if((!playerAnimFiles[modelFileIdx][0].empty()) &&
	(!playerAnimFiles[modelFileIdx][1].empty()))
	{
		// first anim -> modelAnimCtls[i][0]
		modelAnimNP[0] = window->load_model(modelNP, playerAnimFiles[modelFileIdx][0]);
		auto_bind(modelNP.node(), tmpAnims,
		PartGroup::HMF_ok_part_extra |
		PartGroup::HMF_ok_anim_extra |
		PartGroup::HMF_ok_wrong_root_name);
		modelAnimCtls.back()[0] = tmpAnims.get_anim(0);
		tmpAnims.clear_anims();
		modelAnimNP[0].detach_node();
		// second anim -> modelAnimCtls[i][1]
		modelAnimNP[1] = window->load_model(modelNP, playerAnimFiles[modelFileIdx][1]);
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
		WPT(ControlManager)controlMgr = ControlManager::get_global_ptr();
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

// custom update task for controls
AsyncTask::DoneStatus updateControls(GenericAsyncTask*, void* data)
{
	// call update for controls
	double dt = ClockObject::get_global_clock()->get_dt();
	playerDriver->update(dt);
	// handle player on update
	handlePlayerUpdate();
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

// driver update callback function
void driverCallback(PT(P3Driver)driver)
{
	Pair<LVector3f, ValueList<float> > speeds = driver->get_current_speeds();
	cout << *driver << string(" ") + str(globalClock->get_real_time()) + string(" - ") +
			str(globalClock->get_dt()) << endl;
	cout << "current speeds: " << speeds.get_first() << " - " <<
			speeds.get_second().get_value(0) << "," <<
			speeds.get_second().get_value(1) << endl;
	// handle player on update
	handlePlayerUpdate();
}

int main(int argc, char *argv[])
{
	string msg("'P3Driver'");
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

	// create a control manager; set root and mask to manage 'kinematic' players
	WPT(ControlManager)controlMgr = new ControlManager(
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
		// reparent the reference node to render
		controlMgr->get_reference_node_path().reparent_to(window->get_render());

		// get a sceneNP, naming it with "SceneNP" to ease restoring from bam file
		sceneNP = loadTerrainLowPoly("SceneNP");
		// and reparent to the reference node
		sceneNP.reparent_to(controlMgr->get_reference_node_path());

		// set sceneNP's collide mask
		sceneNP.set_collide_mask(mask);

		// set driver's various creation parameters as string
		setParametersBeforeCreation();
		// get a player with anims
		playerNP = getModelAnims("PlayerNP", 1.2, 0, playerAnimCtls);

		// create the driver (attached to the reference node)
		NodePath playerDriverNP = controlMgr->create_driver("PlayerDriver");
		// get a reference to the player driver
		playerDriver = DCAST(P3Driver, playerDriverNP.node());
		// set the position
		playerDriverNP.set_pos(LPoint3f(4.1, -12.0, 1.5));
		// attach some geometry (a model) to control vehicle
		playerNP.reparent_to(playerDriverNP);

		// highlight the player
		playerNP.set_color(1.0, 1.0, 0.0, 0);
	}
	else
	{
		// valid bamFile
		// restore sceneNP: through panda3d
		sceneNP =
				ControlManager::get_global_ptr()->get_reference_node_path().find(
						"**/SceneNP");
		// reparent the reference node to render
		ControlManager::get_global_ptr()->get_reference_node_path().reparent_to(
				window->get_render());

		// restore the player's reference
		playerNP =
				ControlManager::get_global_ptr()->get_reference_node_path().find(
						"**/PlayerNP");

		// restore driver: through control manager
		playerDriver = ControlManager::get_global_ptr()->get_driver(0);
		// restore animations
		playerAnimCtls.resize(1);
		AnimControlCollection tmpAnims;
		auto_bind(playerDriver, tmpAnims);
		playerAnimCtls[0] = vector<PT(AnimControl)>(2);
		for (int j = 0; j < tmpAnims.get_num_anims(); ++j)
		{
			playerAnimCtls[0][j] = tmpAnims.get_anim(j);
		}

		// set creation parameters as strings before other drivers creation
		cout << endl << "Current creation parameters:";
		setParametersBeforeCreation();
	}

	/// first option: start the default update task for all drivers
	controlMgr->start_default_update();
	playerDriver->set_update_callback(driverCallback);
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
