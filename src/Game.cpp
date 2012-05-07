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
 * \file /Ely/src/Game.cpp
 *
 * \date 07/mag/2012 (18:07:26)
 * \author marco
 */

#include "Game.h"

Game::Game(int argc, char* argv[]) :
		PandaFramework()
{
	// Open the framework
	open_framework(argc, argv);
	// Set a nice title
	set_window_title("Hello World!");
	// Open it!
	mWindow = open_window();
	// Check whether the window is loaded correctly
	if (mWindow != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";

		mWindow->enable_keyboard(); // Enable keyboard detection
		mWindow->setup_trackball(); // Enable default camera movement
		mRender = mWindow->get_render();
		mCamera = mWindow->get_camera_group();
		mGlobalClock = ClockObject::get_global_clock();
	}
	else
	{
		std::cout << "Could not load the window!\n";
	}
}

Game::~Game()
{
	// Close the framework
	close_framework();
}

void Game::setup()
{

	mPanda = mWindow->load_model(get_models(), "panda");
	mPanda.reparent_to(mRender);
	mWindow->load_model(mPanda, "panda-walk");
	auto_bind(mPanda.node(), mPandaAnims);
	mPandaAnims.loop("panda_soft", false);

	CardMaker cm("plane");
	cm.set_frame(-10, 10, -10, 10);
	NodePath plane = mRender.attach_new_node(cm.generate());
	plane.set_p(270);

	NodePath trackBallNP = mWindow->get_mouse().find("**/+Trackball");
	PT(Trackball) trackBall = DCAST(Trackball, trackBallNP.node());
	trackBall->set_pos(0, 40, -6);
	//	self.cam.setPos(0, -40, 6)

	PT(AmbientLight) ambLight = new AmbientLight("ambient");
	ambLight->set_color(LColor(0.2, 0.1, 0.1, 1.0));
	NodePath ambNode = mRender.attach_new_node(ambLight);
	mRender.set_light(ambNode);

	PT(DirectionalLight) dirLight = new DirectionalLight("directional");
	dirLight->set_color(LColor(0.1, 0.4, 0.1, 1.0));
	NodePath dirNode = mRender.attach_new_node(dirLight);
	dirNode.set_hpr(60, 0, 90);
	mRender.set_light(dirNode);

	PT(PointLight) pntLight = new PointLight("point");
	pntLight->set_color(LColor(0.8, 0.8, 0.8, 1.0));
	NodePath pntNode = mRender.attach_new_node(pntLight);
	pntNode.set_pos(0, 0, 15);
	mPanda.set_light(pntNode);

	PT(Spotlight) sptLight = new Spotlight("spot");
	PT(PerspectiveLens) sptLens = new PerspectiveLens();
	sptLight->set_lens(sptLens);
	sptLight->set_color(LColor(1.0, 0.0, 0.0, 1.0));
	sptLight->set_shadow_caster(true);
	NodePath sptNode = mRender.attach_new_node(sptLight);
	sptNode.set_pos(-10, -10, 20);
	sptNode.look_at(mPanda);
	mRender.set_light(sptNode);

	mRender.set_shader_auto();

}

AsyncTask::DoneStatus Game::applicationTask(GenericAsyncTask* task, void * data)
{
	ApplicationTaskData* appData = reinterpret_cast<ApplicationTaskData*>(data);
	return ((appData->first)->*(appData->second))(task);
}

//AsyncTask::DoneStatus Application::update(GenericAsyncTask* task)
//{
//	return AsyncTask::DS_cont;
//}

