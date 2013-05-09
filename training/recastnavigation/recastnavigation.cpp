/*
 *   This file is part of Ely.
 *
 *   Ely is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Ely is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Ely.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file /Ely/training/recastnavigation.cpp
 *
 * \date 14/mar/2013 10:05:13
 * \author marco
 */

#include "Utilities/Tools.h"
#include "Support/Raycaster.h"
#include <unistd.h>
#include <cctype>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <load_prc_file.h>
#include <auto_bind.h>
#include <partBundleHandle.h>
#include <character.h>
#include <animControlCollection.h>
#include <pandaFramework.h>
#include <bulletWorld.h>
#include <bulletTriangleMesh.h>
#include <bulletTriangleMeshShape.h>
#include <bulletSphericalConstraint.h>
#include <bulletClosestHitRayResult.h>
#include <bulletRigidBodyNode.h>
#include <bulletCharacterControllerNode.h>
#include <bulletSphericalConstraint.h>
#include <bulletCylinderShape.h>
#include <mouseWatcher.h>

#include "RN.h"

#define DEBUG_DRAW

///Data constants
std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/");
std::string rnDir(
		"/REPOSITORY/KProjects/WORKSPACE/recastnavigation/RecastDemo/Bin/Meshes/");
///
//convert obj to egg: obj2egg -TR 90,0,0 nav_test.obj -o nav_test_panda.egg
//triangulate nav_test_panda.egg and...
//...(re)convert egg to obj:
//egg2obj -cs y-up -o nav_test_panda.obj nav_test_panda.egg

///dungeon
//std::string meshNameEgg("dungeon_panda.egg");
//std::string meshNameObj("dungeon_panda.obj");
//LPoint3f agentPos(2.90322, 5.36927, 1.0);
///nav_test
std::string meshNameEgg("nav_test_panda.egg");
std::string meshNameObj("nav_test_panda.obj");
LPoint3f agentPos(4.19123, 9.90642, 8.3);
///Mesh scale
float meshScale = 2.0;

///eve actor
std::string actorFile("data/models/eve.bam");
std::string anim0File("data/models/eve-walk.bam");
std::string anim1File("data/models/eve-run.bam");
const float agentMaxSpeed = 1.5;
const float rateFactor = 1.25;
const float actorScale = 0.4;
///guy actor
//std::string actorFile("data/models/guy.bam");
//std::string anim0File("data/models/guy-walk.bam");
//const float agentMaxSpeed = 2.0;
//const float rateFactor = 2.00;
//const float actorScale = 0.1;

const int AI_TASK_SORT = 10;
const int PHYSICS_TASK_SORT = 20;

BitMask32 allOnButZeroMask, allOffButZeroMask;

///Obstacles data
std::string obstacleName("box.egg");
NodePath obstacleNP;
std::map<NodePath, TempObstacle*> obstacleTable;

//Declarations
// don't use PT or CPT with AnimControlCollection
AnimControlCollection rn_anim_collection;
AsyncTask::DoneStatus update_physics(GenericAsyncTask* task, void* data);
SMARTPTR(BulletWorld)start(PandaFramework** panda, int argc, char **argv, WindowFramework** window, bool debugPhysics);
void end(PandaFramework* panda);
NodePath createWorldMesh(SMARTPTR(BulletWorld)mBulletWorld, WindowFramework* window, float scale);
NodePath createAgent(SMARTPTR(BulletWorld)mBulletWorld, WindowFramework* window,
MOVTYPE movType, float& agentRadius, float& agentHeight, BulletConstraint** pcs);

//CALLBACKS
const int CALLBACKSNUM = 5;
//
void setCrowdTarget(Raycaster* raycaster, void* data);
const int SET_TARGET_Idx = 0;
std::string SET_TARGET_Key("shift-mouse1");
//
void buildTile(Raycaster* raycaster, void* data);
const int BUILD_TILE_Idx = 1;
std::string BUILD_TILE_Key("shift-mouse3");
void removeTile(Raycaster* raycaster, void* data);
const int REMOVE_TILE_Idx = 2;
std::string REMOVE_TILE_Key("shift-alt-mouse3");
//
void addObstacle(Raycaster* raycaster, void* data);
const int ADD_OBSTACLE_Idx = 3;
std::string ADD_OBSTACLE_Key("shift-mouse2");
void removeObstacle(Raycaster* raycaster, void* data);
const int REMOVE_OBSTACLE_Idx = 4;
std::string REMOVE_OBSTACLE_Key("shift-alt-mouse2");
//

