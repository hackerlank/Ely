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
 * \file /Ely/ely/callbacks/activityActor.cpp
 *
 * \date 26/nov/2012 (10:37:44)
 * \author consultit
 */

#include "../common_configs.h"
#include "ObjectModel/Object.h"
#include "BehaviorComponents/Activity.h"
#include "ControlComponents/Driver.h"

using namespace ely;

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///Actor + Activity related
CALLBACK fast_Activity_Actor;
CALLBACK stop_fast_Activity_Actor;
CALLBACK forward_Activity_Actor;
CALLBACK stop_forward_Activity_Actor;
CALLBACK fast_forward_Activity_Actor;
CALLBACK backward_Activity_Actor;
CALLBACK stop_backward_Activity_Actor;
CALLBACK fast_backward_Activity_Actor;
CALLBACK roll_left_Activity_Actor;
CALLBACK stop_roll_left_Activity_Actor;
CALLBACK fast_roll_left_Activity_Actor;
CALLBACK roll_right_Activity_Actor;
CALLBACK stop_roll_right_Activity_Actor;
CALLBACK fast_roll_right_Activity_Actor;
CALLBACK up_Activity_Actor;
CALLBACK stop_up_Activity_Actor;
CALLBACK fast_up_Activity_Actor;
CALLBACK down_Activity_Actor;
CALLBACK stop_down_Activity_Actor;
CALLBACK fast_down_Activity_Actor;

#ifdef __cplusplus
}
#endif

///Actor + Activity related CALLBACKNAMEs & CALLBACKs
//fast:stop-fast
namespace
{
bool isFast = false;
}

void fast_Activity_Actor(const Event* event, void* data)
{
	//get data
//	SMARTPTR(Activity)activity = reinterpret_cast<Activity*>(data);
//	SMARTPTR(Object) actorObj = activity->getOwnerObject();
//	SMARTPTR(Driver) actorDrv = DCAST (Driver, actorObj->getComponent("Control"));
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	if (not isFast)
	{
		float speedFactor = actorDrv->getFastFactor();
		float maxAngularSpeed;
		actorDrv->setMaxLinearSpeed(actorDrv->getMaxSpeeds(maxAngularSpeed) * speedFactor);
		actorDrv->setMaxAngularSpeed(maxAngularSpeed * speedFactor);
		isFast = not isFast;
	}
}
void stop_fast_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	if (isFast)
	{
		float speedFactor = actorDrv->getFastFactor();
		float maxAngularSpeed;
		actorDrv->setMaxLinearSpeed(actorDrv->getMaxSpeeds(maxAngularSpeed) / speedFactor);
		actorDrv->setMaxAngularSpeed(maxAngularSpeed / speedFactor);
		isFast = not isFast;
	}
}
//forward:stop-forward:fast-forward
void forward_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableForward(true);
}
void stop_forward_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableForward(false);
}
void fast_forward_Activity_Actor(const Event* event, void* data)
{
	fast_Activity_Actor(event, data);
	forward_Activity_Actor(event, data);
}
//backward:stop-backward:fast-backward
void backward_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableBackward(true);
}
void stop_backward_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableBackward(false);
}
void fast_backward_Activity_Actor(const Event* event, void* data)
{
	fast_Activity_Actor(event, data);
	backward_Activity_Actor(event, data);
}
//roll_left:stop-roll_left:fast-roll_left
void roll_left_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableRollLeft(true);
}
void stop_roll_left_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableRollLeft(false);
}
void fast_roll_left_Activity_Actor(const Event* event, void* data)
{
	fast_Activity_Actor(event, data);
	roll_left_Activity_Actor(event, data);
}
//roll_right:stop-roll_right:fast-roll_right
void roll_right_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableRollRight(true);
}
void stop_roll_right_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableRollRight(false);
}
void fast_roll_right_Activity_Actor(const Event* event, void* data)
{
	fast_Activity_Actor(event, data);
	roll_right_Activity_Actor(event, data);
}
//up:stop-up:fast-up
void up_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableUp(true);
}
void stop_up_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableUp(false);
}
void fast_up_Activity_Actor(const Event* event, void* data)
{
	fast_Activity_Actor(event, data);
	up_Activity_Actor(event, data);
}
//down:stop-down:fast-down
void down_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableDown(true);
}
void stop_down_Activity_Actor(const Event* event, void* data)
{
	//get data
	SMARTPTR(Driver)actorDrv =
	DCAST (Driver, reinterpret_cast<Activity*>(data)->
			getOwnerObject()->getComponent("Control"));

	actorDrv->enableDown(false);
}
void fast_down_Activity_Actor(const Event* event, void* data)
{
	fast_Activity_Actor(event, data);
	down_Activity_Actor(event, data);
}

///Init/end functions: see common_configs.cpp
void activityActorInit()
{
}
void activityActorEnd()
{
}
