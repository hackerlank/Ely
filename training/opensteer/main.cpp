// ----------------------------------------------------------------------------
//
//
// OpenSteer -- Steering Behaviors for Autonomous Characters
//
// Copyright (c) 2002-2005, Sony Computer Entertainment America
// Original author: Craig Reynolds <craig_reynolds@playstation.sony.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//
// ------------------------------------------------------------------------
//
//
// Main: top level routine for OpenSteerDemo application
//
//  5-29-02 cwr: created
//
//
// ------------------------------------------------------------------------

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <load_prc_file.h>
#include <textNode.h>

#include "common.h"
//#include "OpenSteer/OpenSteerDemo.h"        // OpenSteerDemo application
//#include "OpenSteer/Draw.h"                 // OpenSteerDemo graphics
#include "DrawMeshDrawer.h"
#include "OneTurning.h"
#include "LowSpeedTurn.h"
#include "Pedestrian.h"

// To include EXIT_SUCCESS
#include <cstdlib>
#include <sstream>

AsyncTask::DoneStatus opensteer_update(GenericAsyncTask* task, void* data);

extern std::string baseDir;

//draw enabling
NodePath drawer3dNP, drawer2dNP;
void toggleDraw(const Event *, void *);
bool gToggleDrawGrid = false;

namespace OpenSteer
{
//global variables initializations
bool enableAnnotation = false;
bool updatePhaseActive = false;
bool drawPhaseActive = true;
bool gDelayedResetPlugInXXX = false;
}

ely::DrawMeshDrawer *gDrawer3d, *gDrawerGrid3d, *gDrawer2d;
OpenSteer::AbstractVehicle* selectedVehicle;

int main(int argc, char *argv[])
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
	trackball->set_pos(0, 50, 0);
	trackball->set_hpr(0, 15, 0);

	///here is room for your own code

	//create global drawers
	drawer3dNP = window->get_render().attach_new_node("Drawer3dNP");
	drawer2dNP = window->get_aspect_2d().attach_new_node("Drawer2dNP");
	gDrawer3d = new ely::DrawMeshDrawer(drawer3dNP,
			window->get_camera_group().get_child(0), 100, 0.04);
	gDrawer3d->depthMask(true);
	gDrawerGrid3d = new ely::DrawMeshDrawer(drawer3dNP,
			window->get_camera_group().get_child(0), 5000, 0.75, true);
	gDrawer3d->depthMask(true);
	gDrawer2d = new ely::DrawMeshDrawer(drawer2dNP,
			window->get_camera_group().get_child(0), 50, 0.04);

	//current plugin: the last specified will be run
	std::string currPlug;
	OpenSteer::AbstractPlugIn* selectedPlugIn = NULL;
	//actor scale
	float actorScale = 1.0;

	//get program options
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "olps:")) != -1)
	{
		switch (c)
		{
		case 'o':
			currPlug = "OneTurning";
			break;
		case 'l':
			currPlug = "LowSpeedTurn";
			break;
		case 'p':
			currPlug = "Pedestrian";
			break;
		case 's':
			//actor scale
		{
			actorScale = atof(optarg);
			if (actorScale <= 0.0)
			{
				actorScale = 1.0;
			}
		}
			break;
		case '?':
			if ((optopt == 's'))
				std::cerr << "Option " << optopt << " requires an argument.\n"
						<< std::endl;
			else if (isprint(optopt))
				std::cerr << "Unknown option " << optopt << std::endl;
			else
				std::cerr << "Unknown option character " << optopt << std::endl;
			return 1;
		default:
			abort();
		}
	}

	//load actor
	NodePath ely = window->load_model(framework.get_models(), "eve.bam");
	ely.set_scale(actorScale);

	//create plugin
	if (currPlug == "LowSpeedTurn")
	{
		//LowSpeedTurn plugin
		selectedPlugIn = new ely::LowSpeedTurnPlugIn;
		selectedPlugIn->open();
		ely::LowSpeedTurnPlugIn* lowSpeedPlugIn =
				dynamic_cast<ely::LowSpeedTurnPlugIn*>(selectedPlugIn);
		for (int i = 0; i < ely::lstCount; i++)
		{
			//view actor
			std::string instNum =
					dynamic_cast<ostringstream&>(ostringstream().operator <<(i)).str();
			NodePath elyInst = window->get_render().attach_new_node(
					"LowSpeedTurn-" + instNum);
			ely.instance_to(elyInst);
			lowSpeedPlugIn->all[i]->setActor(elyInst);
		}
		selectedVehicle = *lowSpeedPlugIn->all.begin();
	}
	else if (currPlug == "Pedestrian")
	{
		//Pedestrian plugin
		selectedPlugIn = new ely::PedestrianPlugIn;
		selectedPlugIn->open();
		ely::PedestrianPlugIn::iterator iter;
		int i;
		ely::PedestrianPlugIn* pedPlugIn =
				dynamic_cast<ely::PedestrianPlugIn*>(selectedPlugIn);
		for (i = 0, iter = pedPlugIn->crowd.begin();
				iter != pedPlugIn->crowd.end(); ++i, ++iter)
		{
			//view actor
			std::string instNum =
					dynamic_cast<ostringstream&>(ostringstream().operator <<(i)).str();
			NodePath elyInst = window->get_render().attach_new_node(
					"Pedestrian-" + instNum);
			ely.instance_to(elyInst);
			dynamic_cast<ely::Pedestrian*>(*iter)->setActor(elyInst);
		}
		//first vehicle is selected by selectedPlugIn->open() too
		selectedVehicle = *pedPlugIn->crowd.begin();
	}
	else
	{
		//OneTurning plugin: default
		selectedPlugIn = new ely::OneTurningPlugIn;
		selectedPlugIn->open();
		//view actor
		NodePath elyInst = window->get_render().attach_new_node("OneTurning");
		ely.instance_to(elyInst);
		dynamic_cast<ely::OneTurningPlugIn*>(selectedPlugIn)->gOneTurning->setActor(
				elyInst);
	}

	// draw the name of the selected PlugIn
	NodePath pluginNameNP(new TextNode("pluginName"));
	DCAST(TextNode, pluginNameNP.node())->set_text(selectedPlugIn->name());
	DCAST(TextNode, pluginNameNP.node())->set_align(TextProperties::A_left);
	pluginNameNP.set_scale(0.05);
	pluginNameNP.set_pos(-1.0, 0.0, 0.95);
	pluginNameNP.reparent_to(drawer2dNP);

	//add opensteer update task
	AsyncTask* task = new GenericAsyncTask("opensteer update",
			&opensteer_update, reinterpret_cast<void*>(selectedPlugIn));
	AsyncTaskManager::get_global_ptr()->add(task);

	//toggle draw grid enabling
	framework.define_key("a", "toggle-annotation", &toggleDraw,
			static_cast<void*>(&OpenSteer::enableAnnotation));
	//toggle draw annotation enabling
	framework.define_key("g", "toggle-draw-grid", &toggleDraw,
			static_cast<void*>(&gToggleDrawGrid));
	//toggle draw enabling
	framework.define_key("d", "toggle-draw-grid", &toggleDraw,
			static_cast<void*>(NULL));

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	//delete selected plugin
	delete selectedPlugIn;
	//delete global drawers
	delete gDrawer3d;
	delete gDrawer2d;
	return (0);
}

