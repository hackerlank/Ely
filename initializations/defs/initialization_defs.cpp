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
 * \file /Ely/initializations/callback_defs.cpp
 *
 * \date 05/set/2012 (20:07:54)
 * \author consultit
 */

#include "../common_configs.h"
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <vector>
#include <nodePath.h>
#include <aiCharacter.h>
#include <aiBehaviors.h>
#include <flock.h>
#include <eventHandler.h>
#include <texturePool.h>
#include "ControlComponents/Driver.h"
#include "ControlComponents/Chaser.h"
#include "AudioComponents/Sound3d.h"
#include "SceneComponents/Model.h"
#include "AIComponents/Steering.h"
#include "AIComponents/NavMesh.h"
#include "BehaviorComponents/Activity.h"
#include "Utilities/Tools.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "ObjectModel/ObjectTemplate.h"
#include "Game/GameAIManager.h"
#include "Support/Picker.h"

///shared between camera, picker, actor1
static bool controlGrabbed = false;

///camera related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION camera_initialization;

#ifdef __cplusplus
}
#endif

static void toggleCameraControl(const Event* event, void* data)
{
	SMARTPTR(Object)camera = reinterpret_cast<Object*>(data);
	SMARTPTR(Component) compControl = camera->getComponent(
			ComponentFamilyType("Control"));
	if (compControl->is_of_type(Driver::get_class_type()))
	{
		SMARTPTR(Driver)cameraControl = DCAST(Driver, camera->getComponent(
						ComponentFamilyType("Control")));
		if (cameraControl->isEnabled())
		{
			//enabled: then disable it
			//disable
			cameraControl->disable();

			///<DEFAULT CAMERA CONTROL>
//			//reset trackball transform
//			LMatrix4 cameraMat = mWindow->get_camera_group().get_transform()->get_mat();
//			cameraMat.invert_in_place();
//			SMARTPTR(Trackball)trackBall =
//			DCAST(Trackball, gameManager->mTrackBall.node());
//			trackBall->set_mat(cameraMat);
//			//(re)enable trackball
//			gameManager->enable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//
			controlGrabbed = false;
		}
		else if (not controlGrabbed)
		{
			//disabled: then enable it

			///<DEFAULT CAMERA CONTROL>
//			//disable the trackball
//			gameManager->disable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//enable
			cameraControl->enable();
			//
			controlGrabbed = true;
		}

	}
	else if (compControl->is_of_type(Chaser::get_class_type()))
	{
		SMARTPTR(Chaser)cameraControl = DCAST(Chaser, camera->getComponent(
						ComponentFamilyType("Control")));

		if (cameraControl->isEnabled())
		{
			//if enabled then disable it
			//disable
			cameraControl->disable();

			///<DEFAULT CAMERA CONTROL>
//			//reset trackball transform
//			LMatrix4 cameraMat = mWindow->get_camera_group().get_transform()->get_mat();
//			cameraMat.invert_in_place();
//			SMARTPTR(Trackball)trackBall =
//			DCAST(Trackball, gameManager->mTrackBall.node());
//			trackBall->set_mat(cameraMat);
//			//(re)enable trackball
//			gameManager->enable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//
			controlGrabbed = false;
		}
		else if (not controlGrabbed)
		{
			//if disabled then enable it

			///<DEFAULT CAMERA CONTROL>
//			//disable the trackball
//			gameManager->disable_mouse();
			///</DEFAULT CAMERA CONTROL>

			//enable
			cameraControl->enable();
			//
			controlGrabbed = true;
		}
	}
}
static void togglePicker(const Event* event, void* data)
{
	SMARTPTR(Object)camera = reinterpret_cast<Object*>(data);
	if (Picker::GetSingletonPtr())
	{
		//picker on: remove
		delete Picker::GetSingletonPtr();
		//
		controlGrabbed = false;
	}
	else if (not controlGrabbed)
	{
		//picker off: add
		new Picker(camera->objectTmpl()->pandaFramework(),
				camera->objectTmpl()->windowFramework(),
				"shift-mouse1", "mouse1-up");
		//
		controlGrabbed = true;
	}
}
void camera_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//camera
	//enable/disable camera control by event
	pandaFramework->define_key("c", "toggleCameraControl", &toggleCameraControl,
			static_cast<void*>(object));
	//enable/disable a picker
	pandaFramework->define_key("x", "togglePicker", &togglePicker,
			static_cast<void*>(object));

}

