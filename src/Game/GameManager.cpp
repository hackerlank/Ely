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
	if (mWindow != (WindowFramework*) NULL)
	{
		PRINT("Opened the window successfully!");
		// common setup
		mWindow->enable_keyboard(); // Enable keyboard detection
		mRender = mWindow->get_render();
		mGlobalClock = ClockObject::get_global_clock();
	}
	else
	{
		PRINT("Could not load the window!");
	}
}

GameManager::~GameManager()
{
	// close the framework
	close_framework();
}

void GameManager::initialize()
{
	mWindow->setup_trackball(); // Enable default camera movement
	mTrackBall = mWindow->get_mouse().find("**/+Trackball");
	mMouse2cam = mTrackBall.find("**/+Transform2SG");
	mCamera = mWindow->get_camera_group();
	//setup camera position
	mCamera.set_pos(0, 0, 20);
	mCamera.look_at(0, 0, 10);
	LMatrix4 cameraMat = mCamera.get_transform()->get_mat();
	cameraMat.invert_in_place();
	SMARTPTR(Trackball)trackBall = DCAST(Trackball, mTrackBall.node());
	trackBall->set_mat(cameraMat);

#ifdef DEBUG
	GamePhysicsManager::GetSingletonPtr()->initDebug(mWindow);
	mPhysicsDebugEnabled = false;
#endif

	//initialize typed objects
	initTypedObjects();

	//manageObjects component template manager
	setupCompTmplMgr();

	//create the game world (static definition)
	createGameWorld(std::string("game.xml"));

	//setup the game world
	setupGameWorld();
}

void GameManager::setupGameWorld()
{
	//Actor1
	SMARTPTR(Object)actor1 =
	ObjectTemplateManager::GetSingleton().getCreatedObject("Actor1");
	//play animation
	SMARTPTR(Model) actor1Model = DCAST(Model, actor1->getComponent(
					ComponentFamilyType("Scene")));
	actor1Model->animations().loop("panda_soft", false);
	//play sound
	SMARTPTR(Sound3d) actor1Sound3d = DCAST(Sound3d, actor1->getComponent(
					ComponentFamilyType("Audio")));
	actor1Sound3d->getSound(0)->set_loop(true);
	actor1Sound3d->getSound(0)->play();

	//InstancedActor1
	SMARTPTR(Object) plane1 =
	ObjectTemplateManager::GetSingleton().getCreatedObject("Plane1");
	SMARTPTR(Model) plane1Model = DCAST(Model, plane1->getComponent(
					ComponentFamilyType("Scene")));
	TextureStage* planeTS0 = new TextureStage("planeTS0");
	Texture* planeTex = TexturePool::load_texture("rock_02.jpg");
	plane1Model->getNodePath().set_texture(planeTS0, planeTex, 1);
	plane1Model->getNodePath().set_tex_scale(planeTS0, 1000, 1000);

	//Terrain1
//	SMARTPTR(Object)terrain1 =
//	ObjectTemplateManager::GetSingleton().getCreatedObject("Terrain1");
//	terrain1->getNodePath().set_render_mode_wireframe(1);

	mControlGrabbed = false;
	//enable/disable Actor1 control by event
	define_key("v", "enableActor1Control", &GameManager::toggleActor1Control,
			(void*) this);
	//enable/disable camera control by event
	define_key("c", "enableCameraControl", &GameManager::toggleCameraControl,
			(void*) this);

#ifdef DEBUG
	define_key("p", "togglePhysicsDebug", &GameManager::togglePhysicsDebug,
			(void*) this);
#endif

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
	// Add all kind of component templates

	///Audio templates
	//Listener
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new ListenerTemplate(this, mWindow));
	//Sound3d
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new Sound3dTemplate(this, mWindow));

	///Behavior templates
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new ActivityTemplate(this, mWindow));

	///Control templates
	//Driver
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new DriverTemplate(this, mWindow));

	///Scene templates
	//InstanceOf
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new InstanceOfTemplate(this, mWindow));
	//Model
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new ModelTemplate(this, mWindow));
	//NodePathWrapper
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new NodePathWrapperTemplate(this, mWindow));
	//Terrain
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new TerrainTemplate(this, mWindow));

	///Physics templates
	//RigidBody
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new RigidBodyTemplate(this, mWindow));

}

