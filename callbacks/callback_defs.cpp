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

///Camera + Driver related
#include "ControlComponents/Driver.h"

void drive(const Event * event, void * data)
{
	//get data
	Driver* driver = (Driver*) data;
	bool enable;
	std::string bareEvent;
	//analyze the event
	size_t upPos = event->get_name().find("-up", 0);
	if (upPos == string::npos)
	{
		//command should be enabled (no "-up" found)
		enable = true;
		//event could be of type STR or shift-STR
		size_t strPos = event->get_name().find("shift-", 0);
		if (strPos == string::npos)
		{
			//event is of type STR (no "shift-" found)
			bareEvent = event->get_name();
		}
		else
		{
			//event is of type shift-STR (STR is from
			//std::string("shift-").length() up to end)
			bareEvent = event->get_name().substr(
					std::string("shift-").length());
		}
	}
	else
	{
		//command should be disabled
		enable = false;
		//event is of type STR-up (STR is starts at 0
		//and is upPos characters long)
		bareEvent = event->get_name().substr(0, upPos);
	}
	//set the right command
	if (bareEvent == "w")
	{
		driver->enableForward(enable);
	}
	else if (bareEvent == "s")
	{
		driver->enableBackward(enable);
	}
	else if (bareEvent == "a")
	{
		driver->enableStrafeLeft(enable);
	}
	else if (bareEvent == "d")
	{
		driver->enableStrafeRight(enable);
	}
	else if (bareEvent == "r")
	{
		driver->enableUp(enable);
	}
	else if (bareEvent == "f")
	{
		driver->enableDown(enable);
	}
	else
	{
		PRINTERR("Event not defined: " << event->get_name());
	}
}

void speed(const Event * event, void * data)
{
	//get data
	Driver* driver = (Driver*) data;
	//analyze the event: shift or shift-up
	(event->get_name().find("-up", 0) == string::npos) ?
			driver->setSpeedFast() : driver->setSpeed();
}

///Actor1 + Activity related
#include "BehaviorComponents/Activity.h"

void state(const Event * event, void * data)
{
	//get data
	Activity* activity = (Activity*) data;
	bool enable;
	std::string bareEvent;
	//analyze the event
	size_t upPos = event->get_name().find("-up", 0);
	if (upPos == string::npos)
	{
		//command should be enabled (no "-up" found)
		enable = true;
		//event could be of type STR or shift-STR
		size_t strPos = event->get_name().find("shift-", 0);
		if (strPos == string::npos)
		{
			//event is of type STR (no "shift-" found)
			bareEvent = event->get_name();
		}
		else
		{
			//event is of type shift-STR (STR is from
			//std::string("shift-").length() up to end)
			bareEvent = event->get_name().substr(
					std::string("shift-").length());
		}
	}
	else
	{
		//command should be disabled
		enable = false;
		//event is of type STR-up (STR is starts at 0
		//and is upPos characters long)
		bareEvent = event->get_name().substr(0, upPos);
	}
	//get a reference to the actor fsm
	fsm& actorFSM = (fsm&) (*activity);
	Object* actorObj = activity->getOwnerObject();
	//set the right command
	if (bareEvent == "w")
	{
		if (enable)
		{
			actorFSM.request("forward");
		}
	}
	else if (bareEvent == "s")
	{
		if (enable)
		{
			actorFSM.request("backward");
		}
	}
	else if (bareEvent == "a")
	{
		if (enable)
		{
			actorFSM.request("strafe_left");
		}
	}
	else if (bareEvent == "d")
	{
		if (enable)
		{
			actorFSM.request("strafe_right");
		}
	}
	else
	{
		PRINTERR("Event not defined: " << event->get_name());
	}
}