int main(int argc, char **argv)
{
	allOnButZeroMask = BitMask32::all_on();
	allOnButZeroMask.clear_bit(0);
	allOffButZeroMask = BitMask32::all_off();
	allOffButZeroMask.set_bit(0);
//	std::cout << allOnButZeroMask << std::endl;
//	std::cout << allOffButZeroMask << std::endl;
//	std::cout << (allOnButZeroMask & allOffButZeroMask) << std::endl;

///use getopt: -r(recast), -c(character), -k(kinematic with z raycast),
///		-d(debug), -s(solo), t(tile), -o(obstacles)
	SAMPLETYPE sampleType = SOLO;
	agentPos *= meshScale;
#ifndef WITHCHARACTER
	MOVTYPE movType = RECAST;
#else
	MOVTYPE movType = CHARACTER;
#endif
	bool debugPhysics = false;
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "rcbkdsto")) != -1)
	{
		switch (c)
		{
#ifndef WITHCHARACTER
		case 'r':
			movType = RECAST;
			break;
		case 'k':
			movType = KINEMATIC;
			break;
		case 'b':
			movType = RIGID;
			break;
#else
			case 'c':
			movType = CHARACTER;
			break;
#endif
		case 'd':
			debugPhysics = true;
			break;
		case 's':
			sampleType = SOLO;
			break;
		case 't':
			sampleType = TILE;
			break;
		case 'o':
			sampleType = OBSTACLE;
			break;
		case '?':
			if (isprint(optopt))
				std::cerr << "Unknown option " << optopt << std::endl;
			else
				std::cerr << "Unknown option character " << optopt << std::endl;
			return 1;
		default:
			abort();
		}
	}
	//start
	PandaFramework* panda;
	WindowFramework* window;
	SMARTPTR(BulletWorld)mBulletWorld = start(&panda, argc, argv, &window, debugPhysics);

#ifdef DEBUG_DRAW
	//set a debug node path
	NodePath renderDebug = window->get_render().attach_new_node("renderDebug");
	renderDebug.set_bin("fixed",10);
	//set transparency attrib on render
//	renderDebug.set_transparency(TransparencyAttrib::M_alpha);
#endif

	//Create world mesh
	NodePath worldMesh = createWorldMesh(mBulletWorld, window, meshScale);
