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
void driveCamera(const Event* event, void* data)
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

///Actor1 + Activity related
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
void stateActor1(const Event * event, void * data)
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
		PRINTERR("stateActor1: Event not defined for state transition: " << event->get_name());
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

///NPC1 + Activity related
static std::string f("i");
static std::string f_up("i-up");
static std::string b("k");
static std::string b_up("k-up");
static std::string r("l");
static std::string r_up("l-up");
static std::string l("j");
static std::string l_up("j-up");

void stateNPC1(const Event * event, void * data)
{
	//get data
	SMARTPTR(Activity)activity = (Activity*) data;
	//get event
	std::string eventName = event->get_name();
	//get fsm
	fsm& npc1FSM = (fsm&) (*activity);
	//get pbject
	SMARTPTR(Object) npc1 = activity->getOwnerObject();
	//get character controller
	SMARTPTR(CharacterController) npc1CharCtrl =
	DCAST (CharacterController, npc1->getComponent("Physics"));
	//set transitions
	std::string currentState = npc1FSM.getCurrentOrNextState();
	///TODO
	if (eventName == f)
	{
		if ( currentState == "idle")
		{
			npc1FSM.request("forward");
		}
		else if (currentState == "backward")
		{
			npc1FSM.request("idle");
		}
	}
	else if (eventName == f_up)
	{
		if ( currentState == "idle")
		{
			npc1FSM.request("forward");
		}
		else if (currentState == "backward")
		{
			npc1FSM.request("idle");
		}
	}
	else if (eventName == b)
	{
		if ( currentState == "idle")
		{
			npc1FSM.request("forward");
		}
		else if (currentState == "backward")
		{
			npc1FSM.request("idle");
		}
	}
	else if (eventName == b_up)
	{
		if ( currentState == "idle")
		{
			npc1FSM.request("forward");
		}
		else if (currentState == "backward")
		{
			npc1FSM.request("idle");
		}
	}
	else if (eventName == r)
	{
		if ( currentState == "idle")
		{
			npc1FSM.request("forward");
		}
		else if (currentState == "backward")
		{
			npc1FSM.request("idle");
		}
	}
	else if (eventName == r_up)
	{
		if ( currentState == "idle")
		{
			npc1FSM.request("forward");
		}
		else if (currentState == "backward")
		{
			npc1FSM.request("idle");
		}
	}
	else if (eventName == l)
	{
		if ( currentState == "idle")
		{
			npc1FSM.request("forward");
		}
		else if (currentState == "backward")
		{
			npc1FSM.request("idle");
		}
	}
	else if (eventName == l_up)
	{
		if ( currentState == "idle")
		{
			npc1FSM.request("forward");
		}
		else if (currentState == "backward")
		{
			npc1FSM.request("idle");
		}
	}
	else
	{
		PRINTERR("stateNPC1: Event not defined for state transition: " << event->get_name());
	}
//	//check if alt or alt-up key pressed
//	if ((eventName == "alt") or (eventName == "alt-up"))
//	{
//		if (eventName.find("-up", 0) == string::npos)
//		{
//			//fast speeds
//			characterDrv->setLinearSpeed(5.0*characterDrv->getLinearSpeed());
//			characterDrv->setAngularSpeed(5.0*characterDrv->getAngularSpeed());
//		}
//		else
//		{
//			//normal speeds
//			characterDrv->setLinearSpeed(0.2*characterDrv->getLinearSpeed());
//			characterDrv->setAngularSpeed(0.2*characterDrv->getAngularSpeed());
//		}
//		return;
//	}
}
