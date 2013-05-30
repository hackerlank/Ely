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
 * \file /Ely/training/recastnavigation/recastnavigation_data.cpp
 *
 * \date 24/mag/2013 (13:24:33)
 * \author consultit
 */

#include "recastnavigation_data.h"

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
//LPoint3f agentPos(0.723763, 17.166, 9.99818);
///nav_test
std::string meshNameEgg("nav_test_panda.egg");
std::string meshNameObj("nav_test_panda.obj");
LPoint3f agentPos(4.19123, 9.90642, 8.3);
///Mesh scale
float meshScale = 2.0;

///eve character
std::string actorFile("data/models/eve.bam");
std::string anim0File("data/models/eve-walk.bam");
std::string anim1File("data/models/eve-run.bam");
const float characterMaxSpeed = 1.5;
const float rateFactor = 1.25;
const float characterScale = 0.4;
///guy character
//std::string actorFile("data/models/guy.bam");
//std::string anim0File("data/models/guy-walk.bam");
//const float characterMaxSpeed = 2.0;
//const float rateFactor = 2.00;
//const float characterScale = 0.1;

const int AI_TASK_SORT = 10;
const int PHYSICS_TASK_SORT = 20;

BitMask32 allOnButZeroMask, allOffButZeroMask;

//Declarations
// don't use PT or CPT with AnimControlCollection
AnimControlCollection rn_anim_collection;

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

NodePath createCharacter(SMARTPTR(BulletWorld)mBulletWorld, WindowFramework* window,
MOVTYPE movType, float& characterRadius, float& characterHeight, BulletConstraint** pcs)
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
	Actor.set_scale(characterScale);
	LPoint3f min_point, max_point;
	Actor.calc_tight_bounds(min_point, max_point);
	characterRadius = sqrt(pow((max_point.get_x() - min_point.get_x()),2)
			+ pow((max_point.get_y() - min_point.get_y()),2)) / 2.0;
	characterHeight = max_point.get_z() - min_point.get_z();
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
		add_shape(new BulletCylinderShape(characterRadius, characterHeight, Z_up),
				TransformState::make_pos(LPoint3f(0, 0, characterHeight / 2.0)));
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
		add_shape(new BulletCylinderShape(characterRadius, characterHeight, Z_up),
				TransformState::make_pos(LPoint3f(0, 0, characterHeight / 2.0)));
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
						new BulletCylinderShape(characterRadius, characterHeight, Z_up), 0.4, "NPCAgent"));
		playerNP.set_collide_mask(BitMask32::all_on());
		DCAST(BulletCharacterControllerNode, playerNP.node())->set_max_slope(1.570796327);
		DCAST(BulletCharacterControllerNode, playerNP.node())->set_max_jump_height(characterHeight);
		mBulletWorld->attach(playerNP.node());
		//attach to scene
		Actor.set_z(-characterHeight/2.0);
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



