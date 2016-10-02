/**
 * \file multiple_pursuit.cpp
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
AsyncTask::DoneStatus updatePlugIn(GenericAsyncTask*, void*);
void addWanderer(const Event*, void*);
void addPursuer(const Event*, void*);

int main(int argc, char *argv[])
{
	string msg("'multiple pursuit'");
	startFramework(argc, argv, msg);

	/// here is room for your own code
	// print some help to screen
	PT(TextNode)text;
	text = new TextNode("Help");
	text->set_text(
            msg + "\n\n"
            "- press \"d\" to toggle debug drawing\n"
            "- press \"w\" to add 'wanderer' vehicle\n"
            "- press \"p\" to add 'pursuer' vehicle\n"
            "- press \"s\"/\"shift-s\" to increase/decrease last inserted vehicle's max speed\n"
            "- press \"f\"/\"shift-f\" to increase/decrease last inserted vehicle's max force\n");
	NodePath textNodePath = window->get_aspect_2d().attach_new_node(text);
	textNodePath.set_pos(-1.25, 0.0, -0.5);
	textNodePath.set_scale(0.035);

	// create a steer manager; set root and mask to manage 'kinematic' vehicles
	WPT(GameAIManager)steerMgr = new GameAIManager(0, window->get_render(), mask);

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

		// set world's center and radius
		steerPlugIn->set_world_center(LPoint3f(147.7, 145.6, 40.8));
		steerPlugIn->set_world_radius(25.0);
	}
	else
	{
		// valid bamFile
		// restore plug-in: through steer manager
		NodePath steerPlugInNP = NodePath::any_path(
				GameAIManager::get_global_ptr()->get_steer_plug_in(0));
		steerPlugIn = DCAST(OSSteerPlugIn, steerPlugInNP.node());
		// restore sceneNP: through panda3d
		sceneNP =
				GameAIManager::get_global_ptr()->get_reference_node_path().find(
						"**/SceneNP");
		// reparent the reference node to render
		GameAIManager::get_global_ptr()->get_reference_node_path().reparent_to(
				window->get_render());

		// restore steer vehicles
		int NUMVEHICLES =
				GameAIManager::get_global_ptr()->get_num_steer_vehicles();
		steerVehicles.resize(NUMVEHICLES);
		vehicleAnimCtls.resize(NUMVEHICLES);
		for (int i = 0; i < NUMVEHICLES; ++i)
		{
			// restore the steer vehicle: through steer manager
			steerVehicles[i] =
					GameAIManager::get_global_ptr()->get_steer_vehicle(i);
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
	HandleVehicleData wandererData(0.7, 0, "kinematic", sceneNP,
						steerPlugIn, steerVehicles, vehicleAnimCtls);
	framework.define_key("w", "addWanderer", &addWanderer,
			(void*) &wandererData);
	HandleVehicleData pursuerData(0.7, 1, "kinematic", sceneNP,
			steerPlugIn, steerVehicles, vehicleAnimCtls);
	framework.define_key("p", "addPursuer", &addPursuer,
			(void*) &pursuerData);

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

	// write to bam file on exit
	window->get_graphics_window()->set_close_request_event(
			"close_request_event");
	framework.define_key("close_request_event", "writeToBamFile",
			&writeToBamFileAndExit, (void*) &bamFileName);

	// place camera trackball (local coordinate)
	PT(Trackball)trackball = DCAST(Trackball, window->get_mouse().find("**/+Trackball").node());
	trackball->set_pos(-60.0, 45.0, -15.0);
	trackball->set_hpr(10.0, 10.0, 0.0);

	// do the main loop, equals to call app.run() in python
	framework.main_loop();

	return (0);
}

// set parameters as strings before plug-ins/vehicles creation
void setParametersBeforeCreation()
{
	GameAIManager* steerMgr = GameAIManager::get_global_ptr();
	ValueList<string> valueList;
	// set plug-in type
	steerMgr->set_parameter_value(GameAIManager::STEERPLUGIN, "plugin_type",
			"multiple_pursuit");

	// set vehicle's max force, max speed, up axis fixed
	steerMgr->set_parameter_value(GameAIManager::STEERVEHICLE, "max_force",
			"1.0");
	steerMgr->set_parameter_value(GameAIManager::STEERVEHICLE, "max_speed",
			"2.0");
	steerMgr->set_parameter_value(GameAIManager::STEERVEHICLE, "up_axis_fixed",
			"true");
	//
	printCreationParameters();
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

// adds a wanderer
void addWanderer(const Event* e, void* data)
{
	if (not data)
	{
		return;
	}

	// set vehicle's type == mp_wanderer
	GameAIManager::get_global_ptr()->set_parameter_value(GameAIManager::STEERVEHICLE, "vehicle_type",
			"mp_wanderer");
	// handle vehicle's addition
	handleVehicles(nullptr, data);
}

// adds a pursuer
void addPursuer(const Event* e, void* data)
{
	if (not data)
	{
		return;
	}

	// set vehicle's type == mp_pursuer
	GameAIManager::get_global_ptr()->set_parameter_value(GameAIManager::STEERVEHICLE, "vehicle_type",
			"mp_pursuer");
	// handle vehicle's addition
	handleVehicles(nullptr, data);
}
