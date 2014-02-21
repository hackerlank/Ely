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
#include <rocketRegion.h>
#include <Rocket/Core.h>
#include <Rocket/Debugger.h>
#include "Utilities/Tools.h"

static std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/ely/");
void LoadFonts(const char* directory);
void showMenu(const Event* e, void* data);
bool toggleMenu = false;
Rocket::Core::ElementDocument* document;

int rocket_main(int argc, char *argv[])
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

	///here is room for your own code
	EventHandler::get_global_event_handler()->add_hook("m", showMenu,
			reinterpret_cast<void*>(&toggleMenu));

	LoadFonts("/REPOSITORY/KProjects/libRocket/Samples/assets/");
	PT(RocketRegion)r = RocketRegion::make("pandaRocket", window->get_graphics_window());
	r->set_active(true);
	PT(RocketInputHandler)ih = new RocketInputHandler();
	window->get_mouse().attach_new_node(ih);
	r->set_input_handler(ih);
	Rocket::Core::Context *context = r->get_context();
///	Rocket::Debugger::Initialise(context);
///	Rocket::Debugger::SetVisible(true);

	// Load and show the tutorial document.
	document = context->LoadDocument(
			(baseDir + "data/models/rocket.rml").c_str());
	if (document != NULL)
	{
		document->Hide();
		document->RemoveReference();
	}

	//do the main loop, equal to run() in python
	framework.main_loop();

	//close the window framework
	framework.close_framework();
	return (0);
}

/// Loads the default fonts from the given path.
void LoadFonts(const char* directory)
{
	Rocket::Core::String font_names[4];
	font_names[0] = "Delicious-Roman.otf";
	font_names[1] = "Delicious-Italic.otf";
	font_names[2] = "Delicious-Bold.otf";
	font_names[3] = "Delicious-BoldItalic.otf";

	for (unsigned int i = 0;
			i < sizeof(font_names) / sizeof(Rocket::Core::String); i++)
	{
		Rocket::Core::FontDatabase::LoadFontFace(
				Rocket::Core::String(directory) + font_names[i]);
	}
}

void showMenu(const Event* e, void* data)
{
	bool* toggleMenu = reinterpret_cast<bool*>(data);
	//remove generator updating
	if (*toggleMenu)
	{
		document->Hide();
	}
	else
	{
		document->Show();
	}
	*toggleMenu = not *toggleMenu;
}
