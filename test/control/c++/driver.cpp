/**
 * \file basic.cpp
 *
 * \date 2016-09-20
 * \author consultit
 */

#include "common.h"

///specific data/functions declarations/definitions
NodePath sceneNP;
vector<vector<PT(AnimControl)> > vehicleAnimCtls;
vector<PT(P3Driver)> drivers;
NodePath playerNP;
P3Driver* playerDriver;
LVector3f playerHeightRayCast;
int forwardMove = 1;
int forwardMoveStop = -1;
int leftMove = 2;
int leftMoveStop = -2;
int backwardMove = 3;
int backwardMoveStop = -3;
int rightMove = 4;
int rightMoveStop = -4;
//
void setParametersBeforeCreation();
AsyncTask::DoneStatus updatePlugIn(GenericAsyncTask*, void*);
NodePath getPlayerModelAnims(const string&, float, int, vector<PT(P3Driver)>&,
		vector<vector<PT(AnimControl)> >&, const LPoint3f& );
void movePlayer(const Event*, void*);

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
            "- press \"d\" to toggle debug drawing\n"
			"- press \"up\"/\"left\"/\"down\"/\"right\" arrows to move the player\n"
			"- press \"a\"/\"k\" to add 'opencontrol'/'kinematic' vehicle\n"
            "- press \"s\"/\"shift-s\" to increase/decrease last inserted vehicle's max speed\n"
            "- press \"f\"/\"shift-f\" to increase/decrease last inserted vehicle's max force\n"
            "- press \"t\" to toggle last inserted vehicle's wander behavior\n"
			"- press \"o\"/\"shift-o\" to add/remove obstacle\n");
	NodePath textNodePath = window->get_aspect_2d().attach_new_node(text);
	textNodePath.set_pos(-1.25, 0.0, 0.8);
	textNodePath.set_scale(0.035);

	// create a control manager; set root and mask to manage 'kinematic' vehicles
	WPT(ControlManager)controlMgr = new ControlManager(
			framework.get_window(0)->get_graphics_window(), window->get_render(),
			mask);

	// print creation parameters: defult values
	cout << endl << "Default creation parameters:";
	printCreationParameters();

	// load or restore all scene stuff: if passed an argument
	// try to read it from bam file
	if ((not (argc > 1)) or (not readFromBamFile(argv[1])))
	{
		// no argument or no valid bamFile
		// reparent the reference node to render
		controlMgr->get_reference_node_path().reparent_to(window->get_render());

		// get a sceneNP, naming it with "SceneNP" to ease restoring from bam
		// file
		sceneNP = loadTerrain("SceneNP");
		// and reparent to the reference node
		sceneNP.reparent_to(controlMgr->get_reference_node_path());

		// set sceneNP's collide mask
		sceneNP.set_collide_mask(mask);

//		// set plug-in type and create it (attached to the reference node)
//		controlMgr->set_parameter_value(ControlManager::DRIVER,
//				"plugin_type", "pedestrian");
//		NodePath plugInNP = controlMgr->create_control_plug_in();
//		controlPlugIn = DCAST(OSSteerPlugIn, plugInNP.node());
//
//		// set player's creation parameters as string: type and externally updated
//		controlMgr->set_parameter_value(ControlManager::DRIVER,
//				"vehicle_type", "pedestrian");
//		controlMgr->set_parameter_value(ControlManager::DRIVER,
//				"external_update", "true");
		// add the player and set a reference to it
		playerNP = getPlayerModelAnims("PlayerNP", 0.8, 0, drivers,
				vehicleAnimCtls, LPoint3f(141.597, 73.496, 2.14218));
		// highlight the player
        playerNP.set_color(1.0, 1.0, 0.0, 0);

		// set remaining creation parameters as strings before
		// the other vehicles' creation
		cout << endl << "Current creation parameters:";
		setParametersBeforeCreation();
	}
	else
	{
//		// valid bamFile
//		// restore plug-in: through control manager
//		controlPlugIn = ControlManager::get_global_ptr()->get_control_plug_in(0);
//		// restore sceneNP: through panda3d
//		sceneNP =
//				ControlManager::get_global_ptr()->get_reference_node_path().find(
//						"**/SceneNP");
//		// reparent the reference node to render
//		ControlManager::get_global_ptr()->get_reference_node_path().reparent_to(
//				window->get_render());
//
//		// restore the player's reference
//		playerNP = ControlManager::get_global_ptr()->get_reference_node_path().find(
//				"**/PlayerNP");
//
//		// restore all control vehicles (including the player)
//		int NUMVEHICLES =
//				ControlManager::get_global_ptr()->get_num_control_vehicles();
//		drivers.resize(NUMVEHICLES);
//		vehicleAnimCtls.resize(NUMVEHICLES);
//		for (int i = 0; i < NUMVEHICLES; ++i)
//		{
//			// restore the control vehicle: through control manager
//			drivers[i] =
//					ControlManager::get_global_ptr()->get_driver(i);
//			// restore animations
//			AnimControlCollection tmpAnims;
//			auto_bind(drivers[i], tmpAnims);
//			vehicleAnimCtls[i] = vector<PT(AnimControl)>(2);
//			for (int j = 0; j < tmpAnims.get_num_anims(); ++j)
//			{
//				vehicleAnimCtls[i][j] = tmpAnims.get_anim(j);
//			}
//		}
//
//		// set creation parameters as strings before other plug-ins/vehicles creation
//		cout << endl << "Current creation parameters:";
//        controlMgr->set_parameter_value(ControlManager::DRIVER, "plugin_type",
//                "pedestrian");
//        controlMgr->set_parameter_value(ControlManager::DRIVER,
//                "vehicle_type", "pedestrian");
//		setParametersBeforeCreation();
	}

	/// first option: start the default update task for all plug-ins
