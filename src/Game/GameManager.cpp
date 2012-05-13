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
 * \file /Ely/src/GameManager.cpp
 *
 * \date 07/mag/2012 (18:07:26)
 * \author marco
 */

#include "Game/GameManager.h"

GameManager::GameManager(int argc, char* argv[]) :
		PandaFramework()
{
	// Open the framework
	open_framework(argc, argv);
	// Set a nice title
	set_window_title("Ely");
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

GameManager::~GameManager()
{
	// Close the framework
	close_framework();
}

void GameManager::setup()
{

	mPanda = mWindow->load_model(get_models(), "panda");
	mPanda.reparent_to(mWindow->get_render());
	mPanda.set_hpr(-90, 0, 0);
	mWindow->load_model(mPanda, "panda-walk");
	auto_bind(mPanda.node(), mPandaAnims);
	mPandaAnims.loop("panda_soft", false);

	Object go(mWindow->load_model(get_models(), "panda"));
	go.getNodePath().set_hpr(0.0, 0.0, 0.0);
	((NodePath) go).set_pos(0.0, 0.0, 0.0);

	NodePath trackBallNP = mWindow->get_mouse().find("**/+Trackball");
	PT(Trackball) trackBall = DCAST(Trackball, trackBallNP.node());
	trackBall->set_pos(0, 50, -6);
//	mCamera.set_pos(0, -50, 6);

	// add a 1st task
	m1stTask = new GameTaskData(this, &GameManager::firstTask);
	AsyncTask * task = new GenericAsyncTask("1st task", &GameManager::gameTask,
			reinterpret_cast<void*>(m1stTask.p()));
	get_task_mgr().add(task);
	// add a 2nd task
	m2ndTask = new GameTaskData(this, &GameManager::secondTask);
	task = new GenericAsyncTask("2nd task", &GameManager::gameTask,
			reinterpret_cast<void*>(m2ndTask.p()));
	get_task_mgr().add(task);

}

AsyncTask::DoneStatus GameManager::gameTask(GenericAsyncTask* task, void * data)
{
	GameTaskData* appData = reinterpret_cast<GameTaskData*>(data);
	return ((appData->first())->*(appData->second()))(task);
}

AsyncTask::DoneStatus GameManager::firstTask(GenericAsyncTask* task)
{
	double timeElapsed = mGlobalClock->get_real_time();
	if (timeElapsed < 10.0)
	{
		std::cout << "firstTask " << timeElapsed << std::endl;
		return AsyncTask::DS_cont;
	}
	return AsyncTask::DS_done;
}

AsyncTask::DoneStatus GameManager::secondTask(GenericAsyncTask* task)
{
	double timeElapsed = mGlobalClock->get_real_time();
	if (timeElapsed < 10.0)
	{
		return AsyncTask::DS_cont;
	}
	else if (timeElapsed < 20.0)
	{
		std::cout << "secondTask " << timeElapsed << std::endl;
		return AsyncTask::DS_cont;
	}
	return AsyncTask::DS_done;
}
