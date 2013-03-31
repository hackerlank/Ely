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
#include <iostream>
#include <string>
#include <vector>
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
#include <mouseWatcher.h>
#include "Raycaster.h"

#include "RN.h"

//Data constants
std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/");
std::string rnDir(
		"/REPOSITORY/KProjects/WORKSPACE/recastnavigation/RecastDemo/Bin/Meshes/");
//convert obj to egg: obj2egg -TR 90,0,0 nav_test.obj -o nav_test_panda.egg
//triangulate nav_test_panda.egg and...
std::string meshNameEgg("nav_test_panda.egg");
//...(re)convert egg to obj:
//egg2obj -cs y-up -o nav_test_panda.obj nav_test_panda.egg
std::string meshNameObj("nav_test_panda.obj");
LPoint3f agentPos(20.2317238, 9.31323242, -2.36828613);
float agentMaxSpeed = 1.5;

//Declarations
// don't use PT or CPT with AnimControlCollection
AnimControlCollection rn_anim_collection;
AsyncTask::DoneStatus update_physics(GenericAsyncTask* task, void* data);
SMARTPTR(BulletWorld)start(PandaFramework** panda, int argc, char **argv, WindowFramework** window);
void end(PandaFramework* panda);
NodePath createWorldMesh(SMARTPTR(BulletWorld)mBulletWorld, WindowFramework* window);
NodePath createActorModel(SMARTPTR(BulletWorld)mBulletWorld, WindowFramework* window);
//
void setCrowdTarget(Raycaster* raycaster, void* data);

int main(int argc, char **argv)
{
	//start
	PandaFramework* panda;
	WindowFramework* window;
	SMARTPTR(BulletWorld)mBulletWorld = start(&panda, argc, argv, &window);

	//Create world mesh
	NodePath worldMesh = createWorldMesh(mBulletWorld, window);

	//Create actor model
	NodePath actor = createActorModel(mBulletWorld, window);

	///RN common
	RN rn;
	//load mesh
	rn.loadMesh(rnDir, meshNameObj);
	//create solo mesh crowd sample
	rn.createSoloMeshSample();
	//build navigation mesh
	rn.buildNavMesh();
	//set ai update task
	AsyncTask* task = new GenericAsyncTask("ai update", &RN::ai_update,
			reinterpret_cast<void*>(&rn));
//	task->set_delay(5);
	panda->get_task_mgr().add(task);

	///Crowd tool
	//set crowd tool
	rn.setCrowdTool();
	//add agent
	rn.addCrowdAgent(actor, agentPos, agentMaxSpeed, &rn_anim_collection);
	//add a crowd raycaster
	new Raycaster(panda, window, mBulletWorld, "shift-mouse1", "mouse1-up");
	//re-target
	Raycaster::GetSingletonPtr()->setHitCallback(setCrowdTarget,
			reinterpret_cast<void*>(&rn));

	// Do the main loop
	panda->main_loop();
	//end
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

SMARTPTR(BulletWorld)start(PandaFramework** panda, int argc, char **argv, WindowFramework** window)
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
	(*panda)->get_task_mgr().add(task);

	return mBulletWorld;
}

void end(PandaFramework* panda)
{
	// close the framework
	panda->close_framework();
	delete panda;
}

NodePath createWorldMesh(SMARTPTR(BulletWorld)mBulletWorld, WindowFramework* window)
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
	collisionShape->set_local_scale(worldMesh.get_scale());
	SMARTPTR(BulletRigidBodyNode)mRigidBodyNode =
			new BulletRigidBodyNode((worldMesh.get_name()+"_physics").c_str());
	mRigidBodyNode->add_shape(collisionShape);
	mRigidBodyNode->set_mass(0.0);
	mRigidBodyNode->set_kinematic(false);
	mRigidBodyNode->set_static(true);
	mRigidBodyNode->set_deactivation_enabled(true);
	mRigidBodyNode->set_active(false);
	mBulletWorld->attach(mRigidBodyNode);
	NodePath mRigidBodyNodePath = NodePath(mRigidBodyNode);
	mRigidBodyNodePath.set_collide_mask(BitMask32::all_on());
	//attach to scene
	worldMesh.reparent_to(mRigidBodyNodePath);
	mRigidBodyNodePath.reparent_to(window->get_render());

	//physics debug
	NodePath mBulletDebugNodePath = NodePath(new BulletDebugNode("Debug"));
	mBulletDebugNodePath.reparent_to(window->get_render());
	SMARTPTR(BulletDebugNode)bulletDebugNode =
	DCAST(BulletDebugNode,mBulletDebugNodePath.node());
	mBulletWorld->set_debug_node(bulletDebugNode);
	bulletDebugNode->show_wireframe(true);
	bulletDebugNode->show_constraints(true);
	bulletDebugNode->show_bounding_boxes(false);
	bulletDebugNode->show_normals(false);
	mBulletDebugNodePath.hide();

	return worldMesh;
}

NodePath createActorModel(SMARTPTR(BulletWorld)mBulletWorld, WindowFramework* window)
{
	//Load the Actor Model
	NodePath Actor = window->load_model(window->get_render(),
			baseDir + "data/models/eve.bam");
	//Load Animations
	std::vector<std::string> animations;
	animations.push_back(std::string(baseDir + "data/models/eve-run.bam"));
	animations.push_back(std::string(baseDir + "data/models/eve-walk.bam"));
	for (unsigned int i = 0; i < animations.size(); ++i)
	{
		window->load_model(Actor, animations[i]);
	}
	auto_bind(Actor.node(), rn_anim_collection);
	//attach to scene
	Actor.reparent_to(window->get_render());
	Actor.set_scale(0.3);

	return Actor;
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

