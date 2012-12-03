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
 * \file /Ely/callbacks/driverCamera.cpp
 *
 * \date 26/nov/2012 (11:04:25)
 * \author marco
 */

#include "../common_configs.h"
#include "Utilities/ComponentSuite.h"

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///Camera + Driver related
CALLBACK driverCamera;

#ifdef __cplusplus
}
#endif

///Camera + Driver related CALLBACKNAMEs
//forward:stop-forward:fast-forward
CALLBACKNAME forward_Driver_Camera = "driverCamera";
CALLBACKNAME stop_forward_Driver_Camera = "driverCamera";
CALLBACKNAME fast_forward_Driver_Camera = "driverCamera";
//backward:stop-backward:fast-backward
CALLBACKNAME backward_Driver_Camera = "driverCamera";
CALLBACKNAME stop_backward_Driver_Camera = "driverCamera";
CALLBACKNAME fast_backward_Driver_Camera = "driverCamera";
//roll_left:stop-roll_left:fast-roll_left
CALLBACKNAME roll_left_Driver_Camera = "driverCamera";
CALLBACKNAME stop_roll_left_Driver_Camera = "driverCamera";
CALLBACKNAME fast_roll_left_Driver_Camera = "driverCamera";
//roll_right:stop-roll_right:fast-roll_right
CALLBACKNAME roll_right_Driver_Camera = "driverCamera";
CALLBACKNAME stop_roll_right_Driver_Camera = "driverCamera";
CALLBACKNAME fast_roll_right_Driver_Camera = "driverCamera";
//up:stop-up:fast-up
CALLBACKNAME up_Driver_Camera = "driverCamera";
CALLBACKNAME stop_up_Driver_Camera = "driverCamera";
CALLBACKNAME fast_up_Driver_Camera = "driverCamera";
//down:stop-down:fast-down
CALLBACKNAME down_Driver_Camera = "driverCamera";
CALLBACKNAME stop_down_Driver_Camera = "driverCamera";
CALLBACKNAME fast_down_Driver_Camera = "driverCamera";
//fast:stop-fast
CALLBACKNAME fast_Driver_Camera = "driverCamera";
CALLBACKNAME stop_fast_Driver_Camera = "driverCamera";

///Camera + Driver related functions/variables
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
	SMARTPTR(Driver)driver = reinterpret_cast<Driver*>(data);
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

///Init/end functions: see common_configs.cpp
void driverCameraInit()
{
}
void driverCameraEnd()
{
}