///	controlMgr->start_default_update();

/// second option: start the custom update task for all plug-ins
//	updateTask = new GenericAsyncTask("updatePlugIn", &updatePlugIn,
//			(void*) controlPlugIn.p());
//	framework.get_task_mgr().add(updateTask);
//
//	/// set events' callbacks
//	// handle OSSteerVehicle(s)' events
//	framework.define_key("avoid_obstacle", "handleVehicleEvent",
//			&handleVehicleEvent, nullptr);
//	framework.define_key("avoid_close_neighbor", "handleVehicleEvent",
//			&handleVehicleEvent, nullptr);
//	framework.define_key("avoid_neighbor", "handleVehicleEvent",
//			&handleVehicleEvent, nullptr);

	// write to bam file on exit
	window->get_graphics_window()->set_close_request_event(
			"close_request_event");
	framework.define_key("close_request_event", "writeToBamFile",
			&writeToBamFileAndExit, (void*) &bamFileName);

	// get player dims for kinematic ray cast
	LVecBase3f modelDims;
	LVector3f modelDeltaCenter;
	controlMgr->get_bounding_dimensions(playerNP,	modelDims, modelDeltaCenter);
	playerHeightRayCast = LVector3f(0.0, 0.0, modelDims.get_z());

	// player will be driven by arrows keys
