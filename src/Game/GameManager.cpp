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
		// common setup
		mWindow->enable_keyboard(); // Enable keyboard detection
		mWindow->setup_trackball(); // Enable default camera movement
		mTrackBall = mWindow->get_mouse().find("**/+Trackball");
		mMouse2cam = mTrackBall.find("**/+Transform2SG");
		mRender = mWindow->get_render();
		mCamera = mWindow->get_camera_group();
		mGlobalClock = ClockObject::get_global_clock();
		// managers setup
		mAudioMgr = AudioManager::create_AudioManager();
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
	//setup camera position
	mCamera.set_pos(0, -50, 20);
	mCamera.look_at(0, 0, 10);
	LMatrix4 cameraMat = mCamera.get_transform()->get_mat();
	cameraMat.invert_in_place();
	NodePath trackBallNP = mWindow->get_mouse().find("**/+Trackball");
	PT(Trackball) trackBall = DCAST(Trackball, trackBallNP.node());
	trackBall->set_mat(cameraMat);

	//initialize typed objects
	initTypedObjects();
	//setup component template manager
	setupCompTmplMgr();

	//setup game world (static)
	createGameWorld(std::string("game.xml"));

	// Manipulate objects
	//Actor1
	PT(Object) actor1 =
			ObjectTemplateManager::GetSingleton().createdObjects()["Actor1"];
	Model* actor1Model = DCAST(Model, actor1->getComponent(
					ComponentFamilyType("Graphics")));
	actor1Model->animations().loop("panda_soft", false);
	//enable/disable control by event
	define_key("c", "enableActor1Control", &GameManager::toggleActor1Control,
			(void*) this);

	//InstancedActor1
	PT(Object) instancedActor1 =
			ObjectTemplateManager::GetSingleton().createdObjects()["InstancedActor1"];

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
			new ControlByEventTemplate(this, mWindow));
	//Sound3d template
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new Sound3dTemplate(this, mWindow, mAudioMgr));

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

void GameManager::createGameWorld(const std::string& gameWorldXML)
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
		objTmplPtr = new ObjectTemplate(ObjectType(type),
				ObjectTemplateManager::GetSingletonPtr(), this, mWindow);
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
	tinyxml2::XMLElement *object;
	//create a priority queue of objects
	std::priority_queue<Orderable<tinyxml2::XMLElement> > orderedObjects;
	for (object = objectSet->FirstChildElement("Object"); object != NULL;
			object = object->NextSiblingElement("Object"))
	{
		Orderable<tinyxml2::XMLElement> ordObj;
		ordObj.setPtr(object);
		const char *priority = object->Attribute("priority", NULL);
		priority != NULL ? ordObj.setPrio(atoi(priority)) : ordObj.setPrio(0);
		orderedObjects.push(ordObj);
	}
	//cycle through the Object(s)' definitions in order of priority
	while (not orderedObjects.empty())
	{
		//access top object
		object = orderedObjects.top().getPtr();
		const char *objType = object->Attribute("type", NULL);
		if (not objType)
		{
			//no object without type allowed
			orderedObjects.pop();
			continue;
		}
		const char *objId = object->Attribute("id", NULL); //may be NULL
		std::cout << "  Creating Object '"
				<< (objId != NULL ? objId : "UNNAMED") << "'..." << std::endl;
		tinyxml2::XMLElement *component;
		//create a priority queue of components
		std::priority_queue<Orderable<tinyxml2::XMLElement> > orderedComponents;
		for (component = object->FirstChildElement("Component");
				component != NULL;
				component = component->NextSiblingElement("Component"))
		{
			Orderable<tinyxml2::XMLElement> ordComp;
			ordComp.setPtr(component);
			const char *priority = component->Attribute("priority", NULL);
			priority != NULL ? ordComp.setPrio(atoi(priority)) :
					ordComp.setPrio(0);
			orderedComponents.push(ordComp);
		}
		//cycle through the Object Component(s)' to be initialized in order of priority
		while (not orderedComponents.empty())
		{
			//access top component
			component = orderedComponents.top().getPtr();
			const char *compType = component->Attribute("type", NULL);
			if (not compType)
			{
				//no component without type allowed
				orderedComponents.pop();
				continue;
			}
			std::cout << "    Initializing Component '" << compType << "'"
					<< std::endl;
			ParameterTable parameterTable;
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
						ParameterTable::value_type(attribute->Name(),
								attribute->Value()));
			}
			componentTmplPtr->setParameters(parameterTable);
			//remove top component from the priority queue
			orderedComponents.pop();
		}
		//create the object
		Object *objectPtr;
		if ((objId != NULL) and (std::string(objId) != std::string("")))
		{
			// set id with the passed id
			objectPtr = ObjectTemplateManager::GetSingleton().createObject(
					ObjectType(objType), ObjectId(objId));
		}
		else
		{
			// set id with the internally generated id
			objectPtr = ObjectTemplateManager::GetSingleton().createObject(
					ObjectType(objType));
		}
		std::cout << "  ...Created Object '" << objectPtr->objectId() << "'"
				<< std::endl;
		//////////////////////////////////////////////////////////////
		//<!-- Object addition to Scene -->
		ParameterTable objParameterTable;
		// get the related object template and ...
		ObjectTemplate *objectTmplPtr =
				ObjectTemplateManager::GetSingleton().getObjectTemplate(
						ObjectType(objType));
		//...reset its parameters to their default values
		objectTmplPtr->resetParameters();
		//cycle through the Object Param(s)' to be initialized
		tinyxml2::XMLElement *objParam;
		for (objParam = object->FirstChildElement("Param"); objParam != NULL;
				objParam = objParam->NextSiblingElement("Param"))
		{
			const tinyxml2::XMLAttribute* attribute =
					objParam->FirstAttribute();
			if (not attribute)
			{
				continue;
			}
			std::cout << "      Param '" << attribute->Name() << "' = '"
					<< attribute->Value() << "'" << std::endl;
			objParameterTable.insert(
					ParameterTable::value_type(attribute->Name(),
							attribute->Value()));
		}
		objectTmplPtr->setParameters(objParameterTable);
		//give a chance to object (and its components) to setup
		//when being added to scene.
		objectPtr->sceneSetup();
		//remove top object from the priority queue
		orderedObjects.pop();
	}
