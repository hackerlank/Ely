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
 * \file /Ely/elygame/callbacks/defs/Character_clbk.cpp
 *
 * \date 26/nov/2012 (10:29:22)
 * \author consultit
 */

#include "../common_configs.h"
#include "SceneComponents/Model.h"
#include "BehaviorComponents/Activity.h"
#include "PhysicsControlComponents/CharacterController.h"
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
void activityPlayer0(const Event * event, void * data)
{
	//get data
	SMARTPTR(Activity)activity = reinterpret_cast<Activity*>(data);
	//get fsm and transition table through type conversions
	//get current_state,event_type@next_state
	std::string eventType = activity->getEventType(event->get_name());
	std::string currentState = static_cast<fsm&>(*activity).getCurrentOrNextState();
	std::string nextState = static_cast<Activity::TransitionTable&>(*activity)
			[Activity::StateEventType(currentState, eventType)];
	//make transition
	if (not nextState.empty())
	{
		static_cast<fsm&>(*activity).request(nextState);
	}
	else
	{
		PRINT_ERR_DEBUG("activityPlayer0: Transition not defined for event-type '" <<
				eventType << "' and state '" << currentState << "'");
	}
}

void groundAirPlayer0(const Event * event, void * data)
{
	//get data
	SMARTPTR(Object) characterObj = DCAST(CharacterController,
			event->get_parameter(0).get_ptr())->getOwnerObject();
	SMARTPTR(Model) characterModel = DCAST(Model,
			characterObj->getComponent(ComponentFamilyType("Scene")));
	if (event->get_name() == "OnGround")
	{
		//stop animation
		characterModel->animations().stop("run");
	}
	else if (event->get_name() == "InAir")
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
void Character_clbkInit()
{
}
void Character_clbkEnd()
{
}