//	worldMesh.hide();

	//create a global ray caster
	new Raycaster(panda, window, mBulletWorld, CALLBACKSNUM);

	//Create agent
	float agentRadius, agentHeight;
	BulletConstraint* cs = NULL;
	NodePath character = createAgent(mBulletWorld, window, movType, agentRadius,
			agentHeight, &cs);

	///RN common
	RN* rn = new RN(window->get_render(), mBulletWorld);
	//load geometry mesh
	rn->loadGeomMesh(rnDir, meshNameObj, meshScale);

	//create nav mesh
	switch (sampleType)
	{
	case SOLO:
#ifdef DEBUG_DRAW
		rn->createGeomMesh(new Sample_SoloMesh(renderDebug), SOLO);
#else
		rn->createGeomMesh(new Sample_SoloMesh(), SOLO);
#endif
		break;
	case TILE:
	{
		TileSettings tileSettings;
#ifdef DEBUG_DRAW
		rn->createGeomMesh(new Sample_TileMesh(renderDebug), TILE);
#else
		rn->createGeomMesh(new Sample_TileMesh(), TILE);
#endif
		//set tile settings
		tileSettings =
				dynamic_cast<Sample_TileMesh*>(rn->getSample())->getTileSettings();
		tileSettings.m_buildAll = false;
		tileSettings.m_tileSize = 32;
		tileSettings.m_maxTiles = 128;
		tileSettings.m_maxPolysPerTile = 32768;
		dynamic_cast<Sample_TileMesh*>(rn->getSample())->setTileSettings(
				tileSettings);
	}
		break;
	case OBSTACLE:
	{
		TileSettings tileSettings;
#ifdef DEBUG_DRAW
		rn->createGeomMesh(new Sample_TempObstacles(renderDebug), OBSTACLE);
#else
		rn->createGeomMesh(new Sample_TempObstacles(), OBSTACLE);
#endif
		//set tile settings
		tileSettings =
				dynamic_cast<Sample_TempObstacles*>(rn->getSample())->getTileSettings();
		tileSettings.m_tileSize = 32;
		tileSettings.m_maxTiles = 128;
		tileSettings.m_maxPolysPerTile = 32768;
		dynamic_cast<Sample_TempObstacles*>(rn->getSample())->setTileSettings(
				tileSettings);
	}
		break;
	default:
		break;
	}

	//set sample settings
	SampleSettings settings = rn->getSettings();
	settings.m_agentRadius = agentRadius;
	settings.m_agentHeight = agentHeight;
	settings.m_agentMaxSlope = 60.0;
	settings.m_agentMaxClimb = 2.5;
	settings.m_cellSize = 0.3;
	settings.m_cellHeight = 0.2;
	rn->setSettings(settings);

	//build navigation mesh
	rn->buildNavMesh();

	//set callbacks
	switch (sampleType)
	{
	case TILE:
	{
		//build first tile around agent pos
		float pos[3];
		LVecBase3fToRecast(agentPos, pos);
		dynamic_cast<Sample_TileMesh*>(rn->getSample())->buildTile(pos);
		//set buildTile/removeTile callbacks
		Raycaster::GetSingletonPtr()->setHitCallback(BUILD_TILE_Idx, buildTile,
				reinterpret_cast<void*>(rn), BUILD_TILE_Key, BitMask32::all_on());
		Raycaster::GetSingletonPtr()->setHitCallback(REMOVE_TILE_Idx, removeTile,
				reinterpret_cast<void*>(rn), REMOVE_TILE_Key, BitMask32::all_on());
	}
		break;
	case OBSTACLE:
	{
		//load an obstacle node path
		obstacleNP = window->load_model(window->get_render(), obstacleName);
		obstacleTable.clear();
		//set addObstacle/removeObstacle callbacks
		Raycaster::GetSingletonPtr()->setHitCallback(ADD_OBSTACLE_Idx, addObstacle,
				reinterpret_cast<void*>(rn), ADD_OBSTACLE_Key, BitMask32::all_on());
		Raycaster::GetSingletonPtr()->setHitCallback(REMOVE_OBSTACLE_Idx, removeObstacle,
				reinterpret_cast<void*>(rn), REMOVE_OBSTACLE_Key, BitMask32::all_on());
	}
		break;
	case SOLO:
	default:
		break;
	}

#ifdef DEBUG_DRAW
	rn->getSample()->handleRender();
#endif

	//set ai update task
	AsyncTask* task;
	switch (movType)
	{
#ifndef WITHCHARACTER
	case RECAST:
	case KINEMATIC:
	case RIGID:
		task = new GenericAsyncTask("ai update", &RN::ai_update,
				reinterpret_cast<void*>(rn));
		break;
#else
	case CHARACTER:
		task = new GenericAsyncTask("ai update", &RN::ai_updateCHARACTER,
				reinterpret_cast<void*>(rn));
		break;
#endif
	default:
		break;
	}
	task->set_sort(AI_TASK_SORT);
	panda->get_task_mgr().add(task);

	///Crowd tool
	//set crowd tool
	rn->setCrowdTool();
	//add agent
//	float maxError = rn->getSample()->getConfig().detailSampleMaxError;
	float maxError = agentHeight;
	int agentIdx = rn->addCrowdAgent(movType, character, agentPos,
			agentMaxSpeed, &rn_anim_collection, cs, mBulletWorld.p(), maxError,
			agentRadius, agentHeight);
	//add a crowd raycaster
//	Raycaster::GetSingletonPtr()->setHitCallback(0, allOffButZeroMask,
//			reinterpret_cast<void*>(rn), "shift-mouse1", BitMask32::all_on());
//	Raycaster::GetSingletonPtr()->setHitCallback(0, allOnButZeroMask,
//			reinterpret_cast<void*>(rn), "shift-mouse1", BitMask32::all_on());
	//crowd re-target
	Raycaster::GetSingletonPtr()->setHitCallback(SET_TARGET_Idx, setCrowdTarget,
			reinterpret_cast<void*>(rn), SET_TARGET_Key, BitMask32::all_on());

