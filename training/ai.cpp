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
 * \date 28/nov/2012 08:44:38
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
#include "Utilities/Tools.h"

#include <aiWorld.h>

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
	///
	// Seeker
	LVector3f ralphStartPos(-10, 0, 0);
	NodePath seeker = window->load_model(panda.get_models(), "models/ralph");
	window->load_model(seeker, "models/ralph");
	AnimControlCollection anim_collection;
	auto_bind(seeker.node(), anim_collection);
	seeker.reparent_to(window->get_render());
	seeker.set_scale(0.5);
	seeker.set_pos(ralphStartPos);
	// Target
	NodePath target = window->load_model(panda.get_models(), "models/arrow");
	target.set_color(1, 0, 0);
	target.set_pos(5, 0, 0);
	target.set_scale(1);
	target.reparent_to(window->get_render());

	/// Do the main loop
	panda.main_loop();
	// close the framework
	panda.close_framework();
	return 0;
}

