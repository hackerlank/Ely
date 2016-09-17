/**
 * \file test2.cpp
 *
 * \date 2016-09-17
 * \author consultit
 */

#include <pandaFramework.h>
#include <auto_bind.h>
#include <load_prc_file.h>
#include <aiManager.h>
#include <rnNavMesh.h>
#include <rnCrowdAgent.h>
#include <collisionRay.h>
#include <mouseWatcher.h>
#include <random>
#include <bamFile.h>

extern string dataDir;

///functions' declarations
void loadAllScene();
void restoreAllScene();
NodePath getOwnerModel();
void getAgentModelAnims();
bool readFromBamFile(string);
void writeToBamFileAndExit2(const Event*, void*);
void printCreationParameters2();
void setParametersBeforeCreation();
void toggleDebugDraw2(const Event*, void*);
void toggleSetupCleanup(const Event*, void*);
void handleCrowdAgentEvent(const Event*, void*);
void placeCrowdAgents(const Event*, void*);
void setMoveTarget(const Event*, void*);
void handleObstacles2(const Event*, void*);
AsyncTask* updateTask2;
AsyncTask::DoneStatus updateNavMesh(GenericAsyncTask*, void*);
LPoint3f getRandomPos(NodePath);
PT(CollisionEntry)getCollisionEntryFromCamera();

///global data
extern PandaFramework framework;
extern WindowFramework *window;
extern CollideMask mask;
PT(RNNavMesh)navMesh;
const int NUMAGENTS = 2;
PT(RNCrowdAgent)crowdAgent[2];
//models and animations
NodePath sceneNP, agentNP[2];
string sceneFile("nav_test.egg");
string agentFile[2] =
{ "eve.egg", "ralph.egg" };
string agentAnimFiles[2][2] =
{
{ "eve-walk.egg", "eve-offbalance.egg" },
{ "ralph-walk.egg", "ralph-offbalance.egg" } };
const float rateFactor = 1.50;
PT(AnimControl)agentAnimCtls[2][2];
//obstacle model
extern string obstacleFile;
//bame file
string bamFileName2("nav_mesh.boo");
//support
extern random_device rd;