#ifdef TESTANOMALIES
	AsyncTask::DoneStatus print_data(GenericAsyncTask* task, void* data);
	AgentData *agentData;
	//print_data post ai
	agentData = new AgentData;
	agentData->msg = std::string("POST AI: ");
	agentData->agent = rn->getCrowdAgent(agentIdx);
	agentData->member = AgentData::VEL;
	task = new GenericAsyncTask("POST AI", &print_data,
			reinterpret_cast<void*>(agentData));
	task->set_sort(AI_TASK_SORT + 1);
	panda->get_task_mgr().add(task);
	//print_data pre physics
	agentData = new AgentData;
	agentData->msg = std::string("PRE PHYSICS: ");
	agentData->agent = rn->getCrowdAgent(agentIdx);
	agentData->member = AgentData::POS;
	task = new GenericAsyncTask("PRE PHYSICS", &print_data,
			reinterpret_cast<void*>(agentData));
	task->set_sort(PHYSICS_TASK_SORT - 1);
	panda->get_task_mgr().add(task);
	//print_data post physics
	agentData = new AgentData;
	agentData->msg = std::string("POST PHYSICS: ");
	agentData->agent = rn->getCrowdAgent(agentIdx);
	agentData->member = AgentData::POS;
	task = new GenericAsyncTask("POST PHYSICS", &print_data,
			reinterpret_cast<void*>(agentData));
	task->set_sort(PHYSICS_TASK_SORT + 1);
	panda->get_task_mgr().add(task);
	//
	task = new GenericAsyncTask("DELIMITER", &print_data,
			reinterpret_cast<void*>(NULL));
	task->set_sort(PHYSICS_TASK_SORT + 2);
	panda->get_task_mgr().add(task);
#endif

	// Do the main loop
	panda->main_loop();
	//end
	delete rn;
	end(panda);
	return 0;
}

AsyncTask::DoneStatus update_physics(GenericAsyncTask* task, void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	BulletWorld* mBulletWorld = reinterpret_cast<BulletWorld*>(data);

	float dt = ClockObject::get_global_clock()->get_dt();

	int maxSubSteps;

	// do physics step simulation
	// timeStep < maxSubSteps * fixedTimeStep (=1/60.0=0.016666667) -->
	// supposing a minimum of 6,666666667 fps, we have a maximum
	// timeStep of 0.15 secs so: maxSubSteps <= 60 * 0.15 = 9
	if (dt < 0.016666667)
	{
		maxSubSteps = 1;
	}
	else if (dt < 0.033333333)
	{
		maxSubSteps = 2;
	}
	else if (dt < 0.05)
	{
		maxSubSteps = 3;
	}
	else if (dt < 0.066666668)
	{
		maxSubSteps = 4;
	}
	else if (dt < 0.083333335)
	{
		maxSubSteps = 5;
	}
	else if (dt < 0.100000002)
	{
		maxSubSteps = 6;
	}
	else if (dt < 0.116666669)
	{
		maxSubSteps = 7;
	}
	else if (dt < 0.133333336)
	{
		maxSubSteps = 8;
	}
	else
	{
		maxSubSteps = 9;
	}
	mBulletWorld->do_physics(dt, maxSubSteps);
	//
	return AsyncTask::DS_cont;
}

