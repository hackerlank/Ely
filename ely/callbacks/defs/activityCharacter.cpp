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
 * \file /Ely/ely/callbacks/activityCharacter.cpp
 *
 * \date 26/nov/2012 (10:29:22)
 * \author consultit
 */

#include "../common_configs.h"
#include "SceneComponents/Model.h"
#include "BehaviorComponents/Activity.h"
#include "ObjectModel/ObjectTemplateManager.h"

using namespace ely;

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///player0 + Activity related
CALLBACK activityPlayer0;
CALLBACK groundAirPlayer0;

#ifdef __cplusplus
}
#endif

//f:f-up:f-q
//b:b-up:b-q
//sr:sr-up:sr-q
//sl:sl-up:sl-q
//rr:rr-up:rr-q
//rl:rl-up:rl-q
//j:j-up:j-q
//q:q-up
//ground:air

///Character + Activity related functions/variables
//Transition table: <eventType, currentState> -> nextState
namespace
{
TransitionTable* tablePtr;
}
void activityPlayer0(const Event * event, void * data)
{
	//get data
	SMARTPTR(Activity)activity = reinterpret_cast<Activity*>(data);
	//get event type
	std::string eventType = activity->getEventType(event->get_name());
	//get fsm
	fsm& characterFSM = (fsm&) (*activity);
	//set transitions
	std::string currentState = characterFSM.getCurrentOrNextState();
	//
	std::string nextState =
	(*tablePtr)[StateEventType(currentState, eventType)];
	//
	if (not nextState.empty())
	{
		characterFSM.request(nextState);
	}
	else
	{
		PRINT_ERR_DEBUG("activityPlayer0: Transition not defined for event type '" <<
				eventType << "' and state '" << currentState << "'");
	}
}

void groundAirPlayer0(const Event * event, void * data)
{
	//get data
	SMARTPTR(Object)characterObj = ObjectTemplateManager::GetSingletonPtr()->
					getCreatedObject(event->get_parameter(1).get_string_value());
	SMARTPTR(Model) characterModel = DCAST(Model, characterObj->getComponent(
					ComponentFamilyType("Scene")));
	if (event->get_name() == "OnGround")
	{
		//stop animation
		characterModel->animations().stop("run");
	}
	else if (event->get_name() == "OnAir")
	{
		//play animation
		characterModel->animations().loop("run", true);
	}
	else
	{
		PRINT_ERR_DEBUG("groundAirPlayer0: unrecognized event");
	}
}