//	//////////////////////////////////////////////////////////////
//	//<!-- Scene Creation -->
//	//Static scene graph creation
//	std::cout << "Creating Scene" << std::endl;
//	//check <Game>--<Scene> tag
//	tinyxml2::XMLElement *scene;
//	std::cout << "  Checking <Scene> tag ..." << std::endl;
//	scene = game->FirstChildElement("Scene");
//	if (not checkTag(scene, "Scene"))
//	{
//		throw GameException(
//				"GameManager::setupGameWorld: No <Scene> in " + gameXml);
//	}
//	//cycle through the Node(s)' definitions
//	tinyxml2::XMLElement *node;
//
//	for (node = scene->FirstChildElement("Node"); node != NULL;
//			node = node->NextSiblingElement("Node"))
//	{
//		const char *nodeId = node->Attribute("id", NULL);
//		if (not nodeId)
//		{
//			continue;
//		}
//		std::cout << "  Creating Node for object '" << nodeId << "'"
//				<< std::endl;
//		//get the object
//		PT(Object) objectNodePtr =
//				ObjectTemplateManager::GetSingleton().createdObjects()[ObjectId(
//						nodeId)];
//		//cycle through Node's tags
//		tinyxml2::XMLElement *tag;
//		//Parent (default: None)
//		tag = node->FirstChildElement("Parent");
//		if (tag != NULL)
//		{
//			if (tag->FirstChild() != NULL)
//			{
//				tinyxml2::XMLText* text = tag->FirstChild()->ToText();
//				if (text != NULL)
//				{
//					std::string value = std::string(text->Value());
//					if (value == std::string("Render"))
//					{
//						objectNodePtr->nodePath().reparent_to(
//								mWindow->get_render());
//					}
//					else
//					{
//						ObjectTemplateManager::ObjectTable::iterator iter =
//								ObjectTemplateManager::GetSingleton().createdObjects().find(
//										ObjectId(value));
//						if (iter
//								!= ObjectTemplateManager::GetSingleton().createdObjects().end())
//						{
//							objectNodePtr->nodePath().reparent_to(
//									*iter->second.p());
//						}
//					}
//				}
//			}
//		}
//		//InstanceOf (default: None)
//		tag = node->FirstChildElement("InstanceOf");
//		if (tag != NULL)
//		{
//			if (tag->FirstChild() != NULL)
//			{
//				tinyxml2::XMLText* text = tag->FirstChild()->ToText();
//				if (text != NULL)
//				{
//					std::string value = std::string(text->Value());
//					ObjectTemplateManager::ObjectTable::iterator iter =
//							ObjectTemplateManager::GetSingleton().createdObjects().find(
//									ObjectId(value));
//					if (iter
//							!= ObjectTemplateManager::GetSingleton().createdObjects().end())
//					{
//						iter->second->nodePath().instance_to(
//								*objectNodePtr.p());
//					}
//				}
//			}
//		}
//		//Position (default: (0,0,0))
//		tag = node->FirstChildElement("Position");
//		if (tag != NULL)
//		{
//			const char *coord;
//			coord = tag->Attribute("x", NULL);
//			if (coord != NULL)
//			{
//				objectNodePtr->nodePath().set_x((float) atof(coord));
//			}
//			coord = tag->Attribute("y", NULL);
//			if (coord != NULL)
//			{
//				objectNodePtr->nodePath().set_y((float) atof(coord));
//			}
//			coord = tag->Attribute("z", NULL);
//			if (coord != NULL)
//			{
//				objectNodePtr->nodePath().set_z((float) atof(coord));
//			}
//		}
//		//Orientation (default: (0,0,0))
//		tag = node->FirstChildElement("Orientation");
//		if (tag != NULL)
//		{
//			const char *coord;
//			coord = tag->Attribute("h", NULL);
//			if (coord != NULL)
//			{
//				objectNodePtr->nodePath().set_h((float) atof(coord));
//			}
//			coord = tag->Attribute("p", NULL);
//			if (coord != NULL)
//			{
//				objectNodePtr->nodePath().set_p((float) atof(coord));
//			}
//			coord = tag->Attribute("r", NULL);
//			if (coord != NULL)
//			{
//				objectNodePtr->nodePath().set_r((float) atof(coord));
//			}
//		}
//		//Scaling (default: (1.0,1.0,1.0))
//		tag = node->FirstChildElement("Scaling");
//		if (tag != NULL)
//		{
//			const char *coord;
//			coord = tag->Attribute("x", NULL);
//			if (coord != NULL)
//			{
//				float res = (float) atof(coord);
//				objectNodePtr->nodePath().set_sx((res != 0.0 ? res : 1.0));
//			}
//			coord = tag->Attribute("y", NULL);
//			if (coord != NULL)
//			{
//				float res = (float) atof(coord);
//				objectNodePtr->nodePath().set_sy((res != 0.0 ? res : 1.0));
//			}
//			coord = tag->Attribute("z", NULL);
//			if (coord != NULL)
//			{
//				float res = (float) atof(coord);
//				objectNodePtr->nodePath().set_sz((res != 0.0 ? res : 1.0));
//			}
//		}
//	}
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

