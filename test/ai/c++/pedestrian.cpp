/**
 * \file pedestrian.cpp
 *
 * \date 2016-05-26
 * \author consultit
 */

#include "common.h"

///specific data/functions declarations/definitions
NodePath sceneNP;
vector<vector<PT(AnimControl)> > vehicleAnimCtls;
PT(OSSteerPlugIn)steerPlugIn;
vector<PT(OSSteerVehicle)>steerVehicles;
//
void setParametersBeforeCreation();
void toggleWanderBehavior(const Event*, void*);
AsyncTask::DoneStatus updatePlugIn(GenericAsyncTask*, void*);

int main(int argc, char *argv[])
{
	string msg("'pedestrian'");
	startFramework(argc, argv, msg);

	/// here is room for your own code
	// print some help to screen
	PT(TextNode)text;
	text = new TextNode("Help");
	text->set_text(
            msg + "\n\n"
            "- press \"d\" to toggle debug drawing\n"
			"- press \"a\"/\"k\" to add 'opensteer'/'kinematic' vehicle\n"
            "- press \"s\"/\"shift-s\" to increase/decrease last inserted vehicle's max speed\n"
            "- press \"f\"/\"shift-f\" to increase/decrease last inserted vehicle's max force\n"
            "- press \"t\" to toggle last inserted vehicle's wander behavior\n"
			"- press \"o\"/\"shift-o\" to add/remove obstacle\n");
	NodePath textNodePath = window->get_aspect_2d().attach_new_node(text);
	textNodePath.set_pos(-1.25, 0.0, -0.5);
	textNodePath.set_scale(0.035);

	// create a steer manager; set root and mask to manage 'kinematic' vehicles
	WPT(AIManager)steerMgr = new AIManager(window->get_render(), mask);

	// print creation parameters: defult values
	cout << endl << "Default creation parameters:";
	printCreationParameters();

	// load or restore all scene stuff: if passed an argument
	// try to read it from bam file
	if ((not (argc > 1)) or (not readFromBamFile(argv[1])))
	{
		// no argument or no valid bamFile
		// reparent the reference node to render
		steerMgr->get_reference_node_path().reparent_to(window->get_render());

		// get a sceneNP, naming it with "SceneNP" to ease restoring from bam
		// file
		sceneNP = loadTerrain("SceneNP");
		// and reparent to the reference node
		sceneNP.reparent_to(steerMgr->get_reference_node_path());

		// set sceneNP's collide mask
		sceneNP.set_collide_mask(mask);

		// set creation parameters as strings before plug-in/vehicles creation
		cout << endl << "Current creation parameters:";
		setParametersBeforeCreation();

		// create the plug-in (attached to the reference node)
		NodePath plugInNP = steerMgr->create_steer_plug_in();
		steerPlugIn = DCAST(OSSteerPlugIn, plugInNP.node());

		// set the pathway
		ValueList<LPoint3f> pointList;
		pointList.add_value(LPoint3f(79.474, 51.7236, 2.0207));
		pointList.add_value(LPoint3f(108.071, 51.1972, 2.7246));
		pointList.add_value(LPoint3f(129.699, 30.1742, 0.720501));
		pointList.add_value(LPoint3f(141.597, 73.496, 2.14218));
		pointList.add_value(LPoint3f(105.917, 107.032, 3.06428));
		pointList.add_value(LPoint3f(61.2637, 109.622, 3.03588));
		// use single radius pathway
		ValueList<float> radiusList;
		radiusList.add_value(4);
		steerPlugIn->set_pathway(pointList, radiusList, true, true);
	}
	else
	{
		// valid bamFile
		// restore plug-in: through steer manager
		steerPlugIn = AIManager::get_global_ptr()->get_steer_plug_in(0);
		// restore sceneNP: through panda3d
		sceneNP =
				AIManager::get_global_ptr()->get_reference_node_path().find(
						"**/SceneNP");
		// reparent the reference node to render
		AIManager::get_global_ptr()->get_reference_node_path().reparent_to(
				window->get_render());

		// restore steer vehicles
		int NUMVEHICLES =
				AIManager::get_global_ptr()->get_num_steer_vehicles();
		steerVehicles.resize(NUMVEHICLES);
		vehicleAnimCtls.resize(NUMVEHICLES);
		for (int i = 0; i < NUMVEHICLES; ++i)
		{
			// restore the steer vehicle: through steer manager
			steerVehicles[i] =
					AIManager::get_global_ptr()->get_steer_vehicle(i);
			// restore animations
			AnimControlCollection tmpAnims;
			auto_bind(steerVehicles[i], tmpAnims);
			vehicleAnimCtls[i] = vector<PT(AnimControl)>(2);
			for (int j = 0; j < tmpAnims.get_num_anims(); ++j)
			{
				vehicleAnimCtls[i][j] = tmpAnims.get_anim(j);
			}
		}

		// set creation parameters as strings before other plug-ins/vehicles creation
		cout << endl << "Current creation parameters:";
		setParametersBeforeCreation();
	}

	/// first option: start the default update task for all plug-ins
///	steerMgr->start_default_update();

/// second option: start the custom update task for all plug-ins
	updateTask = new GenericAsyncTask("updatePlugIn", &updatePlugIn,
			(void*) steerPlugIn.p());
	framework.get_task_mgr().add(updateTask);

	// DEBUG DRAWING: make the debug reference node paths sibling of the reference node
	steerMgr->get_reference_node_path_debug().reparent_to(window->get_render());
	steerMgr->get_reference_node_path_debug_2d().reparent_to(
			window->get_aspect_2d());
	// enable debug drawing
	steerPlugIn->enable_debug_drawing(window->get_camera_group());

	/// set events' callbacks
	// toggle debug draw
	toggleDebugFlag = false;
	framework.define_key("d", "toggleDebugDraw", &toggleDebugDraw,
			(void*) steerPlugIn.p());

	// handle addition steer vehicles, models and animations
	HandleVehicleData vehicleData(0.7, 0, "opensteer", sceneNP,
						steerPlugIn, steerVehicles, vehicleAnimCtls);
	framework.define_key("a", "addVehicle", &handleVehicles,
			(void*) &vehicleData);
	HandleVehicleData vehicleDataKinematic(0.7, 1, "kinematic", sceneNP,
			steerPlugIn, steerVehicles, vehicleAnimCtls);
	framework.define_key("k", "addVehicle", &handleVehicles,
			(void*) &vehicleDataKinematic);

	// handle obstacle addition
	HandleObstacleData obstacleAddition(true, sceneNP, steerPlugIn,
			LVecBase3f(0.03, 0.03, 0.03));
	framework.define_key("o", "addObstacle", &handleObstacles,
			(void*) &obstacleAddition);
	// handle obstacle removal
	HandleObstacleData obstacleRemoval(false, sceneNP, steerPlugIn);
	framework.define_key("shift-o", "removeObstacle", &handleObstacles,
			(void*) &obstacleRemoval);

	// increase/decrease last inserted vehicle's max speed
	framework.define_key("s", "changeVehicleMaxSpeed", &changeVehicleMaxSpeed,
			(void*) &steerVehicles);
	framework.define_key("shift-s", "changeVehicleMaxSpeed",
			&changeVehicleMaxSpeed, (void*) &steerVehicles);
	// increase/decrease last inserted vehicle's max force
	framework.define_key("f", "changeVehicleMaxForce", &changeVehicleMaxForce,
			(void*) &steerVehicles);
	framework.define_key("shift-f", "changeVehicleMaxForce",
			&changeVehicleMaxForce, (void*) &steerVehicles);

	// handle OSSteerVehicle(s)' events
	framework.define_key("avoid_obstacle", "handleVehicleEvent",
			&handleVehicleEvent, nullptr);
	framework.define_key("avoid_close_neighbor", "handleVehicleEvent",
			&handleVehicleEvent, nullptr);

	// write to bam file on exit
	window->get_graphics_window()->set_close_request_event(
			"close_request_event");
	framework.define_key("close_request_event", "writeToBamFile",
			&writeToBamFileAndExit, (void*) &bamFileName);

	// 'pedestrian' specific: toggle wander behavior
	framework.define_key("t", "toggleWanderBehavior", &toggleWanderBehavior,
			nullptr);

	// place camera trackball (local coordinate)
	PT(Trackball)trackball = DCAST(Trackball, window->get_mouse().find("**/+Trackball").node());
	trackball->set_pos(-128.0, 120.0, -40.0);
	trackball->set_hpr(0.0, 20.0, 0.0);

	// do the main loop, equals to call app.run() in python
	framework.main_loop();

	return (0);
}

