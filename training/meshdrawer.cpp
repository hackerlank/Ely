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
 * \file /Ely/training/meshdrawer.cpp
 *
 * \date 15/lug/2013 10:17:13
 * \author marco
 */

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <load_prc_file.h>
#include <meshDrawer.h>

extern std::string baseDir;

AsyncTask::DoneStatus drawTask(GenericAsyncTask* task, void* data);
void stopDrawTask(const Event* e, void* data);
WindowFramework *windowGlobal;
const float SIZE = 0.2;
const int BUDGET = 10000;

int meshdrawer_main(int argc, char *argv[])
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
	trackball->set_pos(0, 30, -5);
	trackball->set_hpr(0, 0, 0);

	//here is room for your own code
	windowGlobal = window;
	MeshDrawer* generator = new MeshDrawer();
	generator->set_budget(BUDGET);
	NodePath generatorNode = generator->get_root();
	generatorNode.reparent_to(window->get_render());
	//
//	generatorNode.set_depth_write(false);
//	generatorNode.set_transparency(TransparencyAttrib::M_alpha);
//	generatorNode.set_two_sided(true);
//	generatorNode.set_texture(TexturePool::load_texture("radarplate.png"));
//	generatorNode.set_bin("fixed",0);
//	generatorNode.set_light_off();
	//add drawTask
	AsyncTask* task = new GenericAsyncTask("check playing", &drawTask,
			reinterpret_cast<void*>(generator));
	AsyncTaskManager::get_global_ptr()->add(task);
	//press "s" to stop generator updating
	//(to be called before closing application)
	EventHandler::get_global_event_handler()->add_hook("s", stopDrawTask,
			reinterpret_cast<void*>(task));

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}

AsyncTask::DoneStatus drawTask(GenericAsyncTask* task, void* data)
{
	MeshDrawer* generator = reinterpret_cast<MeshDrawer*>(data);
	//you'll need access to the window and the generator
	//call this method in your update or use a task.
	generator->begin(windowGlobal->get_camera_group().get_child(0),
			windowGlobal->get_render());
	//
	generator->link_segment(LVector3f(0.0, 0.0, 0.0),
			LVector4f(0.0, 0.0, 1.0, 1.0), SIZE, LVector4f(0.0, 0.0, 1.0, 1.0));
	generator->link_segment(LVector3f(1.0, 0.0, 1.0),
			LVector4f(0.0, 0.0, 1.0, 1.0), SIZE, LVector4f(0.0, 0.0, 1.0, 1.0));
	generator->link_segment_end(LVector4f(0.0, 0.0, 1.0, 1.0),
			LVector4f(0.0, 0.0, 1.0, 1.0));
	//
	generator->billboard(LVector3f(-1.0, 0.5, 1.0),
			LVector4f(0.0, 0.0, 1.0, 1.0), SIZE, LVector4f(0.0, 1.0, 1.0, 1.0));
	//
	generator->cross_segment(LVector3f(-2.0, 1.0, 1.0),
			LVector3f(-2.0, 1.0, 3.0), LVector4f(0.0, 0.0, 1.0, 1.0), SIZE,
			LVector4f(1.0, 1.0, 0.0, 1.0));
	//
	generator->explosion(LVector3f(2.0, -2.0, 5.0),
			LVector4f(0.0, 0.0, 1.0, 1.0), SIZE / 50.0,
			LVector4f(1.0, 0.0, 1.0, 1.0), 20, 500, 0.6);
	//
	generator->particle(LVector3f(0.0, -3.0, 5.0),
			LVector4f(0.0, 0.0, 1.0, 1.0), SIZE, LVector4f(0.3, 0.7, 1.0, 1.0),
			45.0);
	//
	generator->segment(LVector3f(-6.0, 4.0, 3.0), LVector3f(-8.0, 1.0, 7.0),
			LVector4f(0.0, 0.0, 1.0, 1.0), SIZE, LVector4f(0.7, 0.5, 0.3, 1.0));
	//
	generator->stream(LVector3f(2.0, 4.0, 7.0), LVector3f(4.0, 4.0, 8.0),
			LVector4f(0.0, 0.0, 1.0, 1.0), SIZE / 10.0,
			LVector4f(07, 1.0, 0.2, 1.0), 50, 0.6);
	//
	generator->tri(LVector3f(1.0, 7, 5.0), LVector4f(0.0, 1.0, 0.0, 1.0),
			LVector2f(0.0, 0.5), LVector3f(-2.5, 7, 4.0),
			LVector4f(1.0, 0.0, 0.0, 1.0), LVector2f(0.0, 0.0),
			LVector3f(0.0, 7, 3.0), LVector4f(0.0, 0.0, 1.0, 1.0),
			LVector2f(0.0, 1.0));
	//
	generator->uneven_segment(LVector3f(-4.0, -2.0, 5.0),
			LVector3f(-0.0, -2.0, 9.0), LVector4f(0.0, 0.0, 1.0, 1.0), SIZE,
			LVector4f(0.7, 0.5, 0.3, 1.0), SIZE * 4.0, LVector4f(0.3, 0.7, 0.5, 1.0));
	//
	generator->end();

	return AsyncTask::DS_cont;
}

void stopDrawTask(const Event* e, void* data)
{
	AsyncTask* task = reinterpret_cast<AsyncTask*>(data);
	//remove generator updating
	AsyncTaskManager::get_global_ptr()->remove(task);
}