SMARTPTR(BulletWorld)start(PandaFramework** panda, int argc, char **argv, WindowFramework** window, bool debugPhysics)
{
	// Load your configuration
	load_prc_file_data("", "model-path" + baseDir + "data/models");
	load_prc_file_data("", "model-path" + baseDir + "data/shaders");
	load_prc_file_data("", "model-path" + baseDir + "data/sounds");
	load_prc_file_data("", "model-path" + baseDir + "data/textures");
	load_prc_file_data("", "show-frame-rate-meter #t");
	load_prc_file_data("", "lock-to-one-cpu 0");
	load_prc_file_data("", "support-threads 1");
	load_prc_file_data("", "audio-buffering-seconds 5");
	load_prc_file_data("", "audio-preload-threshold 2000000");
	load_prc_file_data("", "sync-video #t");
	//
	*panda = new PandaFramework();
	(*panda)->open_framework(argc, argv);
	(*panda)->set_window_title("recastnavigation training");
	*window = (*panda)->open_window();
	if (*window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		(*window)->enable_keyboard();// Enable keyboard detection
		(*window)->setup_trackball();// Enable default camera movement
	}

	//set camera pos
	(*window)->get_camera_group().set_pos(60, -60, 50);
	(*window)->get_camera_group().look_at(0, 0, 0);

	//physics
	SMARTPTR(BulletWorld)mBulletWorld = new BulletWorld();
	mBulletWorld->set_gravity(0.0, 0.0, -9.81);
	//physics: advance the simulation state
	AsyncTask* task = new GenericAsyncTask("update physics", &update_physics,
			reinterpret_cast<void*>(mBulletWorld.p()));
	task->set_sort(PHYSICS_TASK_SORT);
	(*panda)->get_task_mgr().add(task);

	if (debugPhysics)
	{
		//physics debug
		NodePath mBulletDebugNodePath = NodePath(new BulletDebugNode("Debug"));
		mBulletDebugNodePath.reparent_to((*window)->get_render());
		SMARTPTR(BulletDebugNode)bulletDebugNode =
		DCAST(BulletDebugNode,mBulletDebugNodePath.node());
		mBulletWorld->set_debug_node(bulletDebugNode);
		bulletDebugNode->show_wireframe(true);
		bulletDebugNode->show_constraints(true);
		bulletDebugNode->show_bounding_boxes(false);
		bulletDebugNode->show_normals(false);
		mBulletDebugNodePath.show();
	}

	return mBulletWorld;
}

void end(PandaFramework* panda)
{
	// close the framework
	panda->close_framework();
	delete panda;
}

NodePath createWorldMesh(SMARTPTR(BulletWorld)mBulletWorld, WindowFramework* window, float scale = 1.0)
{
	//Load world mesh
	NodePath worldMesh = window->load_model(window->get_render(),
			rnDir + meshNameEgg);
	worldMesh.set_pos(0.0, 0.0, 0.0);
	//attach bullet body
	//see: https://www.panda3d.org/forums/viewtopic.php?t=13981
	BulletTriangleMesh* triMesh = new BulletTriangleMesh();
	//add geoms from geomNodes to the mesh
	NodePathCollection geomNodes = worldMesh.find_all_matches("**/+GeomNode");
	for (int i = 0; i < geomNodes.get_num_paths(); ++i)
	{
		SMARTPTR(GeomNode)geomNode = DCAST(GeomNode,
				geomNodes.get_path(i).node());
		CSMARTPTR(TransformState) ts = geomNode->get_transform();
		GeomNode::Geoms geoms = geomNode->get_geoms();
		for (int j = 0; j < geoms.get_num_geoms(); ++j)
		{
			triMesh->add_geom(geoms.get_geom(j), true, ts.p());
		}
	}
	SMARTPTR(BulletShape)collisionShape =
	new BulletTriangleMeshShape(triMesh, false);
	SMARTPTR(BulletRigidBodyNode)mRigidBodyNode =
	new BulletRigidBodyNode((worldMesh.get_name()+"_physics").c_str());
	mRigidBodyNode->set_transform(TransformState::make_scale(scale));
	mRigidBodyNode->add_shape(collisionShape);
	mRigidBodyNode->set_mass(0.0);
	mRigidBodyNode->set_kinematic(false);
	mRigidBodyNode->set_static(true);
	mRigidBodyNode->set_deactivation_enabled(true);
	mRigidBodyNode->set_active(false);
	mBulletWorld->attach(mRigidBodyNode);
	NodePath mRigidBodyNodePath = NodePath(mRigidBodyNode);
	mRigidBodyNodePath.set_collide_mask(BitMask32::all_on());
//	mRigidBodyNodePath.set_collide_mask(allOffButZeroMask);
	//attach to scene
	worldMesh.reparent_to(mRigidBodyNodePath);
	mRigidBodyNodePath.reparent_to(window->get_render());

	return worldMesh;
}