// set parameters as strings before plug-ins/vehicles creation
void setParametersBeforeCreation()
{
	AIManager* steerMgr = AIManager::get_global_ptr();
	ValueList<string> valueList;
	// set plug-in type
	steerMgr->set_parameter_value(AIManager::STEERPLUGIN, "plugin_type",
			"pedestrian");

	// set vehicle's type, mass, speed
	steerMgr->set_parameter_value(AIManager::STEERVEHICLE, "vehicle_type",
			"pedestrian");
	steerMgr->set_parameter_value(AIManager::STEERVEHICLE, "mass", "2.0");
	steerMgr->set_parameter_value(AIManager::STEERVEHICLE, "speed",
			"0.01");

	// set vehicle throwing events
	valueList.clear();
	valueList.add_value("avoid_obstacle@avoid_obstacle@1.0:avoid_close_neighbor@avoid_close_neighbor@");
	steerMgr->set_parameter_values(AIManager::STEERVEHICLE,
			"thrown_events", valueList);
	//
	printCreationParameters();
}

// toggle wander behavior of last inserted vehicle
void toggleWanderBehavior(const Event*, void*)
{
    if (steerVehicles.size() == 0)
    {
        return;
    }

	if (steerVehicles.back()->get_wander_behavior())
	{
		steerVehicles.back()->set_wander_behavior(false);
	}
	else
	{
		steerVehicles.back()->set_wander_behavior(true);
	}
	cout << *steerVehicles.back() << "'s wander behavior is "
			<< steerVehicles.back()->get_wander_behavior() << endl;
}

