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
#include "Utilities/ComponentSuite.h"

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
	//setup component template manager
	setupCompTmplMgr();

	//setup game world
	setupGameWorld();

	//SETUP object and game worlds
	//setup object template manager

	ComponentTemplate::ParameterTable parameterTable;
	ComponentTemplate* componentTmpl;
	//Create game objects (from a object.xml)

	//1st object (Panda: "Actor"): initialize component templates
	//a11) get first component template...
	componentTmpl = ObjectTemplateManager::GetSingleton().getObjectTemplate(
			ObjectType("Actor"))->getComponentTemplate(ComponentType("Model"));
	//a12) ...resetParameters it to its default state...
	componentTmpl->resetParameters();
	//a13) ...customize it as needed (e.g. in data driven manner)
	parameterTable.insert(
			ComponentTemplate::ParameterTable::value_type(
					std::string("model_file"), std::string("panda")));
	parameterTable.insert(
			ComponentTemplate::ParameterTable::value_type(
					std::string("anim_file"), std::string("panda-walk")));
	componentTmpl->setParameters(parameterTable);
	parameterTable.clear();
	//a21) get next component template...
	componentTmpl = ObjectTemplateManager::GetSingleton().getObjectTemplate(
			ObjectType("Actor"))->getComponentTemplate(
			ComponentType("ControlByEvent"));
	//a22) ...resetParameters it to its default state...
	componentTmpl->resetParameters();
	//a23) ...customize it as needed (e.g. in data driven manner)
	parameterTable.insert(
			ComponentTemplate::ParameterTable::value_type(std::string("speed"),
					std::string("200.0")));
	parameterTable.insert(
			ComponentTemplate::ParameterTable::value_type(
					std::string("fast_factor"), std::string("10.0")));
	componentTmpl->setParameters(parameterTable);
	parameterTable.clear();
	//a31) get the next component template... (and so on)
	//1st object (Panda: "Actor"): create the object
	mPandaObj = ObjectTemplateManager::GetSingleton().createObject(
			ObjectType("Actor"));

	//2nd object (PandaI: "InstancedActor"): initialize component templates
	//a1) get a component template...
	componentTmpl = ObjectTemplateManager::GetSingleton().getObjectTemplate(
			ObjectType("InstancedActor"))->getComponentTemplate(
			ComponentType("InstanceOf"));
	//a2) ...resetParameters it to its default state...
	componentTmpl->resetParameters();
	//a3) ...customize it as needed (e.g. in data driven manner)
	componentTmpl->setParameters(parameterTable);
	parameterTable.clear();
	//b1) get the next component template... (and so on)
	//2nd object (PandaI: "InstancedActor"): create the object
	mPandaInstObj = ObjectTemplateManager::GetSingleton().createObject(
			ObjectType("InstancedActor"));

	// Create the scene graph (from a scene.xml)
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
	m1stTask = new TaskInterface<GameManager>::TaskData(this,
			&GameManager::firstTask);
	AsyncTask* task = new GenericAsyncTask("1st task",
			&TaskInterface<GameManager>::taskFunction,
			reinterpret_cast<void*>(m1stTask.p()));
	get_task_mgr().add(task);
	// add a 2nd task
	m2ndTask = new TaskInterface<GameManager>::TaskData(this,
			&GameManager::secondTask);
	task = new GenericAsyncTask("2nd task",
			&TaskInterface<GameManager>::taskFunction,
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
	//ControlByEvent template
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new ControlByEventTemplate(this));

}

static bool checkTag(tinyxml2::XMLElement *tag, const char* tagStr)
{
	if (not tag)
	{
		fprintf(stderr, "<%s> tag not found!\n", tagStr);
		return false;
	}
	return true;
}

