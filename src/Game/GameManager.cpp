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
 * \file /Ely/src/Game/GameManager.cpp
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
	// close the framework
	close_framework();
}

void GameManager::setup()
{
	//initialize typed objects
	initTypedObjects();
	// First: setup component template manager
	setupCompTmplMgr();
	// Second: setup object template manager
	setupObjTmplMgr();

	//Create game objects

	//1st object (Panda: "Actor"): initialize component templates
	//a1) get a component template...
	ModelTemplate* modelTmpl =
			DCAST(ModelTemplate,ObjectTemplateManager::GetSingleton().getObjectTemplate(
							ObjectTemplateId("Actor"))->getComponentTemplate(
							ComponentType("Model")));
	//a2) ...reset it to its default state...
	modelTmpl->reset();
	//a3) ...customize it as needed (e.g. in data driven manner)
	modelTmpl->modelFile() = Filename("panda");
	modelTmpl->animFiles().push_back(Filename("panda-walk"));
	//b1) get the next component template... (and so on)
	//1st object (Panda: "Actor"): create the object
	mPandaObj = ObjectTemplateManager::GetSingleton().createObject(
			ObjectTemplateId("Actor"));

	//2nd object (PandaI: "InstancedActor"): initialize component templates
	//a1) get a component template...
	InstanceOfTemplate* instanceOfTmpl =
			DCAST(InstanceOfTemplate,ObjectTemplateManager::GetSingleton().getObjectTemplate(
							ObjectTemplateId("InstancedActor"))->getComponentTemplate(
							ComponentType("InstanceOf")));
	//a2) ...reset it to its default state...
	instanceOfTmpl->reset();
	//a3) ...customize it as needed (e.g. in data driven manner)
	//b1) get the next component template... (and so on)
	//2nd object (PandaI: "InstancedActor"): create the object
	mPandaInstObj = ObjectTemplateManager::GetSingleton().createObject(
			ObjectTemplateId("InstancedActor"));

	// Create the scene graph
	//mPandaObj
	((NodePath) (*mPandaObj)).set_hpr(-90, 0, 0);
	((NodePath) (*mPandaObj)).reparent_to(mWindow->get_render());
	Model* pandaObjModel = DCAST(Model, mPandaObj->getComponent(
					ComponentFamilyType("Graphics")));
	pandaObjModel->animations().loop("panda_soft", false);

	//mPandaInstObj
	((NodePath) (*mPandaInstObj)).set_hpr(-90, 0, 0);
	((NodePath) (*mPandaInstObj)).set_pos(-10, 0, 0);
	((NodePath) (*mPandaInstObj)).reparent_to(mWindow->get_render());
	((NodePath) (*mPandaObj)).instance_to(*mPandaInstObj);

	NodePath trackBallNP = mWindow->get_mouse().find("**/+Trackball");
	PT(Trackball) trackBall = DCAST(Trackball, trackBallNP.node());
	trackBall->set_pos(0, 50, -6);
//	mCamera.set_pos(0, -50, 6);

	// add a 1st task
//	m1stTask = new GameTaskData(this, &GameManager::firstTask);
//	AsyncTask * task = new GenericAsyncTask("1st task", &GameManager::gameTask,
//			reinterpret_cast<void*>(m1stTask.p()));
	m1stTask = new TaskInterface<GameManager>::TaskData(this,
			&GameManager::firstTask);
	AsyncTask * task = new GenericAsyncTask("1st task",
			&TaskInterface<GameManager>::task,
			reinterpret_cast<void*>(m1stTask.p()));
	get_task_mgr().add(task);
	// add a 2nd task
	m2ndTask = new TaskInterface<GameManager>::TaskData(this,
			&GameManager::secondTask);
	task = new GenericAsyncTask("2nd task", &TaskInterface<GameManager>::task,
			reinterpret_cast<void*>(m2ndTask.p()));
	get_task_mgr().add(task);

}

void GameManager::setupCompTmplMgr()
{
	// add all kind of component templates
	//Model template
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new ModelTemplate(this, mWindow));
	//InstanceOf template
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new InstanceOfTemplate());

}

void GameManager::setupObjTmplMgr()
{
	ObjectTemplate* objTmpl;
	// add all kind of object templates

	//1 "Actor" object template
	objTmpl = new ObjectTemplate(ObjectTemplateId("Actor"));
	//add all component templates
	objTmpl->addComponentTemplate(
			ComponentTemplateManager::GetSingleton().getComponentTemplate(
					ComponentType("Model")));
	// add "Actor" object template to manager
	ObjectTemplateManager::GetSingleton().addObjectTemplate(objTmpl);

	//2 "InstancedActor" object template
	objTmpl = new ObjectTemplate(ObjectTemplateId("InstancedActor"));
	//add all component templates
	objTmpl->addComponentTemplate(
			ComponentTemplateManager::GetSingleton().getComponentTemplate(
					ComponentType("InstanceOf")));
	// add "InstancedActor" object template to manager
	ObjectTemplateManager::GetSingleton().addObjectTemplate(objTmpl);

}

//AsyncTask::DoneStatus GameManager::gameTask(GenericAsyncTask* task, void * data)
//{
//	GameTaskData* appData = reinterpret_cast<GameTaskData*>(data);
//	return ((appData->first())->*(appData->second()))(task);
//}

AsyncTask::DoneStatus GameManager::firstTask(GenericAsyncTask* task)
{
	double timeElapsed = mGlobalClock->get_real_time();
	if (timeElapsed < 5.0)
	{
		std::cout << "firstTask " << timeElapsed << std::endl;
		return AsyncTask::DS_cont;
	}
	return AsyncTask::DS_done;
}

AsyncTask::DoneStatus GameManager::secondTask(GenericAsyncTask* task)
{
	double timeElapsed = mGlobalClock->get_real_time();
	if (timeElapsed < 5.0)
	{
		return AsyncTask::DS_cont;
	}
	else if (timeElapsed < 5.0)
	{
		std::cout << "secondTask " << timeElapsed << std::endl;
		return AsyncTask::DS_cont;
	}
	return AsyncTask::DS_done;
}