std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/ely/");

AsyncTask::DoneStatus opensteer_update(GenericAsyncTask* task, void* data)
{
	OpenSteer::PlugIn* selectedPlugIn =
			reinterpret_cast<OpenSteer::PlugIn*>(data);

	float elapsedTime = ClockObject::get_global_clock()->get_dt();
	double currentTime = ClockObject::get_global_clock()->get_real_time();

	//reset drawers
	gDrawer2d->reset();
	gDrawer3d->reset();
	gDrawerGrid3d->reset();

	// service queued reset request, if any
	if (OpenSteer::gDelayedResetPlugInXXX)
	{
		selectedPlugIn->reset();
		OpenSteer::gDelayedResetPlugInXXX = false;
	}
	// invoke selected PlugIn's Update method
	selectedPlugIn->update(currentTime, elapsedTime);

	// invoke selected PlugIn's Redraw method
	selectedPlugIn->redraw(currentTime, elapsedTime);

	// draw any annotation queued up during selected PlugIn's Update method
	OpenSteer::drawAllDeferredLines();
	OpenSteer::drawAllDeferredCirclesOrDisks();

	return AsyncTask::DS_cont;
}

void toggleDraw(const Event * event, void *data)
{
	std::string eventName = event->get_name();
	if (eventName == "a")
	{
		bool* toggleEnable = reinterpret_cast<bool*>(data);
		*toggleEnable = not *toggleEnable;
		//clear drawers
		gDrawer2d->clear();
		gDrawer3d->clear();
	}
	if (eventName == "g")
	{
		bool* toggleEnable = reinterpret_cast<bool*>(data);
		*toggleEnable = not *toggleEnable;
		//clear drawers
		gDrawerGrid3d->clear();
	}
	if (eventName == "d")
	{
		if (drawer3dNP.is_hidden() and drawer2dNP.is_hidden())
		{
			drawer3dNP.show();
			drawer2dNP.show();
		}
		else
		{
			drawer3dNP.hide();
			drawer2dNP.hide();
		}
	}
}

// ------------------------------------------------------------------------
