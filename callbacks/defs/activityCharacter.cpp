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
 * \file /Ely/callbacks/activityCharacter.cpp
 *
 * \date 26/nov/2012 (10:29:22)
 * \author marco
 */

#include "../common_configs.h"
#include "Utilities/ComponentSuite.h"

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///Character + Activity related
CALLBACK activityCharacter;

#ifdef __cplusplus
}
#endif

///Character + Activity related CALLBACKNAMEs
//f:f-up:f-q
CALLBACKNAME f_Activity_Character = "activityCharacter";
CALLBACKNAME f_up_Activity_Character = "activityCharacter";
CALLBACKNAME f_q_Activity_Character = "activityCharacter";
//b:b-up:b-q
CALLBACKNAME b_Activity_Character = "activityCharacter";
CALLBACKNAME b_up_Activity_Character = "activityCharacter";
CALLBACKNAME b_q_Activity_Character = "activityCharacter";
//sr:sr-up:sr-q
CALLBACKNAME sr_Activity_Character = "activityCharacter";
CALLBACKNAME sr_up_Activity_Character = "activityCharacter";
CALLBACKNAME sr_q_Activity_Character = "activityCharacter";
//sl:sl-up:sl-q
CALLBACKNAME sl_Activity_Character = "activityCharacter";
CALLBACKNAME sl_up_Activity_Character = "activityCharacter";
CALLBACKNAME sl_q_Activity_Character = "activityCharacter";
//rr:rr-up:rr-q
CALLBACKNAME rr_Activity_Character = "activityCharacter";
CALLBACKNAME rr_up_Activity_Character = "activityCharacter";
CALLBACKNAME rr_q_Activity_Character = "activityCharacter";
//rl:rl-up:rl-q
CALLBACKNAME rl_Activity_Character = "activityCharacter";
CALLBACKNAME rl_up_Activity_Character = "activityCharacter";
CALLBACKNAME rl_q_Activity_Character = "activityCharacter";
//j:j-up:j-q
CALLBACKNAME j_Activity_Character = "activityCharacter";
CALLBACKNAME j_up_Activity_Character = "activityCharacter";
CALLBACKNAME j_q_Activity_Character = "activityCharacter";
//q:q-up
CALLBACKNAME q_Activity_Character = "activityCharacter";
CALLBACKNAME q_up_Activity_Character = "activityCharacter";

///Character + Activity related functions/variables
//Transition table: <eventType, currentState> -> nextState
static TransitionTable* transitionTablePtr;
void activityCharacter(const Event * event, void * data)
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
	(*transitionTablePtr)[StateEventType(currentState, eventType)];
	//
	if (not nextState.empty())
	{
		characterFSM.request(nextState);
	}
	else
	{
		PRINTERR("activityCharacter: Transition not defined for event type '" <<
				eventType << "' and state '" << currentState << "'");
	}
}

#define TABLEINSERT(state,eventType,nextState) \
	(*transitionTablePtr).insert(TransitionTableItem\
			(StateEventType(state, eventType), NextState(nextState)))