int main(int argc, char *argv[])
{
	// Load your application's configuration
	load_prc_file_data("", "model-path " + dataDir);
	load_prc_file_data("", "win-size 1024 768");
	load_prc_file_data("", "show-frame-rate-meter #t");
	load_prc_file_data("", "sync-video #t");
	// Setup your application
	framework.open_framework(argc, argv);
	framework.set_window_title("p3recastnavigation");
	window = framework.open_window();
	if (window != (WindowFramework *) NULL)
	{
		cout << "Opened the window successfully!\n";
		window->enable_keyboard();
		window->setup_trackball();
	}

	/// here is room for your own code

	/// typed object init; not needed if you build inside panda source tree
	RNNavMesh::init_type();
	RNCrowdAgent::init_type();
	AIManager::init_type();
	RNNavMesh::register_with_read_factory();
	RNCrowdAgent::register_with_read_factory();
	///
	// print some help to screen
	PT(TextNode) text;
	text = new TextNode("Help");
	text->set_text(
			"- press \"d\" to toggle debug drawing\n"
			"- press \"s\" to toggle setup cleanup\n"
			"- press \"p\" to place agents randomly\n"
			"- press \"t\", \"y\" to set agents' targets under mouse cursor\n"
			"- press \"o\" to add obstacle under mouse cursor\n"
			"- press \"shift-o\" to remove obstacle under mouse cursor\n");
	NodePath textNodePath = window->get_aspect_2d().attach_new_node(text);
	textNodePath.set_pos(-1.25, 0.0, 0.9);
	textNodePath.set_scale(0.035);

	// create a nav mesh manager; set root and mask to manage 'kinematic' agents
	WPT(AIManager)navMesMgr = new AIManager(window->get_render(), mask);

	// print creation parameters: defult values
	cout << endl << "Default creation parameters:";
	printCreationParameters2();

	// set creation parameters as strings before nav meshes/crowd agent creation
	cout << endl << "Current creation parameters:";
	setParametersBeforeCreation();

	// load or restore all scene stuff: if passed an argument
	// try to read it from bam file
	if ((not (argc > 1)) or (not readFromBamFile(argv[1])))
	{
		// no argument or no valid bamFile
		loadAllScene();
	}
	else
	{
		// valid bamFile
		restoreAllScene();
	}

	// show the added agents
	cout << "Agents added to nav mesh:" << endl;
	for (int i = 0; i < navMesh->get_num_crowd_agents(); ++i)
	{
		cout << "\t- " << *((*navMesh)[i]) << endl;
	}

	/// first option: start the path finding default update task
///	navMesMgr->start_default_update();

/// second option: start a path finding custom update task
	updateTask2 = new GenericAsyncTask("updateNavMesh", &updateNavMesh,
			(void*) navMesh.p());
	framework.get_task_mgr().add(updateTask2);

	// DEBUG DRAWING
	// make the debug reference node path sibling of the reference node
	navMesMgr->get_reference_node_path_debug().reparent_to(
			window->get_render());
	// enable debug drawing
	navMesh->enable_debug_drawing(window->get_camera_group());

	/// set events' callbacks
	// toggle debug draw
	bool toggleDebugFlag = false;
	framework.define_key("d", "toggleDebugDraw2", &toggleDebugDraw2,
			(void*) &toggleDebugFlag);

	// toggle setup (true) and cleanup (false)
	bool setupCleanupFlag = false;
	framework.define_key("s", "toggleSetupCleanup", &toggleSetupCleanup,
			(void*) &setupCleanupFlag);

	// handle CrowdAgents' events
	framework.define_key("move-event", "handleCrowdAgentEvent",
			&handleCrowdAgentEvent, nullptr);

	// place crowd agents randomly
	framework.define_key("p", "placeCrowdAgents", &placeCrowdAgents,
			nullptr);

	// handle move targets on scene surface
	framework.define_key("t", "setMoveTarget", &setMoveTarget,
			(void*) crowdAgent[0].p());
	framework.define_key("y", "setMoveTarget", &setMoveTarget,
			(void*) crowdAgent[1].p());

	// handle obstacle addition
	bool TRUE = true;
	framework.define_key("o", "addObstacle", &handleObstacles2, (void*) &TRUE);

	// handle obstacle removal
	bool FALSE = false;
	framework.define_key("shift-o", "removeObstacle", &handleObstacles2,
			(void*) &FALSE);

	// write to bam file on exit
	window->get_graphics_window()->set_close_request_event(
			"close_request_event");
	framework.define_key("close_request_event", "writeToBamFile",
			&writeToBamFileAndExit2, (void*) &bamFileName2);

	// place camera trackball (local coordinate)
	PT(Trackball)trackball = DCAST(Trackball, window->get_mouse().find("**/+Trackball").node());
	trackball->set_pos(-10.0, 90.0, -2.0);
	trackball->set_hpr(0.0, 15.0, 0.0);

	// do the main loop, equals to call app.run() in python
	framework.main_loop();

	return (0);
}

///functions' definitions
// load all scene stuff
void loadAllScene()
{
	AIManager* navMesMgr = AIManager::get_global_ptr();

	// get a sceneNP as owner model
	sceneNP = getOwnerModel();
	// set name: to ease restoring from bam file
	sceneNP.set_name("Owner");

	// create a nav mesh; its parent is the reference node
	NodePath navMeshNP = navMesMgr->create_nav_mesh();
	navMesh = DCAST(RNNavMesh, navMeshNP.node());

	// mandatory: set sceneNP as owner of navMesh
	navMesh->set_owner_node_path(sceneNP);

	// setup the nav mesh with scene as its owner object
	navMesh->setup();

	// reparent sceneNP to the reference node
	sceneNP.reparent_to(navMesMgr->get_reference_node_path());

	// reparent the reference node to render
	navMesMgr->get_reference_node_path().reparent_to(window->get_render());

	// get agentNP[] (and agentAnimNP[]) as models for crowd agents
	getAgentModelAnims();

	// create crowd agents and attach agentNP[] (and agentAnimNP[]) as children
	for (int i = 0; i < NUMAGENTS; ++i)
	{
		// set parameter for crowd agent's type (RECAST or RECAST_KINEMATIC)
		string agentType;
		(i % 2) == 0 ? agentType = "recast" : agentType = "kinematic";
		navMesMgr->set_parameter_value(AIManager::CROWDAGENT,
				"mov_type", agentType);
		// create the crowd agent
		NodePath crowdAgentNP = navMesMgr->create_crowd_agent(
				"crowdAgent" + str(i));
		crowdAgent[i] = DCAST(RNCrowdAgent, crowdAgentNP.node());
		// set the position randomly
		LPoint3f randPos = getRandomPos(sceneNP);
		crowdAgentNP.set_pos(randPos);
		// attach some geometry (a model) to crowdAgent
		agentNP[i].reparent_to(crowdAgentNP);
		// attach the crowd agent to the nav mesh
		// (crowdAgentNP is automatically reparented to navMeshNP)
		navMesh->add_crowd_agent(crowdAgentNP);
	}
}