void GameManager::setupGameWorld()
{
	//read the game configuration file
	std::string gameXml = "game.xml";
	tinyxml2::XMLDocument gameDoc;
	//load file
	std::cout << "Loading '" << gameXml << "'..." << std::endl;
	if (tinyxml2::XML_NO_ERROR != gameDoc.LoadFile(gameXml.c_str()))
	{
		fprintf(stderr, "Error detected on '%s':\n", gameXml.c_str());
		gameDoc.PrintError();
		fprintf(stderr, "%s\n%s\n", gameDoc.GetErrorStr1(),
				gameDoc.GetErrorStr2());
		throw GameException(
				"GameManager::setupGameWorld: Failed to load/parse " + gameXml);
	}
	tinyxml2::XMLElement *game, *objectTmplSet, *scene;
	//check <Game> tag
	std::cout << "Checking <Game> tag ..." << std::endl;
	game = gameDoc.FirstChildElement("Game");
	if (not checkTag(game, "Game"))
	{
		throw GameException(
				"GameManager::setupGameWorld: No <Game> in " + gameXml);
	}
	//check <Game>--<ObjectTmplSet> tag
	std::cout << "Checking <ObjectTmplSet> tag ..." << std::endl;
	objectTmplSet = game->FirstChildElement("ObjectTmplSet");
	if (not checkTag(objectTmplSet, "ObjectTmplSet"))
	{
		throw GameException(
				"GameManager::setupGameWorld: No <ObjectTmplSet> in "
						+ gameXml);
	}
	//cycle through the ObjectTmpl(s)' definitions and
	// add all kind of object templates
	tinyxml2::XMLElement *objectTmpl, *componentTmpl;
	for (objectTmpl = objectTmplSet->FirstChildElement("ObjectTmpl");
			objectTmpl != NULL;
			objectTmpl = objectTmpl->NextSiblingElement("ObjectTmpl"))
	{
		const char *type = objectTmpl->Attribute("type", NULL);
		if (not type)
		{
			continue;
		}
		std::cout << "Object of type '" << type << "'" << std::endl;
		//create a new object template
		ObjectTemplate* objTmplPtr;
		objTmplPtr = new ObjectTemplate(ObjectType(type));
		//cycle through the ComponentTmpl(s)' definitions ...
		for (componentTmpl = objectTmpl->FirstChildElement("ComponentTmpl");
				componentTmpl != NULL;
				componentTmpl = componentTmpl->NextSiblingElement(
						"ComponentTmpl"))
		{
			const char *family = componentTmpl->Attribute("family", NULL);
			const char *type = componentTmpl->Attribute("type", NULL);
			if (not family or not type)
			{
				continue;
			}
			std::cout << "Component of family '" << family << "' and type '"
					<< type << "'" << std::endl;
			//... add all component templates
			objTmplPtr->addComponentTemplate(
					ComponentTemplateManager::GetSingleton().getComponentTemplate(
							ComponentType(type)));
		}
		// add 'type' object template to manager
		ObjectTemplateManager::GetSingleton().addObjectTemplate(objTmplPtr);
	}

	//check <Game>--<Scene> tag
	std::cout << "Checking <Scene> tag ..." << std::endl;
	scene = game->FirstChildElement("Scene");
	if (not checkTag(scene, "Scene"))
	{
		exit(1);
	}

	//////////////////////////////////////////77
//	ObjectTemplate* objTmpl;
//	// add all kind of object templates
//
//	//1 "Actor" object template
//	objTmpl = new ObjectTemplate(ObjectType("Actor"));
//	//add all component templates
//	objTmpl->addComponentTemplate(
//			ComponentTemplateManager::GetSingleton().getComponentTemplate(
//					ComponentType("Model")));
//	objTmpl->addComponentTemplate(
//			ComponentTemplateManager::GetSingleton().getComponentTemplate(
//					ComponentType("ControlByEvent")));
//	// add "Actor" object template to manager
//	ObjectTemplateManager::GetSingleton().addObjectTemplate(objTmpl);
//
//	//2 "InstancedActor" object template
//	objTmpl = new ObjectTemplate(ObjectType("InstancedActor"));
//	//add all component templates
//	objTmpl->addComponentTemplate(
//			ComponentTemplateManager::GetSingleton().getComponentTemplate(
//					ComponentType("InstanceOf")));
//	// add "InstancedActor" object template to manager
//	ObjectTemplateManager::GetSingleton().addObjectTemplate(objTmpl);

}

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
	else if (timeElapsed > 5.0 and timeElapsed < 10.0)
	{
		std::cout << "secondTask " << timeElapsed << std::endl;
		return AsyncTask::DS_cont;
	}
	return AsyncTask::DS_done;
}