///Init/end functions: see common_configs.cpp
void activityCharacterInit()
{
	transitionTablePtr = new TransitionTable();
	//insert transitions
	//state I
	TABLEINSERT("I", "f", "F");
	TABLEINSERT("I", "f-q", "F-Q");
	TABLEINSERT("I", "b", "B");
	TABLEINSERT("I", "sr", "Sr");
	TABLEINSERT("I", "sr-q", "Sr-Q");
	TABLEINSERT("I", "sl", "Sl");
	TABLEINSERT("I", "sl-q", "Sl-Q");
	TABLEINSERT("I", "rr", "Rr");
	TABLEINSERT("I", "rr-q", "Rr-Q");
	TABLEINSERT("I", "rl", "Rl");
	TABLEINSERT("I", "rl-q", "Rl-Q");
	TABLEINSERT("I", "j", "J");
	//state F
	TABLEINSERT("F", "f-up","I");
	TABLEINSERT("F", "f-q","F-Q");
	TABLEINSERT("F", "sr","F-Sr");
	TABLEINSERT("F", "sl","F-Sl");
	TABLEINSERT("F", "rr","F-Rr");
	TABLEINSERT("F", "rr-q","F-Rr-Q");
	TABLEINSERT("F", "rl","F-Rl");
	TABLEINSERT("F", "rl-q","F-Rl-Q");
	TABLEINSERT("F", "j","F-J");
	TABLEINSERT("F", "j-q","F-J-Q");
	TABLEINSERT("F", "q","F-Q");
	//state B
	TABLEINSERT("B", "b-up","I");
	TABLEINSERT("B", "sr","B-Sr");
	TABLEINSERT("B", "sl","B-Sl");
	TABLEINSERT("B", "rr","B-Rr");
	TABLEINSERT("B", "rl","B-Rl");
	//Sr
	TABLEINSERT("Sr", "sr-up","I");
	TABLEINSERT("Sr", "sr-q","Sr-Q");
	TABLEINSERT("Sr", "rr", "Sr-Rr");
	TABLEINSERT("Sr", "rr-q", "Sr-Rr-Q");
	TABLEINSERT("Sr", "rl", "Sr-Rl");
	TABLEINSERT("Sr", "rl-q", "Sr-Rl-Q");
	TABLEINSERT("Sr", "q","Sr-Q");
	//Sl
	TABLEINSERT("Sl", "sl-up","I");
	TABLEINSERT("Sl", "sl-q","Sl-Q");
	TABLEINSERT("Sl", "rr", "Sl-Rr");
	TABLEINSERT("Sl", "rr-q", "Sl-Rr-Q");
	TABLEINSERT("Sl", "rl", "Sl-Rl");
	TABLEINSERT("Sl", "rl-q", "Sl-Rl-Q");
	TABLEINSERT("Sl", "q","Sl-Q");
	//Rr
	TABLEINSERT("Rr", "f","F-Rr");
	TABLEINSERT("Rr", "f-q","F-Rr-Q");
	TABLEINSERT("Rr", "b","B-Rr");
	TABLEINSERT("Rr", "sr", "Sr-Rr");
	TABLEINSERT("Rr", "sr-q", "Sr-Rr-Q");
	TABLEINSERT("Rr", "sl", "Sl-Rr");
	TABLEINSERT("Rr", "sl-q", "Sr-Rl-Q");
	TABLEINSERT("Rr", "rr-up","I");
	TABLEINSERT("Rr", "rr-q","Rr-Q");
	TABLEINSERT("Rr", "q","Rr-Q");
	//Rl
	TABLEINSERT("Rl", "f","F-Rl");
	TABLEINSERT("Rl", "f-q","F-Rl-Q");
	TABLEINSERT("Rl", "b","B-Rl");
	TABLEINSERT("Rl", "sr", "Sr-Rl");
	TABLEINSERT("Rl", "sr-q", "Sr-Rl-Q");
	TABLEINSERT("Rl", "sl", "Sl-Rl");
	TABLEINSERT("Rl", "sl-q", "Sl-Rl-Q");
	TABLEINSERT("Rl", "rl-up","I");
	TABLEINSERT("Rl", "rl-q","Rl-Q");
	TABLEINSERT("Rl", "q","Rl-Q");
	//J
	TABLEINSERT("J", "f","F-J");
	TABLEINSERT("J", "f-q","F-J-Q");
	TABLEINSERT("J", "j-up","I");
	//F-Rr
	TABLEINSERT("F-Rr", "f-up", "Rr");
	TABLEINSERT("F-Rr", "f-q", "F-Rr-Q");
	TABLEINSERT("F-Rr", "rr-up", "F");
	TABLEINSERT("F-Rr", "rr-q", "F-Rr-Q");
	TABLEINSERT("F-Rr", "q", "F-Rr-Q");
	//F-Rl
	TABLEINSERT("F-Rl", "f-up", "Rl");
	TABLEINSERT("F-Rl", "f-q", "F-Rl-Q");
	TABLEINSERT("F-Rl", "rl-up", "F");
	TABLEINSERT("F-Rl", "rl-q", "F-Rl-Q");
	TABLEINSERT("F-Rl", "q", "F-Rl-Q");
	//F-J
	TABLEINSERT("F-J", "f-up","J");
	TABLEINSERT("F-J", "f-q","F-J-Q");
	TABLEINSERT("F-J", "j-up","F");
	TABLEINSERT("F-J", "j-q","F-J-Q");
	TABLEINSERT("F-J", "q","F-J-Q");
	//B-Rr
	TABLEINSERT("B-Rr", "b-up","Rr");
	TABLEINSERT("B-Rr", "rr-up", "B");
	//B-Rl
	TABLEINSERT("B-Rl", "b-up", "Rl");
	TABLEINSERT("B-Rl", "rl-up", "B");
	//Sr-Rr
	TABLEINSERT("Sr-Rr", "sr-up", "Rr");
	TABLEINSERT("Sr-Rr", "sr-q", "Sr-Rr-Q");
	TABLEINSERT("Sr-Rr", "rr-up", "Sr");
	TABLEINSERT("Sr-Rr", "rr-q", "Sr-Rr-Q");
	TABLEINSERT("Sr-Rr", "q", "Sr-Rr-Q");
	//Sr-Rl
	TABLEINSERT("Sr-Rl", "sr-up", "Rl");
	TABLEINSERT("Sr-Rl", "sr-q", "Sr-Rl-Q");
	TABLEINSERT("Sr-Rl", "rl-up", "Sr");
	TABLEINSERT("Sr-Rl", "rl-q", "Sr-Rl-Q");
	TABLEINSERT("Sr-Rl", "q", "Sr-Rl-Q");
	//Sl-Rr
	TABLEINSERT("Sl-Rr", "sl-up", "Rr");
	TABLEINSERT("Sl-Rr", "sl-q", "Sl-Rr-Q");
	TABLEINSERT("Sl-Rr", "rr-up", "Sl");
	TABLEINSERT("Sl-Rr", "rr-q", "Sl-Rr-Q");
	TABLEINSERT("Sl-Rr", "q", "Sl-Rr-Q");
	//Sl-Rl
	TABLEINSERT("Sl-Rl", "sl-up", "Rl");
	TABLEINSERT("Sl-Rl", "sl-q", "Sl-Rl-Q");
	TABLEINSERT("Sl-Rl", "rl-up", "Sl");
	TABLEINSERT("Sl-Rl", "rl-q", "Sl-Rl-Q");
	TABLEINSERT("Sl-Rl", "q", "Sl-Rl-Q");
	//F-Q
	TABLEINSERT("F-Q", "f-up", "I");
	TABLEINSERT("F-Q", "rr", "F-Rr-Q");
	TABLEINSERT("F-Q", "rr-q", "F-Rr-Q");
	TABLEINSERT("F-Q", "rl", "F-Rl-Q");
	TABLEINSERT("F-Q", "rl-q", "F-Rl-Q");
	TABLEINSERT("F-Q", "j", "F-J-Q");
	TABLEINSERT("F-Q", "q-up", "F");
	//Sr-Q
	TABLEINSERT("Sr-Q", "sr-up", "I");
	TABLEINSERT("Sr-Q", "rr", "Sr-Rr-Q");
	TABLEINSERT("Sr-Q", "rr-q", "Sr-Rr-Q");
	TABLEINSERT("Sr-Q", "rl", "Sr-Rl-Q");
	TABLEINSERT("Sr-Q", "rl-q", "Sr-Rl-Q");
	TABLEINSERT("Sr-Q", "q-up", "Sr");
	//Sl-Q
	TABLEINSERT("Sl-Q", "sl-up", "I");
	TABLEINSERT("Sl-Q", "rr", "Sl-Rr-Q");
	TABLEINSERT("Sl-Q", "rr-q", "Sl-Rr-Q");
	TABLEINSERT("Sl-Q", "rl", "Sl-Rl-Q");
	TABLEINSERT("Sl-Q", "rl-q", "Sl-Rl-Q");
	TABLEINSERT("Sl-Q", "q-up", "Sl");
	//Rr-Q
	TABLEINSERT("Rr-Q", "f", "F-Rr-Q");
	TABLEINSERT("Rr-Q", "f-q", "F-Rr-Q");
	TABLEINSERT("Rr-Q", "sr", "Sr-Rr-Q");
	TABLEINSERT("Rr-Q", "sr-q", "Sr-Rr-Q");
	TABLEINSERT("Rr-Q", "sl", "Sl-Rr-Q");
	TABLEINSERT("Rr-Q", "sl-q", "Sl-Rr-Q");
	TABLEINSERT("Rr-Q", "rr-up", "I");
	TABLEINSERT("Rr-Q", "q-up", "Rr");
	//Rl-Q
	TABLEINSERT("Rl-Q", "f", "F-Rl-Q");
	TABLEINSERT("Rl-Q", "f-q", "F-Rl-Q");
	TABLEINSERT("Rl-Q", "sr", "Sr-Rl-Q");
	TABLEINSERT("Rl-Q", "sr-q", "Sr-Rl-Q");
	TABLEINSERT("Rl-Q", "sl", "Sl-Rl-Q");
	TABLEINSERT("Rl-Q", "sl-q", "Sl-Rl-Q");
	TABLEINSERT("Rl-Q", "rl-up", "I");
	TABLEINSERT("Rl-Q", "q-up", "Rl");
	//F-Rr-Q
	TABLEINSERT("F-Rr-Q", "f-up", "Rr-Q");
	TABLEINSERT("F-Rr-Q", "rr-up", "F-Q");
	TABLEINSERT("F-Rr-Q", "q-up", "F-Rr");
	//F-Rl-Q
	TABLEINSERT("F-Rl-Q", "f-up", "Rl-Q");
	TABLEINSERT("F-Rl-Q", "rl-up", "F-Q");
	TABLEINSERT("F-Rl-Q", "q-up", "F-Rl");
	//F-J-Q
	TABLEINSERT("F-J-Q", "f-up", "J");
	TABLEINSERT("F-J-Q", "j-up", "F-Q");
	TABLEINSERT("F-J-Q", "q-up", "F-J");
	//Sr-Rr-Q
	TABLEINSERT("Sr-Rr-Q", "sr-up", "Rr-Q");
	TABLEINSERT("Sr-Rr-Q", "rr-up", "Sr-Q");
	TABLEINSERT("Sr-Rr-Q", "q-up", "Sr-Rr");
	//Sr-Rl-Q
	TABLEINSERT("Sr-Rl-Q", "sr-up", "Rl-Q");
	TABLEINSERT("Sr-Rl-Q", "rl-up", "Sr-Q");
	TABLEINSERT("Sr-Rl-Q", "q-up", "Sr-Rl");
	//Sl-Rr-Q
	TABLEINSERT("Sl-Rr-Q", "sl-up", "Rr-Q");
	TABLEINSERT("Sl-Rr-Q", "rr-up", "Sl-Q");
	TABLEINSERT("Sl-Rr-Q", "q-up", "Sl-Rr");
	//Sl-Rl-Q
	TABLEINSERT("Sl-Rl-Q", "sl-up", "Rl-Q");
	TABLEINSERT("Sl-Rl-Q", "rl-up", "Sl-Q");
	TABLEINSERT("Sl-Rl-Q", "q-up", "Sr-Rl");
}
void activityCharacterEnd()
{
	delete transitionTablePtr;
}

