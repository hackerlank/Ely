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
#include <pgButton.h>
#include <pgVirtualFrame.h>
#include <mouseButton.h>
#include <texturePool.h>

static void GUI_Callback_Button_Clicked(const Event *ev, void *data);
static std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/ely/");

int pgGUI_main(int argc, char *argv[])
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
	PT(PGVirtualFrame)MyFrame = new PGVirtualFrame("MyFrame");;
	MyFrame->setup(1.0, 1.0);
//	MyFrame->set_clip_frame(-0.1, 0.1, -0.1, 0.1);
	LVecBase4f frame = MyFrame->get_frame();
	frame = MyFrame->get_clip_frame();
	NodePath frameNP = window->get_aspect_2d().attach_new_node(MyFrame);
	frameNP.set_pos(-0.75, 0.0, 0.0);
	NodePath canvasNP = NodePath(MyFrame->get_canvas_node());

	PT(PGButton)MyButton;
	MyButton = new PGButton("MyButton");
	MyButton->setup("", 0.1);
	MyButton->set_frame(-0.5, 0.5, -0.5, 0.5);
//	PT(Texture)ButtonReady=TexturePool::load_texture(
//			baseDir + "data/textures/"+"button_ready.png");
//	PT(Texture)ButtonRollover=TexturePool::load_texture(
//			baseDir + "data/textures/"+"button_depressed.png");
//	PT(Texture)ButtonPressed=TexturePool::load_texture(
//			baseDir + "data/textures/"+"button_rollover.png");
//	PT(Texture)ButtonInactive=TexturePool::load_texture(
//			baseDir + "data/textures/"+"button_inactive.png");
//	// PGFrameStyle is a powerful way to change the appearance of the button:
//	PGFrameStyle MyStyle = MyButton->get_frame_style(0); // frame_style(0): ready state
//	MyStyle.set_type(PGFrameStyle::T_flat);
//	MyStyle.set_texture(ButtonReady);
//	MyButton->set_frame_style(PGButton::S_ready, MyStyle);
//	MyStyle.set_texture(ButtonRollover);
//	MyButton->set_frame_style(PGButton::S_depressed, MyStyle);
//	MyStyle.set_texture(ButtonPressed);
//	MyButton->set_frame_style(PGButton::S_rollover, MyStyle);
//	MyStyle.set_texture(ButtonInactive);
//	MyButton->set_frame_style(PGButton::S_inactive, MyStyle);

	NodePath defbutNP = canvasNP.attach_new_node(MyButton);
	defbutNP.set_pos(0.5, 0.0, 0.5);
	defbutNP.set_scale(0.1);

	// Setup callback function
	framework.define_key(MyButton->get_click_event(MouseButton::one()),
			"button press", &GUI_Callback_Button_Clicked, MyButton);

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}

static void GUI_Callback_Button_Clicked(const Event *ev, void *data)
{
	PGButton* CurrentButton = (PGButton *) data;
	// Your action here
	printf("%s has been pressed.\n", CurrentButton->get_name().c_str());
}
