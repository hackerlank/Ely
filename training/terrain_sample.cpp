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

//#include <string>
#include <load_prc_file.h>
#include <pandaFramework.h>
#include <texture.h>
#include <texturePool.h>
#include <textureStage.h>
#include <geoMipTerrain.h>
#include <bulletWorld.h>
#include <bulletRigidBodyNode.h>
#include <bulletHeightfieldShape.h>
#include <bulletSphereShape.h>

//Globals
AsyncTask::DoneStatus update_terrain(GenericAsyncTask* task, void* data);
PNMImage heightField;
PT(Texture)textureImage;
const float heightScale = 100.0, widthScale = 3.0;
PT(BulletWorld)bulletWorld;
NodePath debugNP;
void toggleDebug(const Event* e, void* data);

//GeoMipTerrain globals
GeoMipTerrain* terrain;
const int blockSize = 64;
const bool bruteForce = true;
//NodePath mFocalPointNP;
//LPoint3f mTerrainRootNetPos;
//const int mMinimumLevel = 0;
//const float mNearPercent = 0.1, mFarPercent = 0.7;
//const GeoMipTerrain::AutoFlattenMode mFlattenMode = GeoMipTerrain::AFM_off;
//const bool mDoScale = true;

int terrain_sample(int argc, char **argv)
{
	///setup
	// Load your configuration
//	load_prc_file("/etc/Config.prc");
	load_prc_file_data("", "show-frame-rate-meter #t");
	load_prc_file_data("", "win-size 1024 768");

	PandaFramework panda = PandaFramework();
	panda.open_framework(argc, argv);
	panda.set_window_title("terrain error");
	WindowFramework* window = panda.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}
	//setup camera trackball (local coordinate)
	NodePath tballnp = window->get_mouse().find("**/+Trackball");
	PT(Trackball)trackball = DCAST(Trackball, tballnp.node());
	trackball->set_pos(0, 1000, -90);
	trackball->set_hpr(0, 8, 0);

	//update terrain
	AsyncTask* task = new GenericAsyncTask("update terrain", &update_terrain,
	NULL);
	panda.get_task_mgr().add(task);

	//GeoMipTerrain
	heightField = PNMImage(Filename("heightfield.png"));
	terrain = new GeoMipTerrain("terrain");
	terrain->set_heightfield(heightField);
//	//sizing
//	float environmentWidthX = (mHeightField.get_x_size() - 1) * mWidthScale;
//	float environmentWidthY = (mHeightField.get_y_size() - 1) * mWidthScale;
//	float environmentWidth = (environmentWidthX + environmentWidthY) / 2.0;
	//terrain properties
	terrain->set_block_size(blockSize);
//	mTerrain->set_near(mNearPercent * environmentWidth);
//	mTerrain->set_far(mFarPercent * environmentWidth);
//	float terrainLODmin = min<float>(mMinimumLevel, mTerrain->get_max_level());
//	mTerrain->set_min_level(terrainLODmin);
//	mTerrain->set_auto_flatten(mFlattenMode);
	terrain->set_bruteforce(bruteForce);
//	if (mDoScale)
//	{
//		mTerrain->get_root().set_sx(mWidthScale);
//		mTerrain->get_root().set_sy(mWidthScale);
//		mTerrain->get_root().set_sz(mHeightScale);
//	}
	//GeoMipTerrain texturing
	textureImage = TexturePool::load_texture(Filename("terrain.png"));
	PT(TextureStage)textureStage0 =
	new TextureStage("textureStage0");
	terrain->get_root().set_tex_scale(textureStage0, 1.0, 1.0);
	terrain->get_root().set_texture(textureStage0, textureImage, 1);
//	//set the focal point
//	mFocalPointNP = window->get_camera_group();
//	//save the net pos of terrain root
//	mTerrainRootNetPos = mTerrain->get_root().get_net_transform()->get_pos();
	//Generate the terrain
	terrain->generate();
	//adjust position
	terrain->get_root().set_pos_hpr(
			LVector3f(-heightField.get_x_size() / 2.0,
					-heightField.get_y_size() / 2.0, -1 / 2.0),
			LVecBase3::zero());
	//reparent this Terrain node path to render
