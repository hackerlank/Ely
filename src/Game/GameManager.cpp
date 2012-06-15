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

	// Create the scene graph (from a scene.xml)
	//mPandaObj
	PT(Object) mPandaObj = mObjects["Actor1"];
	((NodePath) (*mPandaObj)).set_hpr(-90, 0, 0);
//	((NodePath) (*mPandaObj)).reparent_to(mWindow->get_render());
	Model* pandaObjModel = DCAST(Model, mPandaObj->getComponent(
					ComponentFamilyType("Graphics")));
	pandaObjModel->animations().loop("panda_soft", false);

	//mPandaInstObj
	PT(Object) mPandaInstObj = mObjects["InstancedActor1"];
	((NodePath) (*mPandaInstObj)).set_hpr(-90, 0, 0);
	((NodePath) (*mPandaInstObj)).set_pos(-10, 0, 0);
//	((NodePath) (*mPandaInstObj)).reparent_to(mWindow->get_render());
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
	tinyxml2::XMLElement *game;
	//check <Game> tag
	std::cout << "Checking <Game> tag ..." << std::endl;
	game = gameDoc.FirstChildElement("Game");
	if (not checkTag(game, "Game"))
	{
		throw GameException(
				"GameManager::setupGameWorld: No <Game> in " + gameXml);
	}
	//////////////////////////////////////////////////////////////
	//<!-- Object Templates Definition -->
	//Setup object template manager
	std::cout << "Setting up Object Template Manager" << std::endl;
	//check <Game>--<ObjectTmplSet> tag
	tinyxml2::XMLElement *objectTmplSet;
	std::cout << "  Checking <ObjectTmplSet> tag ..." << std::endl;
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
		std::cout << "  Adding Object Template for '" << type << "' type"
				<< std::endl;
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
			std::cout << "    Component of family '" << family << "' and type '"
					<< type << "'" << std::endl;
			//... add all component templates
			objTmplPtr->addComponentTemplate(
					ComponentTemplateManager::GetSingleton().getComponentTemplate(
							ComponentType(type)));
		}
		// add 'type' object template to manager
		ObjectTemplateManager::GetSingleton().addObjectTemplate(objTmplPtr);
	}
	//////////////////////////////////////////////////////////////
	//<!-- Objects Creation -->
	//Create game objects
	std::cout << "Creating Game Objects" << std::endl;
	//check <Game>--<ObjectSet> tag
	tinyxml2::XMLElement *objectSet;
	std::cout << "  Checking <ObjectSet> tag ..." << std::endl;
	objectSet = game->FirstChildElement("ObjectSet");
	if (not checkTag(objectSet, "ObjectSet"))
	{
		throw GameException(
				"GameManager::setupGameWorld: No <ObjectSet> in " + gameXml);
	}
	//reset all component templates parameters to their default values
	ComponentTemplateManager::GetSingleton().resetComponentTemplatesParams();
	//cycle through the Object(s)' definitions
	tinyxml2::XMLElement *object;
	for (object = objectSet->FirstChildElement("Object"); object != NULL;
			object = object->NextSiblingElement("Object"))
	{
		const char *objType = object->Attribute("type", NULL);
		if (not objType)
		{
			continue;
		}
		const char *objId = object->Attribute("id", NULL); //may be NULL
		std::cout << "  Creating Object '"
				<< (objId != NULL ? objId : "UNNAMED") << "'" << std::endl;
		//cycle through the Object Component(s)' to be initialized
		tinyxml2::XMLElement *component;
		for (component = object->FirstChildElement("Component");
				component != NULL;
				component = component->NextSiblingElement("Component"))
		{
			const char *compType = component->Attribute("type", NULL);
			if (not compType)
			{
				continue;
			}
			std::cout << "    Initializing Component '" << compType << "'"
					<< std::endl;
			ComponentTemplate::ParameterTable parameterTable;
			// get the related component template and ...
			ComponentTemplate *componentTmplPtr =
					ObjectTemplateManager::GetSingleton().getObjectTemplate(
							ObjectType(objType))->getComponentTemplate(
							ComponentType(compType));
			//...reset its parameters to their default values
			componentTmplPtr->resetParameters();
			//cycle through the Component Param(s)' to be initialized
			tinyxml2::XMLElement *param;
			for (param = component->FirstChildElement("Param"); param != NULL;
					param = param->NextSiblingElement("Param"))
			{
				const tinyxml2::XMLAttribute* attribute =
						param->FirstAttribute();
				if (not attribute)
				{
					continue;
				}
				std::cout << "      Param '" << attribute->Name() << "' = '"
						<< attribute->Value() << "'" << std::endl;
				parameterTable.insert(
						ComponentTemplate::ParameterTable::value_type(
								attribute->Name(), attribute->Value()));
			}
			componentTmplPtr->setParameters(parameterTable);
		}
		//create the object
		Object *objectPtr = ObjectTemplateManager::GetSingleton().createObject(
				ObjectType(objType));
		if (objId != NULL)
		{
			// replace the internally generated id
			objectPtr->objectId() = ObjectId(objId);
		}
		else
		{
			std::cout << "  Set Object name to '" << objectPtr->objectId()
					<< "'" << std::endl;
		}
		//insert the just created object in the table
		mObjects[objectPtr->objectId()] = PT(Object)(objectPtr);
	}
	//////////////////////////////////////////////////////////////
	//<!-- Scene Creation -->
	//Static scene graph creation
	std::cout << "Creating Scene" << std::endl;
	//check <Game>--<Scene> tag
	tinyxml2::XMLElement *scene;
	std::cout << "  Checking <Scene> tag ..." << std::endl;
	scene = game->FirstChildElement("Scene");
	if (not checkTag(scene, "Scene"))
	{
		throw GameException(
				"GameManager::setupGameWorld: No <Scene> in " + gameXml);
	}
	//cycle through the Node(s)' definitions
	tinyxml2::XMLElement *node;

	for (node = scene->FirstChildElement("Node"); node != NULL;
			node = node->NextSiblingElement("Node"))
	{
		const char *nodeId = node->Attribute("id", NULL);
		if (not nodeId)
		{
			continue;
		}
		std::cout << "  Creating Node for object '" << nodeId << "'"
				<< std::endl;
		//get the object
		PT(Object) objectNodePtr = mObjects[ObjectId(nodeId)];
		//cycle through Node's tags
		tinyxml2::XMLElement *tag;
		//Parent (default: None)
		tag = node->FirstChildElement("Parent");
		if (tag != NULL)
		{
			tinyxml2::XMLText* text = tag->FirstChild()->ToText();
			if (text != NULL)
			{
				std::string value = std::string(text->Value());
				if (value == std::string("Render"))
				{
					((NodePath) (*objectNodePtr)).reparent_to(
							mWindow->get_render());
				}
				else
				{
					ObjectTable::iterator iter = mObjects.find(
							ObjectId(text->Value()));
					if (iter != mObjects.end())
					{
						((NodePath) (*objectNodePtr)).reparent_to(
								*iter->second.p());
					}
				}
			}
		}
	}
}

AsyncTask::DoneStatus GameManager::firstTask(GenericAsyncTask* task)
{
	double timeElapsed = mGlobalClock->get_real_time();
	if (timeElapsed < 1.0)
	{
		std::cout << "firstTask " << timeElapsed << std::endl;
		return AsyncTask::DS_cont;
	}
	return AsyncTask::DS_done;
}

AsyncTask::DoneStatus GameManager::secondTask(GenericAsyncTask* task)
{
	double timeElapsed = mGlobalClock->get_real_time();
	if (timeElapsed < 1.0)
	{
		return AsyncTask::DS_cont;
	}
	else if (timeElapsed > 1.0 and timeElapsed < 2.0)
	{
		std::cout << "secondTask " << timeElapsed << std::endl;
		return AsyncTask::DS_cont;
	}
	return AsyncTask::DS_done;
}
