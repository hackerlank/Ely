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
 * \author consultit
 */

#include "Game/GameManager.h"
#include "ObjectModel/ComponentTemplateManager.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Utilities/ComponentSuite.h"
#include "tinyxml2.h"

namespace ely
{

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

#ifndef TESTING
	LTDL_SET_PRELOADED_SYMBOLS();
#endif
}

GameManager::~GameManager()
{
	// close the framework
	close_framework();
}

void GameManager::initialize()
{
	///<DEFAULT CAMERA CONTROL>
//	mWindow->setup_trackball(); // Enable default camera movement
//	mTrackBall = mWindow->get_mouse().find("**/+Trackball");
//  mMouse2cam = mTrackBall.find("**/+Transform2SG");
//	//setup camera position
//	mWindow->get_camera_group().set_pos(0, -50, 20);
//	mWindow->get_camera_group().look_at(0, 0, 10);
//	LMatrix4 cameraMat = mWindow->get_camera_group().get_transform()->get_mat();
//	cameraMat.invert_in_place();
//	SMARTPTR(Trackball)trackBall = DCAST(Trackball, mTrackBall.node());
//	trackBall->set_mat(cameraMat);
	///</DEFAULT CAMERA CONTROL>

#ifdef ELY_DEBUG
	GamePhysicsManager::GetSingletonPtr()->initDebug(mWindow);
	mPhysicsDebugEnabled = false;
#endif

	//initialize typed objects
	initTypedObjects();

	//manageObjects component template manager
	setupCompTmplMgr();

	//create the game world (static definition)
	createGameWorld(std::string(ELY_GAMEXML));

	//play the game
	GamePlay();
}

void GameManager::GamePlay()
{

#ifdef ELY_DEBUG
	mPhysicsDebugData =
			new EventCallbackInterface<GameManager>::EventCallbackData(this,
					&GameManager::togglePhysicsDebug);
	define_key("p", "togglePhysicsDebug",
			&EventCallbackInterface<GameManager>::eventCallbackFunction,
			reinterpret_cast<void*>(mPhysicsDebugData.p()));
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

	///AI templates
	//Steering
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new SteeringTemplate(this, mWindow));
	//CrowdAgent
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new CrowdAgentTemplate(this, mWindow));
	//NavMesh
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new NavMeshTemplate(this, mWindow));
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
	//Chaser
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new ChaserTemplate(this, mWindow));

	//Driver
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new DriverTemplate(this, mWindow));

	///Physics templates
	//RigidBody
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new RigidBodyTemplate(this, mWindow));
	//Character
	ComponentTemplateManager::GetSingleton().addComponentTemplate(
			new CharacterControllerTemplate(this, mWindow));

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

}
} //ely

namespace
{
bool checkTag(tinyxml2::XMLElement* tag, const char* tagStr)
{
	if (not tag)
	{
		fprintf(stderr, "<%s> tag not found!\n", tagStr);
		return false;
	}
	return true;
}
}