// restore all scene stuff when read from bam file
void restoreAllScene()
{
	// restore nav mesh: through nav mesh manager
	navMesh = AIManager::get_global_ptr()->get_nav_mesh(0);
	// restore sceneNP: through panda3d
	sceneNP =
			AIManager::get_global_ptr()->get_reference_node_path().find(
					"**/Owner");
	// reparent the reference node to render
	AIManager::get_global_ptr()->get_reference_node_path().reparent_to(
			window->get_render());

	// restore crowd agents
	for (int i = 0; i < NUMAGENTS; ++i)
	{
		// restore the crowd agent: through nav mesh manager
		crowdAgent[i] =
				AIManager::get_global_ptr()->get_crowd_agent(i);
		// restore animations
		AnimControlCollection tmpAnims;
		auto_bind(crowdAgent[i], tmpAnims);
		for (int j = 0; j < tmpAnims.get_num_anims(); ++j)
		{
			agentAnimCtls[i][j] = tmpAnims.get_anim(j);
		}
	}
}

// load the owner model
NodePath getOwnerModel()
{
	// get a model to use as nav mesh' owner object
	NodePath modelNP = window->load_model(framework.get_models(), sceneFile);
	modelNP.set_collide_mask(mask);
//	modelNP.set_pos(5.0, 20.0, 5.0);
//	modelNP.set_h(45.0);
//	modelNP.set_scale(2.0);
	return modelNP;
}

// load the agents' models and anims
void getAgentModelAnims()
{
	// get some models, with animations, to attach to crowd agents
	for (int i = 0; i < NUMAGENTS; ++i)
	{
		// get the model
		agentNP[i] = window->load_model(framework.get_models(), agentFile[i]);
		// set random scale (0.35 - 0.45)
		float scale = 0.35 + 0.1 * ((float) rd() / (float) rd.max());
		agentNP[i].set_scale(scale);
		// associate an anim with a given anim control
		AnimControlCollection tmpAnims;
		NodePath agentAnimNP[2];
		// first anim -> modelAnimCtls[i][0]
		agentAnimNP[0] = window->load_model(agentNP[i],
				agentAnimFiles[i][0]);
		auto_bind(agentNP[i].node(), tmpAnims);
		agentAnimCtls[i][0] = tmpAnims.get_anim(0);
		tmpAnims.clear_anims();
		agentAnimNP[0].detach_node();
		// second anim -> modelAnimCtls[i][1]
		agentAnimNP[1] = window->load_model(agentNP[i],
				agentAnimFiles[i][1]);
		auto_bind(agentNP[i].node(), tmpAnims);
		agentAnimCtls[i][1] = tmpAnims.get_anim(0);
		tmpAnims.clear_anims();
		agentAnimNP[1].detach_node();
		// reparent all node paths
		agentAnimNP[0].reparent_to(agentNP[i]);
		agentAnimNP[1].reparent_to(agentNP[i]);
	}
}

// write scene to a file (and exit)
void writeToBamFileAndExit2(const Event* e, void* data)
{
	string fileName = *reinterpret_cast<string*>(data);
	AIManager::get_global_ptr()->write_to_bam_file(fileName);
	/// second option: remove custom update updateTask2
	framework.get_task_mgr().remove(updateTask2);
	// delete nav mesh manager
	delete AIManager::get_global_ptr();
	// close the window framework
	framework.close_framework();
	//
	exit(0);
}

// print creation parameters
void printCreationParameters2()
{
	AIManager* navMesMgr = AIManager::get_global_ptr();
	//
	ValueList<string> valueList = navMesMgr->get_parameter_name_list(
			AIManager::NAVMESH);
	cout << endl << "RNNavMesh creation parameters:" << endl;
	for (int i = 0; i < valueList.get_num_values(); ++i)
	{
		cout << "\t" << valueList[i] << " = "
				<< navMesMgr->get_parameter_value(AIManager::NAVMESH,
						valueList[i]) << endl;
	}
	//
	valueList = navMesMgr->get_parameter_name_list(
			AIManager::CROWDAGENT);
	cout << endl << "RNCrowdAgent creation parameters:" << endl;
	for (int i = 0; i < valueList.get_num_values(); ++i)
	{
		cout << "\t" << valueList[i] << " = "
				<< navMesMgr->get_parameter_value(AIManager::CROWDAGENT,
						valueList[i]) << endl;
	}
}