//	playerDriver = new (nothrow) Driver(&framework, playerNP, 10);
//    playerDriver->set_max_angular_speed(100.0);
//    playerDriver->set_angular_accel(50.0);
//    playerDriver->set_max_linear_speed(LVector3f(8.0, 8.0, 8.0));
//    playerDriver->set_linear_accel(LVecBase3f(1.0, 1.0, 1.0));
//    playerDriver->set_linear_friction(1.5);
//	playerDriver->enable();
//	framework.define_key("arrow_up", "forwardMove", &movePlayer, &forwardMove);
//	framework.define_key("arrow_up-up", "forwardMoveStop", &movePlayer, &forwardMoveStop);
//	framework.define_key("arrow_left", "leftMove", &movePlayer, &leftMove);
//	framework.define_key("arrow_left-up", "leftMoveStop", &movePlayer, &leftMoveStop);
//	framework.define_key("arrow_down", "backwardMove", &movePlayer, &backwardMove);
//	framework.define_key("arrow_down-up", "backwardMoveStop", &movePlayer, &backwardMoveStop);
//	framework.define_key("arrow_right", "rightMove", &movePlayer, &rightMove);
//	framework.define_key("arrow_right-up", "rightMoveStop", &movePlayer, &rightMoveStop);

	// place camera trackball (local coordinate)
	PT(Trackball)trackball = DCAST(Trackball, window->get_mouse().find("**/+Trackball").node());
	trackball->set_pos(-128.0, 120.0, -40.0);
	trackball->set_hpr(0.0, 20.0, 0.0);

	// do the main loop, equals to call app.run() in python
	framework.main_loop();

//	delete playerDriver;

	return (0);
}

// set parameters as strings before plug-ins/vehicles creation
void setParametersBeforeCreation()
{
	ControlManager* controlMgr = ControlManager::get_global_ptr();
	ValueList<string> valueList;
	// set vehicle's mass, speed
	controlMgr->set_parameter_value(ControlManager::DRIVER, "external_update",
			"false");
	controlMgr->set_parameter_value(ControlManager::DRIVER, "mass", "2.0");
	controlMgr->set_parameter_value(ControlManager::DRIVER, "speed",
			"0.01");

	// set vehicle throwing events
	valueList.clear();
	valueList.add_value("avoid_obstacle@avoid_obstacle@1.0:"
			"avoid_close_neighbor@avoid_close_neighbor@1.0:"
			"avoid_neighbor@avoid_neighbor@1.0");
	controlMgr->set_parameter_values(ControlManager::DRIVER,
			"thrown_events", valueList);
	//
	printCreationParameters();
}

// custom update task for plug-ins
AsyncTask::DoneStatus updatePlugIn(GenericAsyncTask* task, void* data)
{
//	PT(OSSteerPlugIn)controlPlugIn = reinterpret_cast<OSSteerPlugIn*>(data);
//	// call update for controlPlugIn
//	double dt = ClockObject::get_global_clock()->get_dt();
//	controlPlugIn->update(dt);
//	// handle vehicle's animation
//	for (int i = 0; i < (int)vehicleAnimCtls.size(); ++i)
//	{
//		// get current velocity size
//		float currentVelSize = drivers[i]->get_speed();
//		if (currentVelSize > 0.0)
//		{
//			int animOnIdx, animOffIdx;
//			currentVelSize < 4.0 ? animOnIdx = 0: animOnIdx = 1;
//			animOffIdx = (animOnIdx + 1) % 2;
//			// Off anim (0:walk, 1:run)
//			if (vehicleAnimCtls[i][animOffIdx]->is_playing())
//			{
//				vehicleAnimCtls[i][animOffIdx]->stop();
//			}
//			// On amin (0:walk, 1:run)
//			vehicleAnimCtls[i][animOnIdx]->set_play_rate(
//					currentVelSize / animRateFactor[animOnIdx]);
//			if (! vehicleAnimCtls[i][animOnIdx]->is_playing())
//			{
//				vehicleAnimCtls[i][animOnIdx]->loop(true);
//			}
//		}
//		else
//		{
//			// stop any animation
//			vehicleAnimCtls[i][0]->stop();
//			vehicleAnimCtls[i][1]->stop();
//		}
//	}
//	// make playerNP kinematic (ie stand on floor)
//	if (DCAST(OSSteerVehicle, playerNP.node())->get_speed() > 0.0)
//	{
//		// get control manager
//		WPT(ControlManager)controlMgr = ControlManager::get_global_ptr();
//		// correct panda's Z: set the collision ray origin wrt collision root
//		LPoint3f pOrig = controlMgr->get_collision_root().get_relative_point(
//				controlMgr->get_reference_node_path(), playerNP.get_pos()) + playerHeightRayCast * 2.0;
//		// get the collision height wrt the reference node path
//		Pair<bool,float> gotCollisionZ = controlMgr->get_collision_height(pOrig,
//				controlMgr->get_reference_node_path());
//		if (gotCollisionZ.get_first())
//		{
//			//updatedPos.z needs correction
//			playerNP.set_z(gotCollisionZ.get_second());
//		}
//	}
//	//
	return AsyncTask::DS_cont;
}