void cameraInit()
{
}

void cameraEnd()
{
}

///Actor1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Actor1_initialization;

#ifdef __cplusplus
}
#endif

static void toggleActor1Control(const Event* event, void* data)
{
	SMARTPTR(Object)actor1 = reinterpret_cast<Object*>(data);
	SMARTPTR(Driver)actor1Control = DCAST(Driver, actor1->getComponent(
					ComponentFamilyType("Control")));

	if (actor1Control->isEnabled())
	{
		//if enabled then disable it
		//disable
		actor1Control->disable();

		///<DEFAULT CAMERA CONTROL>
//		//reset trackball transform
//		LMatrix4 cameraMat = mWindow->get_camera_group().get_transform()->get_mat();
//		cameraMat.invert_in_place();
//		SMARTPTR(Trackball)trackBall =
//		DCAST(Trackball, gameManager->mTrackBall.node());
//		trackBall->set_mat(cameraMat);
//		//(re)enable trackball
//		gameManager->enable_mouse();
		///</DEFAULT CAMERA CONTROL>

		//
		controlGrabbed = false;
	}
	else if (not controlGrabbed)
	{
		//if disabled then enable it

		///<DEFAULT CAMERA CONTROL>
//		//disable the trackball
//		gameManager->disable_mouse();
		///</DEFAULT CAMERA CONTROL>

		//enable
		actor1Control->enable();
		//
		controlGrabbed = true;
	}
}
void Actor1_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//Actor1
	//play animation
//	SMARTPTR(Model) actor1Model = DCAST(Model, object->getComponent(
//					ComponentFamilyType("Scene")));
//	actor1Model->animations().loop("walk", false);
	//play sound
	SMARTPTR(Sound3d) actor1Sound3d = DCAST(Sound3d, object->getComponent(
					ComponentFamilyType("Audio")));
	actor1Sound3d->getSound("walk-sound")->set_loop(true);
	actor1Sound3d->getSound("walk-sound")->play();

	//enable/disable Actor1 control by event
	pandaFramework->define_key("v", "enableActor1Control", &toggleActor1Control,
			static_cast<void*>(object));
}

void Actor1Init()
{
}
void Actor1End()
{
}

///Plane1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Plane1_initialization;

#ifdef __cplusplus
}
#endif

///Plane1 related
void Plane1_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//Plane1
	SMARTPTR(Model) plane1Model = DCAST(Model, object->getComponent(
					ComponentFamilyType("Scene")));
	SMARTPTR(TextureStage) planeTS0 = new TextureStage("planeTS0");
	SMARTPTR(Texture) planeTex = TexturePool::load_texture("maps/envir-ground.jpg");
	plane1Model->getNodePath().set_texture(planeTS0, planeTex, 1);
	plane1Model->getNodePath().set_tex_scale(planeTS0, 100, 100);
}

void Plane1Init()
{
}
void Plane1End()
{
}

///Terrain1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Terrain1_initialization;

#ifdef __cplusplus
}
#endif

///Terrain1 related
void Terrain1_initialization(SMARTPTR(Object)object, const ParameterTable&paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//Terrain1
//	object->getNodePath().set_render_mode_wireframe(1);
}

void Terrain1Init()
{
}
void Terrain1End()
{
}

///NPC1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION NPC1_initialization;

#ifdef __cplusplus
}
#endif

void NPC1_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//NPC1
	fsm& npc1FSM = (fsm&) (*DCAST(Activity, object->getComponent(
							ComponentFamilyType("Behavior"))));
	npc1FSM.request("I");
	//play sound
	SMARTPTR(Sound3d) npc1Sound3d = DCAST(Sound3d, object->getComponent(
					ComponentFamilyType("Audio")));
	npc1Sound3d->getSound("walk-sound")->set_loop(true);
	npc1Sound3d->getSound("walk-sound")->play();
}

void NPC1Init()
{
}
void NPC1End()
{
}

///Seeker1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Steerer1_initialization;

#ifdef __cplusplus
}
#endif