///Init/end functions: see common_configs.cpp
void activityCharacterInit()
{
	tablePtr = new TransitionTable();
	//insert transitions
	//state I
	TABLEINSERT(tablePtr, "I", "f", "F");
	TABLEINSERT(tablePtr, "I", "f-q", "F-Q");
	TABLEINSERT(tablePtr, "I", "b", "B");
	TABLEINSERT(tablePtr, "I", "sr", "Sr");
	TABLEINSERT(tablePtr, "I", "sr-q", "Sr-Q");
	TABLEINSERT(tablePtr, "I", "sl", "Sl");
	TABLEINSERT(tablePtr, "I", "sl-q", "Sl-Q");
	TABLEINSERT(tablePtr, "I", "rr", "Rr");
	TABLEINSERT(tablePtr, "I", "rr-q", "Rr-Q");
	TABLEINSERT(tablePtr, "I", "rl", "Rl");
	TABLEINSERT(tablePtr, "I", "rl-q", "Rl-Q");
	TABLEINSERT(tablePtr, "I", "j", "J");
	//state F
	TABLEINSERT(tablePtr, "F", "f-up", "I");
	TABLEINSERT(tablePtr, "F", "f-q", "F-Q");
	TABLEINSERT(tablePtr, "F", "sr", "F-Sr");
	TABLEINSERT(tablePtr, "F", "sl", "F-Sl");
	TABLEINSERT(tablePtr, "F", "rr", "F-Rr");
	TABLEINSERT(tablePtr, "F", "rr-q", "F-Rr-Q");
	TABLEINSERT(tablePtr, "F", "rl", "F-Rl");
	TABLEINSERT(tablePtr, "F", "rl-q", "F-Rl-Q");
	TABLEINSERT(tablePtr, "F", "j", "F-J");
	TABLEINSERT(tablePtr, "F", "j-q", "F-J-Q");
	TABLEINSERT(tablePtr, "F", "q", "F-Q");
	//state B
	TABLEINSERT(tablePtr, "B", "b-up", "I");
	TABLEINSERT(tablePtr, "B", "sr", "B-Sr");
	TABLEINSERT(tablePtr, "B", "sl", "B-Sl");
	TABLEINSERT(tablePtr, "B", "rr", "B-Rr");
	TABLEINSERT(tablePtr, "B", "rl", "B-Rl");
	//Sr
	TABLEINSERT(tablePtr, "Sr", "sr-up", "I");
	TABLEINSERT(tablePtr, "Sr", "sr-q", "Sr-Q");
	TABLEINSERT(tablePtr, "Sr", "rr", "Sr-Rr");
	TABLEINSERT(tablePtr, "Sr", "rr-q", "Sr-Rr-Q");
	TABLEINSERT(tablePtr, "Sr", "rl", "Sr-Rl");
	TABLEINSERT(tablePtr, "Sr", "rl-q", "Sr-Rl-Q");
	TABLEINSERT(tablePtr, "Sr", "q", "Sr-Q");
	//Sl
	TABLEINSERT(tablePtr, "Sl", "sl-up", "I");
	TABLEINSERT(tablePtr, "Sl", "sl-q", "Sl-Q");
	TABLEINSERT(tablePtr, "Sl", "rr", "Sl-Rr");
	TABLEINSERT(tablePtr, "Sl", "rr-q", "Sl-Rr-Q");
	TABLEINSERT(tablePtr, "Sl", "rl", "Sl-Rl");
	TABLEINSERT(tablePtr, "Sl", "rl-q", "Sl-Rl-Q");
	TABLEINSERT(tablePtr, "Sl", "q", "Sl-Q");
	//Rr
	TABLEINSERT(tablePtr, "Rr", "f", "F-Rr");
	TABLEINSERT(tablePtr, "Rr", "f-q", "F-Rr-Q");
	TABLEINSERT(tablePtr, "Rr", "b", "B-Rr");
	TABLEINSERT(tablePtr, "Rr", "sr", "Sr-Rr");
	TABLEINSERT(tablePtr, "Rr", "sr-q", "Sr-Rr-Q");
	TABLEINSERT(tablePtr, "Rr", "sl", "Sl-Rr");
	TABLEINSERT(tablePtr, "Rr", "sl-q", "Sr-Rl-Q");
	TABLEINSERT(tablePtr, "Rr", "rr-up", "I");
	TABLEINSERT(tablePtr, "Rr", "rr-q", "Rr-Q");
	TABLEINSERT(tablePtr, "Rr", "q", "Rr-Q");
	//Rl
	TABLEINSERT(tablePtr, "Rl", "f", "F-Rl");
	TABLEINSERT(tablePtr, "Rl", "f-q", "F-Rl-Q");
	TABLEINSERT(tablePtr, "Rl", "b", "B-Rl");
	TABLEINSERT(tablePtr, "Rl", "sr", "Sr-Rl");
	TABLEINSERT(tablePtr, "Rl", "sr-q", "Sr-Rl-Q");
	TABLEINSERT(tablePtr, "Rl", "sl", "Sl-Rl");
	TABLEINSERT(tablePtr, "Rl", "sl-q", "Sl-Rl-Q");
	TABLEINSERT(tablePtr, "Rl", "rl-up", "I");
	TABLEINSERT(tablePtr, "Rl", "rl-q", "Rl-Q");
	TABLEINSERT(tablePtr, "Rl", "q", "Rl-Q");
	//J
	TABLEINSERT(tablePtr, "J", "f", "F-J");
	TABLEINSERT(tablePtr, "J", "f-q", "F-J-Q");
	TABLEINSERT(tablePtr, "J", "j-up", "I");
	//F-Rr
	TABLEINSERT(tablePtr, "F-Rr", "f-up", "Rr");
	TABLEINSERT(tablePtr, "F-Rr", "f-q", "F-Rr-Q");
	TABLEINSERT(tablePtr, "F-Rr", "rr-up", "F");
	TABLEINSERT(tablePtr, "F-Rr", "rr-q", "F-Rr-Q");
	TABLEINSERT(tablePtr, "F-Rr", "rl", "F-Rr-Rl");
	TABLEINSERT(tablePtr, "F-Rr", "rl-q", "F-Rr-Rl-Q");
	TABLEINSERT(tablePtr, "F-Rr", "q", "F-Rr-Q");
	//F-Rl
	TABLEINSERT(tablePtr, "F-Rl", "f-up", "Rl");
	TABLEINSERT(tablePtr, "F-Rl", "f-q", "F-Rl-Q");
	TABLEINSERT(tablePtr, "F-Rl", "rr", "F-Rr-Rl");
	TABLEINSERT(tablePtr, "F-Rl", "rr-q", "F-Rr-Rl-Q");
	TABLEINSERT(tablePtr, "F-Rl", "rl-up", "F");
	TABLEINSERT(tablePtr, "F-Rl", "rl-q", "F-Rl-Q");
	TABLEINSERT(tablePtr, "F-Rl", "q", "F-Rl-Q");
	//F-Rr-Rl
	TABLEINSERT(tablePtr, "F-Rr-Rl", "f-up", "Rr-Rl");
	TABLEINSERT(tablePtr, "F-Rr-Rl", "f-q", "F-Rr-Rl-Q");
	TABLEINSERT(tablePtr, "F-Rr-Rl", "rr-up", "F-Rl");
	TABLEINSERT(tablePtr, "F-Rr-Rl", "rr-q", "F-Rr-Rl-Q");
	TABLEINSERT(tablePtr, "F-Rr-Rl", "rl-up", "F-Rr");
	TABLEINSERT(tablePtr, "F-Rr-Rl", "rl-q", "F-Rr-Rl-Q");
	TABLEINSERT(tablePtr, "F-Rr-Rl", "q", "F-Rr-Rl-Q");
	//Rr-Rl
	TABLEINSERT(tablePtr, "Rr-Rl", "f", "F-Rr-Rl");
	TABLEINSERT(tablePtr, "Rr-Rl", "f-q", "F-Rr-Rl-Q");
	TABLEINSERT(tablePtr, "Rr-Rl", "rr-up", "Rl");
	TABLEINSERT(tablePtr, "Rr-Rl", "rr-q", "Rr-Rl-Q");
	TABLEINSERT(tablePtr, "Rr-Rl", "rl-up", "Rr");
	TABLEINSERT(tablePtr, "Rr-Rl", "rl-q", "Rr-Rl-Q");
	TABLEINSERT(tablePtr, "Rr-Rl", "q", "Rr-Rl-Q");
	//F-J
	TABLEINSERT(tablePtr, "F-J", "f-up", "J");
	TABLEINSERT(tablePtr, "F-J", "f-q", "F-J-Q");
	TABLEINSERT(tablePtr, "F-J", "j-up", "F");
	TABLEINSERT(tablePtr, "F-J", "j-q", "F-J-Q");
	TABLEINSERT(tablePtr, "F-J", "q", "F-J-Q");
	//B-Rr
	TABLEINSERT(tablePtr, "B-Rr", "b-up", "Rr");
	TABLEINSERT(tablePtr, "B-Rr", "rr-up", "B");
	//B-Rl
	TABLEINSERT(tablePtr, "B-Rl", "b-up", "Rl");
	TABLEINSERT(tablePtr, "B-Rl", "rl-up", "B");
	//Sr-Rr
	TABLEINSERT(tablePtr, "Sr-Rr", "sr-up", "Rr");
	TABLEINSERT(tablePtr, "Sr-Rr", "sr-q", "Sr-Rr-Q");
	TABLEINSERT(tablePtr, "Sr-Rr", "rr-up", "Sr");
	TABLEINSERT(tablePtr, "Sr-Rr", "rr-q", "Sr-Rr-Q");
	TABLEINSERT(tablePtr, "Sr-Rr", "q", "Sr-Rr-Q");
	//Sr-Rl
	TABLEINSERT(tablePtr, "Sr-Rl", "sr-up", "Rl");
	TABLEINSERT(tablePtr, "Sr-Rl", "sr-q", "Sr-Rl-Q");
	TABLEINSERT(tablePtr, "Sr-Rl", "rl-up", "Sr");
	TABLEINSERT(tablePtr, "Sr-Rl", "rl-q", "Sr-Rl-Q");
	TABLEINSERT(tablePtr, "Sr-Rl", "q", "Sr-Rl-Q");
	//Sl-Rr
	TABLEINSERT(tablePtr, "Sl-Rr", "sl-up", "Rr");
	TABLEINSERT(tablePtr, "Sl-Rr", "sl-q", "Sl-Rr-Q");
	TABLEINSERT(tablePtr, "Sl-Rr", "rr-up", "Sl");
	TABLEINSERT(tablePtr, "Sl-Rr", "rr-q", "Sl-Rr-Q");
	TABLEINSERT(tablePtr, "Sl-Rr", "q", "Sl-Rr-Q");
	//Sl-Rl
	TABLEINSERT(tablePtr, "Sl-Rl", "sl-up", "Rl");
	TABLEINSERT(tablePtr, "Sl-Rl", "sl-q", "Sl-Rl-Q");
	TABLEINSERT(tablePtr, "Sl-Rl", "rl-up", "Sl");
	TABLEINSERT(tablePtr, "Sl-Rl", "rl-q", "Sl-Rl-Q");
	TABLEINSERT(tablePtr, "Sl-Rl", "q", "Sl-Rl-Q");
	//F-Q
	TABLEINSERT(tablePtr, "F-Q", "f-up", "I");
	TABLEINSERT(tablePtr, "F-Q", "rr", "F-Rr-Q");
	TABLEINSERT(tablePtr, "F-Q", "rr-q", "F-Rr-Q");
	TABLEINSERT(tablePtr, "F-Q", "rl", "F-Rl-Q");
	TABLEINSERT(tablePtr, "F-Q", "rl-q", "F-Rl-Q");
	TABLEINSERT(tablePtr, "F-Q", "j", "F-J-Q");
	TABLEINSERT(tablePtr, "F-Q", "q-up", "F");
	//Sr-Q
	TABLEINSERT(tablePtr, "Sr-Q", "sr-up", "I");
	TABLEINSERT(tablePtr, "Sr-Q", "rr", "Sr-Rr-Q");
	TABLEINSERT(tablePtr, "Sr-Q", "rr-q", "Sr-Rr-Q");
	TABLEINSERT(tablePtr, "Sr-Q", "rl", "Sr-Rl-Q");
	TABLEINSERT(tablePtr, "Sr-Q", "rl-q", "Sr-Rl-Q");
	TABLEINSERT(tablePtr, "Sr-Q", "q-up", "Sr");
	//Sl-Q
	TABLEINSERT(tablePtr, "Sl-Q", "sl-up", "I");
	TABLEINSERT(tablePtr, "Sl-Q", "rr", "Sl-Rr-Q");
	TABLEINSERT(tablePtr, "Sl-Q", "rr-q", "Sl-Rr-Q");
	TABLEINSERT(tablePtr, "Sl-Q", "rl", "Sl-Rl-Q");
	TABLEINSERT(tablePtr, "Sl-Q", "rl-q", "Sl-Rl-Q");
	TABLEINSERT(tablePtr, "Sl-Q", "q-up", "Sl");
	//Rr-Q
	TABLEINSERT(tablePtr, "Rr-Q", "f", "F-Rr-Q");
	TABLEINSERT(tablePtr, "Rr-Q", "f-q", "F-Rr-Q");
	TABLEINSERT(tablePtr, "Rr-Q", "sr", "Sr-Rr-Q");
	TABLEINSERT(tablePtr, "Rr-Q", "sr-q", "Sr-Rr-Q");
	TABLEINSERT(tablePtr, "Rr-Q", "sl", "Sl-Rr-Q");
	TABLEINSERT(tablePtr, "Rr-Q", "sl-q", "Sl-Rr-Q");
	TABLEINSERT(tablePtr, "Rr-Q", "rr-up", "I");
	TABLEINSERT(tablePtr, "Rr-Q", "q-up", "Rr");
	//Rl-Q
	TABLEINSERT(tablePtr, "Rl-Q", "f", "F-Rl-Q");
	TABLEINSERT(tablePtr, "Rl-Q", "f-q", "F-Rl-Q");
	TABLEINSERT(tablePtr, "Rl-Q", "sr", "Sr-Rl-Q");
	TABLEINSERT(tablePtr, "Rl-Q", "sr-q", "Sr-Rl-Q");
	TABLEINSERT(tablePtr, "Rl-Q", "sl", "Sl-Rl-Q");
	TABLEINSERT(tablePtr, "Rl-Q", "sl-q", "Sl-Rl-Q");
	TABLEINSERT(tablePtr, "Rl-Q", "rl-up", "I");
	TABLEINSERT(tablePtr, "Rl-Q", "q-up", "Rl");
	//F-Rr-Q
	TABLEINSERT(tablePtr, "F-Rr-Q", "f-up", "Rr-Q");
	TABLEINSERT(tablePtr, "F-Rr-Q", "rr-up", "F-Q");
	TABLEINSERT(tablePtr, "F-Rr-Q", "rl", "F-Rr-Rl");
	TABLEINSERT(tablePtr, "F-Rr-Q", "rl-q", "F-Rr-Rl-Q");
	TABLEINSERT(tablePtr, "F-Rr-Q", "q-up", "F-Rr");
	//F-Rl-Q
	TABLEINSERT(tablePtr, "F-Rl-Q", "f-up", "Rl-Q");
	TABLEINSERT(tablePtr, "F-Rl-Q", "rr", "F-Rr-Rl");
	TABLEINSERT(tablePtr, "F-Rl-Q", "rr-q", "F-Rr-Rl-Q");
	TABLEINSERT(tablePtr, "F-Rl-Q", "rl-up", "F-Q");
	TABLEINSERT(tablePtr, "F-Rl-Q", "q-up", "F-Rl");
	//F-Rr-Rl-Q
	TABLEINSERT(tablePtr, "F-Rr-Rl-Q", "f-up", "Rr-Rl-Q");
	TABLEINSERT(tablePtr, "F-Rr-Rl-Q", "rr-up", "F-Rl-Q");
	TABLEINSERT(tablePtr, "F-Rr-Rl-Q", "rl-up", "F-Rr-Q");
	TABLEINSERT(tablePtr, "F-Rr-Rl-Q", "q-up", "F-Rr-Rl");
	//Rr-Rl-Q
	TABLEINSERT(tablePtr, "Rr-Rl-Q", "f", "F-Rr-Rl-Q");
	TABLEINSERT(tablePtr, "Rr-Rl-Q", "f-q", "F-Rr-Rl-Q");
	TABLEINSERT(tablePtr, "Rr-Rl-Q", "rr-up", "Rl-Q");
	TABLEINSERT(tablePtr, "Rr-Rl-Q", "rl-up", "Rr-Q");
	TABLEINSERT(tablePtr, "Rr-Rl-Q", "q-up", "Rr-Rl");
	//F-J-Q
	TABLEINSERT(tablePtr, "F-J-Q", "f-up", "J");
	TABLEINSERT(tablePtr, "F-J-Q", "j-up", "F-Q");
	TABLEINSERT(tablePtr, "F-J-Q", "q-up", "F-J");
	//Sr-Rr-Q
	TABLEINSERT(tablePtr, "Sr-Rr-Q", "sr-up", "Rr-Q");
	TABLEINSERT(tablePtr, "Sr-Rr-Q", "rr-up", "Sr-Q");
	TABLEINSERT(tablePtr, "Sr-Rr-Q", "q-up", "Sr-Rr");
	//Sr-Rl-Q
	TABLEINSERT(tablePtr, "Sr-Rl-Q", "sr-up", "Rl-Q");
	TABLEINSERT(tablePtr, "Sr-Rl-Q", "rl-up", "Sr-Q");
	TABLEINSERT(tablePtr, "Sr-Rl-Q", "q-up", "Sr-Rl");
	//Sl-Rr-Q
	TABLEINSERT(tablePtr, "Sl-Rr-Q", "sl-up", "Rr-Q");
	TABLEINSERT(tablePtr, "Sl-Rr-Q", "rr-up", "Sl-Q");
	TABLEINSERT(tablePtr, "Sl-Rr-Q", "q-up", "Sl-Rr");
	//Sl-Rl-Q
	TABLEINSERT(tablePtr, "Sl-Rl-Q", "sl-up", "Rl-Q");
	TABLEINSERT(tablePtr, "Sl-Rl-Q", "rl-up", "Sl-Q");
	TABLEINSERT(tablePtr, "Sl-Rl-Q", "q-up", "Sr-Rl");
}
void activityCharacterEnd()
{
	delete tablePtr;
}

