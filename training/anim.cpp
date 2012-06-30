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
 * \file /Ely/training/anim.cpp
 *
 * \date 30/giu/2012 (09:45:19)
 * \author marco
 */

#include <iostream>
#include <string>
#include <vector>
#include <load_prc_file.h>
#include <auto_bind.h>
#include <animControlCollection.h>
#include <pandaFramework.h>

//Bind the Model and the Animation
// don't use PT or CPT with AnimControlCollection
AnimControlCollection anim_collection;
AsyncTask::DoneStatus check_playing(GenericAsyncTask* task, void* data);

int main(int argc, char **argv)
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
	//Load the Actor Model
	NodePath Actor = window->load_model(window->get_render(),
			"bvw-f2004--airbladepilot/pilot-model");
	//Load Animations
	std::vector<std::string> animations;
//	animations.push_back(std::string("pilot-charge"));
//	animations.push_back(std::string("pilot-chargeloop"));
	animations.push_back(std::string("pilot-chargeshoot"));
	animations.push_back(std::string("pilot-chargewindup"));
	animations.push_back(std::string("pilot-crash"));
	animations.push_back(std::string("pilot-discloop"));
	animations.push_back(std::string("pilot-discwinddown"));
	animations.push_back(std::string("pilot-discwindup"));
//	animations.push_back(std::string("pilot-fire"));
	animations.push_back(std::string("pilot-firewinddown"));
	animations.push_back(std::string("pilot-firewindup"));
	animations.push_back(std::string("pilot-idle"));
	animations.push_back(std::string("pilot-newdeath"));
	animations.push_back(std::string("pilot-newidle"));
//	animations.push_back(std::string("pilot-pain"));
	for (unsigned int i = 0; i < animations.size(); ++i)
	{
		window->load_model(Actor, "bvw-f2004--airbladepilot/" + animations[i]);
	}
	auto_bind(Actor.node(), anim_collection);
	int actualAnim = 0;
	//switch among animations
	AsyncTask* task = new GenericAsyncTask("check playing", &check_playing,
			reinterpret_cast<void*>(&actualAnim));
	panda.get_task_mgr().add(task);
	// the name of an animation is preceded in the .egg file with <Bundle>:
	// loop a specific animation
//	anim_collection.loop(anim_collection.get_anim_name(1), true);
	// loop all animations
//	anim_collection.loop_all(true);
	// play an animation once:
//	anim_collection.play("panda_soft");
	// pose
//	anim_collection.pose("panda_soft", 5);

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
	int *actualAnim = reinterpret_cast<int*>(data);
	if (*actualAnim >= anim_collection.get_num_anims())
	{
		return AsyncTask::DS_done;
	}
	if (not anim_collection.is_playing())
	{
		std::cout << anim_collection.get_anim_name(*actualAnim) << std::endl;
		anim_collection.play(anim_collection.get_anim_name(*actualAnim));
		*actualAnim += 1;
	}
	return AsyncTask::DS_cont;
}
