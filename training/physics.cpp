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
 * \file /Ely/training/physics.cpp
 *
 * \date 30/giu/2012 (09:56:35)
 * \author consultit
 */

#include "Utilities/Tools.h"
#include <iostream>
#include <string>
#include <vector>
#include <load_prc_file.h>
#include <pandaFramework.h>
#include <texture.h>
#include <texturePool.h>
#include <nurbsCurveEvaluator.h>
#include <ropeNode.h>
#include "PhysicsComponents/BulletLocal/bulletWorld.h"
#include "PhysicsComponents/BulletLocal/bulletRigidBodyNode.h"
#include "PhysicsComponents/BulletLocal/bulletSoftBodyNode.h"
#include "PhysicsComponents/BulletLocal/bulletSoftBodyWorldInfo.h"

//Bind the Model and the Animation
// don't use PT or CPT with AnimControlCollection
AnimControlCollection physics_anim_collection;
AsyncTask::DoneStatus update_physics(GenericAsyncTask* task, void* data);

int physics_main(int argc, char **argv)
{
	///setup
	// Load your configuration
	load_prc_file("/REPOSITORY/KProjects/WORKSPACE/Ely/"
			"build-Devel_Debug_Thread/ely/config.prc");
	PandaFramework panda = PandaFramework();
	panda.open_framework(argc, argv);
	panda.set_window_title("animation training");
	WindowFramework* window = panda.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}
	//physics
	SMARTPTR(ely::BulletWorld)physicsWorld = new ely::BulletWorld();
	physicsWorld->set_gravity(0.0, 0.0, -9.81);
	//physics: advance the simulation state
	AsyncTask* task = new GenericAsyncTask("update physics", &update_physics,
			reinterpret_cast<void*>(physicsWorld.p()));
	panda.get_task_mgr().add(task);

	//RopeNode
	SMARTPTR(Texture)textureImage = TexturePool::load_texture(
			Filename(std::string("iron.jpg")));
	SMARTPTR(RopeNode)ropeNode = new RopeNode("rope_node");
	ropeNode->set_render_mode(RopeNode::RM_tube);
	ropeNode->set_uv_mode(RopeNode::UV_parametric);
	ropeNode->set_normal_mode(RopeNode::NM_none);
	ropeNode->set_num_subdiv(4);
	ropeNode->set_num_slices(8);
	ropeNode->set_thickness(0.4);
	NodePath ropeNP = NodePath(ropeNode);
	SMARTPTR(TextureStage)textureStage0 =
	new TextureStage("TextureStage0");
	ropeNP.set_tex_scale(textureStage0, 1.0, 1.0);
	ropeNP.set_texture(textureStage0, textureImage, 1);
	ropeNP.set_scale(1.0);
	ropeNP.set_name("rope");

	//Soft body world information
	ely::BulletSoftBodyWorldInfo info = physicsWorld->get_world_info();
	info.set_air_density(1.2);
	info.set_water_density(0);
	info.set_water_offset(0);
	info.set_water_normal(LVector3f::zero());
	//create soft rope
	SMARTPTR(ely::BulletSoftBodyNode)softRopeNode = ely::BulletSoftBodyNode::make_rope(info, LPoint3f(-2, -1, 8),
			LPoint3f(8, -1, 8), 16, 1);
	//link with NURBS curve
	SMARTPTR(NurbsCurveEvaluator)curve = new NurbsCurveEvaluator();
	curve->reset(16 + 2);
	softRopeNode->link_curve(curve);
	//visualize with RopeNode
	DCAST(RopeNode, ropeNP.node())->set_curve(curve);
	softRopeNode->set_total_mass(1.0);
	NodePath softRopeNP = NodePath(softRopeNode);
	//attach to Bullet World
	physicsWorld->attach(softRopeNode);
	//set collide mask
	softRopeNP.set_collide_mask(BitMask32::all_on());

	//attach to scene
	ropeNP.reparent_to(softRopeNP);
//	softRopeNP.flatten_strong();
	softRopeNP.set_pos(0, 0, 0);
	softRopeNP.reparent_to(window->get_render());
	ropeNP.reparent_to(window->get_render());

	// Do the main loop
	panda.main_loop();
	// close the framework
	panda.close_framework();
	return 0;
}

AsyncTask::DoneStatus update_physics(GenericAsyncTask* task, void* data)
{
	//lock (guard) the mutex
	HOLD_MUTEX(mMutex)

	ely::BulletWorld* physicsWorld = reinterpret_cast<ely::BulletWorld*>(data);

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
	physicsWorld->do_physics(dt, maxSubSteps);
	//
	return AsyncTask::DS_cont;
}

