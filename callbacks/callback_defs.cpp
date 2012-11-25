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
 * \file /Ely/callbacks/callback_defs.cpp
 *
 * \date 20/ago/2012 (12:50:42)
 * \author marco
 */

#include "callback_defs.h"
#include "Utilities/ComponentSuite.h"

//common typedefs
typedef std::pair<std::string, std::string> EventTypeState;
typedef std::string NextState;
typedef std::pair<EventTypeState, NextState> TransitionTableItem;
typedef std::map<EventTypeState, NextState> TransitionTable;

//inits - ends
//Character + Activity
static void activityCharacterInit();
static void activityCharacterEnd();

void callAllInits()
{
	activityCharacterInit();
	//
}
void callAllEnds()
{
	activityCharacterEnd();
	//
}

///get key bare event: from key, key-up, shift-key returns key and
///with *isEnabled false on key-up and true otherwise
static std::string getBareEvent(const std::string& eventName,
		const std::string& modifier, bool* isEnabled)
{
	//analyze the event
	std::string bareEvent;
	size_t upPos = eventName.find("-up", 0);
	if (upPos == string::npos)
	{
		//command should be enabled (no "-up" found)
		*isEnabled = true;
		//event could be of type STR or modifierSTR
		size_t strPos = eventName.find(modifier, 0);
		if (strPos == string::npos)
		{
			//event is of type STR (no modifier found)
			bareEvent = eventName;
		}
		else
		{
			//event is of type modifierSTR (STR is from
			//std::string(modifier).length() up to end)
			bareEvent = eventName.substr(std::string(modifier).length());
		}
	}
	else
	{
		//command should be disabled
		*isEnabled = false;
		//event is of type STR-up (STR is starts at 0
		//and is upPos characters long)
		bareEvent = eventName.substr(0, upPos);
	}
	//
	return bareEvent;
}

///Driver helper functions for key events in this order:
//static const char* keys[] =
//{
//		"w", //forward
//		"s", //backward
//		"a", //strafe_left
//		"d", //strafe_right
//		"q", //roll_left
//		"e", //roll_right
//		"r", //up
//		"f", //down
//};
static void setDriverCommand(SMARTPTR(Driver)driver,
const std::string& bareEvent, bool enable, const char* keys[])
{
	//set the right command
		if (bareEvent == keys[0])
		{
			driver->enableForward(enable);
		}
		else if (bareEvent == keys[1])
		{
			driver->enableBackward(enable);
		}
		else if (bareEvent == keys[2])
		{
			driver->enableStrafeLeft(enable);
		}
		else if (bareEvent == keys[3])
		{
			driver->enableStrafeRight(enable);
		}
		else if (bareEvent == keys[4])
		{
			driver->enableRollLeft(enable);
		}
		else if (bareEvent == keys[5])
		{
			driver->enableRollRight(enable);
		}
		else if (bareEvent == keys[6])
		{
			driver->enableUp(enable);
		}
		else if (bareEvent == keys[7])
		{
			driver->enableDown(enable);
		}
		else
		{
			PRINTERR("setDriverCommand: Event not defined: " << bareEvent);
		}
	}

///Camera + Driver related
static const char* camera_keys[] =
{ "w", //forward
		"s", //backward
		"a", //strafe_left
		"d", //strafe_right
		"", //roll_left
		"", //roll_right
		"r", //up
		"f", //down
		};
void driverCamera(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)driver = (Driver*) data;
	bool enable;
	std::string eventName = event->get_name();
	//check if shift or shift-up key pressed
	if ((eventName == "shift") or (eventName == "shift-up"))
	{
		(eventName.find("-up", 0) == string::npos) ?
		driver->setSpeedFast() : driver->setSpeed();
		return;
	}
	//get bare event
	std::string bareEvent = getBareEvent(eventName, "shift-", &enable);
	//execute command
	setDriverCommand(driver, bareEvent, enable, camera_keys);
}

///Actor + Activity related
static const char* Actor1_keys[] =
{ "arrow_up", //forward
		"arrow_down", //backward
		"arrow_left", //strafe_left
		"arrow_right", //strafe_right
		"", //roll_left
		"", //roll_right
		"page_up", //up
		"page_down", //down
		};