static bool aiEnabled = false;
static std::vector<SMARTPTR(Object)> flockObjects;
static Flock * flockPtr;
static const unsigned int flockId = 1;
static void toggleSteerer1Control(const Event* event, void* data)
{
	SMARTPTR(Object)steerer1 = reinterpret_cast<Object*>(data);
	SMARTPTR(Steering)steerer1AI = DCAST(Steering, steerer1->getComponent(
					ComponentFamilyType("AI")));
	if (steerer1AI->isEnabled())
	{
		//enabled: then disable it
		steerer1AI->disable();
		//un-flock
//		for (unsigned int i = 0; i < flockObjects.size(); ++i)
//		{
//			SMARTPTR(Steering)steerer = DCAST(Steering,
//					flockObjects[i]->getComponent(ComponentFamilyType("AI")));
//			steerer->disable();
//		}
//		GameAIManager::GetSingletonPtr()->aiWorld()->flock_off(flockId);
//		GameAIManager::GetSingletonPtr()->aiWorld()->remove_flock(flockId);
//		delete flockPtr;
		//
		aiEnabled = false;
	}
	else if (not aiEnabled)
	{
		//disabled: then enable it
		steerer1AI->enable();
		//enable behaviors
		NodePath targetObjectNP = steerer1AI->getTargetNodePath(ObjectId("NPC1"));
		//seek NPC1
//		steerer1AI->getAiCharacter()->get_ai_behaviors()->seek(targetObjectNP);
		//flee NPC1
//		steerer1AI->getAiCharacter()->get_ai_behaviors()->flee(targetObjectNP, 50.0, 200.0);
		//pursue NPC1
		steerer1AI->getAiCharacter()->get_ai_behaviors()->pursue(targetObjectNP);
		//evade NPC1
//		steerer1AI->getAiCharacter()->get_ai_behaviors()->evade(targetObjectNP, 50.0, 150.0);
		//arrival
//		steerer1AI->getAiCharacter()->get_ai_behaviors()->arrival(100.0);
		//flock
//		flockPtr = new Flock(flockId, 270, 10, 2, 4, 0.2);
//		GameAIManager::GetSingletonPtr()->aiWorld()->add_flock(flockPtr);
//		GameAIManager::GetSingletonPtr()->aiWorld()->flock_on(flockId);
//		for (unsigned int i = 0; i < flockObjects.size(); ++i)
//		{
//			SMARTPTR(Steering)steerer = DCAST(Steering,
//					flockObjects[i]->getComponent(ComponentFamilyType("AI")));
//			steerer->enable();
//			flockPtr->add_ai_char(steerer->getAiCharacter());
//			steerer->getAiCharacter()->get_ai_behaviors()->flock(0.5);
//			steerer->getAiCharacter()->get_ai_behaviors()->pursue(targetObjectNP, 0.5);
//		}
		//wander NPC1
//		steerer1AI->getAiCharacter()->get_ai_behaviors()->wander(10.0, 0, 100.0, 1.0);
		//obstacle avoidance
		steerer1AI->getAiCharacter()->get_ai_behaviors()->obstacle_avoidance(1.0);
		//
		aiEnabled = true;
	}
}

static void steerer1SteeringForceOn(const Event* event, void* data)
{
	std::string throwerObject = event->get_parameter(1).get_string_value();
	if (throwerObject == "Steerer1")
	{
		SMARTPTR(Object)gorilla1 =
		ObjectTemplateManager::GetSingletonPtr()->getCreatedObject("Gorilla1");
		SMARTPTR(Model) gorilla1Model = DCAST(Model, gorilla1->getComponent(
						ComponentFamilyType("Scene")));
		//play animation
		gorilla1Model->animations().loop("walk", false);
	}
}
static void steerer1SteeringForceOff(const Event* event, void* data)
{
	std::string throwerObject = event->get_parameter(1).get_string_value();
	if (throwerObject == "Steerer1")
	{
		SMARTPTR(Object)gorilla1 =
		ObjectTemplateManager::GetSingletonPtr()->getCreatedObject("Gorilla1");
		SMARTPTR(Model) gorilla1Model = DCAST(Model, gorilla1->getComponent(
						ComponentFamilyType("Scene")));
		//stop animation
		gorilla1Model->animations().stop("walk");
	}
}

