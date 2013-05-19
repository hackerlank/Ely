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
 * \file /Ely/training/manual_example.cpp
 *
 * \date 10/mar/2013 (15:39:11)
 * \author consultit
 */

#include "pandaFramework.h"
#include "pandaSystem.h"
#include "geoMipTerrain.h"
#include "texturePool.h"
#include "load_prc_file.h"
#include "bulletWorld.h"
#include "bulletHeightfieldShape.h"
#include "bulletSphereShape.h"

AsyncTask::DoneStatus updateTerrain(GenericAsyncTask* task, void* data);
AsyncTask::DoneStatus updatePhysics(GenericAsyncTask* task, void* data);
PandaFramework framework;
WindowFramework *window;
LVector3f terrainRootNetPos;
NodePath focalPointNP;
PT(BulletWorld) physicsWorld;

int test_wo_local_scale(int argc, char *argv[])
{
	// Load your configuration
	load_prc_file_data("", "show-frame-rate-meter #t");
	//open a new window framework
	framework.open_framework(argc, argv);
	//set the window title to My Panda3D Window
	framework.set_window_title("My Panda3D Window");
	//open the window
	window = framework.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}

	//here is room for your own code

	//create the current terrain
	//terrain definition
	GeoMipTerrain* terrain = new GeoMipTerrain("terrain");
	//set height field
	PNMImage heightField = PNMImage(
				Filename("/REPOSITORY/KProjects/WORKSPACE/Ely/data/textures/heightfield.png"));
	terrain->set_heightfield(heightField);
	//sizing: scale = 5x5x150
	float environmentWidthX = (heightField.get_x_size() - 1) * 5;
	float environmentWidthY = (heightField.get_y_size() - 1) * 5;
	float environmentWidth = (environmentWidthX + environmentWidthY) / 2.0;
	//set terrain properties
	terrain->set_block_size(64);
	terrain->set_near(0.1 * environmentWidth);
	terrain->set_far(0.7 * environmentWidth);
	//other properties
	float terrainLODmin = min<float>(0, terrain->get_max_level());
	terrain->set_min_level(terrainLODmin);
	terrain->set_auto_flatten(GeoMipTerrain::AFM_off);
	terrain->set_bruteforce(true);
//	terrain->get_root().set_sx(5);
//	terrain->get_root().set_sy(5);
//	terrain->get_root().set_sz(150);
	//terrain texturing
	PT(Texture) textureImage = TexturePool::load_texture(
				Filename("/REPOSITORY/KProjects/WORKSPACE/Ely/data/textures/terrain.png"));
	terrain->get_root().set_tex_scale(TextureStage::get_default(),
			1.0, 1.0);
	terrain->get_root().set_texture(TextureStage::get_default(),
				textureImage, 1);
	//Generate the terrain
	terrain->generate();
	//for focal point
	terrainRootNetPos = terrain->get_root().get_net_transform()->get_pos();
	focalPointNP = window->get_camera_group();
	// Add a task to keep updating the terrain
	AsyncTaskManager::get_global_ptr()->add(new GenericAsyncTask("update terrain", &updateTerrain, (void*) terrain));
	//put into scene
//	terrain->get_root().set_render_mode_wireframe();
//	terrain->get_root().hide();

	//physics
	physicsWorld = new BulletWorld();
	physicsWorld->set_gravity(0, 0, -9.8);
	AsyncTaskManager::get_global_ptr()->add(new GenericAsyncTask("update physics", &updatePhysics, (void*) NULL));
	//setup terrain body
	PT(BulletRigidBodyNode)terrainBody = new BulletRigidBodyNode("terrainBody");
	terrainBody->add_shape(new BulletHeightfieldShape(heightField, 150, Z_up));
	terrainBody->set_mass(0.0);
	terrainBody->set_kinematic(false);
	terrainBody->set_static(true);
	terrainBody->set_deactivation_enabled(true);
	terrainBody->set_active(false);
	physicsWorld->attach(terrainBody);
	//attach to scene and scale
	NodePath terrainBodyNP = NodePath(terrainBody);
	terrainBodyNP.set_collide_mask(BitMask32::all_on());
	terrainBodyNP.set_sx(5);
	terrainBodyNP.set_sy(5);
	terrainBodyNP.set_sz(150);
	terrainBodyNP.reparent_to(window->get_render());
	terrain->get_root().reparent_to(terrainBodyNP);
//	terrain->get_root().set_pos_hpr(LVector3f(-heightField.get_x_size()/2.0,-heightField.get_y_size()/2.0,-1/2.0), LVecBase3::zero());

	//Load the Actor Model
	NodePath actor = window->load_model(window->get_render(), "panda");
	PT(BulletRigidBodyNode)actorBody = new BulletRigidBodyNode("actorBody");
	actorBody->add_shape(new BulletSphereShape(1.0));
	actorBody->set_mass(10.0);
	actorBody->set_kinematic(false);
	actorBody->set_static(false);
	actorBody->set_deactivation_enabled(true);
	actorBody->set_active(true);
	physicsWorld->attach(actorBody);
	//attach to scene and scale
	NodePath actorBodyNP = NodePath(actorBody);
	actorBodyNP.set_collide_mask(BitMask32::all_on());
	actorBodyNP.set_scale(2.0);
	actorBodyNP.set_pos(500,500,300.0);
	actorBodyNP.reparent_to(window->get_render());
	actor.reparent_to(actorBodyNP);

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}

AsyncTask::DoneStatus updateTerrain(GenericAsyncTask* task, void* data)
{
	GeoMipTerrain* mTerrain = (GeoMipTerrain*)data;
	//set focal point
	///see https://www.panda3d.org/forums/viewtopic.php?t=5384
	//set the focal point
	LPoint3 focalPointNetPos = focalPointNP.get_pos();
	mTerrain->set_focal_point(focalPointNetPos - terrainRootNetPos);
	//update every frame
	mTerrain->update();
	return AsyncTask::DS_cont;
}

AsyncTask::DoneStatus updatePhysics(GenericAsyncTask* task, void* data) {
    // Get dt (from Python example) and apply to do_physics(float, int, int);
    ClockObject *co = ClockObject::get_global_clock();
    physicsWorld->do_physics(co->get_dt(), 10, 1.0 / 180.0);

    return AsyncTask::DS_cont;
}