static bool checkTag(tinyxml2::XMLElement* tag, const char* tagStr)
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
	PRINT("Loading '" << gameXml << "'...");
	if (tinyxml2::XML_NO_ERROR != gameDoc.LoadFile(gameXml.c_str()))
	{
		fprintf(stderr, "Error detected on '%s':\n", gameXml.c_str());
		gameDoc.PrintError();
		fprintf(stderr, "%s\n%s\n", gameDoc.GetErrorStr1(),
				gameDoc.GetErrorStr2());
		throw GameException(
				"GameManager::setupGameWorld: Failed to load/parse " + gameXml);
	}
	tinyxml2::XMLElement* game;
	//check <Game> tag
	PRINT("Checking <Game> tag ...");
	game = gameDoc.FirstChildElement("Game");
	if (not checkTag(game, "Game"))
	{
		throw GameException(
				"GameManager::setupGameWorld: No <Game> in " + gameXml);
	}
	//////////////////////////////////////////////////////////////
	//<!-- Object Templates Definition -->
	//Setup object template manager
	PRINT("Setting up Object Template Manager");
	//check <Game>--<ObjectTmplSet> tag
	tinyxml2::XMLElement* objectTmplSet;
	PRINT("  Checking <ObjectTmplSet> tag ...");
	objectTmplSet = game->FirstChildElement("ObjectTmplSet");
	if (not checkTag(objectTmplSet, "ObjectTmplSet"))
	{
		throw GameException(
				"GameManager::setupGameWorld: No <ObjectTmplSet> in "
						+ gameXml);
	}
	//cycle through the ObjectTmpl(s)' definitions and
	// add all kind of object templates
	tinyxml2::XMLElement* objectTmpl, *componentTmpl;
	for (objectTmpl = objectTmplSet->FirstChildElement("ObjectTmpl");
			objectTmpl != NULL;
			objectTmpl = objectTmpl->NextSiblingElement("ObjectTmpl"))
	{
		const char* objectType = objectTmpl->Attribute("type", NULL);
		if (not objectType)
		{
			continue;
		}
		PRINT("  Adding Object Template for '" << objectType << "' type");
		//create a new object template
		SMARTPTR(ObjectTemplate)objTmplPtr;
		objTmplPtr = new ObjectTemplate(ObjectType(objectType),
				ObjectTemplateManager::GetSingletonPtr(), this, mWindow);
		//cycle through the ComponentTmpl(s)' definitions ...
		for (componentTmpl = objectTmpl->FirstChildElement("ComponentTmpl");
				componentTmpl != NULL;
				componentTmpl = componentTmpl->NextSiblingElement(
						"ComponentTmpl"))
		{
			const char* compFamily = componentTmpl->Attribute("family", NULL);
			const char* compType = componentTmpl->Attribute("type", NULL);
			if (not compFamily or not compType)
			{
				continue;
			}
			PRINT(
					"    Component of family '" << compFamily << "' and type '" << compType << "'");
			//... add all component templates
			SMARTPTR(ComponentTemplate)compTmpl =
			ComponentTemplateManager::GetSingleton().getComponentTemplate(
					ComponentType(compType));
			if (compTmpl == NULL)
			{
				continue;
			}
			objTmplPtr->addComponentTemplate(compTmpl);
		}
		// add 'type' object template to manager
		ObjectTemplateManager::GetSingleton().addObjectTemplate(objTmplPtr);
	}
	//////////////////////////////////////////////////////////////
	//<!-- Objects Creation -->
	//Create game objects
	PRINT("Creating Game Objects");
	//check <Game>--<ObjectSet> tag
	tinyxml2::XMLElement* objectSet;
	PRINT("  Checking <ObjectSet> tag ...");
	objectSet = game->FirstChildElement("ObjectSet");
	if (not checkTag(objectSet, "ObjectSet"))
	{
		throw GameException(
				"GameManager::setupGameWorld: No <ObjectSet> in " + gameXml);
	}
	//reset all component templates parameters to their default values
	ComponentTemplateManager::GetSingleton().resetComponentTemplatesParams();
	tinyxml2::XMLElement* object;
	//create a priority queue of objects
	std::priority_queue<Orderable<tinyxml2::XMLElement> > orderedObjects;
	for (object = objectSet->FirstChildElement("Object"); object != NULL;
			object = object->NextSiblingElement("Object"))
	{
		Orderable<tinyxml2::XMLElement> ordObj;
		ordObj.setPtr(object);
		const char* priority = object->Attribute("priority", NULL);
		priority != NULL ? ordObj.setPrio(atoi(priority)) : ordObj.setPrio(0);
		orderedObjects.push(ordObj);
	}
	//cycle through the Object(s)' definitions in order of priority
	while (not orderedObjects.empty())
	{
		//access top object
		object = orderedObjects.top().getPtr();
		const char* objType = object->Attribute("type", NULL);
		// get the related object template
		SMARTPTR(ObjectTemplate)objectTmplPtr =
		ObjectTemplateManager::GetSingleton().getObjectTemplate(
				ObjectType(objType));
		if ((not objType) or (not objectTmplPtr))
		{
			//no object without type allowed or object type doesn't exist
			orderedObjects.pop();
			continue;
		}
		const char* objId = object->Attribute("id", NULL); //may be NULL
		PRINT(
				"  Creating Object '" << (objId != NULL ? objId : "UNNAMED") << "'...");
		tinyxml2::XMLElement* component;
		//create a priority queue of components
		std::priority_queue<Orderable<tinyxml2::XMLElement> > orderedComponents;
		for (component = object->FirstChildElement("Component");
				component != NULL;
				component = component->NextSiblingElement("Component"))
		{
			Orderable<tinyxml2::XMLElement> ordComp;
			ordComp.setPtr(component);
			const char* priority = component->Attribute("priority", NULL);
			priority != NULL ?
					ordComp.setPrio(atoi(priority)) : ordComp.setPrio(0);
			orderedComponents.push(ordComp);
		}
		//reset all object's component parameters to their default values
		ObjectTemplate::ComponentTemplateList::iterator iter;
		ObjectTemplate::ComponentTemplateList compTmplList =
				objectTmplPtr->getComponentTemplates();
		for (iter = compTmplList.begin(); iter != compTmplList.end(); ++iter)
		{
			(*iter).p()->setParametersDefaults();
		}
		ComponentTemplateManager::GetSingleton().resetComponentTemplatesParams();
		//cycle through the Object Component(s)' to be initialized in order of priority
		while (not orderedComponents.empty())
		{
			//access top component
			component = orderedComponents.top().getPtr();
			const char* compType = component->Attribute("type", NULL);
			// get the related component template
			SMARTPTR(ComponentTemplate)componentTmplPtr =
			objectTmplPtr->getComponentTemplate(
					ComponentType(compType));
			if ((not compType) or (not componentTmplPtr))
			{
				//no component without type allowed or component type doesn't exist
				orderedComponents.pop();
				continue;
			}
			PRINT( "    Initializing Component '" << compType << "'");
			ParameterTable parameterTable;
			//reset component' parameters to their default values
			componentTmplPtr->setParametersDefaults();
			//cycle through the Component Param(s)' to be initialized
			tinyxml2::XMLElement* param;
			for (param = component->FirstChildElement("Param"); param != NULL;
					param = param->NextSiblingElement("Param"))
			{
				const tinyxml2::XMLAttribute* attribute =
						param->FirstAttribute();
				if (not attribute)
				{
					continue;
				}
				PRINT(
						"      Param '" << attribute->Name() << "' = '" << attribute->Value() << "'");
				parameterTable.insert(
						ParameterTable::value_type(attribute->Name(),
								attribute->Value()));
			}
			componentTmplPtr->setParameters(parameterTable);
			//remove top component from the priority queue
			orderedComponents.pop();
		}
		//create the object
		SMARTPTR(Object)objectPtr;
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
		if (objectPtr == NULL)
		{
			continue;
		}
		PRINT( "  ...Created Object '" << objectPtr->objectId() << "'");
		//////////////////////////////////////////////////////////////
		//<!-- Object addition to Scene -->
		ParameterTable objParameterTable;
		//reset object' parameters to their default values
		objectTmplPtr->setParametersDefaults();
		//cycle through the Object Param(s)' to be initialized
		tinyxml2::XMLElement* objParam;
		for (objParam = object->FirstChildElement("Param"); objParam != NULL;
				objParam = objParam->NextSiblingElement("Param"))
		{
			const tinyxml2::XMLAttribute* attribute =
					objParam->FirstAttribute();
			if (not attribute)
			{
				continue;
			}
			PRINT(
					"      Param '" << attribute->Name() << "' = '" << attribute->Value() << "'");
			objParameterTable.insert(
					ParameterTable::value_type(attribute->Name(),
							attribute->Value()));
		}
		objectTmplPtr->setParameters(objParameterTable);
		//give a chance to object (and its components) to customize
		//themselves when being added to scene.
		objectPtr->sceneSetup();
		//remove top object from the priority queue
		orderedObjects.pop();
	}
}