// get the player, model and animations
NodePath getPlayerModelAnims(const string& name, float scale,
		int vehicleFileIdx, vector<PT(P3Driver)>&driver,
		vector<vector<PT(AnimControl)> >& vehicleAnimCtls, const LPoint3f& pos)
{
	// get some models, with animations, to attach to vehicles
	// get the model
	NodePath vehicleNP = window->load_model(framework.get_models(), vehicleFile[vehicleFileIdx]);
	// associate an anim with a given anim control
	AnimControlCollection tmpAnims;
	NodePath vehicleAnimNP[2];
	vehicleAnimCtls.push_back(vector<PT(AnimControl)>(2));
	if((!vehicleAnimFiles[vehicleFileIdx][0].empty()) &&
			(!vehicleAnimFiles[vehicleFileIdx][1].empty()))
	{
		// first anim -> modelAnimCtls[i][0]
		vehicleAnimNP[0] = window->load_model(vehicleNP, vehicleAnimFiles[vehicleFileIdx][0]);
		auto_bind(vehicleNP.node(), tmpAnims,
                PartGroup::HMF_ok_part_extra |
                PartGroup::HMF_ok_anim_extra |
                PartGroup::HMF_ok_wrong_root_name);
		vehicleAnimCtls.back()[0] = tmpAnims.get_anim(0);
		tmpAnims.clear_anims();
		vehicleAnimNP[0].detach_node();
		// second anim -> modelAnimCtls[i][1]
		vehicleAnimNP[1] = window->load_model(vehicleNP, vehicleAnimFiles[vehicleFileIdx][1]);
		auto_bind(vehicleNP.node(), tmpAnims,
                PartGroup::HMF_ok_part_extra |
                PartGroup::HMF_ok_anim_extra |
                PartGroup::HMF_ok_wrong_root_name);
		vehicleAnimCtls.back()[1] = tmpAnims.get_anim(0);
		tmpAnims.clear_anims();
		vehicleAnimNP[1].detach_node();
		// reparent all node paths
		vehicleAnimNP[0].reparent_to(vehicleNP);
		vehicleAnimNP[1].reparent_to(vehicleNP);
	}
	//
	WPT(ControlManager) controlMgr = ControlManager::get_global_ptr();
	// create the control vehicle (attached to the reference node)
	// note: vehicle's move type is ignored
	NodePath controlVehicleNP = controlMgr->create_driver("PlayerVehicle");
//	driver.push_back(DCAST(OSSteerVehicle, controlVehicleNP.node()));
	// set the name
	controlVehicleNP.set_name(name);
	// set scale
	controlVehicleNP.set_scale(scale);
	// set the position
	controlVehicleNP.set_pos(pos);
	// attach some geometry (a model) to control vehicle
	vehicleNP.reparent_to(controlVehicleNP);
	// return the controlVehicleNP
	return controlVehicleNP;
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
		playerDriver->enable_forward(enable);
	}
	else if(action == leftMove)
	{
		playerDriver->enable_head_left(enable);
	}
	else if(action == backwardMove)
	{
		playerDriver->enable_backward(enable);
	}
	else if(action == rightMove)
	{
		playerDriver->enable_head_right(enable);
	}
}