NodePath createAgent(SMARTPTR(BulletWorld)mBulletWorld, WindowFramework* window,
MOVTYPE movType, float& agentRadius, float& agentHeight, BulletConstraint** pcs)
{
	NodePath playerNP;
	//Load the Actor Model
	NodePath Actor = window->load_model(window->get_render(),
			baseDir + actorFile);
	//Load Animations
	std::vector<std::string> animations;
	animations.push_back(baseDir + anim0File);
//	animations.push_back(baseDir + anim1File);
	for (unsigned int i = 0; i < animations.size(); ++i)
	{
		window->load_model(Actor, animations[i]);
	}
	auto_bind(Actor.node(), rn_anim_collection,
			PartGroup::HMF_ok_wrong_root_name|
			PartGroup::HMF_ok_part_extra|
			PartGroup::HMF_ok_anim_extra);
	Actor.set_scale(actorScale);
	LPoint3f min_point, max_point;
	Actor.calc_tight_bounds(min_point, max_point);
	agentRadius = sqrt(pow((max_point.get_x() - min_point.get_x()),2)
			+ pow((max_point.get_y() - min_point.get_y()),2)) / 2.0;
	agentHeight = max_point.get_z() - min_point.get_z();
	//
#ifndef WITHCHARACTER
	switch (movType)
	{
		case RECAST:
		Actor.reparent_to(window->get_render());
		playerNP = Actor;
		break;
		case KINEMATIC:
		//https://groups.google.com/forum/?fromgroups=#!searchin/recastnavigation/physics/recastnavigation/NzK6yGUXqXs/6mT6P-Nu89sJ
		//Add kinematic character
		playerNP = NodePath(new BulletRigidBodyNode("kinematic"));
		DCAST(BulletRigidBodyNode, playerNP.node())->
		add_shape(new BulletCylinderShape(agentRadius, agentHeight, Z_up),
				TransformState::make_pos(LPoint3f(0, 0, agentHeight / 2.0)));
		playerNP.set_collide_mask(allOffButZeroMask);
//		playerNP.set_collide_mask(BitMask32::all_on());
		DCAST(BulletRigidBodyNode, playerNP.node())->set_mass(0.0);
		DCAST(BulletRigidBodyNode, playerNP.node())->set_kinematic(true);
		DCAST(BulletRigidBodyNode, playerNP.node())->set_static(false);
		DCAST(BulletRigidBodyNode, playerNP.node())->set_deactivation_enabled(false);
		DCAST(BulletRigidBodyNode, playerNP.node())->set_active(false);
		mBulletWorld->attach(playerNP.node());
		//attach to scene
		Actor.reparent_to(playerNP);
		playerNP.reparent_to(window->get_render());
		break;
		case RIGID:
		//https://groups.google.com/forum/?fromgroups=#!searchin/recastnavigation/physics/recastnavigation/NzK6yGUXqXs/6mT6P-Nu89sJ
		//Add rigid body
		playerNP = NodePath(new BulletRigidBodyNode("rigid_body"));
		DCAST(BulletRigidBodyNode, playerNP.node())->
		add_shape(new BulletCylinderShape(agentRadius, agentHeight, Z_up),
				TransformState::make_pos(LPoint3f(0, 0, agentHeight / 2.0)));
		playerNP.set_collide_mask(BitMask32::all_on());
		DCAST(BulletRigidBodyNode, playerNP.node())->set_mass(1.0);
		DCAST(BulletRigidBodyNode, playerNP.node())->set_kinematic(false);
		DCAST(BulletRigidBodyNode, playerNP.node())->set_static(false);
		DCAST(BulletRigidBodyNode, playerNP.node())->set_deactivation_enabled(true);
		DCAST(BulletRigidBodyNode, playerNP.node())->set_active(true);
		mBulletWorld->attach(playerNP.node());
		//add a spherical constraint
		*pcs = new BulletSphericalConstraint(DCAST(BulletRigidBodyNode, playerNP.node()), LPoint3f(0, 0, -0.1));
		mBulletWorld->attach(*pcs);
		//attach to scene
		Actor.reparent_to(playerNP);
		playerNP.reparent_to(window->get_render());
		break;
		default:
		break;
	}
#else
	switch (movType)
	{
		case CHARACTER:
		//https://groups.google.com/forum/?fromgroups=#!searchin/recastnavigation/physics/recastnavigation/NzK6yGUXqXs/6mT6P-Nu89sJ
		//Add character controller
		playerNP = NodePath(new BulletCharacterControllerNode(
						new BulletCylinderShape(agentRadius, agentHeight, Z_up), 0.4, "NPCAgent"));
		playerNP.set_collide_mask(BitMask32::all_on());
		DCAST(BulletCharacterControllerNode, playerNP.node())->set_max_slope(1.570796327);
		DCAST(BulletCharacterControllerNode, playerNP.node())->set_max_jump_height(agentHeight);
		mBulletWorld->attach(playerNP.node());
		//attach to scene
		Actor.set_z(-agentHeight/2.0);
		Actor.reparent_to(playerNP);
		playerNP.reparent_to(window->get_render());
		break;
		default:
		break;
	}
#endif
	//
	return playerNP;
}