void activityActor(const Event * event, void * data)
{
	//get data
	SMARTPTR(Activity)activity = (Activity*) data;
	bool enable;
	//get bare event
	std::string eventName = event->get_name();
	std::string bareEvent = getBareEvent(eventName, "shift-", &enable);
	//get a reference to the actor fsm
	fsm& actorFSM = (fsm&) (*activity);
	SMARTPTR(Object) actorObj = activity->getOwnerObject();
	//set the right command
	if (bareEvent == "arrow_up")
	{
		if (enable)
		{
			actorFSM.request("forward");
		}
	}
	else if (bareEvent == "arrow_down")
	{
		if (enable)
		{
			actorFSM.request("backward");
		}
	}
	else if (bareEvent == "arrow_left")
	{
		if (enable)
		{
			actorFSM.request("strafe_left");
		}
	}
	else if (bareEvent == "arrow_right")
	{
		if (enable)
		{
			actorFSM.request("strafe_right");
		}
	}
	else if (bareEvent == "page_up")
	{
		if (enable)
		{
			actorFSM.request("up");
		}
	}
	else if (bareEvent == "page_down")
	{
		if (enable)
		{
			actorFSM.request("down");
		}
	}
	else
	{
		PRINTERR("activityActor: Event not defined for state transition: " << event->get_name());
	}
	//call the Driver event handler to move actor
	SMARTPTR(Driver) actorDrv = DCAST (Driver, actorObj->getComponent("Control"));
	//check if shift or shift-up key pressed
	if ((eventName == "shift") or (eventName == "shift-up"))
	{
		(eventName.find("-up", 0) == string::npos) ?
		actorDrv->setSpeedFast() : actorDrv->setSpeed();
		return;
	}
	//execute command
	setDriverCommand(actorDrv, bareEvent, enable, Actor1_keys);
}

///Character + Activity related
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
static TransitionTable activityCharacterTransitionTable;
static void activityCharacterInit()
{
	activityCharacterTransitionTable.clear();
	//insert transitions
	//row 1
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("forward", "idle"),
					NextState("forward")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("forward", "backward"),
					NextState("idle")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("forward", "roll_left"),
					NextState("forward_roll_left")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("forward", "roll_right"),
					NextState("forward_roll_right")));
	//row 2
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("stop-forward", "forward"),
					NextState("idle")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(
					EventTypeState("stop-forward", "forward_roll_left"),
					NextState("roll_left")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(
					EventTypeState("stop-forward", "forward_roll_right"),
					NextState("roll_right")));
	//row 3
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("backward", "idle"),
					NextState("backward")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("backward", "forward"),
					NextState("idle")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("backward", "roll_left"),
					NextState("idle")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("backward", "roll_right"),
					NextState("idle")));
	//row 4
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("stop-backward", "backward"),
					NextState("idle")));
	//row 5
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("roll_right", "idle"),
					NextState("roll_right")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("roll_right", "forward"),
					NextState("forward_roll_right")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("roll_right", "roll_left"),
					NextState("idle")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(
					EventTypeState("roll_right", "forward_roll_left"),
					NextState("forward")));
	//row 6
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("stop-roll_right", "roll_right"),
					NextState("idle")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(
					EventTypeState("stop-roll_right", "forward_roll_right"),
					NextState("forward")));
	//row 7
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("roll_left", "idle"),
					NextState("roll_left")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("roll_left", "forward"),
					NextState("forward_roll_left")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("roll_left", "roll_right"),
					NextState("idle")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(
					EventTypeState("roll_left", "forward_roll_right"),
					NextState("forward")));
	//row 8
	activityCharacterTransitionTable.insert(
			TransitionTableItem(EventTypeState("stop-roll_left", "roll_left"),
					NextState("idle")));
	activityCharacterTransitionTable.insert(
			TransitionTableItem(
					EventTypeState("stop-roll_left", "forward_roll_left"),
					NextState("forward")));
}
static void activityCharacterEnd()
{
}

void activityCharacter(const Event * event, void * data)
{
	//get data
	SMARTPTR(Activity)activity = (Activity*) data;
	//get event type
	std::string eventType = activity->getEventType(event->get_name());
	//get fsm
	fsm& characterFSM = (fsm&) (*activity);
	//set transitions
	std::string currentState = characterFSM.getCurrentOrNextState();
	//
	std::string nextState =
			activityCharacterTransitionTable[EventTypeState(eventType, currentState)];
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
