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
#include <auto_bind.h>
#include <load_prc_file.h>
#include <textNode.h>

#include "common.h"
//#include "OpenSteer/OpenSteerDemo.h"        // OpenSteerDemo application
//#include "OpenSteer/Draw.h"                 // OpenSteerDemo graphics
#include "DrawMeshDrawer.h"
#include "OneTurning.h"
#include "LowSpeedTurn.h"
#include "Pedestrian.h"
#include "PedestriansWalkingAnEight.h"
#include "CaptureTheFlag.h"
#include "Boids.h"
#include "MultiplePursuit.h"

// To include EXIT_SUCCESS
#include <cstdlib>
#include <sstream>

AsyncTask::DoneStatus opensteer_update(GenericAsyncTask* task, void* data);

NodePath loadActorAndAnims(PandaFramework& framework, WindowFramework *window,
		const std::string& actorName, std::vector<std::string>& animNames,
		AnimControlCollection& animCollection);

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
	std::string currPlugInName;
	OpenSteer::AbstractPlugIn* selectedPlugIn = NULL;
	//actor scale
	float actorScale = 1.0;
	//actor anim rate
	float actorAnimRateFactor = 1.0;

	//get program options
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "olpwcbms:f:")) != -1)
	{
		switch (c)
		{
		case 'o':
			currPlugInName = "OneTurning";
			break;
		case 'l':
			currPlugInName = "LowSpeedTurn";
			break;
		case 'p':
			currPlugInName = "Pedestrian";
			break;
		case 'w':
			currPlugInName = "PedestriansWalkingAnEight";
			break;
		case 'c':
			currPlugInName = "CaptureTheFlag";
			break;
		case 'b':
			currPlugInName = "Boids";
			break;
		case 'm':
			currPlugInName = "MultiplePursuit";
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
		case 'f':
			//actor anim rate
		{
			actorAnimRateFactor = atof(optarg);
			if (actorAnimRateFactor <= 0.0)
			{
				actorAnimRateFactor = 1.0;
			}
		}
			break;
		case '?':
			if ((optopt == 's') or (optopt == 'f'))
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

	//create plugin
	if (currPlugInName == "LowSpeedTurn")
	{
		//LowSpeedTurn plugin
		selectedPlugIn = new ely::LowSpeedTurnPlugIn;
		selectedPlugIn->open();
		ely::LowSpeedTurnPlugIn* currPlugIn =
				dynamic_cast<ely::LowSpeedTurnPlugIn*>(selectedPlugIn);
		//add actor and anims
		for (int i = 0; i < ely::lstCount; i++)
		{
			std::vector<std::string> animNames;
			animNames.push_back("eve-walk.bam");
			NodePath ely = loadActorAndAnims(framework, window, "eve.bam",
					animNames, currPlugIn->all[i]->getAnims());
			ely.set_scale(actorScale);
			ely.reparent_to(window->get_render());
			currPlugIn->all[i]->setActor(ely);
			currPlugIn->all[i]->setAnimRateFactor(actorAnimRateFactor);
		}
		selectedVehicle = *currPlugIn->all.begin();
	}
	else if (currPlugInName == "Pedestrian")
	{
		//Pedestrian plugin
		selectedPlugIn = new ely::PedestrianPlugIn;
		selectedPlugIn->open();
		ely::PedestrianPlugIn::iterator iter;
		int i;
		ely::PedestrianPlugIn* currPlugIn =
				dynamic_cast<ely::PedestrianPlugIn*>(selectedPlugIn);
		//add actor and anims
		for (i = 0, iter = currPlugIn->crowd.begin();
				iter != currPlugIn->crowd.end(); ++i, ++iter)
		{
			ely::Pedestrian* pedestrian = dynamic_cast<ely::Pedestrian*>(*iter);
			std::vector<std::string> animNames;
			animNames.push_back("eve-walk.bam");
			NodePath ely = loadActorAndAnims(framework, window, "eve.bam",
					animNames, pedestrian->getAnims());
			ely.set_scale(actorScale);
			ely.reparent_to(window->get_render());
			pedestrian->setActor(ely);
			pedestrian->setAnimRateFactor(actorAnimRateFactor);
		}
		//first vehicle is selected by selectedPlugIn->open() too
		selectedVehicle = *currPlugIn->crowd.begin();
	}
	else if (currPlugInName == "PedestriansWalkingAnEight")
	{
		//PedestriansWalkingAnEight plugin
		selectedPlugIn = new ely::PedestriansWalkingAnEightPlugIn;
		selectedPlugIn->open();
		ely::PedestriansWalkingAnEightPlugIn::iterator iter;
		int i;
		ely::PedestriansWalkingAnEightPlugIn* currPlugIn =
				dynamic_cast<ely::PedestriansWalkingAnEightPlugIn*>(selectedPlugIn);
		//add actor and anims
		for (i = 0, iter = currPlugIn->crowd.begin();
				iter != currPlugIn->crowd.end(); ++i, ++iter)
		{
			ely::PedestrianWalkingAnEight* pedestrian =
					dynamic_cast<ely::PedestrianWalkingAnEight*>(*iter);
			std::vector<std::string> animNames;
			animNames.push_back("eve-walk.bam");
			NodePath ely = loadActorAndAnims(framework, window, "eve.bam",
					animNames, pedestrian->getAnims());
			ely.set_scale(actorScale);
			ely.reparent_to(window->get_render());
			pedestrian->setActor(ely);
			pedestrian->setAnimRateFactor(actorAnimRateFactor);
		}
		//first vehicle is selected by selectedPlugIn->open() too
		selectedVehicle = *currPlugIn->crowd.begin();
	}
	else if (currPlugInName == "CaptureTheFlag")
	{
		//CaptureTheFlag plugin
		selectedPlugIn = new ely::CtfPlugIn;
		selectedPlugIn->open();
		std::vector<ely::CtfBase*>::iterator iter;
		int i = 0;
		ely::CtfPlugIn* currPlugIn =
				dynamic_cast<ely::CtfPlugIn*>(selectedPlugIn);
		for (i = 0, iter = currPlugIn->all.begin(); iter != currPlugIn->all.end();
				++i, ++iter)
		{
			if (i == 0)
			{
				//seeker
				ely::CtfSeeker* seeker = dynamic_cast<ely::CtfSeeker*>(*iter);
				std::vector<std::string> animNames;
				animNames.push_back("eve-walk.bam");
				NodePath ely = loadActorAndAnims(framework, window, "eve.bam",
						animNames, seeker->getAnims());
				ely.set_scale(actorScale);
				ely.reparent_to(window->get_render());
				seeker->setActor(ely);
				seeker->setAnimRateFactor(actorAnimRateFactor);
			}
			else
			{
				//enemy
				ely::CtfEnemy* enemy = dynamic_cast<ely::CtfEnemy*>(*iter);
				std::vector<std::string> animNames;
				animNames.push_back("panda-walk.bam");
				NodePath panda = loadActorAndAnims(framework, window, "panda.bam",
						animNames, enemy->getAnims());
				panda.set_scale(actorScale * 0.5);
				panda.reparent_to(window->get_render());
				enemy->setActor(panda);
				enemy->setAnimRateFactor(actorAnimRateFactor);
			}
		}
		//seeker is selected by selectedPlugIn->open() too
		selectedVehicle = *currPlugIn->all.begin();
	}
	else if (currPlugInName == "Boids")
	{
		//Boids plugin
		selectedPlugIn = new ely::BoidsPlugIn;
		selectedPlugIn->open();
		ely::BoidsPlugIn::iterator iter;
		int i;
		ely::BoidsPlugIn* currPlugIn =
				dynamic_cast<ely::BoidsPlugIn*>(selectedPlugIn);
		//add actor and anims
		for (i = 0, iter = currPlugIn->flock.begin();
				iter != currPlugIn->flock.end(); ++i, ++iter)
		{
			ely::Boid* boid =
					dynamic_cast<ely::Boid*>(*iter);
			std::vector<std::string> animNames;
			animNames.push_back("eve-walk.bam");
			NodePath ely = loadActorAndAnims(framework, window, "eve.bam",
					animNames, boid->getAnims());
			ely.set_scale(actorScale);
			ely.reparent_to(window->get_render());
			boid->setActor(ely);
			boid->setAnimRateFactor(actorAnimRateFactor);
		}
		currPlugIn->nextBoundaryCondition();
		//first vehicle is selected by selectedPlugIn->open() too
		selectedVehicle = *currPlugIn->flock.begin();
	}
	else if (currPlugInName == "MultiplePursuit")
	{
		//Boids plugin
		selectedPlugIn = new ely::MpPlugIn;
		selectedPlugIn->open();
		ely::MpPlugIn::iterator iter;
		int i;
		ely::MpPlugIn* currPlugIn =
				dynamic_cast<ely::MpPlugIn*>(selectedPlugIn);
		///TODO
		//add actor and anims
//		for (i = 0, iter = currPlugIn->allMP.begin();
//				iter != currPlugIn->allMP.end(); ++i, ++iter)
//		{
//			ely::Boid* boid =
//					dynamic_cast<ely::Boid*>(*iter);
//			std::vector<std::string> animNames;
//			animNames.push_back("eve-walk.bam");
//			NodePath ely = loadActorAndAnims(framework, window, "eve.bam",
//					animNames, boid->getAnims());
//			ely.set_scale(actorScale);
//			ely.reparent_to(window->get_render());
//			boid->setActor(ely);
//			boid->setAnimRateFactor(actorAnimRateFactor);
//		}
//		currPlugIn->nextBoundaryCondition();
//		//first vehicle is selected by selectedPlugIn->open() too
//		selectedVehicle = *currPlugIn->flock.begin();
	}
	else
	{
		//OneTurning plugin: default
		selectedPlugIn = new ely::OneTurningPlugIn;
		selectedPlugIn->open();
		ely::OneTurningPlugIn* currPlugIn =
				dynamic_cast<ely::OneTurningPlugIn*>(selectedPlugIn);
		//add actor and anims
		std::vector<std::string> animNames;
		animNames.push_back("eve-walk.bam");
		NodePath ely = loadActorAndAnims(framework, window, "eve.bam",
				animNames, currPlugIn->gOneTurning->getAnims());
		ely.set_scale(actorScale);
		ely.reparent_to(window->get_render());
		currPlugIn->gOneTurning->setActor(ely);
		currPlugIn->gOneTurning->setAnimRateFactor(actorAnimRateFactor);
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

NodePath loadActorAndAnims(PandaFramework& framework, WindowFramework* window,
		const std::string& actorName, std::vector<std::string>& animNames,
		AnimControlCollection& animCollection)
{
	NodePath actor = window->load_model(framework.get_models(), actorName);
	//load animations
	for (unsigned int i = 0; i < animNames.size(); ++i)
	{
		window->load_model(actor, animNames[i]);
	}
	//bind animations
	auto_bind(actor.node(), animCollection,
			PartGroup::HMF_ok_wrong_root_name | PartGroup::HMF_ok_part_extra
					| PartGroup::HMF_ok_anim_extra);
	return actor;
}

// ------------------------------------------------------------------------
