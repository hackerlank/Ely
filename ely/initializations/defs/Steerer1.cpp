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
 * \file /Ely/ely/initializations/defs/Steerer1.cpp
 *
 * \date 14/lug/2013 (08:48:24)
 * \author consultit
 */

#include "../common_configs.h"
#include "SceneComponents/Model.h"
#include "AIComponents/Steering.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAIManager.h"

///Steerer1 related
#ifdef __cplusplus
extern "C"
{
#endif

INITIALIZATION Steerer1_initialization;

#ifdef __cplusplus
}
#endif

namespace
{
bool aiEnabled = false;
std::vector<SMARTPTR(Object)> flockObjects;
Flock * flockPtr;
const unsigned int flockId = 1;
void toggleSteerer1Control(const Event* event, void* data)
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
		SMARTPTR(Object )targetObject =
		ObjectTemplateManager::GetSingletonPtr()->getCreatedObject(ObjectId("player0"));
		NodePath targetObjectNP = targetObject->getNodePath();
		//seek Player1
//		steerer1AI->getAiCharacter()->get_ai_behaviors()->seek(targetObjectNP);
		//flee Player1
//		steerer1AI->getAiCharacter()->get_ai_behaviors()->flee(targetObjectNP, 50.0, 200.0);
		//pursue Player1
		steerer1AI->getAiCharacter()->get_ai_behaviors()->pursue(targetObjectNP);
		//evade Player1
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
		//wander Player1
//		steerer1AI->getAiCharacter()->get_ai_behaviors()->wander(10.0, 0, 100.0, 1.0);
		//obstacle avoidance
		steerer1AI->getAiCharacter()->get_ai_behaviors()->obstacle_avoidance(1.0);
		//
		aiEnabled = true;
	}
}

void steerer1SteeringForceOn(const Event* event, void* data)
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
void steerer1SteeringForceOff(const Event* event, void* data)
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

void createClones(SMARTPTR(Object)cloned, std::vector<SMARTPTR(Object)>& clones,
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
			createObject(cloned->objectTmpl()->objectType(), ObjectId(id),
					steerer1ObjParams, steerer1CompParams, false));
		}
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

