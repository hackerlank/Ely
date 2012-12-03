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
//forward:stop-forward:fast-forward
CALLBACKNAME forward_Activity_Character = "activityCharacter";
CALLBACKNAME stop_forward_Activity_Character = "activityCharacter";
CALLBACKNAME fast_forward_Activity_Character = "activityCharacter";
//backward:stop-backward:fast-backward
CALLBACKNAME backward_Activity_Character = "activityCharacter";
CALLBACKNAME stop_backward_Activity_Character = "activityCharacter";
CALLBACKNAME fast_backward_Activity_Character = "activityCharacter";
//roll_left:stop-roll_left:fast-roll_left
CALLBACKNAME roll_left_Activity_Character = "activityCharacter";
CALLBACKNAME stop_roll_left_Activity_Character = "activityCharacter";
CALLBACKNAME fast_roll_left_Activity_Character = "activityCharacter";
//roll_right:stop-roll_right:fast-roll_right
CALLBACKNAME roll_right_Activity_Character = "activityCharacter";
CALLBACKNAME stop_roll_right_Activity_Character = "activityCharacter";
CALLBACKNAME fast_roll_right_Activity_Character = "activityCharacter";
//jump:stop-jump:fast-jump
CALLBACKNAME jump_Activity_Character = "activityCharacter";
CALLBACKNAME stop_jump_Activity_Character = "activityCharacter";
CALLBACKNAME fast_jump_Activity_Character = "activityCharacter";
//fast:stop-fast
CALLBACKNAME fast_Activity_Character = "activityCharacter";
CALLBACKNAME stop_fast_Activity_Character = "activityCharacter";

///Character + Activity related functions/variables
///EventType(s)
//"forward:stop-forward:fast-forward"
//"backward:stop-backward:fast-backward"
//"roll_left:stop-roll_left:fast-roll_left"
//"roll_right:stop-roll_right:fast-roll_right"
//"jump:stop-jump:fast-jump"
//"fast:stop-fast"
///State(s)
//"forward:backward"
//"roll_left:roll_right"
//"forward_roll_left:forward_roll_right"
//"jump"
//"idle"

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
	(*transitionTablePtr)[EventTypeState(eventType, currentState)];
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

///Init/end functions: see common_configs.cpp
void activityCharacterInit()
{
	transitionTablePtr = new TransitionTable();
	//insert transitions
	//row 1
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("forward", "idle"),
					NextState("forward")));
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("forward", "backward"),
					NextState("idle")));
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("forward", "roll_left"),
					NextState("forward_roll_left")));
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("forward", "roll_right"),
					NextState("forward_roll_right")));
	//row 2
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("stop-forward", "forward"),
					NextState("idle")));
	(*transitionTablePtr).insert(
			TransitionTableItem(
					EventTypeState("stop-forward", "forward_roll_left"),
					NextState("roll_left")));
	(*transitionTablePtr).insert(
			TransitionTableItem(
					EventTypeState("stop-forward", "forward_roll_right"),
					NextState("roll_right")));
	//row 3
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("backward", "idle"),
					NextState("backward")));
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("backward", "forward"),
					NextState("idle")));
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("backward", "roll_left"),
					NextState("idle")));
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("backward", "roll_right"),
					NextState("idle")));
	//row 4
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("stop-backward", "backward"),
					NextState("idle")));
	//row 5
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("roll_right", "idle"),
					NextState("roll_right")));
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("roll_right", "forward"),
					NextState("forward_roll_right")));
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("roll_right", "roll_left"),
					NextState("idle")));
	(*transitionTablePtr).insert(
			TransitionTableItem(
					EventTypeState("roll_right", "forward_roll_left"),
					NextState("forward")));
	//row 6
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("stop-roll_right", "roll_right"),
					NextState("idle")));
	(*transitionTablePtr).insert(
			TransitionTableItem(
					EventTypeState("stop-roll_right", "forward_roll_right"),
					NextState("forward")));
	//row 7
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("roll_left", "idle"),
					NextState("roll_left")));
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("roll_left", "forward"),
					NextState("forward_roll_left")));
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("roll_left", "roll_right"),
					NextState("idle")));
	(*transitionTablePtr).insert(
			TransitionTableItem(
					EventTypeState("roll_left", "forward_roll_right"),
					NextState("forward")));
	//row 8
	(*transitionTablePtr).insert(
			TransitionTableItem(EventTypeState("stop-roll_left", "roll_left"),
					NextState("idle")));
	(*transitionTablePtr).insert(
			TransitionTableItem(
					EventTypeState("stop-roll_left", "forward_roll_left"),
					NextState("forward")));
}
void activityCharacterEnd()
{
	delete transitionTablePtr;
}