// custom update task for plug-ins
AsyncTask::DoneStatus updatePlugIn(GenericAsyncTask* task, void* data)
{
	PT(OSSteerPlugIn)steerPlugIn = reinterpret_cast<OSSteerPlugIn*>(data);
	// call update for steerPlugIn
	double dt = ClockObject::get_global_clock()->get_dt();
	steerPlugIn->update(dt);
	// handle vehicle's animation
	for (int i = 0; i < (int)vehicleAnimCtls.size(); ++i)
	{
		// get current velocity size
		float currentVelSize = steerVehicles[i]->get_speed();
		if (currentVelSize > 0.0)
		{
			int animOnIdx, animOffIdx;
			currentVelSize < 4.0 ? animOnIdx = 0: animOnIdx = 1;
			animOffIdx = (animOnIdx + 1) % 2;
			// Off anim (0:walk, 1:run)
			if (vehicleAnimCtls[i][animOffIdx]->is_playing())
			{
				vehicleAnimCtls[i][animOffIdx]->stop();
			}
			// On amin (0:walk, 1:run)
			vehicleAnimCtls[i][animOnIdx]->set_play_rate(
					currentVelSize / animRateFactor[animOnIdx]);
			if (! vehicleAnimCtls[i][animOnIdx]->is_playing())
			{
				vehicleAnimCtls[i][animOnIdx]->loop(true);
			}
		}
		else
		{
			// stop any animation
			vehicleAnimCtls[i][0]->stop();
			vehicleAnimCtls[i][1]->stop();
		}
	}
	//
	return AsyncTask::DS_cont;
}
