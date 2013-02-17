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
 * \file /Ely/training/ai.cpp
 *
 * \date 28/nov/2012 (08:44:38)
 * \author marco
 */

#include <iostream>
#include <string>
#include <vector>
#include <load_prc_file.h>
#include <pandaFramework.h>
#include <lvector3.h>
#include <nodePath.h>
#include <auto_bind.h>
#include <animControlCollection.h>
#include <genericAsyncTask.h>
#include "Utilities/Tools.h"

#include <aiWorld.h>
#include <aiCharacter.h>
#include <aiBehaviors.h>

AsyncTask::DoneStatus AIUpdate(GenericAsyncTask* task, void* data);

int ai_main(int argc, char **argv)
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
	///
	//Seeker
	LVector3f ralphStartPos(-10, 0, 0);
	NodePath seeker = window->load_model(panda.get_models(), "ralph/ralph");
	window->load_model(seeker, "ralph/ralph-run");
	AnimControlCollection seeker_anims;
	auto_bind(seeker.node(), seeker_anims);
	seeker.reparent_to(window->get_render());
	seeker.set_scale(1);
	seeker.set_pos(ralphStartPos);
	//Target
	NodePath target = window->load_model(panda.get_models(), "smiley");
	target.set_color(1, 0, 0);
	target.set_pos(5, 0, 0);
	target.set_scale(0.5);
	target.reparent_to(window->get_render());
	//Creating AI World
	AIWorld AIworld(window->get_render());
	AICharacter AIchar("seeker", seeker, 100, 0.05, 5);
	AIworld.add_ai_char(&AIchar);
	AIBehaviors* AIbehaviors = AIchar.get_ai_behaviors();
	AIbehaviors->seek(target);
	seeker_anims.loop(seeker_anims.get_anim_name(0), true);
	//AI World update
	AsyncTask* task = new GenericAsyncTask("check playing", &AIUpdate,
			reinterpret_cast<void*>(&AIworld));
	panda.get_task_mgr().add(task);

	/// Do the main loop
	panda.main_loop();
	// close the framework
	panda.close_framework();
	return 0;
}

AsyncTask::DoneStatus AIUpdate(GenericAsyncTask* task, void* data)
{
	AIWorld* AIworld = reinterpret_cast<AIWorld*>(data);
	AIworld->update();
	return AsyncTask::DS_again;
}