void GameManager::enable_mouse()
{
	if (mMouse2cam)
	{
		mMouse2cam.reparent_to(mTrackBall);
	}
}

void GameManager::disable_mouse()
{
	if (mMouse2cam)
	{
		mMouse2cam.detach_node();
	}
}

void GameManager::toggleActor1Control(const Event* event, void* data)
{
	GameManager* gameManager = (GameManager*) data;
	PT(Object) actor1 =
			ObjectTemplateManager::GetSingleton().createdObjects()["Actor1"];
	ControlByEvent* actor1Control = DCAST(ControlByEvent, actor1->getComponent(
					ComponentFamilyType("Input")));
	bool isEnabled = actor1Control->isEnabled();

	if (isEnabled)
	{
		//if enabled then disable it
		//disable
		actor1Control->disable();
		//reset trackball transform
		LMatrix4 cameraMat = gameManager->mCamera.get_transform()->get_mat();
		cameraMat.invert_in_place();
		PT(Trackball) trackBall =
				DCAST(Trackball, gameManager->mTrackBall.node());
		trackBall->set_mat(cameraMat);
		//(re)enable trackball
		gameManager->enable_mouse();
	}
	else
	{
		//if disabled then enable it
		//disable the trackball
		gameManager->disable_mouse();
		//enable
		actor1Control->enable();
	}

}