void setCrowdTarget(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	rn->setCrowdTarget(raycaster->getHitPos());
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
}

void buildTile(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	float m_hitPos[3];
	LVecBase3fToRecast(raycaster->getHitPos(), m_hitPos);
	dynamic_cast<Sample_TileMesh*>(rn->getSample())->buildTile(m_hitPos);
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
#ifdef DEBUG_DRAW
	rn->getSample()->handleRender();
#endif
}

void removeTile(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	float m_hitPos[3];
	LVecBase3fToRecast(raycaster->getHitPos(), m_hitPos);
	dynamic_cast<Sample_TileMesh*>(rn->getSample())->removeTile(m_hitPos);
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
#ifdef DEBUG_DRAW
	rn->getSample()->handleRender();
#endif
}

void addObstacle(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	TempObstacle* tempObstacle = new TempObstacle(obstacleNP, 0.5,
			rn->getBulletWorld());
	tempObstacle->m_nodePath.reparent_to(rn->getRender());
	tempObstacle->m_nodePath.set_pos(raycaster->getHitPos());
	//add to obstacle table
	obstacleTable[tempObstacle->m_nodePath] = tempObstacle;
	//add detour obstacle
	float m_hitPos[3];
	LVecBase3fToRecast(raycaster->getHitPos(), m_hitPos);
	Sample_TempObstacles* sample =
				dynamic_cast<Sample_TempObstacles*>(rn->getSample());
	dtTileCache* tileCache = sample->getTileCache();
	tileCache->addObstacle(m_hitPos, tempObstacle->m_radius,
					tempObstacle->m_heigth, &tempObstacle->m_ref);
	tempObstacle->m_tileCache = tileCache;
	//update tile cache
	tileCache->update(0, sample->getNavMesh());
	//
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
#ifdef DEBUG_DRAW
	rn->getSample()->handleRender();
#endif
}

void removeObstacle(Raycaster* raycaster, void* data)
{
	RN* rn = reinterpret_cast<RN*>(data);
	NodePath hitNP = NodePath(const_cast<PandaNode*>(raycaster->getHitNode()));
	if(hitNP.get_name() == "TempObstacle")
	{
		//delete TempObstacle
		delete obstacleTable[hitNP];
		//remove from obstacle table
		obstacleTable.erase(hitNP);
		Sample_TempObstacles* sample =
					dynamic_cast<Sample_TempObstacles*>(rn->getSample());
		//update tile cache
		sample->getTileCache()->update(0, sample->getNavMesh());

#ifdef DEBUG_DRAW
		rn->getSample()->handleRender();
#endif
	}
	//
	std::cout << "| panda node: " << raycaster->getHitNode() << "| hit pos: "
			<< raycaster->getHitPos() << "| hit normal: "
			<< raycaster->getHitNormal() << "| hit fraction: "
			<< raycaster->getHitFraction() << "| from pos: "
			<< raycaster->getFromPos() << "| to pos: " << raycaster->getToPos()
			<< std::endl;
}

#ifdef TESTANOMALIES
AsyncTask::DoneStatus print_data(GenericAsyncTask* task, void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	AgentData* agentData = (AgentData*)data;
	if (agentData)
	{
		switch (agentData->member)
		{
			case AgentData::POS:
			std::cout << agentData->msg << " POS " << agentData->agent->getPos() << std::endl;
			break;
			case AgentData::VEL:
			std::cout << agentData->msg << " VEL " << agentData->agent->getVel() << std::endl;
			break;
			default:
			break;
		}
	}
	else
	{
		std::cout << std::endl;
	}
	//
	return AsyncTask::DS_cont;
}
#endif