static void createClones(SMARTPTR(Object) cloned,
		std::vector<SMARTPTR(Object)>& clones,
		int numX, int numY, float deltaPos)
{
	//add cloned as first flocker
	clones.clear();
	clones.push_back(cloned);
	//clone itself a few times (with id = clonedId_cloneX)
	//get the object parameter table
	ParameterTable steerer1ObjParams =
	cloned->getStoredObjTmplParams();
	steerer1ObjParams.erase("store_params");
	steerer1ObjParams.insert(std::pair<std::string,
			std::string>("store_params", "false"));
	//get the components' parameter tables
	ParameterTableMap steerer1CompParams =
	cloned->getStoredCompTmplParams();
	//object already added to scene
	float pox_x = cloned->getNodePath().get_x();
	float pox_y = cloned->getNodePath().get_y();
	/* initialize random seed: */
	srand(time(NULL));
	for (int x = 0; x < numX; ++x)
	{
		for (int y = 0; y < numY; ++y)
		{
			std::ostringstream pos_xStr, pos_yStr, idx;
			//change x,y position
			steerer1ObjParams.erase("pos_x");
			steerer1ObjParams.erase("pos_y");
			pos_xStr << pox_x + ((float(rand())/float(RAND_MAX) - 0.5) +
					float(x + 1)) * deltaPos;
			pos_yStr << pox_y + ((float(rand())/float(RAND_MAX) - 0.5) +
					float(y + 1)) * deltaPos;
			steerer1ObjParams.insert(std::pair<std::string,
					std::string>("pos_x", pos_xStr.str()));
			steerer1ObjParams.insert(std::pair<std::string,
					std::string>("pos_y", pos_yStr.str()));
			//create the clone
			idx << x << "_" << y;
			std::string id = std::string(cloned->objectId()) + "_clone" + idx.str();
			clones.push_back(ObjectTemplateManager::GetSingletonPtr()->
					createObject(cloned->objectTmpl()->name(), ObjectId(id),
							true, steerer1ObjParams, steerer1CompParams, false));
		}
	}
}

void Steerer1_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	SMARTPTR(Steering)steerer1AI = DCAST(Steering, object->getComponent(
					ComponentFamilyType("AI")));

	//Steerer1
	//enable/disable Steerer1 control by event
	pandaFramework->define_key("b", "enableSteerer1Control",
			&toggleSteerer1Control, static_cast<void*>(object));
	//respond to Steerer1 events
	EventHandler::get_global_event_handler()->add_hook("SteeringForceOn",
			&steerer1SteeringForceOn, static_cast<void*>(object));
	EventHandler::get_global_event_handler()->add_hook("SteeringForceOff",
			&steerer1SteeringForceOff, static_cast<void*>(object));
	///
	//flock management
//	createClones(object, flockObjects, 3, 3, 20.0);
}

void Steerer1Init()
{
}
void Steerer1End()
{
}

///course2 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION course2_initialization;

#ifdef __cplusplus
}
#endif

#ifdef ELY_DEBUG
///debug flag
static bool debugOn = false;

static void toggleDebugNavMesh(const Event* event, void* data)
{
	SMARTPTR(Object)course2 = reinterpret_cast<Object*>(data);
	SMARTPTR(Component) compAI = course2->getComponent(
			ComponentFamilyType("AI"));
	if (compAI->is_of_type(NavMesh::get_class_type()))
	{
		SMARTPTR(NavMesh)course2NavMesh = DCAST(NavMesh, course2->getComponent(
						ComponentFamilyType("AI")));
		//toggle debug
		course2NavMesh->debug(not debugOn);
		//toggle flag
		debugOn = not debugOn;
	}
}
#endif

void course2_initialization(SMARTPTR(Object)object, const ParameterTable& paramTable,
PandaFramework* pandaFramework, WindowFramework* windowFramework)
{
	//course2
#ifdef ELY_DEBUG
	//enable/disable navigation mesh debugging by event
	pandaFramework->define_key("m", "toggleDebugNavMesh", &toggleDebugNavMesh,
			static_cast<void*>(object));
#endif
}

void course2Init()
{
}

void course2End()
{
}