// 	mTerrain->get_root().reparent_to(window->get_render());

	//Physics
	bulletWorld = new BulletWorld();
	bulletWorld->set_gravity(0.0, 0.0, -9.81);
	PT(BulletDebugNode)debugNode = new BulletDebugNode("Debug");
	debugNode->show_wireframe(true);
	debugNode->show_constraints(true);
	debugNode->show_bounding_boxes(false);
	debugNode->show_normals(false);
	debugNP = window->get_render().attach_new_node(debugNode);
	debugNP.hide();
	bulletWorld->set_debug_node(debugNode);
	EventHandler::get_global_event_handler()->add_hook("d", toggleDebug,
	NULL);

	//BulletHeightfieldShape
	//Create RigidBodyNode
	PT(BulletRigidBodyNode)bulletNode = new BulletRigidBodyNode("terrainBodyNode");
	//create and add a Collision Shape
	PT(BulletShape)collisionShape =
	new BulletHeightfieldShape(Filename("heightfield.png"), 1.0, Z_up);;
	bulletNode->add_shape(collisionShape);
	//set physical parameters
	bulletNode->set_friction(0.8);
	bulletNode->set_restitution(0.1);
	bulletNode->set_mass(0.0);
	bulletNode->set_kinematic(false);
	bulletNode->set_static(true);
	bulletNode->set_deactivation_enabled(true);
	bulletNode->set_active(false);
	//attach to Bullet World
	bulletWorld->attach(bulletNode);
	//create a node path for the rigid body
	NodePath bulletHeightfieldNP = NodePath(bulletNode);
	//set collide mask
	bulletHeightfieldNP.set_collide_mask(BitMask32::all_on());

	//reparent GeoMipTerrain::root to bulletHeightfieldNP
	terrain->get_root().reparent_to(bulletHeightfieldNP);
	//scaling is applied through nodepaths
	bulletHeightfieldNP.set_scale(
			LVecBase3f(widthScale, widthScale, heightScale));

	//reparent bulletHeightfieldNP to render
	bulletHeightfieldNP.reparent_to(window->get_render());

	//Load the Actor model
	NodePath actor = window->load_model(window->get_render(), "panda");
	actor.set_scale(4);
	LPoint3f minP, maxP;
	actor.calc_tight_bounds(minP, maxP);
	LVecBase3 delta = maxP - minP;
	LVector3f actorDims = LVector3f(abs(delta.get_x()), abs(delta.get_y()),
			abs(delta.get_z()));
	float actorRadius = max(max(actorDims.get_x(), actorDims.get_y()),
			actorDims.get_z()) / 2.0;
	//
	PT(BulletRigidBodyNode)actorBody = new BulletRigidBodyNode("actorBody");
	actorBody->add_shape(new BulletSphereShape(actorRadius));
	actorBody->set_mass(10.0);
	actorBody->set_kinematic(false);
	actorBody->set_static(false);
	actorBody->set_deactivation_enabled(true);
	actorBody->set_active(true);
	bulletWorld->attach(actorBody);
	//attach to scene
	NodePath actorBodyNP = NodePath(actorBody);
	actorBodyNP.set_collide_mask(BitMask32::all_on());
	actorBodyNP.set_pos(0, 0, 250.0);
	actor.reparent_to(actorBodyNP);
	actor.set_z(-actorRadius / 2.0);
	actorBodyNP.reparent_to(window->get_render());

	// Do the main loop
	panda.main_loop();
	// close the framework
	panda.close_framework();
	return 0;
}

AsyncTask::DoneStatus update_terrain(GenericAsyncTask* task, void* data)
{
	//terrain
//	//see https://www.panda3d.org/forums/viewtopic.php?t=5384
//	LPoint3f focalPointNetPos = mFocalPointNP.get_net_transform()->get_pos();
//	mTerrain->set_focal_point(focalPointNetPos - mTerrainRootNetPos);
//	//update every frame
//	mTerrain->update();

	//physics
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
	bulletWorld->do_physics(dt, maxSubSteps);
	//
	return AsyncTask::DS_cont;
}

void toggleDebug(const Event* e, void* data)
{
	if (debugNP.is_hidden())
	{
		debugNP.show();
	}
	else
	{
		debugNP.hide();
	}
}