namespace ely
{

void GameManager::createGameWorld(const std::string& gameWorldXML)
{
	//read the game configuration file
	tinyxml2::XMLDocument gameDoc;
	//load file
	PRINT("Loading '" << gameWorldXML << "'...");
	if (tinyxml2::XML_NO_ERROR != gameDoc.LoadFile(gameWorldXML.c_str()))
	{
		fprintf(stderr, "Error detected on '%s':\n", gameWorldXML.c_str());
		gameDoc.PrintError();
		fprintf(stderr, "%s\n%s\n", gameDoc.GetErrorStr1(),
				gameDoc.GetErrorStr2());
		throw GameException(
				"GameManager::setupGameWorld: Failed to load/parse "
						+ gameWorldXML);
	}
	tinyxml2::XMLElement* gameTAG;
	//check <Game> tag
	PRINT("Checking <Game> tag ...");
	gameTAG = gameDoc.FirstChildElement("Game");
	if (not checkTag(gameTAG, "Game"))
	{
		throw GameException(
				"GameManager::setupGameWorld: No <Game> in " + gameWorldXML);
	}
	//////////////////////////////////////////////////////////////
	//<!-- Object Templates Definition -->
	//Setup object template manager
	PRINT("Setting up Object Template Manager");
	//check <Game>--<ObjectTmplSet> tag
	tinyxml2::XMLElement* objectTmplSetTAG;
	PRINT("  Checking <ObjectTmplSet> tag ...");
	objectTmplSetTAG = gameTAG->FirstChildElement("ObjectTmplSet");
	if (not checkTag(objectTmplSetTAG, "ObjectTmplSet"))
	{
		throw GameException(
				"GameManager::setupGameWorld: No <ObjectTmplSet> in "
						+ gameWorldXML);
	}
	//cycle through the ObjectTmpl(s)' definitions and
	// add all kind of object templates
	tinyxml2::XMLElement* objectTmplTAG, *componentTmplTAG;
	for (objectTmplTAG = objectTmplSetTAG->FirstChildElement("ObjectTmpl");
			objectTmplTAG != NULL;
			objectTmplTAG = objectTmplTAG->NextSiblingElement("ObjectTmpl"))
	{
		const char* objectTypeTAG = objectTmplTAG->Attribute("type", NULL);
		if (not objectTypeTAG)
		{
			continue;
		}
		PRINT("  Adding Object Template for '" << objectTypeTAG << "' type");
		//create a new object template
		SMARTPTR(ObjectTemplate)objTmplPtr;
		objTmplPtr = new ObjectTemplate(ObjectType(objectTypeTAG),
				ObjectTemplateManager::GetSingletonPtr(), this, mWindow);
		//create a priority queue of component templates
		std::priority_queue<Orderable<tinyxml2::XMLElement> > orderedComponentTmplsTAG;
		for (componentTmplTAG = objectTmplTAG->FirstChildElement(
				"ComponentTmpl"); componentTmplTAG != NULL; componentTmplTAG =
				componentTmplTAG->NextSiblingElement("ComponentTmpl"))
		{
			Orderable<tinyxml2::XMLElement> ordCompTAG;
			ordCompTAG.setPtr(componentTmplTAG);
			const char* priorityTAG = componentTmplTAG->Attribute("priority",
					NULL);
			priorityTAG != NULL ?
					ordCompTAG.setPrio(strtol(priorityTAG, NULL, 0)) :
					ordCompTAG.setPrio(0);
			orderedComponentTmplsTAG.push(ordCompTAG);
		}
		//cycle through the ComponentTmpl(s)' definitions ...
		while (not orderedComponentTmplsTAG.empty())
		{
			//access top object
			componentTmplTAG = orderedComponentTmplsTAG.top().getPtr();
			const char* compFamilyTAG = componentTmplTAG->Attribute("family",
					NULL);
			const char* compTypeTAG = componentTmplTAG->Attribute("type", NULL);
			if (not compFamilyTAG or not compTypeTAG)
			{
				continue;
			}
			PRINT(
					"    Component of family '" << compFamilyTAG << "' and type '" << compTypeTAG << "'");
			//cycle through the ComponentTmpl Param(s)' to be initialized
			tinyxml2::XMLElement* paramTAG;
			for (paramTAG = componentTmplTAG->FirstChildElement("Param");
					paramTAG != NULL;
					paramTAG = paramTAG->NextSiblingElement("Param"))
			{
				const tinyxml2::XMLAttribute* attributeTAG =
						paramTAG->FirstAttribute();
				if (not attributeTAG)
				{
					continue;
				}
				PRINT(
						"      Param '" << attributeTAG->Name() << "' = '" << attributeTAG->Value() << "'");
				//add attribute for this component type of this object.
				objTmplPtr->addComponentParameter(attributeTAG->Name(),
						attributeTAG->Value(), compTypeTAG);
			}
			//... add all component templates
			SMARTPTR(ComponentTemplate)compTmpl =
			ComponentTemplateManager::GetSingleton().getComponentTemplate(
					ComponentType(compTypeTAG));
			if (compTmpl == NULL)
			{
				continue;
			}
			objTmplPtr->addComponentTemplate(compTmpl);
			//remove top object from the priority queue
			orderedComponentTmplsTAG.pop();
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
	objectSet = gameTAG->FirstChildElement("ObjectSet");
	if (not checkTag(objectSet, "ObjectSet"))
	{
		throw GameException(
				"GameManager::setupGameWorld: No <ObjectSet> in "
						+ gameWorldXML);
	}
	//reset all component templates parameters to their default values
	ComponentTemplateManager::GetSingleton().resetComponentTemplatesParams();
	tinyxml2::XMLElement* objectTAG;
	//store created objects in this queue
	std::queue<SMARTPTR(Object)> createdObjectQueue;
	//create a priority queue of objects
	std::priority_queue<Orderable<tinyxml2::XMLElement> > orderedObjectsTAG;
	for (objectTAG = objectSet->FirstChildElement("Object"); objectTAG != NULL;
			objectTAG = objectTAG->NextSiblingElement("Object"))
	{
		Orderable<tinyxml2::XMLElement> ordObjTAG;
		ordObjTAG.setPtr(objectTAG);
		const char* priorityTAG = objectTAG->Attribute("priority", NULL);
		priorityTAG != NULL ?
				ordObjTAG.setPrio(strtol(priorityTAG, NULL, 0)) :
				ordObjTAG.setPrio(0);
		orderedObjectsTAG.push(ordObjTAG);
	}
	//cycle through the Object(s)' definitions in order of priority
	while (not orderedObjectsTAG.empty())
	{
		//access top object
		objectTAG = orderedObjectsTAG.top().getPtr();
		const char* objTypeTAG = objectTAG->Attribute("type", NULL);
		// get the related object template
		SMARTPTR(ObjectTemplate)objectTmplPtr =
		ObjectTemplateManager::GetSingleton().getObjectTemplate(
				ObjectType(objTypeTAG));
		if ((not objTypeTAG) or (not objectTmplPtr))
		{
			//no object without type allowed or object type doesn't exist
			orderedObjectsTAG.pop();
			continue;
		}
		const char* objIdTAG = objectTAG->Attribute("id", NULL); //may be NULL
		PRINT(
				"  Creating Object '" << (objIdTAG != NULL ? objIdTAG : "UNNAMED") << "'...");
		//set a ParameterTable for each component
		ParameterTableMap compTmplParams;
		//cycle through the Object Component(s)' to be initialized in order of priority
		tinyxml2::XMLElement* componentTAG;
		for (componentTAG = objectTAG->FirstChildElement("Component");
				componentTAG != NULL;
				componentTAG = componentTAG->NextSiblingElement("Component"))
		{
			const char* compTypeTAG = componentTAG->Attribute("type", NULL);
			if ((not compTypeTAG))
			{
				//no component without type allowed
				//but not defined component types are allowed!
				continue;
			}
			PRINT( "    Initializing Component '" << compTypeTAG << "'");
			//cycle through the Component Param(s)' to be initialized
			tinyxml2::XMLElement* paramTAG;
			for (paramTAG = componentTAG->FirstChildElement("Param");
					paramTAG != NULL;
					paramTAG = paramTAG->NextSiblingElement("Param"))
			{
				const tinyxml2::XMLAttribute* attributeTAG =
						paramTAG->FirstAttribute();
				if (not attributeTAG)
				{
					continue;
				}
				PRINT(
						"      Param '" << attributeTAG->Name() << "' = '" << attributeTAG->Value() << "'");
				compTmplParams[compTypeTAG].insert(
						ParameterTable::value_type(attributeTAG->Name(),
								attributeTAG->Value()));
			}
		}
		//////////////////////////////////////////////////////////////
		//set parameters for the object
		PRINT( "    Initializing object '" <<
				(objIdTAG != NULL ? objIdTAG : "UNNAMED") << "' itself" );
		ParameterTable objTmplParams;
		//cycle through the Object Param(s)' to be initialized
		tinyxml2::XMLElement* objParamTAG;
		for (objParamTAG = objectTAG->FirstChildElement("Param");
				objParamTAG != NULL;
				objParamTAG = objParamTAG->NextSiblingElement("Param"))
		{
			const tinyxml2::XMLAttribute* attributeTAG =
					objParamTAG->FirstAttribute();
			if (not attributeTAG)
			{
				continue;
			}
			PRINT(
					"      Param '" << attributeTAG->Name() << "' = '" << attributeTAG->Value() << "'");
			objTmplParams.insert(
					ParameterTable::value_type(attributeTAG->Name(),
							attributeTAG->Value()));
		}
		//////////////////////////////////////////////////////////////
		//create the object actually
		SMARTPTR(Object)objectPtr;
		if ((objIdTAG != NULL) and (std::string(objIdTAG) != std::string("")))
		{
			// set id with the passed id
			objectPtr = ObjectTemplateManager::GetSingleton().createObject(
					ObjectType(objTypeTAG), ObjectId(objIdTAG),
					objTmplParams, compTmplParams);
		}
		else
		{
			// set id with the internally generated id
			objectPtr = ObjectTemplateManager::GetSingleton().createObject(
					ObjectType(objTypeTAG), ObjectId(""), objTmplParams,
					compTmplParams);
		}
		if (objectPtr == NULL)
		{
			continue;
		}
		createdObjectQueue.push(objectPtr);
		PRINT( "  ...Created Object '" << objectPtr->objectId() << "'");
		//remove top object from the priority queue
		orderedObjectsTAG.pop();
	}
	//give a chance to objects to initialize themselves,
	//in order of creation, after the game world has been created.
	while(not createdObjectQueue.empty())
	{
		//get front element
		SMARTPTR(Object) object = createdObjectQueue.front();
		//use front element
		object->worldSetup();
		//remove front element
		createdObjectQueue.pop();
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

PandaFramework* const GameManager::pandaFramework() const
{
	return dynamic_cast<PandaFramework* const >(const_cast<GameManager* const >(this));
}

WindowFramework* const GameManager::windowFramework() const
{
	return mWindow;
}

ReMutex& GameManager::getMutex()
{
	return mMutex;
}

#ifdef ELY_DEBUG
void GameManager::togglePhysicsDebug(const Event* event)
{
	if (mPhysicsDebugEnabled)
	{
		GamePhysicsManager::GetSingleton().debug(false);
	}
	else
	{
		GamePhysicsManager::GetSingleton().debug(true);
	}
	mPhysicsDebugEnabled = not mPhysicsDebugEnabled;
}
#endif

} // namespace ely