// set parameters as strings before nav meshes/crowd agents creation
void setParametersBeforeCreation()
{
	AIManager* navMesMgr = AIManager::get_global_ptr();
	ValueList<string> valueList;
	// tweak some nav mesh single-valued parameters
	navMesMgr->set_parameter_value(AIManager::NAVMESH, "navmesh_type",
			"obstacle");
	navMesMgr->set_parameter_value(AIManager::NAVMESH, "build_all_tiles",
			"true");
	navMesMgr->set_parameter_value(AIManager::NAVMESH, "agent_max_climb",
			"2.5");
	navMesMgr->set_parameter_value(AIManager::NAVMESH, "agent_radius",
			"1.0");
	// change an area flags cost (tricky because multi-valued)
	valueList = navMesMgr->get_parameter_values(AIManager::NAVMESH,
			"area_flags_cost");
	valueList.remove_value("1@0x02@10.0");
	valueList.add_value("1@0x02@100.0");
	navMesMgr->set_parameter_values(AIManager::NAVMESH,
			"area_flags_cost", valueList);

	valueList.clear();
	// set some off mesh connections: "area_type@flag1[:flag2...:flagN]@cost"
	valueList.add_value("31.6,24.5,-2.0:20.2,9.4,-2.4@true");
	valueList.add_value("21.1,-4.5,-2.4:32.3,-3.0,-1.5@true");
	valueList.add_value("1.2,-13.1,15.2:11.8,-18.3,10.0@true");
	navMesMgr->set_parameter_values(AIManager::NAVMESH,
			"offmesh_connection", valueList);
	// set some convex volumes: "x1,y1,z1[:x2,y2,z2...:xN,yN,zN]@area_type"
	valueList.clear();
	valueList.add_value(
			"-15.2,-22.9,-2.4:-13.4,-22.6,-2.4:-13.1,-26.5,-2.4:-16.4,-26.4,-2.7@1");
	navMesMgr->set_parameter_values(AIManager::NAVMESH, "convex_volume",
			valueList);

	// set crowd agent throwing events
	valueList.clear();
	valueList.add_value("move@move-event@0.5");
	navMesMgr->set_parameter_values(AIManager::CROWDAGENT,
			"thrown_events", valueList);
	//
	printCreationParameters2();
}

// toggle debug draw
void toggleDebugDraw2(const Event* e, void* data)
{
	if(not navMesh->is_setup())
	{
		return;
	}
	bool* toggleDebugFlag = reinterpret_cast<bool*>(data);
	*toggleDebugFlag = not *toggleDebugFlag;
	navMesh->toggle_debug_drawing(*toggleDebugFlag);
}

// toggle setup/cleanup
void toggleSetupCleanup(const Event* e, void* data)
{
	bool* setupCleanupFlag = reinterpret_cast<bool*>(data);
	if (*setupCleanupFlag)
	{
		// true: setup
		navMesh->set_owner_node_path(sceneNP);
		navMesh->setup();
		navMesh->enable_debug_drawing(window->get_camera_group());
		//
		updateTask2 = new GenericAsyncTask("updateNavMesh", &updateNavMesh,
				(void*) navMesh.p());
		framework.get_task_mgr().add(updateTask2);
	}
	else
	{
		framework.get_task_mgr().remove(updateTask2);
		// false: cleanup
		navMesh->cleanup();
	}
	*setupCleanupFlag = not *setupCleanupFlag;
}

// handle crowd agent's events
void handleCrowdAgentEvent(const Event* e, void* data)
{
	PT(RNCrowdAgent)crowAgent = DCAST(RNCrowdAgent, e->get_parameter(0).get_ptr());
	NodePath agent = NodePath::any_path(crowAgent);
	cout << "move-event - " << agent.get_name() << " - "<< agent.get_pos() << endl;
}

// place crowd agents randomly
void placeCrowdAgents(const Event* e, void* data)
{
	int i;
	for (i = 0; i < NUMAGENTS; ++i)
	{
		// remove agent from nav mesh
		navMesh->remove_crowd_agent(NodePath::any_path(crowdAgent[i]));
		// set its random position
		LPoint3f randPos = getRandomPos(sceneNP);
		NodePath::any_path(crowdAgent[i]).set_pos(randPos);
		// re-add agent to nav mesh
		navMesh->add_crowd_agent(NodePath::any_path(crowdAgent[i]));
	}
}

