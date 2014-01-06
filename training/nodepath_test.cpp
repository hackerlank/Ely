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
 * \file /Ely/training/nodepath_test.cpp
 *
 * \date 06/gen/2014 (11:42:30)
 * \author consultit
 */

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

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <load_prc_file.h>

extern std::string baseDir;

int nodepath_test(int argc, char *argv[])
{
	// Load your configuration
	load_prc_file_data("", "model-path " + baseDir + "data/models");
	load_prc_file_data("", "model-path " + baseDir + "data/shaders");
	load_prc_file_data("", "model-path " + baseDir + "data/sounds");
	load_prc_file_data("", "model-path " + baseDir + "data/textures");
	load_prc_file_data("", "show-frame-rate-meter #t");
	load_prc_file_data("", "lock-to-one-cpu 0");
	load_prc_file_data("", "support-threads 1");
	load_prc_file_data("", "audio-buffering-seconds 5");
	load_prc_file_data("", "audio-preload-threshold 2000000");
	load_prc_file_data("", "sync-video #t");
	//open a new window framework
	PandaFramework framework;
	framework.open_framework(argc, argv);
	//set the window title to My Panda3D Window
	framework.set_window_title("My Panda3D Window");
	//open the window
	WindowFramework *window = framework.open_window();
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
	trackball->set_pos(0, 200, 0);
	trackball->set_hpr(0, 15, 0);

	//here is room for your own code
	NodePath np1 = window->load_model(framework.get_models(), "smiley");
	np1.set_pos(-40, 40, 0);
	np1.set_color(LColorf(1,0,0,1), 10);
	std::cout << "\nnp1: " << np1.node() << "|" << *(np1.node()) << "|"
			<< *(np1.node()->get_transform())
			<< "\nnp1.child: " << np1.get_child(0).node()
			<< "|" << *(np1.get_child(0).node()) << "|"
			<< *(np1.get_child(0).node()->get_transform()) << std::endl;
	//
	NodePath np2("np2");
	np1.instance_to(np2);
	np2.set_pos(40,0,0);
	np2.set_color(LColorf(0,1,0,1), 20);
	std::cout << "\nnp2: " << np2.node() << "|" << *(np2.node()) << "|"
			<< *(np2.node()->get_transform())
			<< "\nnp2.child: " << np2.get_child(0).node()
			<< "|" << *(np2.get_child(0).node()) << "|"
			<< *(np2.get_child(0).node()->get_transform())
			<< "\nnp2.child.child: " << np2.get_child(0).get_child(0).node()
			<< "|" << *(np2.get_child(0).get_child(0).node()) << "|"
			<< *(np2.get_child(0).get_child(0).node()->get_transform())<< std::endl;
	//
	np1.reparent_to(window->get_render());
	np2.reparent_to(window->get_render());

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}