AsyncTask::DoneStatus GameManager::firstTask(GenericAsyncTask* task)
{
	double timeElapsed = mGlobalClock->get_real_time();
	if (timeElapsed < 1.0)
	{
		PRINT("firstTask " << timeElapsed);
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
		PRINT("secondTask " << timeElapsed);
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
	SMARTPTR(Object)actor1 =
	ObjectTemplateManager::GetSingleton().getCreatedObject("Actor1");
	SMARTPTR(Driver)actor1Control = DCAST(Driver, actor1->getComponent(
					ComponentFamilyType("Control")));
	bool isEnabled = actor1Control->isEnabled();

	if (isEnabled)
	{
		//if enabled then disable it
		//disable
		actor1Control->disable();
		//reset trackball transform
		LMatrix4 cameraMat = gameManager->mCamera.get_transform()->get_mat();
		cameraMat.invert_in_place();
		SMARTPTR(Trackball)trackBall =
		DCAST(Trackball, gameManager->mTrackBall.node());
		trackBall->set_mat(cameraMat);
		//(re)enable trackball
		gameManager->enable_mouse();
		//
		gameManager->mControlGrabbed = false;
	}
	else if (not gameManager->mControlGrabbed)
	{
		//if disabled then enable it
		//disable the trackball
		gameManager->disable_mouse();
		//enable
		actor1Control->enable();
		//
		gameManager->mControlGrabbed = true;
	}

}

#ifdef DEBUG
void GameManager::togglePhysicsDebug(const Event* event, void* data)
{
	GameManager* gameManager = (GameManager*) data;
	if (gameManager->mPhysicsDebugEnabled)
	{
		GamePhysicsManager::GetSingleton().debug(false);
	}
	else
	{
		GamePhysicsManager::GetSingleton().debug(true);
	}
	gameManager->mPhysicsDebugEnabled = not gameManager->mPhysicsDebugEnabled;
}
#endif

void GameManager::toggleCameraControl(const Event* event, void* data)
{
	GameManager* gameManager = (GameManager*) data;
	SMARTPTR(Object)camera =
	ObjectTemplateManager::GetSingleton().getCreatedObject("camera");
	SMARTPTR(Driver)cameraControl = DCAST(Driver, camera->getComponent(
					ComponentFamilyType("Control")));
	bool isEnabled = cameraControl->isEnabled();

	if (isEnabled)
	{
		//if enabled then disable it
		//disable
		cameraControl->disable();
		//reset trackball transform
		LMatrix4 cameraMat = gameManager->mCamera.get_transform()->get_mat();
		cameraMat.invert_in_place();
		SMARTPTR(Trackball)trackBall =
		DCAST(Trackball, gameManager->mTrackBall.node());
		trackBall->set_mat(cameraMat);
		//(re)enable trackball
		gameManager->enable_mouse();
		//
		gameManager->mControlGrabbed = false;
	}
	else if (not gameManager->mControlGrabbed)
	{
		//if disabled then enable it
		//disable the trackball
		gameManager->disable_mouse();
		//enable
		cameraControl->enable();
		//
		gameManager->mControlGrabbed = true;
	}

}