// handle set move target
void setMoveTarget(const Event* e, void* data)
{
	PT(RNCrowdAgent)agent = reinterpret_cast<RNCrowdAgent*>(data);
	// get the collision entry, if any
	PT(CollisionEntry)entry0 = getCollisionEntryFromCamera();
	if (entry0)
	{
		LPoint3f target = entry0->get_surface_point(NodePath());
		agent->set_move_target(target);
	}
}

// handle add/remove obstacles
void handleObstacles2(const Event* e, void* data)
{
	if(not navMesh->is_setup())
	{
		return;
	}
	bool addObstacle = *reinterpret_cast<bool*>(data);
	// get the collision entry, if any
	PT(CollisionEntry)entry0 = getCollisionEntryFromCamera();
	if (entry0)
	{
		// get the hit object
		NodePath hitObject = entry0->get_into_node_path();
		// check if hitObject is the nav mesh owner object or
		// this last one is one of its anchestors
		if (addObstacle
				and ((hitObject == navMesh->get_owner_node_path())
						or (navMesh->get_owner_node_path().is_ancestor_of(
								hitObject))))
		{
			// the hit object is the scene
			// add an obstacle to the scene
			// get a model as obstacle
			NodePath obstacleNP = window->load_model(framework.get_models(),
					obstacleFile);
			obstacleNP.set_collide_mask(mask);
			// set random scale (0.01 - 0.02)
			float scale = 0.01 + 0.01 * ((float) rd() / (float) rd.max());
			obstacleNP.set_scale(scale);
			// set obstacle position
			LPoint3f pos = entry0->get_surface_point(sceneNP);
			obstacleNP.set_pos(sceneNP, pos);
			// try to add to nav mesh
			if (navMesh->add_obstacle(obstacleNP) < 0)
			{
				// something went wrong remove from scene
				obstacleNP.remove_node();
			}
		}
		// check if hitObject is not the nav mesh owner object and
		// this last one is not one of its anchestors
		else if ((not addObstacle)
				and ((hitObject != navMesh->get_owner_node_path())
						and (not navMesh->get_owner_node_path().is_ancestor_of(
								hitObject))))
		{
			// cycle the obstacle list
			for (int index = 0; index < navMesh->get_num_obstacles();
					++index)
			{
				unsigned int ref = navMesh->get_obstacle(index);
				NodePath obstacleNP = navMesh->get_obstacle_by_ref(ref);
				// check if the hitObject == obstacle or
				// obstacle is an ancestor of the hitObject
				if ((hitObject == obstacleNP)
						or (obstacleNP.is_ancestor_of(hitObject)))
				{
					// try to remove from nav mesh
					if (navMesh->remove_obstacle(obstacleNP)
							>= 0)
					{
						// all ok remove from scene
						obstacleNP.remove_node();
						hitObject.remove_node();
						break;
					}
				}
			}
		}
	}
}

// custom path finding update task to correct panda's Z to stay on floor
AsyncTask::DoneStatus updateNavMesh(GenericAsyncTask* task, void* data)
{
	PT(RNNavMesh)navMesh = reinterpret_cast<RNNavMesh*>(data);
	// call update for navMesh
	double dt = ClockObject::get_global_clock()->get_dt();
	navMesh->update(dt);
	// handle crowd agents' animation
	for (int i = 0; i < NUMAGENTS; ++i)
	{
		// get current velocity size
		float currentVelSize = crowdAgent[i]->get_actual_velocity().length();
		if (currentVelSize > 0.0)
		{
			// walk
			agentAnimCtls[i][0]->set_play_rate(currentVelSize / rateFactor);
			if (not agentAnimCtls[i][0]->is_playing())
			{
				agentAnimCtls[i][0]->loop(true);
			}
		}
		else
		{
			// check if crowd agent is on off mesh connection
			if (crowdAgent[i]->get_traversing_state() == RNCrowdAgent::STATE_OFFMESH)
			{
				// off-balance
				if (not agentAnimCtls[i][1]->is_playing())
				{
					agentAnimCtls[i][1]->loop(true);
				}
			}
			else
			{
				// stop any animation
				agentAnimCtls[i][0]->stop();
				agentAnimCtls[i][1]->stop();
			}
		}
	}
	//
	return AsyncTask::DS_cont;
}
