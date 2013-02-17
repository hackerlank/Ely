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
 * \author marco
 */

#include <iostream>
#include <string>
#include <vector>
#include <load_prc_file.h>
#include <auto_bind.h>
#include <partBundleHandle.h>
#include <character.h>
#include <animControlCollection.h>
#include <pandaFramework.h>

#include <panda3d/bulletWorld.h>

//Bind the Model and the Animation
// don't use PT or CPT with AnimControlCollection
AnimControlCollection anim_collection;
AsyncTask::DoneStatus check_playing(GenericAsyncTask* task, void* data);
AsyncTask::DoneStatus update_physics(GenericAsyncTask* task, void* data);

int physics_main(int argc, char **argv)
{
	///setup
	// Load your configuration
	load_prc_file("config.prc");
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
	SMARTPTR(BulletWorld) physicsWorld = new BulletWorld();
	//physics: advance the simulation state
//	AsyncTask* task = new GenericAsyncTask("update physics", &update_physics,
//			reinterpret_cast<void*>(&actualAnim));
//	task->set_delay(3);
//	panda.get_task_mgr().add(task);

	//Load the Actor Model
	NodePath Actor = window->load_model(window->get_render(),
			"bvw-f2004--airbladepilot/pilot-model");
	SMARTPTR(Character) character =
			DCAST(Character, Actor.find("**/+Character").node());
	SMARTPTR(PartBundle) pbundle = character->get_bundle(0);
	//Load Animations
	std::vector<std::string> animations;
	animations.push_back(std::string("pilot-chargeshoot"));
	animations.push_back(std::string("pilot-discloop"));
	for (unsigned int i = 0; i < animations.size(); ++i)
	{
		window->load_model(Actor, "bvw-f2004--airbladepilot/" + animations[i]);
	}
	auto_bind(Actor.node(), anim_collection);
	pbundle->set_anim_blend_flag(true);
	pbundle->set_control_effect(anim_collection.get_anim(0), 0.5);
	pbundle->set_control_effect(anim_collection.get_anim(1), 0.5);
	int actualAnim = 0;
	//switch among animations
	AsyncTask* task = new GenericAsyncTask("check playing", &check_playing,
			reinterpret_cast<void*>(&actualAnim));
	task->set_delay(3);
	panda.get_task_mgr().add(task);
	//attach to scene
	Actor.reparent_to(window->get_render());
	Actor.set_pos(0.0, 100.0, -30.0);

	// Do the main loop
	panda.main_loop();
	// close the framework
	panda.close_framework();
	return 0;
}

AsyncTask::DoneStatus check_playing(GenericAsyncTask* task, void* data)
{
	//Control the Animations
	double time = ClockObject::get_global_clock()->get_real_time();
	int *actualAnim = reinterpret_cast<int*>(data);
	int num = *actualAnim % 3;
	if (num == 0)
	{
		std::cout << time << " - Blending" << std::endl;
		if (not anim_collection.get_anim(0)->is_playing())
		{
			anim_collection.get_anim(0)->play();
		}
		if (not anim_collection.get_anim(1)->is_playing())
		{
			anim_collection.get_anim(1)->play();
		}
	}
	else if (num == 1)
	{
		std::cout << time << " - Playing: " << anim_collection.get_anim_name(0)
				<< std::endl;
		if (not anim_collection.get_anim(0)->is_playing())
		{
			anim_collection.get_anim(0)->play();
		}
		if (anim_collection.get_anim(1)->is_playing())
		{
			anim_collection.get_anim(1)->stop();
		}
	}
	else
	{
		std::cout << time << " - Playing: " << anim_collection.get_anim_name(1)
				<< std::endl;
		anim_collection.get_anim(1)->play();
		if (anim_collection.get_anim(0)->is_playing())
		{
			anim_collection.get_anim(0)->stop();
		}
		if (not anim_collection.get_anim(1)->is_playing())
		{
			anim_collection.get_anim(1)->play();
		}
	}
	*actualAnim += 1;
	return AsyncTask::DS_again;
}



